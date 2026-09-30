# w8_mess.py <dir> <f_ohne> <f_mit> <x0,y0,x1,y1> [abr]
# ABR0 (0.5*B + 0.5*F*m): F*m = 2*out - B ; ABR1 (B + F*m): F*m = out - B. Gibt je Kanal das Maximum
# und die Verteilung der wirksamen Beitraege im Rechteck aus (nur Pixel mit Aenderung).
import sys, os
from PIL import Image
d, fa, fb, box = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), [int(v) for v in sys.argv[4].split(',')]
abr = int(sys.argv[5]) if len(sys.argv) > 5 else 0
A = Image.open(os.path.join(d, 'f_%06d.ppm' % fa)).convert('RGB')
B = Image.open(os.path.join(d, 'f_%06d.ppm' % fb)).convert('RGB')
x0, y0, x1, y1 = box
vals = []
for y in range(y0, y1):
    for x in range(x0, x1):
        a = A.getpixel((x, y)); b = B.getpixel((x, y))
        if a == b: continue
        if abr == 0: f = tuple(2*b[i] - a[i] for i in range(3))
        else:        f = tuple(b[i] - a[i] for i in range(3))
        vals.append((f, a, b, (x, y)))
print('geaenderte Pixel:', len(vals))
for c in range(3):
    m = max(v[0][c] for v in vals)
    print('Kanal', 'RGB'[c], 'max F*m =', m)
top = sorted(vals, key=lambda v: -sum(v[0]))[:8]
for v in top: print('  F*m', v[0], 'B', v[1], 'out', v[2], 'bei', v[3])
