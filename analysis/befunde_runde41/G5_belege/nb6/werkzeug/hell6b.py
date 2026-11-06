# NB6-Abschluss: Lampenhelligkeit mit GEMEINSAMER Grundlinie. hell6.py zieht je Bild den eigenen Median ab; der Hintergrund im
# Fenster x 70..100 / y 115..127 ist im Port aber 3,4 Stufen heller (Median 16,7 gegen 13,3), das druckt die Port-Summe.
# Hier: Rohsumme, Summe ueber der RE2-Grundlinie fuer beide, Beitrag je Spalte. Lampe x -6000, RE2-VRAM s04 gegen Port nativ F1581.
import numpy as np, sys
from PIL import Image
sys.path.insert(0, ".")
from vram5 import vram, rgb
L = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g5nb6/lauf"
a = rgb(vram(4)[0:240, 0:320]).astype(float).mean(axis=2)[115:127, 70:100]
p = np.asarray(Image.open(L + "/nativ/fd/f001581.ppm").convert("RGB")).astype(float).mean(axis=2)[115:127, 70:100]
ma, mp = np.median(a), np.median(p)
print("Fenster x 70..99 / y 115..126 (360 Pixel)")
print("  Median: RE2 %.1f  Port %.1f" % (ma, mp))
print("  Rohsumme: RE2 %.0f  Port %.0f  Port/RE2 %.3f" % (a.sum(), p.sum(), p.sum() / a.sum()))
print("  Summe ueber eigenem Median (hell6.py): RE2 %.0f  Port %.0f  %.2f" % (np.clip(a - ma, 0, None).sum(), np.clip(p - mp, 0, None).sum(),
      np.clip(p - mp, 0, None).sum() / np.clip(a - ma, 0, None).sum()))
for g in (ma, 20.0, 40.0):
    sa, sp = np.clip(a - g, 0, None).sum(), np.clip(p - g, 0, None).sum()
    print("  Summe ueber gemeinsamer Grundlinie %.1f: RE2 %.0f  Port %.0f  %.2f" % (g, sa, sp, sp / sa))
d = np.clip(a - ma, 0, None) - np.clip(p - mp, 0, None)
print("  Fehlbetrag (eigener Median) je Spalte x 70..99:", " ".join("%d" % x for x in d.sum(axis=0)))
print("  Spalte x 88, Zeilen 115..126 RE2:", " ".join("%d" % x for x in a[:, 18]), "| Port:", " ".join("%d" % x for x in p[:, 18]))
# 3x-Fenster (RE15_FRAMEDUMP 960x720, Leon/Elza), Summe / 9 gegen dieselben Grundlinien
for lauf in ("fd_leon", "fd_elza"):
    q = np.asarray(Image.open(L + "/%s/fd/f001581.ppm" % lauf).convert("RGB")).astype(float).mean(axis=2)[345:381, 210:300]
    mq = np.median(q)
    print("3x-Fenster %s: Median %.1f, Rohsumme/9 %.0f (%.3f)" % (lauf, mq, q.sum() / 9, q.sum() / 9 / a.sum()), end="")
    for g in (ma, 20.0, 40.0):
        print(" | ueber %.1f: %.0f (%.2f)" % (g, np.clip(q - g, 0, None).sum() / 9, np.clip(q - g, 0, None).sum() / 9 / np.clip(a - g, 0, None).sum()), end="")
    print()
