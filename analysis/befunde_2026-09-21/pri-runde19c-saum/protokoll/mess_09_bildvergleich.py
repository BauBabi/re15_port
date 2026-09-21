"""VORHER gegen NACHHER am echten Abzug (Vollbild-Readback vor SDL_RenderPresent,
960x720 = 3x 320x240). Zwei Fragen:
  1. Sind die beiden Laeufe sonst bildgleich (gleiche Pose, gleiche Beleuchtung)? Sonst
     waere jede Differenz nicht dem Saum zuzuschreiben.
  2. WO unterscheiden sie sich — und liegt das im Saum des Nutzers?
Erzeugt ausserdem die Ausschnitte, die ich mir selbst ansehe.
"""
import os
import sys

import numpy as np
from PIL import Image

V = "build/r19c/abzug_vorher/bild000260.png"
N = "build/r19c/abzug_nachher/bild000260.png"
ZIEL = "analysis/befunde_2026-09-21/pri-runde19c-saum"
v = np.asarray(Image.open(V).convert("RGB")).astype(int)
n = np.asarray(Image.open(N).convert("RGB")).astype(int)
print("Abzuege %s / %s, je %dx%d" % (os.path.basename(V), os.path.basename(N), v.shape[1], v.shape[0]))
d = np.abs(v - n).sum(2)
print("unterschiedliche Bildpunkte (960x720): %d von %d (%.3f %%), groesste Abweichung %d"
      % (int((d > 0).sum()), d.size, 100.0 * (d > 0).mean(), int(d.max())))
ys, xs = np.nonzero(d > 0)
if len(xs):
    print("Rechteck der Unterschiede: x%d..%d y%d..%d  (in 320x240: x%d..%d y%d..%d)"
          % (xs.min(), xs.max(), ys.min(), ys.max(),
             xs.min() // 3, xs.max() // 3, ys.min() // 3, ys.max() // 3))
# Liegen die Unterschiede im Saum bzw. in der Silhouette?
saum = np.load("build/r19c/neu.npy") & ~np.load("build/r19c/alt.npy")
alt = np.load("build/r19c/alt.npy")
saum3 = np.kron(saum, np.ones((3, 3), bool))
alt3 = np.kron(alt, np.ones((3, 3), bool))
m = d > 0
print("davon im SAUM des Nutzers: %d ; in seiner alten Freistellung: %d ; ausserhalb beider: %d"
      % (int((m & saum3).sum()), int((m & alt3).sum()), int((m & ~saum3 & ~alt3).sum())))
# Figur-Kasten der Marke: x59..95 y118..182 (befund.log F3843) -> x3 y3
kx0, kx1, ky0, ky1 = 59 * 3, 96 * 3, 118 * 3, 183 * 3
print("im Koerperkasten der Marke (x59..95 y118..182): %d Punkte" % int(m[ky0:ky1, kx0:kx1].sum()))

# Ausschnitte zum Selbst-Ansehen: das Stuhlfenster mit dem Spieler, 3-fach vergroessert
x0, x1, y0, y1 = 0, 110, 118, 240          # 320x240-Koordinaten
def schnitt(a, faktor=3):
    s = a[y0 * 3:y1 * 3, x0 * 3:x1 * 3]
    return Image.fromarray(s.astype(np.uint8)).resize(
        ((x1 - x0) * 3 * faktor, (y1 - y0) * 3 * faktor), Image.NEAREST)


breit = (x1 - x0) * 3 * 3
hoch = (y1 - y0) * 3 * 3
blatt = Image.new("RGB", (breit * 2 + 12, hoch), (24, 24, 24))
blatt.paste(schnitt(v), (0, 0))
blatt.paste(schnitt(n), (breit + 12, 0))
blatt.save(os.path.join(ZIEL, "50_vorher_nachher_stuhl.png"))
print("geschrieben: %s (links VORHER, rechts NACHHER, 9-fach)" % os.path.join(ZIEL, "50_vorher_nachher_stuhl.png"))

# Differenzbild: was hat der Saum im Bild geaendert, rot markiert auf dem NACHHER-Bild
mark = n.copy()
mark[m] = [255, 0, 0]
Image.fromarray(mark[y0 * 3:y1 * 3, x0 * 3:x1 * 3].astype(np.uint8)).resize(
    ((x1 - x0) * 9, (y1 - y0) * 9), Image.NEAREST).save(os.path.join(ZIEL, "51_differenz_rot.png"))
print("geschrieben: %s (die geaenderten Punkte rot)" % os.path.join(ZIEL, "51_differenz_rot.png"))
