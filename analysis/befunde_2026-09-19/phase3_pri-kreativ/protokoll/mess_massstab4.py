# Warum treffen die Massstab-4-Freistellungen nur 85..93 %?
# Hypothese: das PNG ist ein 4x-NEAREST-Bildschirmfoto, aber der Zuschnitt liegt nicht auf
# einem Vielfachen von 4 -> BOX-Verkleinerung mittelt ueber ZWEI Quellpunkte.
# Messung: alle 16 Phasen (Vorschnitt 0..3 in x und y) und drei Verkleinerungsarten.
import os, sys
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
from geom import load_bg

FAELLE = [
    ("ROOM1000", 1, "pri/STAGE1/1000/01_01.png", 188, 54),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_02.png", 181, 207),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_03.png", 179, 151),
    ("ROOM1000", 1, "pri/STAGE1/1000/01_04.png", 296, 130),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_01.png", 139, 91),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_02.png", 108, 108),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_03.png", 107, 107),
    ("ROOM1000", 3, "pri/STAGE1/1000/03_04.png", 163, 106),
    ("ROOM11F0", 1, "pri/STAGE1/11F0/01.png", 99, 161),
]
SCHWELLE = 110


def treffer(bg, rgb, m, x, y):
    """Anteil der Maskenpunkte, deren Farbe zum Hintergrund passt (|dRGB| < 26)."""
    h, w = m.shape
    x0, y0 = max(0, x), max(0, y)
    x1, y1 = min(320, x + w), min(240, y + h)
    if x1 <= x0 or y1 <= y0:
        return 0.0, 0
    mm = m[y0 - y:y1 - y, x0 - x:x1 - x]
    tt = rgb[y0 - y:y1 - y, x0 - x:x1 - x]
    bb = bg[y0:y1, x0:x1].astype(np.float64)
    d = np.abs(bb - tt.astype(np.float64)).max(2)
    n = int(mm.sum())
    return (float((d[mm] < 26).mean()) if n else 0.0), n


for (room, cut, png, X, Y) in FAELLE:
    rid = int(room[4:], 16)
    bg = load_bg("build/bg_ppm", rid, cut)
    im = Image.open(png).convert("RGBA")
    a0 = np.asarray(im)
    voll = int((a0[:, :, 3] > SCHWELLE).sum())
    print("%s C%d %s  %dx%d, %d Alphapunkte -> erwartet %d bei 1/16"
          % (room, cut, os.path.basename(png), im.width, im.height, voll, round(voll / 16.0)))
    beste = []
    for py in range(4):
        for px in range(4):
            k = im.crop((px, py, im.width, im.height))
            kw, kh = k.width // 4, k.height // 4
            if kw < 1 or kh < 1:
                continue
            # (a) BOX wie bisher
            b = np.asarray(k.resize((kw, kh), Image.BOX))
            mb = b[:, :, 3] > SCHWELLE
            qb, nb = treffer(bg, b[:, :, :3], mb, X, Y)
            # (b) NEAREST-Abtastung der Blockmitte
            n_ = np.asarray(k)[2::4, 2::4][:kh, :kw]
            mn = n_[:, :, 3] > SCHWELLE
            qn, nn = treffer(bg, n_[:, :, :3], mn, X, Y)
            beste.append((qb, "BOX", px, py, nb))
            beste.append((qn, "MITTE", px, py, nn))
    beste.sort(reverse=True)
    for (q, art, px, py, n) in beste[:3]:
        print("      %-5s Phase (%d,%d): %5.1f %%  (%d Punkte)" % (art, px, py, 100 * q, n))
    b0 = [b for b in beste if b[1] == "BOX" and b[2] == 0 and b[3] == 0][0]
    print("      BISHER (BOX, Phase 0,0):        %5.1f %%  (%d Punkte)" % (100 * b0[0], b0[4]))
