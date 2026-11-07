# Abnahme 5: Truemmer/Staub-Signal je Bild in den zwei Fallsaeulen (RE2-Aenderungskarte): Zahl der Pixel mit |Bild - Bild(k-1)| > 14
# (Graustufe 320x240), RE2 echt (Videobild 10 + 2k) gegen Port Leon/Elza (F3551 + k). Saeule A x 88..132 y 14..100, B x 200..255 y 14..205.
import numpy as np
from PIL import Image
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
v = np.load(B + "/re2/re2_6130_rgb.npy").astype(np.int16).mean(axis=3)
def port(l, f): return np.asarray(Image.open(B + "/lauf/%s/fd/f%06d.ppm" % (l, f)).convert("L")).astype(np.int16)
S = {'A': (88, 132, 14, 100), 'B': (200, 255, 14, 205)}
def sig(a, b, s):
    x0, x1, y0, y1 = S[s]
    return int((np.abs(a[y0:y1, x0:x1] - b[y0:y1, x0:x1]) > 14).sum())
rows = []
print('  k | RE2 A  B | Leon A  B | Elza A  B')
for k in range(15, 61):
    r = [sig(v[10 + 2 * k], v[10 + 2 * (k - 1)], s) for s in 'AB']
    pl = [sig(port('fdp_leon', 3551 + k), port('fdp_leon', 3550 + k), s) for s in 'AB']
    pe = [sig(port('fdp_elza', 3551 + k), port('fdp_elza', 3550 + k), s) for s in 'AB']
    rows.append((k, r, pl, pe))
    print('%3d | %4d %4d | %4d %4d | %4d %4d' % (k, r[0], r[1], pl[0], pl[1], pe[0], pe[1]))
import numpy as np
for s, i in (('A', 0), ('B', 1)):
    R = np.array([x[1][i] for x in rows], float); L = np.array([x[2][i] for x in rows], float); E = np.array([x[3][i] for x in rows], float)
    print('Saeule %s: Summe RE2 %d Leon %d Elza %d; Korrelation RE2-Leon %.2f, RE2-Elza %.2f; letztes Bild mit Signal >5: RE2 %s Leon %s Elza %s' % (
        s, R.sum(), L.sum(), E.sum(), np.corrcoef(R, L)[0, 1], np.corrcoef(R, E)[0, 1],
        max([x[0] for x in rows if x[1][i] > 5] or [None]), max([x[0] for x in rows if x[2][i] > 5] or [None]), max([x[0] for x in rows if x[3][i] > 5] or [None])))
