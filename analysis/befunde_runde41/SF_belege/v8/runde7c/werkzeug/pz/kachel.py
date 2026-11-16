"""Kacheln aus eigenen echten Dumps: Zeilen Staende, Spalten Bilder; Ausschnitt um den Kopf (Klasse >= 3 in V7), x2 vergroessert."""
import sys, numpy as np
from PIL import Image, ImageDraw
import szene as S
def kachel(staende, bilder, out, halb=100, z=2, lm=False):
    KV = np.load(S.L_ + 'V7_kl.npz')['K']
    rows = []
    for n in staende:
        cols = []
        for f in bilder:
            H = KV[f - S.F0] >= 3; ys, xs = np.nonzero(H); cy, cx = int(ys.mean()), int(xs.mean())
            im = S.ppm(n + ('_LM' if lm else ''), f).astype(np.uint8)
            y0, x0 = max(cy - halb, 0), max(cx - halb, 0)
            c = Image.fromarray(im[y0:y0 + 2 * halb, x0:x0 + 2 * halb]).resize((2 * halb * z, 2 * halb * z), Image.NEAREST)
            d = ImageDraw.Draw(c); d.rectangle([0, 0, 120, 16], fill=(0, 0, 0)); d.text((3, 2), '%s F%d' % (n, f), fill=(255, 255, 0))
            cols.append(np.asarray(c))
        rows.append(np.concatenate(cols, 1))
    Image.fromarray(np.concatenate(rows, 0)).save(out)
if __name__ == '__main__':
    st = sys.argv[1].split(','); bi = [int(x) for x in sys.argv[2].split(',')]
    kachel(st, bi, sys.argv[3], halb=int(sys.argv[4]) if len(sys.argv) > 4 else 100, lm='--lm' in sys.argv)
