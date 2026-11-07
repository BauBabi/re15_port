"""Pruefung V4 r5 Sicht: senkrechter Schwanzteil (y 262..345). Je Zeile Mittelpunkt der Maske, Lab (3x3) im Schirm / Videoraum (Port,
Ids 16-22 Pose 85) bzw. roh (Vorlage 2, Leser-Maske). Kennzahlen: groesster Sprung dE76 ueber 3 Zeilen, Zahl der Spruenge > 8
(Mindestabstand 6 Zeilen), Zahl der hellen Ringe (zusammenhaengende Zeilenlaeufe mit L* > 22), Laenge und Kantenhaerte."""
import sys, json, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
SF = sys.argv[1]; R = SF + '/sicht_r5'; out = {}
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def achse(lab, m):
    ys, vals = [], []
    for y in range(262, 346):
        xs = np.nonzero(m[y, :340])[0]
        if len(xs) < 3: continue
        x = int((xs.min() + xs.max()) / 2); vals.append(lab[y-1:y+2, x-1:x+2].reshape(-1, 3).mean(0)); ys.append(y)
    v = np.array(vals); d = np.linalg.norm(v[3:] - v[:-3], axis=1); j = []
    for i in np.argsort(-d):
        if d[i] <= 8: break
        if all(abs(i - k) >= 6 for k in j): j.append(int(i))
    hell = v[:, 0] > 22; ringe = int(np.sum(hell[1:] & ~hell[:-1]) + hell[0])
    return dict(n=len(ys), max_dE76_3z=round(float(d.max()), 1), spruenge_gt8=len(j), spruenge_y=sorted(ys[i] + 1 for i in j), helle_ringe_L22=ringe,
                L_p50_max=[round(float(np.median(v[:, 0])), 1), round(float(v[:, 0].max()), 1)], a_mittel=round(float(v[:, 1].mean()), 1),
                L_verlauf=[round(float(a), 1) for a in v[::3, 0]])
M = np.load(SF + '/schwanz_maske.npy'); out['orig'] = achse(G.lab(L(R + '/vorlage2_entbl.png')), M)
for nm in ('V4', 'V4r4'):
    img = L(R + '/%s_85.png' % nm); ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]; sw = np.isin(ids, range(16, 23))
    out[nm + '_videoraum'] = achse(G.lab(G.videoraum(img)), sw); out[nm + '_schirm'] = achse(G.lab(img), sw)
    out[nm + '_neutral'] = achse(G.lab(L(R + '/%s_85_neutral.png' % nm)), sw)
json.dump(out, open(sys.argv[2], 'w'), indent=1)
for k, v in out.items(): print(k, json.dumps(v))
