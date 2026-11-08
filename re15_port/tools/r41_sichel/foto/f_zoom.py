"""Zoom-Ausschnitt einer Vorlage mit Gitter (10 px), optional Kontrast (CLAHE auf L) und Polygon-Overlay."""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
def zoom(img, x0, y0, x1, y1, f=4, clahe=True, polys=(), maske=None, pkt=()):
    v = np.clip(img, 0, 255).astype(np.uint8)
    if clahe:
        lab = cv2.cvtColor(v, cv2.COLOR_RGB2LAB)
        lab[..., 0] = cv2.createCLAHE(3.0, (8, 8)).apply(lab[..., 0]); v = cv2.cvtColor(lab, cv2.COLOR_LAB2RGB)
    c = cv2.resize(v[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    c = cv2.cvtColor(c, cv2.COLOR_RGB2BGR)
    if maske is not None:
        m = cv2.resize(maske[y0:y1, x0:x1].astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
        cnts, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(c, cnts, -1, (0, 255, 0), 1)
    for x in range((x0 // 10 + 1) * 10, x1, 10):
        X = (x - x0) * f; cv2.line(c, (X, 0), (X, 6 if x % 50 else 14), (0, 255, 255), 1)
        if x % 50 == 0:
            for yy in range(0, c.shape[0], 6): c[yy, X] = (0, 255, 255)
        if x % 20 == 0: cv2.putText(c, str(x), (X + 2, 12), 0, 0.4, (0, 255, 255), 1)
    for y in range((y0 // 10 + 1) * 10, y1, 10):
        Y = (y - y0) * f; cv2.line(c, (0, Y), (6 if y % 50 else 14, Y), (0, 255, 255), 1)
        if y % 50 == 0:
            for xx in range(0, c.shape[1], 6): c[Y, xx] = (0, 255, 255)
        if y % 20 == 0: cv2.putText(c, str(y), (2, Y - 2), 0, 0.4, (0, 255, 255), 1)
    for pl in polys:
        p = ((np.array(pl, float) - [x0, y0]) * f + f / 2).astype(np.int32)
        cv2.polylines(c, [p], True, (255, 0, 255), 1)
    for (x, y) in pkt:
        cv2.circle(c, (int((x - x0) * f + f / 2), int((y - y0) * f + f / 2)), 4, (0, 0, 255), 1)
    return c
if __name__ == '__main__':
    b = int(sys.argv[1]); x0, y0, x1, y1 = map(int, sys.argv[2:6]); f = int(sys.argv[6]) if len(sys.argv) > 6 else 4
    cl = (sys.argv[7] != '0') if len(sys.argv) > 7 else True
    cv2.imwrite(os.path.join(G.SFV4, 'foto', 'zoom_%d_%d_%d.png' % (b, x0, y0)), zoom(G.vorlage(b), x0, y0, x1, y1, f, cl))
