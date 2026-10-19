# Nachbesserung 1 M3: "Raum offen" ohne Referenzbild - Raumkoerper (rot/gruen) grenzt 4-nachbarlich direkt an Grund
# (Panel) im Kartenfeld x40..300 y40..210. Zaehlt solche Koerperpixel je Abzug.
import sys, numpy as np
from PIL import Image
def klassen(p):
    C = np.array(Image.open(p).convert('RGB')).astype(int)
    r, g, b = C[..., 0], C[..., 1], C[..., 2]
    K = np.full(r.shape, 'o', dtype='<U1')
    K[(r == 0) & (g == 16)] = '.'
    K[(r == 16) & (g == 56)] = 'g'
    K[(r == 48) & (g == 8)] = 'R'
    K[(r == 176) & (g == 176) & (b == 176)] = 'W'
    K[(r == 224) & (g == 168) & (b == 40)] = 'D'
    K[(r == 240) & (g == 240) & (b == 216)] = 'S'
    K[(r == 104) & (g == 136) & (b == 88)] = 'U'
    return K
def offen(K, x0=40, x1=300, y0=40, y1=210):
    out = []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if K[y, x] not in ('g', 'R'): continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                if K[y + dy, x + dx] == '.':
                    out.append((x, y, K[y, x], dx, dy)); break
    return out
if __name__ == '__main__':
    for p in sys.argv[1:]:
        K = klassen(p); o = offen(K)
        xs = [t[0] for t in o]; ys = [t[1] for t in o]
        print(f'{p}: offen {len(o)}' + (f' bbox ({min(xs)},{min(ys)})-({max(xs)},{max(ys)}) z.B. {o[:5]}' if o else ''))
