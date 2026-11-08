import sys, numpy as np, cv2, json
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6, v4_gemein as G
from PIL import Image
from skimage.morphology import skeletonize
out = {}
def lum(a): return np.asarray(a, float) @ np.array([0.299, 0.587, 0.114])
def kern_ring(L, p):
    x, y = p; k = L[y-1:y+2, x-1:x+2].mean()
    r = L[y-7:y+8, x-7:x+8].copy(); r[4:11, 4:11] = np.nan; return float(k), float(np.nanmean(r))
# Original Auge vorn (Nutzerpunkt 382,164) roh und entblendet
v = cv2.cvtColor(c6.vid(135), cv2.COLOR_BGR2RGB).astype(float)
ve = np.asarray(Image.open('vorlage1_entbl.png').convert('RGB')).astype(float)
Lv, Le = lum(v), lum(ve)
# hellster 3x3-Punkt nahe dem Nutzerpunkt (+-6 px)
def hellster(L, p, r=6):
    x, y = p; b = cv2.blur(L.astype(np.float32), (3, 3))[y-r:y+r+1, x-r:x+r+1]; iy, ix = np.unravel_index(np.argmax(b), b.shape); return (x-r+ix, y-r+iy)
p = hellster(Lv, (382, 164)); out['orig_auge'] = dict(punkt=p, kern_ring_roh=kern_ring(Lv, p), kern_ring_entbl=kern_ring(Le, p))
# Spiel V4 h135: hellster Punkt im Kopfbereich (x 300-400, y 150-240), Kern/Ring; dazu Blob-Groesse bei Schwelle (Kern+Ring)/2
for k in ('V4', 'V3r2'):
    s = cv2.cvtColor(cv2.imread('spiel_%s_h135.png' % k), cv2.COLOR_BGR2RGB).astype(float); Ls = lum(s)
    b = cv2.blur(Ls.astype(np.float32), (3, 3)); sub = b[150:240, 300:400]; iy, ix = np.unravel_index(np.argmax(sub), sub.shape); q = (300+ix, 150+iy)
    kk, rr = kern_ring(Ls, q); thr = (kk+rr)/2
    m = (Ls > thr).astype(np.uint8); n, lab, st, cen = cv2.connectedComponentsWithStats(m, connectivity=4); c = lab[q[1], q[0]]
    out['spiel_%s_auge' % k] = dict(punkt=q, kern_ring=(kk, rr), schwelle=thr, blob_px=int(st[c, cv2.CC_STAT_AREA]), blob_wh=(int(st[c, cv2.CC_STAT_WIDTH]), int(st[c, cv2.CC_STAT_HEIGHT])))
    # Kantensteilheit: max. Lum-Sprung zwischen Nachbarpixeln auf der Zeile durch q, innerhalb +-10 px
    zeile = Ls[q[1], q[0]-10:q[0]+11]; out['spiel_%s_auge' % k]['max_sprung_zeile'] = float(np.abs(np.diff(zeile)).max())
zeile = Lv[p[1], p[0]-10:p[0]+11]; out['orig_auge']['max_sprung_zeile_roh'] = float(np.abs(np.diff(zeile)).max())
zeile = Le[p[1], p[0]-10:p[0]+11]; out['orig_auge']['max_sprung_zeile_entbl'] = float(np.abs(np.diff(zeile)).max())
# ---- Schwanz V4 Pose 85: Skelett-Richtung alle 10 px
ids = np.load('V4_85_ids.npy')[:480]; sw = np.isin(ids, range(16, 23))
sk = skeletonize(sw); ys, xs = np.nonzero(sk)
# Pfad: von Wurzel (naechster Punkt an Id 8 Rumpf) zum Endpunkt (unterster Punkt)
pts = list(zip(xs.tolist(), ys.tolist())); P = set(pts)
start = max(pts, key=lambda p: p[0]); ende = max(pts, key=lambda p: p[1])
# BFS
from collections import deque
prev = {start: None}; dq = deque([start])
while dq:
    c = dq.popleft()
    for dx in (-1, 0, 1):
        for dy in (-1, 0, 1):
            n_ = (c[0]+dx, c[1]+dy)
            if n_ in P and n_ not in prev: prev[n_] = c; dq.append(n_)
pfad = []; c = ende
while c is not None: pfad.append(c); c = prev.get(c)
pfad = pfad[::-1]
richt = []
for i in range(0, len(pfad)-10, 10):
    a, b_ = pfad[i], pfad[i+10]; richt.append((a, round(float(np.degrees(np.arctan2(-(b_[1]-a[1]), b_[0]-a[0]))), 0)))
out['v4_schwanz_pfad_richtung'] = richt
# groesste Richtungsaenderung je 10 px
d = [abs(((richt[i+1][1]-richt[i][1]) + 180) % 360 - 180) for i in range(len(richt)-1)]
out['v4_schwanz_max_drehung_je10px'] = (max(d), richt[int(np.argmax(d))][0]) if d else None
# ---- Kopf seitlich V4: Spaltenhoehe des Kopfes (ids 9) je x
k9 = ids == 9; prof = {}
for x in range(439, 484, 4):
    col = np.nonzero(k9[:, x])[0]; prof[x] = (int(col.min()), int(col.max()), int(col.max()-col.min()+1)) if len(col) else None
out['v4_kopf_seite_spaltenhoehe'] = prof
# Original Seite: dunkle Vorderkante + Augenfleck (Leser A): Front y 229..257 bei x 457-465 -> Hoehe >= 28 px = 0.11 H2
json.dump(out, open('auge_schwanz_sicht.json', 'w'), indent=1, default=str)
for k, v_ in out.items(): print(k, v_)
