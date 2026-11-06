# Leser B v3: Maul der Sichel-Form im KI-Video 4 _slp (1280x960), Mass A7.
# Maulinneres = L* < 22 im Fenster (geglaettet 3x3), Teile >= 4 px, gefuellte konvexe Huelle = Maulflaeche.
# Zaehne = helle Flecken L* > ZAHN_L (Standard 45) und Chroma < 25 in der Maulflaeche 3 px dilatiert, je zusammenhaengender
#   Fleck >= 1 px ein Zahn; oben/unten = ueber/unter der Maulmitte (y).
# Aufruf: python b_maul.py <bild.png> <name> x0:y0:x1:y1 [OUT=png]
import sys, os, json, cv2, numpy as np
im = cv2.imread(sys.argv[1]); name = sys.argv[2]; x0, y0, x1, y1 = map(int, sys.argv[3].split(":"))
f = cv2.cvtColor(im.astype(np.float32) / 255, cv2.COLOR_BGR2Lab); L = cv2.GaussianBlur(f[..., 0], (3, 3), 0)
ZL = float(os.environ.get("ZAHN_L", "45"))
win = np.zeros(L.shape, bool); win[y0:y1, x0:x1] = True
dark = ((L < 22) & win).astype(np.uint8)
k, l, s, _ = cv2.connectedComponentsWithStats(dark)
comp = np.isin(l, [j for j in range(1, k) if s[j, 4] >= 4]).astype(np.uint8)
hull = cv2.convexHull(cv2.findNonZero(comp)); maul = np.zeros_like(comp); cv2.fillPoly(maul, [hull], 1)
bx, by, bw, bh = cv2.boundingRect(hull)
md = cv2.dilate(maul, np.ones((7, 7), np.uint8)).astype(bool)
chroma = np.hypot(f[..., 1], f[..., 2])
zahn = (f[..., 0] > ZL) & (chroma < 25) & md
kz, lz, sz, cz = cv2.connectedComponentsWithStats(zahn.astype(np.uint8))
cy = by + bh / 2
zaehne = [(round(float(cz[j][0]), 1), round(float(cz[j][1]), 1), int(sz[j, 4])) for j in range(1, kz)]
inn = maul.astype(bool) & ~zahn
res = dict(bild=name, fenster=[x0, y0, x1, y1], box=[bx, by, bw, bh], mitte=[round(bx + bw / 2, 1), round(by + bh / 2, 1)],
           breite_px=bw, hoehe_px=bh, seitenverhaeltnis_b_zu_h=round(bw / bh, 2), flaeche_px=int(maul.sum()),
           innen=dict(rgb=[round(float(v), 1) for v in im[inn][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[inn].mean(0)]),
           zaehne_oben=sum(1 for z in zaehne if z[1] < cy), zaehne_unten=sum(1 for z in zaehne if z[1] >= cy),
           zaehne=zaehne,
           zahn_farbe=dict(rgb=[round(float(v), 1) for v in im[zahn][:, ::-1].mean(0)], lab=[round(float(v), 2) for v in f[zahn].mean(0)]) if zahn.any() else None)
print(json.dumps(res))
if len(sys.argv) > 4:
    pad = 8; c = im[by - pad:by + bh + pad, bx - pad:bx + bw + pad].copy(); c = cv2.resize(c, None, fx=8, fy=8, interpolation=cv2.INTER_NEAREST)
    cc, _ = cv2.findContours(maul[by - pad:by + bh + pad, bx - pad:bx + bw + pad], cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(c, [(q * 8 + 4).astype(np.int32) for q in cc], -1, (0, 255, 0), 1)
    for (zx, zy, a) in zaehne:
        cv2.circle(c, (int((zx - bx + pad) * 8 + 4), int((zy - by + pad) * 8 + 4)), 6, (0, 255, 255), 1)
    cv2.putText(c, f"{name} Maul x{bx}-{bx + bw - 1} y{by}-{by + bh - 1}", (3, 12), 0, 0.4, (0, 255, 255), 1)
    cv2.imwrite(sys.argv[4], c)
