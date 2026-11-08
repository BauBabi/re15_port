"""Gitterbild fuer die Handspur: Ausschnitt x f, Gamma 0,6 (dunkle Teile sichtbar), Linien alle 10 px, Zahlen alle 20 px,
optional Polygon(e) aus f_maske.POLY und Schwanzmaske."""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G
def gitter(img, x0, y0, x1, y1, f=4, gamma=0.6, polys=(), farbe=(255, 0, 255), masken=()):
    v = np.clip(img, 0, 255) / 255.0
    v = (255 * v ** gamma).astype(np.uint8)
    c = cv2.resize(v[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_CUBIC)
    c = cv2.cvtColor(c, cv2.COLOR_RGB2BGR)
    for x in range((x0 // 10 + 1) * 10, x1, 10):
        X = (x - x0) * f
        for yy in range(0, c.shape[0], 4 if x % 50 else 2): c[yy, X] = (0, 200, 255) if x % 50 else (0, 255, 255)
        if x % 20 == 0: cv2.putText(c, str(x), (X + 2, 12), 0, 0.4, (0, 255, 255), 1)
    for y in range((y0 // 10 + 1) * 10, y1, 10):
        Y = (y - y0) * f
        for xx in range(0, c.shape[1], 4 if y % 50 else 2): c[Y, xx] = (0, 200, 255) if y % 50 else (0, 255, 255)
        if y % 20 == 0: cv2.putText(c, str(y), (2, Y - 2), 0, 0.4, (0, 255, 255), 1)
    for mk, col in masken:
        m = cv2.resize(mk[y0:y1, x0:x1].astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
        cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE); cv2.drawContours(c, cs, -1, col, 1)
    for pl in polys:
        p = ((np.array(pl, float) - [x0, y0]) * f + f / 2).astype(np.int32)
        cv2.polylines(c, [p], True, farbe, 1)
    return c
if __name__ == '__main__':
    b = int(sys.argv[1]); x0, y0, x1, y1 = map(int, sys.argv[2:6]); f = int(sys.argv[6])
    polys = []
    if len(sys.argv) > 7 and sys.argv[7] == 'poly':
        import f_maske as FM
        polys = FM.POLY[b] + FM.WEG[b]
    cv2.imwrite(os.path.join(G.SFV4, 'foto', 'g_%d_%d_%d.png' % (b, x0, y0)), gitter(G.vorlage(b), x0, y0, x1, y1, f, polys=polys))
