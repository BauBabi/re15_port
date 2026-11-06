# Leser B v3: Schwanz beim Halten (Vorderansicht h64/h68/h70), KI und Original: Farbe gegen Haut.
# Schwanzmaske KI: im Fenster (x0,y0,x1,y1) alle Pixel mit dE > 12 zum Hintergrund-Modus UND Lab a* > 10, groesste Komponente,
#   2 px erodiert. Original: dieselbe Maske halbiert (640x480), Schwarzwert -17 gestreckt, nochmals 2 px erodiert.
# Haut: Rechteck Brust/Schulter (an der Lupe gewaehlt), 2 px eingerueckt.
# Aufruf: python b_halten.py <sfv3-dir> <beleg.png> n:x0:y0:x1:y1:hx0:hy0:hx1:hy1 ...
import sys, json, cv2, numpy as np
S, BEL = sys.argv[1], sys.argv[2]


def lab(i):
    return cv2.cvtColor(i.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)


bg = lab(cv2.imread(f"{S}/tmp/bg_modus_ki.png"))
tiles = []
for arg in sys.argv[3:]:
    n, x0, y0, x1, y1, hx0, hy0, hx1, hy1 = map(int, arg.split(":"))
    raw = cv2.imread(f"{S}/ki/n{n}.png"); f = lab(raw)
    d = np.sqrt(((f - bg) ** 2).sum(2))
    m = ((d > 12) & (f[..., 1] > 10)).astype(np.uint8); w = np.zeros_like(m); w[y0:y1, x0:x1] = 1; m *= w
    m = cv2.morphologyEx(m, cv2.MORPH_CLOSE, np.ones((5, 5), np.uint8))
    k, l, s, _ = cv2.connectedComponentsWithStats(m); i = 1 + np.argmax(s[1:, 4]); m = (l == i).astype(np.uint8)
    te = cv2.erode(m, np.ones((5, 5), np.uint8)) > 0
    hs = np.zeros(te.shape, bool); hs[hy0 + 2:hy1 - 2, hx0 + 2:hx1 - 2] = True
    r = {}
    for nm, img, sel in [("KI", raw, None), ("Original", None, None)]:
        if nm == "KI":
            ff = f; im = raw; ts = te; hh = hs
        else:
            o = cv2.imread(f"{S}/orig/n{n}.png").astype(np.float32); im = np.clip((o - 17) * 255 / 238, 0, 255).astype(np.uint8); ff = lab(im)
            ts = cv2.erode(cv2.resize(m, (640, 480), interpolation=cv2.INTER_NEAREST), np.ones((5, 5), np.uint8)) > 0
            hh = np.zeros((480, 640), bool); hh[hy0 // 2 + 2:hy1 // 2 - 2, hx0 // 2 + 2:hx1 // 2 - 2] = True
        tl = ff[ts].mean(0); hl = ff[hh].mean(0)
        r[nm] = dict(schwanz_lab=[round(float(v), 2) for v in tl], schwanz_rgb=[round(float(v), 1) for v in im[ts][:, ::-1].mean(0)],
                     haut_lab=[round(float(v), 2) for v in hl], dE=round(float(np.linalg.norm(tl - hl)), 2),
                     schwanz_anteil_a_gt_20=round(float((ff[ts][:, 1] > 20).mean()), 3), haut_anteil_a_gt_20=round(float((ff[hh][:, 1] > 20).mean()), 3),
                     schwanz_px=int(ts.sum()))
    print(n, json.dumps(r))
    v = raw.copy(); cs, _ = cv2.findContours(te.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(v, cs, -1, (0, 255, 0), 1)
    cv2.rectangle(v, (hx0, hy0), (hx1, hy1), (0, 255, 255), 1)
    cv2.putText(v, f"KI n{n} h{n - 1350}", (x0 - 280, y0 - 200), 0, 0.5, (0, 255, 255), 1)
    tiles.append(v[y0 - 220:y1 + 10, x0 - 300:x1 + 10])
h = min(t.shape[0] for t in tiles)
cv2.imwrite(BEL, np.hstack([t[:h] for t in tiles]))
