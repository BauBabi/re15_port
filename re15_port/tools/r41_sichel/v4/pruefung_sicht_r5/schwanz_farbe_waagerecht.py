"""Pruefung V4 r5 Sicht: Farbe und Verteilung des waagerechten Schwanzteils (x 284-356, y < 262) und des senkrechten Teils
(y 262-345): Original Vorlage 2 roh in der Leser-Maske (sfv4/schwanz_maske.npy) gegen V4 r5 / V4 r4 (Ids 16-22, Pose 85,
Videoraum). Dazu Querprofil-Kennzahlen je x 290..350 (Plateau |dL/dy|<0,3, Stufe 3 px) und Facetten: Zahl der Laengsbaender
(Laufstrecken gleicher Helligkeit +-1 L* auf >= 3 px) im Schirmbild."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
SF = sys.argv[1]; R = SF + '/sicht_r5'; out = {}
M = np.load(SF + '/schwanz_maske.npy')
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
yy = np.arange(480)[:, None]; xx = np.arange(640)[None, :]
def er(m): return cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
def st(img, m):
    px = img[m]; lab = G.lab(px); C = np.hypot(lab[:, 1], lab[:, 2])
    return dict(n=int(m.sum()), lab=G.r1(lab.mean(0)), L_p10_50_90=G.r1(np.percentile(lab[:, 0], [10, 50, 90])),
                a_p10_50_90=G.r1(np.percentile(lab[:, 1], [10, 50, 90])), rot_a12=round(float((lab[:, 1] > 12).mean()), 3),
                rosagrau=round(float(((lab[:, 0] > 25) & (C < 12)).mean()), 3), C_mittel=round(float(C.mean()), 1))
WA = (xx >= 284) & (xx <= 356) & (yy < 262); SE = (yy >= 262) & (yy <= 345)
o2 = L(R + '/vorlage2_entbl.png')
out['orig'] = dict(waagerecht=st(o2, er(M) & WA), senkrecht=st(o2, er(M) & SE))
def kenn(Lm, m, x):
    ys = np.nonzero(m[:262, x])[0]
    if len(ys) < 5: return None
    v = Lm[ys.min():ys.max() + 1, x]; d = np.diff(v)
    return dict(plateau=round(float((np.abs(d) < 0.3).mean()), 2), stufe3=round(float(np.abs(v[3:] - v[:-3]).max()), 1))
Lo = G.lab(o2)[..., 0]
out['orig']['quer'] = {x: kenn(Lo, M, x) for x in range(290, 352, 6)}
for nm in ('V4', 'V4r4', 'REF2'):
    img = L(R + '/%s_85.png' % nm); ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]; sw = np.isin(ids, range(16, 23)); V = G.videoraum(img)
    d = dict(waagerecht=st(V, er(sw) & WA), senkrecht=st(V, er(sw) & SE))
    d['waagerecht_dE00'] = round(G.de00(out['orig']['waagerecht']['lab'], d['waagerecht']['lab']), 1)
    d['senkrecht_dE00'] = round(G.de00(out['orig']['senkrecht']['lab'], d['senkrecht']['lab']), 1)
    Lv = G.lab(V)[..., 0]; d['quer'] = {x: kenn(Lv, sw, x) for x in range(290, 352, 6)}
    # Laengsbaender im Schirm: entlang x 300..340 je Spalte Zahl der Laufstrecken gleicher Helligkeit (+-1 L*) mit Laenge >= 3
    Ls = G.lab(img)[..., 0]; nb = []
    for x in range(300, 341, 4):
        ys = np.nonzero(sw[:262, x])[0]
        if len(ys) < 5: continue
        v = Ls[ys.min():ys.max() + 1, x]; runs = 0; k = 1
        for i in range(1, len(v)):
            if abs(v[i] - v[i - 1]) <= 1.0: k += 1
            else:
                runs += k >= 3; k = 1
        runs += k >= 3; nb.append(runs)
    d['schirm_baender_je_spalte'] = nb
    out[nm] = d
json.dump(out, open(sys.argv[2], 'w'), indent=1, default=str)
for k, v in out.items(): print(k, json.dumps(v, default=str)[:1500])
