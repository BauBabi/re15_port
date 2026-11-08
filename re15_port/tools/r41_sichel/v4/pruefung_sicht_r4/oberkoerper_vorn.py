"""Pruefung V4 r4 Sicht, Beobachtung (i): Gesamteindruck Oberkoerper vorn. Original = Vorlage 1 entblendet, Polygon um
Buckel/Schultern/Brust (Kopfoberkante bis Brustunterkante, 0,37 H1); Port = Buehne Pose 135 im Videoraum, Ids 8/9/10/13
oberhalb derselben Hoehe in H (0,37 H_port ab Figur-Oberkante). Verteilungen: L* p10/50/90, Anteil L*>30, Anteil rosa-grau
(L*>25 und C*<12), Rotanteil (R-G>=15), a* Mittel; dazu Drittel links/Mitte/rechts der Breite."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R = sys.argv[1]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def st(img, m):
    px = img[m]; lab = G.lab(px); C = np.hypot(lab[:, 1], lab[:, 2])
    return dict(n=int(m.sum()), lab=G.r1(lab.mean(0)), L_p10_50_90=G.r1(np.percentile(lab[:, 0], [10, 50, 90])),
                hell30=round(float((lab[:, 0] > 30).mean()), 3), rosagrau=round(float(((lab[:, 0] > 25) & (C < 12)).mean()), 3),
                rot=round(float(((px[:, 0] - px[:, 1]) >= 15).mean()), 3), dunkel15=round(float((lab[:, 0] < 15).mean()), 3))
POLY = np.array([(297, 175), (320, 150), (360, 140), (410, 138), (450, 150), (490, 165), (505, 195), (500, 235), (470, 250), (330, 250), (300, 230)], np.int32)
mo = np.zeros((480, 640), np.uint8); cv2.fillPoly(mo, [POLY], 1); mo = mo.astype(bool)
o = L(R + '/vorlage1_entbl.png'); out = {'orig': {'ganz': st(o, mo)}}
x0, x1 = POLY[:, 0].min(), POLY[:, 0].max(); xx = np.arange(640)[None, :]
for i, nm in enumerate(('B_links', 'Mitte', 'B_rechts')):
    a = x0 + i * (x1 - x0) / 3; b = x0 + (i + 1) * (x1 - x0) / 3
    out['orig'][nm] = st(o, mo & (xx >= a) & (xx < b))
for name in ('V4', 'V4r3', 'REF2'):
    img = L(R + '/%s_135.png' % name); ids = np.load(R + '/%s_135_ids.npy' % name)[:480]; V = G.videoraum(img)
    top = np.nonzero((ids >= 0).any(1))[0].min(); ycut = int(round(top + 0.37 * 364))
    m = np.isin(ids, (8, 9, 10, 13)) & (np.arange(480)[:, None] <= ycut)
    m = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    ys, xs = np.nonzero(m); a0, a1 = xs.min(), xs.max(); d = {'ganz': st(V, m), 'ycut': ycut, 'x': [int(a0), int(a1)]}
    for i, nm in enumerate(('B_links', 'Mitte', 'B_rechts')):
        a = a0 + i * (a1 - a0) / 3; b = a0 + (i + 1) * (a1 - a0) / 3
        d[nm] = st(V, m & (xx >= a) & (xx < b))
    d['dE00_ganz'] = round(G.de00(out['orig']['ganz']['lab'], d['ganz']['lab']), 1)
    for nm in ('B_links', 'Mitte', 'B_rechts'): d['dE00_' + nm] = round(G.de00(out['orig'][nm]['lab'], d[nm]['lab']), 1)
    out[name] = d
vis = o.copy(); cv2.polylines(vis, [POLY], True, (255, 255, 0), 1)
Image.fromarray(vis.astype(np.uint8)).save(R + '/oberkoerper_poly_orig.png')
json.dump(out, open(sys.argv[2], 'w'), indent=1, default=str); print(json.dumps(out, indent=1))
