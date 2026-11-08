import sys, numpy as np, cv2, json
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, W + '/nb2neu'); sys.path.insert(0, W + '/v4')
import c6
from PIL import Image
from skimage.morphology import skeletonize
from collections import deque
def lum(a): return np.asarray(a, float) @ np.array([0.299, 0.587, 0.114])
def kern_ring(L, p):
    x, y = p; k = L[y-1:y+2, x-1:x+2].mean(); r = L[y-7:y+8, x-7:x+8].copy(); r[4:11, 4:11] = np.nan; return round(float(k),1), round(float(np.nanmean(r)),1)
out = {}
ids = np.load('V4_135_ids.npy')[:480]; kopf = cv2.erode((ids == 9).astype(np.uint8), np.ones((5,5), np.uint8)).astype(bool)
for k in ('V4', 'V3r2'):
    s = cv2.cvtColor(cv2.imread('spiel_%s_h135.png' % k), cv2.COLOR_BGR2RGB).astype(float); Ls = lum(s)
    b = cv2.blur(Ls.astype(np.float32), (3, 3)); b[~kopf] = -1; iy, ix = np.unravel_index(np.argmax(b), b.shape); q = (int(ix), int(iy))
    kk, rr = kern_ring(Ls, q); thr = (kk+rr)/2
    m = ((Ls > thr) & (ids == 9)).astype(np.uint8); n, lab, st, cen = cv2.connectedComponentsWithStats(m, connectivity=4); c = lab[q[1], q[0]]
    zeile = Ls[q[1], q[0]-10:q[0]+11]
    out['spiel_%s_auge' % k] = dict(punkt=q, kern_ring=(kk, rr), sprung=round(kk-rr,1), blob_px=int(st[c, cv2.CC_STAT_AREA]), blob_wh=(int(st[c, cv2.CC_STAT_WIDTH]), int(st[c, cv2.CC_STAT_HEIGHT])), max_sprung_nachbar=round(float(np.abs(np.diff(zeile)).max()),1))
# Original: Blob um (376,166) mit derselben Schwelle-Regel, Fenster Kopfbox
v = cv2.cvtColor(c6.vid(135), cv2.COLOR_BGR2RGB).astype(float); Lv = lum(v)
q = (376, 166); kk, rr = kern_ring(Lv, q); thr = (kk+rr)/2
kb = np.zeros_like(Lv, bool); kb[139:192, 368:417] = True
m = ((Lv > thr) & kb).astype(np.uint8); n, lab, st, cen = cv2.connectedComponentsWithStats(m, connectivity=4); c = lab[q[1], q[0]]
zeile = Lv[q[1], q[0]-10:q[0]+11]
out['orig_auge_roh'] = dict(punkt=q, kern_ring=(kk, rr), sprung=round(kk-rr,1), blob_px=int(st[c, cv2.CC_STAT_AREA]), blob_wh=(int(st[c, cv2.CC_STAT_WIDTH]), int(st[c, cv2.CC_STAT_HEIGHT])), max_sprung_nachbar=round(float(np.abs(np.diff(zeile)).max()),1))
# Schwanzpfad V4 Pose 85
ids85 = np.load('V4_85_ids.npy')[:480]; sw = np.isin(ids85, range(16, 23)); sk = skeletonize(sw)
n, lab, st, cen = cv2.connectedComponentsWithStats(sk.astype(np.uint8), connectivity=8)
big = 1 + np.argmax(st[1:, cv2.CC_STAT_AREA]); ys, xs = np.nonzero(lab == big); P = set(zip(xs.tolist(), ys.tolist()))
def bfs(s):
    prev = {s: None}; dq = deque([s]); last = s
    while dq:
        c = dq.popleft(); last = c
        for dx in (-1,0,1):
            for dy in (-1,0,1):
                n_ = (c[0]+dx, c[1]+dy)
                if n_ in P and n_ not in prev: prev[n_] = c; dq.append(n_)
    return prev, last
prev, a = bfs(next(iter(P))); prev, b_ = bfs(a)
pfad = []; c = b_
while c is not None: pfad.append(c); c = prev[c]
if pfad[0][0] < pfad[-1][0]: pfad = pfad[::-1]   # Wurzel (groesseres x, am Ruecken) zuerst
richt = []
for i in range(0, len(pfad)-10, 10):
    p, q2 = pfad[i], pfad[i+10]; richt.append((p, int(round(np.degrees(np.arctan2(-(q2[1]-p[1]), q2[0]-p[0]))))))
d = [abs(((richt[i+1][1]-richt[i][1]) + 180) % 360 - 180) for i in range(len(richt)-1)]
out['v4_schwanz_pfad'] = dict(laenge_px=len(pfad), richtungen=richt, max_drehung_je10px=(max(d), richt[int(np.argmax(d))][0]), drehungen=d)
json.dump(out, open('auge2_sicht.json', 'w'), indent=1, default=str)
for k, v_ in out.items(): print(k, v_)
