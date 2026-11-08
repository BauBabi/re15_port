# Leser B v3: Port V1 Schwanz aus der Leiste 08_rundum_V1.jpg, Kachel "270 Grad rechte Flanke" (neutrales Licht).
# Maske: dE > 12 zum flachen Hintergrund (Mittel x 1300-1400, y 600-620 derselben Datei), im Fenster x 520-700, y 575-900,
#   ohne x >= 680 fuer y >= 640 (Koerper); Wurzelschnitt = senkrechte Linie x = 688 (Glied-Ansatz an der Flanke, an der Lupe).
# Mittellinie, Dicke, Winkel, Knicke wie b_schwanz.py (geodaetisch vom Schnitt; Gauss sigma 0,015 H; Fenster 0,05 H).
# H der Kachel: Buckel-Oberkante bis Sohle in derselben Kachel (an der Lupe: y 590 bis 990 -> 400 px, geschaetzt +-5).
# Haut-Vergleich: Rechtecke Flanke/Ruecken x 720-780 y 650-700 und Oberarm x 840-870 y 760-800.
import sys, json, cv2, numpy as np
from skimage.graph import MCP_Geometric
from scipy.ndimage import gaussian_filter1d
P, OUT = sys.argv[1], sys.argv[2]
im = cv2.imread(P); f = cv2.cvtColor(im.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)
bgl = f[600:620, 1300:1400].reshape(-1, 3).mean(0)
d = np.sqrt(((f - bgl) ** 2).sum(2))
m = (d > 12).astype(np.uint8); w = np.zeros_like(m); w[575:900, 520:700] = 1; w[640:, 680:] = 0; w[:, 688:] = 0; m *= w
m = cv2.morphologyEx(m, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
k, l, s, _ = cv2.connectedComponentsWithStats(m); i = 1 + np.argmax(s[1:, 4]); tail = (l == i).astype(np.uint8)
H = 400.0
start = np.argwhere((tail > 0) & (np.arange(tail.shape[1])[None, :] >= 685))
g, _ = MCP_Geometric(np.where(tail > 0, 1.0, np.inf)).find_costs([tuple(p) for p in start])
g = g.astype(float); g[tail == 0] = np.nan; g[~np.isfinite(g)] = np.nan
gmax = np.nanmax(g); tip = np.unravel_index(np.nanargmax(g), g.shape)
pts = []
for b0 in np.arange(0, gmax, 3):
    sel = (g >= b0) & (g < b0 + 3)
    if sel.sum():
        yy, xx = np.where(sel); pts.append((xx.mean(), yy.mean()))
Pk = np.vstack([np.array(pts), [tip[1], tip[0]]]); seg = np.hypot(*np.diff(Pk, axis=0).T); sl = np.concatenate([[0], np.cumsum(seg)]); Ltot = sl[-1]


def pt(t):
    t = min(max(t, 0), Ltot); q = min(max(np.searchsorted(sl, t), 1), len(sl) - 1)
    return Pk[q - 1] + (Pk[q] - Pk[q - 1]) * ((t - sl[q - 1]) / max(seg[q - 1], 1e-6))


tt = np.arange(0, Ltot, 1.0); C = np.array([pt(t) for t in tt]); sig = 0.015 * H
Cs = np.stack([gaussian_filter1d(C[:, 0], sig, mode='nearest'), gaussian_filter1d(C[:, 1], sig, mode='nearest')], 1)
np.save(f"{OUT}/port_tail.npy", tail); np.save(f"{OUT}/port_mittellinie.npy", Cs)
v = np.gradient(Cs, axis=0); ang = np.degrees(np.unwrap(np.arctan2(v[:, 1], v[:, 0])))
win = int(round(0.05 * H)); dA = np.abs(ang[win:] - ang[:-win]); kn = dA > 20
knicke = int(((kn[1:].astype(int) - kn[:-1].astype(int)) == 1).sum() + int(kn[0]))
T = np.gradient(Cs, axis=0); T /= np.linalg.norm(T, axis=1, keepdims=True); N = np.stack([-T[:, 1], T[:, 0]], 1)
dick = {}
for sf in [0.1, 0.2, 0.3, 0.5, 0.7, 0.9]:
    j = int(sf * (len(Cs) - 1)); p = Cs[j]; wd = 0
    for sg in (1, -1):
        r = 0
        while True:
            x, y = (p + sg * N[j] * (r + 1)).round().astype(int)
            if tail[y, x] == 0: break
            r += 1
        wd += r
    dick[str(sf)] = dict(px=wd + 1, H=round((wd + 1) / H, 3))
te = cv2.erode(tail, np.ones((5, 5), np.uint8)) > 0
hsv = cv2.cvtColor(im, cv2.COLOR_BGR2HSV); hue = hsv[..., 0].astype(int) * 2; rot = ((hue <= 20) | (hue >= 340)) & (hsv[..., 1] >= 90)


def st(sel):
    return dict(rgb=[round(float(x), 1) for x in im[sel][:, ::-1].mean(0)], lab=[round(float(x), 2) for x in f[sel].mean(0)],
                rot_hsv=round(float(rot[sel].mean()), 3), a_gt_12=round(float((f[sel][:, 1] > 12).mean()), 3), px=int(sel.sum()))


res = dict(H_px=H, bogenlaenge_H=round(Ltot / H, 3), spitze=[int(tip[1]), int(tip[0])], dicke=dick,
           max_winkel_je_005H=round(float(dA.max()), 1), bei_s=round(float(np.argmax(dA) / len(ang)), 2), knicke=knicke,
           gesamtdrehung=round(float(ang[-1] - ang[0]), 1), schwanz=st(te))
for nm, (x0, y0, x1, y1) in {"Flanke/Ruecken": (720, 650, 780, 700), "Oberarm": (840, 760, 870, 800)}.items():
    sel = np.zeros(te.shape, bool); sel[y0 + 2:y1 - 2, x0 + 2:x1 - 2] = True; res[nm] = st(sel)
    res[nm]["dE_zu_Schwanz"] = round(float(np.linalg.norm(np.array(res[nm]["lab"]) - np.array(res["schwanz"]["lab"]))), 2)
print(json.dumps(res, ensure_ascii=False))
o = im.copy(); cs, _ = cv2.findContours(te.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(o, cs, -1, (0, 255, 0), 1)
cv2.polylines(o, [Cs.round().astype(np.int32)], False, (255, 0, 255), 1)
cv2.rectangle(o, (720, 650), (780, 700), (0, 255, 255), 1); cv2.rectangle(o, (840, 760), (870, 800), (0, 255, 255), 1)
cv2.imwrite(f"{OUT}/port_v1_270.png", o[545:1025, 480:960])
