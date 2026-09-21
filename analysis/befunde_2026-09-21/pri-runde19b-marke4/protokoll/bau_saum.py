"""Erzeugt die Zusatz-Freistellung pri/STAGE1/10D0/07_01_saum.png aus den 27 Punkten,
die BEIDE unabhaengigen Kriterien von mess_09_saum.py erfuellen.

⛔ Das ist NICHT die Datei des Nutzers. 07_01.png bleibt bitgenau unangetastet; der Saum
ist eine eigene Datei mit eigenem Namen, und die Vereinigung beider ist die Silhouette
des Gegenstands. Herkunft: siehe Kopf von mess_09_saum.py und den grund-Text in
analysis/esp_masken_2026-09-03/auswahl.json.
"""
import numpy as np
from PIL import Image

saum = np.load("build/r19b/saum.npy")
ys, xs = np.nonzero(saum)
x0, x1, y0, y1 = int(xs.min()), int(xs.max()), int(ys.min()), int(ys.max())
w, h = x1 - x0 + 1, y1 - y0 + 1
a = np.zeros((h, w, 4), np.uint8)
sub = saum[y0:y1 + 1, x0:x1 + 1]
a[:, :, 0] = np.where(sub, 255, 0)
a[:, :, 1] = np.where(sub, 255, 0)
a[:, :, 2] = np.where(sub, 255, 0)
a[:, :, 3] = np.where(sub, 255, 0)
Image.fromarray(a, "RGBA").save("pri/STAGE1/10D0/07_01_saum.png")
print("pri/STAGE1/10D0/07_01_saum.png  %dx%d  Lage x=%d y=%d  %d opake Punkte"
      % (w, h, x0, y0, int(saum.sum())))

# Gegenprobe: zurueckgelesen muss es genau dieselbe Punktmenge sein
import sys, os
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import maske_aus_png
r = maske_aus_png.setze("pri/STAGE1/10D0/07_01_saum.png", x0, y0, 1, alpha_schwelle=110)
print("zurueckgelesen: %d Punkte, bitgleich mit der Messung: %s"
      % (int(r.sum()), bool((r == saum).all())))
