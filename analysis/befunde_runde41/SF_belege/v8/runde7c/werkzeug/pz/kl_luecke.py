"""Kachel: Klassenbild 640x480 mit Kerben (gelb) aus szene.mass, je Stand und Bild."""
import sys, numpy as np, szene as S
from PIL import Image, ImageDraw
from scipy import ndimage
from skimage.morphology import convex_hull_image
FARBE = {-1: (25, 35, 25), 0: (0, 150, 0), 1: (200, 0, 0), 2: (0, 0, 220), 3: (220, 0, 220), 4: (0, 220, 220), 5: (255, 160, 0)}
def G_von(k):
    S_ = k >= 0; H = k >= 3; m8 = (k == 1) | (k == 2); R = k == 2
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard'); Hn = dH <= 12; Hd = dH <= 1
    hull = convex_hull_image(H | (m8 & Hn), offset_coordinates=False); G = np.zeros_like(S_)
    lab, n = ndimage.label(hull & ~S_, np.ones((3, 3), bool))
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, np.ones((3, 3), bool))
        if (cd & Hd).any() and (cd & R).any(): G |= c
    return G
if __name__ == '__main__':
    st = sys.argv[1].split(','); bi = [int(x) for x in sys.argv[2].split(',')]; rows = []
    for n in st:
        K = np.load(S.L_ + n + '_kl.npz')['K']; cols = []
        for f in bi:
            k = S.klein(K[f - S.F0]); m = S.mass(k); G = G_von(k)
            im = np.zeros(k.shape + (3,), np.uint8)
            for c, col in FARBE.items(): im[k == c] = col
            im[G] = (255, 255, 0)
            H = k >= 3; ys, xs = np.nonzero(H); cy, cx = int(ys.mean()), int(xs.mean())
            c = Image.fromarray(im[max(cy - 50, 0):cy + 50, max(cx - 50, 0):cx + 50]).resize((300, 300), Image.NEAREST)
            d = ImageDraw.Draw(c); d.text((3, 3), '%s F%d %d/%d/%.1f' % (n, f, m['summe'], m['tief'], m['maxtiefe']), fill=(255, 255, 255))
            cols.append(np.asarray(c))
        rows.append(np.concatenate(cols, 1))
    Image.fromarray(np.concatenate(rows, 0)).save(sys.argv[3])
