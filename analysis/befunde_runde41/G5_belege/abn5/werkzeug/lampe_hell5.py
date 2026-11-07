# Abnahme 5: Lampenhelligkeit x -6000 - RE2-VRAM s04 Puffer 0 gegen Port nativ (eigene Laeufe fdp_leon/fdp_elza F3573 = Vorlaufbild 23,
# Objekt 3 bei x -6000) und Port 3x (x3_leon/x3_elza F3573, 960x720, Summe/9). Fenster x 70..99 / y 115..126 wie NB6, dazu x 60..124 / y 110..134.
import numpy as np, sys
from PIL import Image
from vram5b import vram, rgb
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
a_full = rgb(vram(4)[0:240, 0:320]).astype(float).mean(axis=2)
Image.fromarray(rgb(vram(4)[0:240, 0:320])).save(B + "/out/re2_vram_s04_puffer0.png")
def port(l, f): return np.asarray(Image.open("%s/lauf/%s/fd/f%06d.ppm" % (B, l, f)).convert("RGB")).astype(float).mean(axis=2)
F = int(sys.argv[1]) if len(sys.argv) > 1 else 3573
for (x0, x1, y0, y1) in ((70, 100, 115, 127), (60, 125, 110, 135)):
    a = a_full[y0:y1, x0:x1]; ma = np.median(a)
    print('Fenster x %d..%d / y %d..%d: RE2-VRAM Median %.1f Rohsumme %.0f' % (x0, x1 - 1, y0, y1 - 1, ma, a.sum()))
    for l in ('fdp_leon', 'fdp_elza', 'x3_leon', 'x3_elza'):
        try: p = port(l, F)
        except Exception as e: print('  %s fehlt' % l); continue
        s = p.shape[0] // 240
        q = p[y0 * s:y1 * s, x0 * s:x1 * s] / (s * s)
        if s > 1: q = q.reshape(y1 - y0, s, x1 - x0, s).sum(axis=(1, 3))
        mq = np.median(q)
        zeile = '  %-9s (%dx) Median %.1f Rohsumme %.0f (%.3f)' % (l, s, mq, q.sum(), q.sum() / a.sum())
        for g in (ma, 20.0, 40.0):
            zeile += ' | ueber %.1f: %.0f/%.0f (%.2f)' % (g, np.clip(q - g, 0, None).sum(), np.clip(a - g, 0, None).sum(), np.clip(q - g, 0, None).sum() / np.clip(a - g, 0, None).sum())
        d = np.abs(q - a)
        zeile += ' | mittl. |d| %.2f, Pixel |d|>16: %d/%d' % (d.mean(), (d > 16).sum(), d.size)
        print(zeile)
