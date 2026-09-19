# Gehoeren die nicht platzierbaren Freistellungen zu einem ANDEREN Cut/Raum?
# Kriterium: EINDEUTIGER Gipfel (wie bei den nachweislich richtig sitzenden 1x-PNGs:
# genau 1 Lage >= 95 % des Bestwerts, Gipfel 14..17 sigma).
import os, sys, glob
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
from geom import load_bg

SCHWELLE = 110
PNGS = sys.argv[1].split(",")
RAEUME = sys.argv[2].split(",")
for png in PNGS:
    im0 = Image.open(png).convert("RGBA")
    print("== %s  %dx%d" % (png, im0.width, im0.height))
    treffer = []
    for raum in RAEUME:
        rid = int(raum[4:], 16)
        for cut in range(10):
            try:
                bg = load_bg("build/bg_ppm", rid, cut)
            except Exception:
                bg = None
            if bg is None:
                continue
            bgf = bg.astype(np.float64)
            for s in ([int(v) for v in os.environ.get("SKALEN","1").split(",")]):
                im = im0 if s == 1 else im0.resize((max(1, im0.width // s), max(1, im0.height // s)), Image.BOX)
                a = np.asarray(im)
                if a.shape[0] > 240 or a.shape[1] > 320:
                    continue
                m = a[:, :, 3] > SCHWELLE
                if m.sum() < 20:
                    continue
                t = a[:, :, :3].astype(np.float64)
                h, w = m.shape
                ys, xs = np.nonzero(m)
                st = max(1, len(ys) // 400)
                ys2, xs2 = ys[::st], xs[::st]
                tv = t[ys2, xs2]
                feld = np.zeros((240 - h + 1, 320 - w + 1))
                for dy in range(240 - h + 1):
                    for dx in range(320 - w + 1):
                        feld[dy, dx] = (np.abs(bgf[ys2 + dy, xs2 + dx] - tv).max(1) < 26).mean()
                best = feld.max(); sd = feld.std(); med = np.median(feld)
                nah = int((feld >= best * 0.95).sum())
                iy, ix = np.unravel_index(feld.argmax(), feld.shape)
                treffer.append((nah, -(best - med) / (sd or 1), raum, cut, s, ix, iy, best, (best - med) / (sd or 1)))
    treffer.sort()
    for (nah, _, raum, cut, s, ix, iy, best, sig) in treffer[:6]:
        print("   %s C%d s=%d  x=%-3d y=%-3d  best %5.1f %%  Gipfel %5.1f sigma  Lagen >=95%%: %d"
              % (raum, cut, s, ix, iy, 100 * best, sig, nah))
