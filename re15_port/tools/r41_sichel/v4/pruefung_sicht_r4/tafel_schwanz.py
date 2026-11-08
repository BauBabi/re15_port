"""Pruefung V4 r4 Sicht: Tafel Schwanz waagerechter Teil - Vorlage 2 x4 | V4 r4 Pose 85 x4 | Spiel V4 r3 h90 x3 | Querprofil L* bei x 320."""
import sys, json, numpy as np, cv2
R = sys.argv[1]; OUT = sys.argv[2]
o = cv2.imread(R + '/vorlage2_entbl.png'); v = cv2.imread(R + '/V4_85.png'); s = cv2.imread(R + '/spiel_V4r4_h90.png')
def cr(im, x0, y0, w, h, f): return cv2.resize(im[y0:y0+h, x0:x0+w], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
a = cr(o, 260, 185, 110, 90, 4); b = cr(v, 260, 185, 110, 90, 4); c = cr(s, 280, 130, 147, 120, 3)
for im in (a, b):
    cv2.line(im, ((320-260)*4, 0), ((320-260)*4, im.shape[0]-1), (0, 255, 255), 1)
K = json.load(open(R + '/PS4_schwanz_brett.json'))
pl = np.full((360, 300, 3), 30, np.uint8)
for (nm, pr, col) in (('Original', K['orig']['profile']['320'], (255, 0, 255)), ('V4 r4', K['V4']['profile_videoraum']['320'], (255, 255, 0))):
    L = pr['L']; pts = [(10 + int(i * 280 / 45), 340 - int(l * 6)) for i, l in enumerate(L)]
    for p, q in zip(pts[:-1], pts[1:]): cv2.line(pl, p, q, col, 2)
    cv2.putText(pl, nm, (10, 20 if nm == 'Original' else 40), 0, 0.5, col, 1)
cv2.putText(pl, 'L* quer bei x 320 (oben -> unten)', (10, 60), 0, 0.42, (200, 200, 200), 1)
H = 360
def pad(x): return np.vstack([x, np.zeros((H - x.shape[0], x.shape[1], 3), np.uint8)]) if x.shape[0] < H else x[:H]
t = np.concatenate([pad(a), np.zeros((H, 4, 3), np.uint8), pad(b), np.zeros((H, 4, 3), np.uint8), pad(c), np.zeros((H, 4, 3), np.uint8), pl], 1)
for txt, x in (('Vorlage 2 x4', 4), ('V4 r4 Pose 85 x4', a.shape[1] + 8), ('V4 r4 Spiel h90 x3', a.shape[1] + b.shape[1] + 12)): cv2.putText(t, txt, (x, 16), 0, 0.5, (0, 255, 255), 1)
cv2.imwrite(OUT, t)
