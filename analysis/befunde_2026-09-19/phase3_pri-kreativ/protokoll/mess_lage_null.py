# Ist die gefundene Lage ein ECHTER Gipfel oder nur der Bestwert eines weichen Feldes?
# Gegen ein Nullmodell: Verteilung der Uebereinstimmung ueber ALLE Lagen.
# (Die reine Bestwert-Zahl waechst mit kleinerem Objekt und taugt deshalb nicht zum
#  Vergleich der Massstaebe — selbstbestaetigende Metrik.)
import os, sys, json
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
from geom import load_bg

SCHWELLE = 110
FAELLE = [
    ("ROOM1000", 3, "pri/STAGE1/1000/03_01.png"),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_02.png"),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_03.png"),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_04.png"),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_01.png"),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_02.png"),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_03.png"),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_04.png"),
    ("ROOM11F0", 1, "pri/STAGE1/11F0/01.png"),
    # Gegenproben: zwei Freistellungen, die als 1x eingetragen sind und 100 % treffen
    ("ROOM11F0", 6, "pri/STAGE1/11F0/06_02.png"),
    ("ROOM10E0", 7, "pri/STAGE1/10E0/07_01.png"),
]
for (room, cut, png) in FAELLE:
    rid = int(room[4:], 16)
    bg = load_bg("build/bg_ppm", rid, cut).astype(np.float64)
    im0 = Image.open(png).convert("RGBA")
    print("%s C%d %-12s %dx%d" % (room, cut, os.path.basename(png), im0.width, im0.height))
    for s in (1, 2, 3, 4):
        im = im0 if s == 1 else im0.resize((max(1, im0.width // s), max(1, im0.height // s)), Image.BOX)
        a = np.asarray(im)
        if a.shape[0] > 240 or a.shape[1] > 320:
            print("      s=%d: passt nicht ins Bild" % s); continue
        m = a[:, :, 3] > SCHWELLE
        if m.sum() < 20:
            continue
        t = a[:, :, :3].astype(np.float64)
        h, w = m.shape
        ys, xs = np.nonzero(m)
        schritt = max(1, len(ys) // 1500)
        ys2, xs2 = ys[::schritt], xs[::schritt]
        tv = t[ys2, xs2]
        feld = np.zeros((240 - h + 1, 320 - w + 1))
        for dy in range(240 - h + 1):
            for dx in range(320 - w + 1):
                d = np.abs(bg[ys2 + dy, xs2 + dx] - tv).max(1)
                feld[dy, dx] = (d < 26).mean()
        best = feld.max()
        med = np.median(feld)
        sd = feld.std()
        iy, ix = np.unravel_index(feld.argmax(), feld.shape)
        # Wie viele Lagen kommen dem Bestwert nahe (>= 95 % davon)?
        nah = int((feld >= best * 0.95).sum())
        print("      s=%d: x=%-3d y=%-3d  best %5.1f %%  median %5.1f %%  sigma %4.1f  "
              "Gipfelhoehe %5.1f sigma  Lagen >=95%% des Bestwerts: %d von %d"
              % (s, ix, iy, 100 * best, 100 * med, 100 * sd,
                 (best - med) / sd if sd > 0 else 0, nah, feld.size))
