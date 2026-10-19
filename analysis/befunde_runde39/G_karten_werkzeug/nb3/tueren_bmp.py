"""Nachbesserung 3 (N2): schwebende Tuerbalken in einem Kartenabzug der echten exe (RE15_INV_FB_SHOT, 320x240).
Kriterium wie tuer2.py der Abnahme 2: ein gelber Balken (224,168,40) schwebt, wenn ein Ende (laengs) auf dem Grund
(0,16,?) liegt oder quer direkt daneben ein Wandpixel (176,176,176) steht. Ausgabe je Balken: Lage, Enden, Wand quer.
tueren_bmp.py <bmp> [x0 y0 x1 y1]  (Kasten: nur Balken darin melden)"""
import sys
import numpy as np
from PIL import Image
def klassen(p):
    C = np.array(Image.open(p).convert('RGB')).astype(int)
    K = np.full(C.shape[:2], 7, int)
    r, g, b = C[..., 0], C[..., 1], C[..., 2]
    K[(r == 0) & (g == 16)] = 0
    K[((r == 16) & (g == 56)) | ((r == 48) & (g == 8))] = 1
    K[(r == 176) & (g == 176) & (b == 176)] = 2
    K[(r == 224) & (g == 168) & (b == 40)] = 4
    K[(r == 240) & (g == 240) & (b == 216)] = 5
    return K
def balken(K):
    seen = set(); out = []
    for y in range(40, 210):
        for x in range(40, 300):
            if K[y, x] == 4 and (x, y) not in seen:
                st = [(x, y)]; seen.add((x, y)); pts = []
                while st:
                    a, b = st.pop(); pts.append((a, b))
                    for da, db in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                        q = (a + da, b + db)
                        if K[b + db, a + da] == 4 and q not in seen: seen.add(q); st.append(q)
                out.append(pts)
    return out
def pruef(p, kasten=None):
    K = klassen(p); erg = []
    for pts in balken(K):
        xs = [q[0] for q in pts]; ys = [q[1] for q in pts]
        if kasten and not (kasten[0] <= min(xs) and max(xs) <= kasten[2] and kasten[1] <= min(ys) and max(ys) <= kasten[3]):
            continue
        if max(xs) - min(xs) >= max(ys) - min(ys):
            y = ys[0]; a, b = min(xs), max(xs); ends = [K[y, a - 1], K[y, b + 1]]
            sides = [K[y - 1, x] for x in range(a, b + 1)] + [K[y + 1, x] for x in range(a, b + 1)]
        else:
            x = xs[0]; a, b = min(ys), max(ys); ends = [K[a - 1, x], K[b + 1, x]]
            sides = [K[y, x - 1] for y in range(a, b + 1)] + [K[y, x + 1] for y in range(a, b + 1)]
        schwebt = any(e == 0 for e in ends) or any(s == 2 for s in sides)
        erg.append((min(xs), min(ys), max(xs), max(ys), [int(e) for e in ends], sum(1 for s in sides if s == 2), schwebt))
    return erg
if __name__ == '__main__':
    p = sys.argv[1]
    kasten = tuple(map(int, sys.argv[2:6])) if len(sys.argv) >= 6 else None
    erg = pruef(p, kasten)
    print('%s: %d Tuerbalken, schwebend %d' % (p, len(erg), sum(1 for e in erg if e[6])))
    for e in erg:
        if e[6] or kasten:
            print('   x%d..%d y%d..%d Enden %s Wand quer %d %s' % (e[0], e[2], e[1], e[3], e[4], e[5], 'SCHWEBT' if e[6] else 'ok'))
