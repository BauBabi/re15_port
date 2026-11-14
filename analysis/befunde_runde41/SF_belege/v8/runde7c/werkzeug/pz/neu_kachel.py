"""Kachel: R2 offline (Maler, unbeleuchtet) mit Bildpunkten von Mesh 8|9 ausserhalb R6- UND V7-Umriss (gelb) und Kopf-Umriss V7 (cyan Rand).
Bilder F278, F310, F311, F316 (Spitzen in vorn_R2.json)."""
import sys, os, numpy as np
from PIL import Image, ImageDraw
from scipy import ndimage
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pl, luecke_eigen as L, vorn as Vn
def rgb(E, r):
    m = r['bone'] >= 0; c = np.zeros(r['bone'].shape, np.int64)
    c[m] = E.clut[r['zeile'][m] - E.clut_y, E.tex[r['v'][m], r['u'][m]]]
    out = np.zeros(r['bone'].shape + (3,), np.uint8)
    out[m] = (np.stack([(c[m] & 31), (c[m] >> 5) & 31, (c[m] >> 10) & 31], -1) * 255 // 31).astype(np.uint8)
    return out
A = dict(L.ansichten()); K, R6, V7 = Vn.lade('R2'), Vn.lade('R6'), Vn.lade('V7'); cols = []
for nm in sys.argv[1].split(','):
    L.setvar(L.VAR[0]); rk = pl.raster_maler(K, pl.world_tris(K, A[nm])); r6 = pl.raster_maler(R6, pl.world_tris(R6, A[nm])); r7 = pl.raster_maler(V7, pl.world_tris(V7, A[nm]))
    f = lambda r: (r['bone'] == 8) | (r['bone'] == 9)
    neu = f(rk) & ~f(r6) & ~f(r7); im = rgb(K, rk); im[neu] = (255, 255, 0)
    H7 = r7['bone'] == 9; im[H7 & ~ndimage.binary_erosion(H7)] = (0, 255, 255)
    ys, xs = np.nonzero(H7); cy, cx = int(ys.mean()), int(xs.mean())
    c = Image.fromarray(im[cy - 45:cy + 45, cx - 55:cx + 55]).resize((440, 360), Image.NEAREST)
    ImageDraw.Draw(c).text((3, 3), 'R2 %s neu %d px' % (nm, int(neu.sum())), fill=(255, 255, 255)); cols.append(np.asarray(c))
Image.fromarray(np.concatenate(cols, 1)).save(sys.argv[2])
