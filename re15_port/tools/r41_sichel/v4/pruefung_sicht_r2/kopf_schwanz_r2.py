"""Pruefung V4 r2 (Sicht): Kopf seitlich (Spaltenhoehe, Auge-Komponenten, dunkle Kante), Auge vorn im Spielbild (Kante),
Maulzone, Schwanz-Skelett (Drehung je 10 px), Schwanz-Zweitoenigkeit (Kante quer), helle Flecken auf den Armen."""
import sys, os, json, numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6, v4_gemein as G
from PIL import Image
from skimage.morphology import skeletonize
from collections import deque
OUT = sys.argv[1]; os.chdir(OUT)
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def S(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)
def lum(a): return np.asarray(a, float) @ np.array([0.299, 0.587, 0.114])
def kern_ring(Lm, p):
    x, y = p; k = Lm[y-1:y+2, x-1:x+2].mean(); r = Lm[y-7:y+8, x-7:x+8].copy(); r[4:11, 4:11] = np.nan
    return round(float(k), 1), round(float(np.nanmean(r)), 1)
out = {}
# ---------- Kopf seitlich: V4 Pose 85
v4s = L('V4_85.png'); ids85 = np.load('V4_85_ids.npy')[:480]; k9 = ids85 == 9
ys, xs = np.nonzero(k9); kb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
prof = {}
for x in range(kb[0], kb[2]+1, 3):
    col = np.nonzero(k9[:, x])[0]; prof[x] = (int(col.min()), int(col.max()), int(col.max()-col.min()+1)) if len(col) else None
out['v4_kopf_seite_spalten'] = prof
# Vorderkante: Spalte mit der letzten Kopfhoehe >= 50 % der Maximalhoehe
hmax = max(v[2] for v in prof.values() if v)
front = max(x for x, v in prof.items() if v and v[2] >= 0.5*hmax)
out['v4_kopf_seite_vorderkante_x'] = dict(x_50pct=front, hmax=hmax, kopfbox=kb)
lab85 = G.lab(v4s); Vv = G.videoraum(v4s); labV = G.lab(Vv)
dunkel = k9 & (lab85[..., 0] < 8)
out['v4_kopf_seite_schwarz'] = dict(n=int(dunkel.sum()), anteil=round(float(dunkel.sum()/k9.sum()), 3))
# Original Vorderkante (Messblatt 3): Streifen L<40 x 458-465, y 228-248; Breite je Zeile
o2 = L('vorlage2_entbl.png'); lo2 = G.lab(o2)
st = {}
for y in range(226, 252, 2):
    z = lo2[y, 440:475, 0]; xs_ = np.nonzero(z < 40)[0]
    st[y] = (int(440+xs_.min()), int(440+xs_.max()), int(len(xs_))) if len(xs_) else None
out['orig_seite_dunkelstreifen_L<40_x440-475'] = st
# V4: dunkle Kante (L<22 im Videoraum) je Zeile in der Kopfbox, Breite
st4 = {}
for y in range(kb[1], kb[3]+1, 2):
    z = labV[y, kb[0]:kb[2]+1, 0]; m = (z < 22) & k9[y, kb[0]:kb[2]+1]; xs_ = np.nonzero(m)[0]
    st4[y] = (int(kb[0]+xs_.min()), int(kb[0]+xs_.max()), int(len(xs_))) if len(xs_) else None
out['v4_seite_dunkelstreifen_L<22'] = st4
# Auge seitlich V4: helle Komponenten (L > 38 im Videoraum) in der Kopfbox
m = (labV[..., 0] > 38) & k9
n, lab_, stt, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
comps = [(int(stt[i, cv2.CC_STAT_AREA]), int(stt[i, cv2.CC_STAT_WIDTH]), int(stt[i, cv2.CC_STAT_HEIGHT]), (int(cen[i][0]), int(cen[i][1]))) for i in range(1, n)]
comps.sort(reverse=True); out['v4_auge_seite_komponenten_L>38'] = comps[:6]
# Original Auge seitlich: L > 38 in Kopfbox 412-467/190-263
mo = np.zeros(o2.shape[:2], bool); mo[190:264, 412:468] = True; m = (lo2[..., 0] > 38) & mo
n, lab_, stt, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
comps = [(int(stt[i, cv2.CC_STAT_AREA]), int(stt[i, cv2.CC_STAT_WIDTH]), int(stt[i, cv2.CC_STAT_HEIGHT]), (int(cen[i][0]), int(cen[i][1]))) for i in range(1, n)]
comps.sort(reverse=True); out['orig_auge_seite_komponenten_L>38'] = comps[:6]
# ---------- Kopf vorn: Auge im Spielbild h135 und in der Buehne (Kern/Ring, Blob, Kante)
ids135 = np.load('V4_135_ids.npy')[:480]; kopf = cv2.erode((ids135 == 9).astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
for name, img in (('buehne_V4_135', L('V4_135.png')), ('spiel_V4r2_h135', S('spiel_V4r2_h135.png')), ('spiel_V4r2_h118', S('spiel_V4r2_h118.png'))):
    Ls = lum(img); b = cv2.blur(Ls.astype(np.float32), (3, 3))
    if name.startswith('buehne'): b[~kopf] = -1
    else: bb = np.full_like(b, -1); bb[140:230, 300:400] = b[140:230, 300:400]; b = bb
    iy, ix = np.unravel_index(np.argmax(b), b.shape); q = (int(ix), int(iy)); kk, rr = kern_ring(Ls, q); thr = (kk+rr)/2
    mm = (Ls > thr).astype(np.uint8); n, lab_, stt, cen = cv2.connectedComponentsWithStats(mm, connectivity=4); c = lab_[q[1], q[0]]
    zeile = Ls[q[1], q[0]-10:q[0]+11]
    out['auge_vorn_' + name] = dict(punkt=q, kern_ring=(kk, rr), sprung=round(kk-rr, 1), blob_px=int(stt[c, cv2.CC_STAT_AREA]), blob_wh=(int(stt[c, cv2.CC_STAT_WIDTH]), int(stt[c, cv2.CC_STAT_HEIGHT])), max_sprung_nachbar=round(float(np.abs(np.diff(zeile)).max()), 1))
v = S('video_h135.png'); Lv = lum(v); q = (376, 166); kk, rr = kern_ring(Lv, q); thr = (kk+rr)/2
kbm = np.zeros_like(Lv, bool); kbm[139:192, 368:417] = True
mm = ((Lv > thr) & kbm).astype(np.uint8); n, lab_, stt, cen = cv2.connectedComponentsWithStats(mm, connectivity=4); c = lab_[q[1], q[0]]
zeile = Lv[q[1], q[0]-10:q[0]+11]
out['auge_vorn_orig_roh'] = dict(punkt=q, kern_ring=(kk, rr), sprung=round(kk-rr, 1), blob_px=int(stt[c, cv2.CC_STAT_AREA]), blob_wh=(int(stt[c, cv2.CC_STAT_WIDTH]), int(stt[c, cv2.CC_STAT_HEIGHT])), max_sprung_nachbar=round(float(np.abs(np.diff(zeile)).max()), 1))
# Kopf vorn: Flaechigkeit = Lum-Std und Kantenanteil im Kopf (Buehne, Videoraum) gegen Original-Kopfbox
o1 = L('vorlage1_entbl.png'); V135 = G.videoraum(L('V4_135.png'))
def textur(img, mask):
    Lm = lum(img); gx = np.abs(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0)); gy = np.abs(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1)); g = np.hypot(gx, gy)
    return dict(lum_std=round(float(Lm[mask].std()), 1), grad_mittel=round(float(g[mask].mean()), 1), grad_p90=round(float(np.percentile(g[mask], 90)), 1))
mo1 = np.zeros(o1.shape[:2], bool); mo1[142:189, 371:414] = True
out['kopf_vorn_textur_orig'] = textur(o1, mo1); out['kopf_vorn_textur_v4_videoraum'] = textur(V135, kopf); out['kopf_vorn_textur_v4_schirm'] = textur(L('V4_135.png'), kopf)
sp = S('spiel_V4r2_h135.png'); msp = np.zeros(sp.shape[:2], bool); msp[150:215, 320:380] = True
out['kopf_vorn_textur_spiel_h135_box320-380/150-215'] = textur(cv2.GaussianBlur(sp.astype(np.float32), (0, 0), 1.5), msp)
# ---------- Schwanz V4 Pose 85: Skelett, Drehung je 10 px
sw = np.isin(ids85, range(16, 23)); sk = skeletonize(sw)
n, lab_, stt, cen = cv2.connectedComponentsWithStats(sk.astype(np.uint8), connectivity=8)
big = 1 + np.argmax(stt[1:, cv2.CC_STAT_AREA]); ys, xs = np.nonzero(lab_ == big); P = set(zip(xs.tolist(), ys.tolist()))
def bfs(s):
    prev = {s: None}; dq = deque([s]); last = s
    while dq:
        c = dq.popleft(); last = c
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                n_ = (c[0]+dx, c[1]+dy)
                if n_ in P and n_ not in prev: prev[n_] = c; dq.append(n_)
    return prev, last
prev, a = bfs(next(iter(P))); prev, b_ = bfs(a)
pfad = []; c = b_
while c is not None: pfad.append(c); c = prev[c]
if pfad[0][0] < pfad[-1][0]: pfad = pfad[::-1]
richt = []
for i in range(0, len(pfad)-10, 10):
    p, q2 = pfad[i], pfad[i+10]; richt.append((p, int(round(np.degrees(np.arctan2(-(q2[1]-p[1]), q2[0]-p[0]))))))
d = [abs(((richt[i+1][1]-richt[i][1]) + 180) % 360 - 180) for i in range(len(richt)-1)]
out['v4_schwanz_pfad'] = dict(laenge_px=len(pfad), richtungen=richt, drehungen_je10px=d, max=(max(d), richt[int(np.argmax(d))][0]))
# Schwanz Zweitoenigkeit: Zeile y 240 und 236 (breiter Teil), Lum-Profil quer (Videoraum) V4 vs Original roh
for y in (234, 240, 246):
    xs_ = np.nonzero(sw[y, :380])[0]
    if len(xs_) == 0: continue
    x0, x1 = xs_.min(), xs_.max()
    out['schwanz_zeile_%d' % y] = dict(v4_x=(int(x0), int(x1)), v4_lum=[int(round(v)) for v in lum(Vv)[y, x0:x1+1]], orig_lum=[int(round(v)) for v in lum(o2)[y, x0:x1+1]])
# Spalte durch den breiten Teil (x 320): Lum von oben (y 215) bis unten (y 260)
for x in (320, 300):
    ys_ = np.nonzero(sw[:, x])[0]
    if len(ys_) == 0: continue
    y0, y1 = ys_.min(), ys_.max()
    out['schwanz_spalte_%d' % x] = dict(v4_y=(int(y0), int(y1)), v4_lum=[int(round(v)) for v in lum(Vv)[y0:y1+1, x]], orig_lum=[int(round(v)) for v in lum(o2)[y0:y1+1, x]])
# Oberseite Kante: Lum-Sprung an der Oberkante des Schwanzes (V4) gegen Hintergrund: Original Schwanz-Oberkante weich?
# ---------- helle Flecken an Armen (Buehne 135, Videoraum): Pixel L > 45 je Id
for i, nm in ((13, 'Oberarm_B_links'), (10, 'Oberarm_B_rechts'), (11, 'Unterarm_B_rechts'), (14, 'Unterarm_B_links'), (8, 'Rumpf'), (7, 'Hals_7'), (9, 'Kopf')):
    m = ids135 == i; lb = G.lab(V135[m])
    out['hell_' + nm] = dict(n=int(m.sum()), anteil_L_gt_45=round(float((lb[:, 0] > 45).mean()), 3), anteil_L_gt_55=round(float((lb[:, 0] > 55).mean()), 3), L_max5=G.r1(np.sort(lb[:, 0])[-5:]))
# Original: Boxen
for nm, bx in (('Oberarm_B_links', (300, 186, 345, 250)), ('Oberarm_B_rechts', (455, 196, 500, 262)), ('Rumpf', (352, 196, 448, 292)), ('Schulter_B_links', (318, 160, 360, 190)), ('Unterarm_B_rechts', (468, 268, 508, 330))):
    sub = o1[bx[1]:bx[3]+1, bx[0]:bx[2]+1].reshape(-1, 3); lb = G.lab(sub)
    out['hell_orig_' + nm] = dict(n=len(lb), anteil_L_gt_45=round(float((lb[:, 0] > 45).mean()), 3), anteil_L_gt_55=round(float((lb[:, 0] > 55).mean()), 3), L_max5=G.r1(np.sort(lb[:, 0])[-5:]))
# Spielbild h135: helle Flecken im Figurbereich (Box 300-500 x 130-440), Anteil L > 55 (nach Gauss 1,5)
spb = cv2.GaussianBlur(sp.astype(np.float32), (0, 0), 1.5); lbs = G.lab(spb[130:440, 300:500].reshape(-1, 3))
vb = v[130:440, 300:500].reshape(-1, 3); lbv = G.lab(vb)
out['hell_spiel_h135_box'] = dict(anteil_L_gt_55=round(float((lbs[:, 0] > 55).mean()), 3), anteil_L_gt_45=round(float((lbs[:, 0] > 45).mean()), 3))
out['hell_video_h135_box'] = dict(anteil_L_gt_55=round(float((lbv[:, 0] > 55).mean()), 3), anteil_L_gt_45=round(float((lbv[:, 0] > 45).mean()), 3))
json.dump(out, open('kopf_schwanz_r2.json', 'w'), indent=1, default=str)
for k, v_ in out.items(): print(k, v_)
