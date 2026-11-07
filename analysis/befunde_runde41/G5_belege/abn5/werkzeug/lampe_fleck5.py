# Abnahme 5: Lampenfleck im Tuerloch (Grauwert > 120, x 30..150, y 95..150, 320x240) je Szenenbild b: RE2 echt (Videobild 10 + 2(58+b))
# gegen Port Leon/Elza (F3605 + b). Schwerpunkt und Pixelzahl; Vergleich nur, wo beide einen Fleck haben.
import numpy as np, sys
from PIL import Image
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
v = np.load(B + "/re2/re2_6130_rgb.npy").astype(np.int16).mean(axis=3)
def port(l, f): return np.asarray(Image.open(B + "/lauf/%s/fd/f%06d.ppm" % (l, f)).convert("L")).astype(np.int16)
def fleck(g, s=120):
    a = g[95:150, 30:150]; m = a > s
    n = int(m.sum())
    if n < 2: return None
    ys, xs = np.nonzero(m)
    return (xs.mean() + 30, ys.mean() + 95, n)
beide = nur_re2 = nur_port = keiner = 0; dxs = []; dys = []; zeilen = []
for b in range(-54, 111):
    vi = 10 + 2 * (58 + b)
    if vi >= len(v): break
    r = fleck(v[vi]); pl = fleck(port('fdp_leon', 3605 + b)); pe = fleck(port('fdp_elza', 3605 + b))
    if r and pl:
        beide += 1; dxs.append(abs(r[0] - pl[0])); dys.append(abs(r[1] - pl[1]))
    elif r and not pl: nur_re2 += 1
    elif pl and not r: nur_port += 1
    else: keiner += 1
    zeilen.append('b %4d RE2 %-22s Leon %-22s Elza %-22s' % (b, r and '(%.1f,%.1f,%d)' % r, pl and '(%.1f,%.1f,%d)' % pl, pe and '(%.1f,%.1f,%d)' % pe))
print('\n'.join(zeilen))
print('Bilder mit Fleck in RE2 und Port %d (|dx| Median %.1f, |dy| Median %.1f, 90%% %.1f/%.1f), nur RE2 %d, nur Port %d, keiner %d' % (
    beide, np.median(dxs), np.median(dys), np.percentile(dxs, 90), np.percentile(dys, 90), nur_re2, nur_port, keiner))
