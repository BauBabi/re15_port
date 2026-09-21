"""Nachpruefung der ANGABEN zur gruenen Markierung des Nutzers — jede Zahl selbst gemessen,
keine uebernommen: Punktzahl, Lage im Fenster, Nachbarschaft/Abstand zur Silhouette,
Farbklasse (b-r), Herkunft der Farbe im Saum-PNG, und ob die 12 offenen Punkte aus
pri-runde19b-marke4.md §3.3 darin enthalten sind.
"""
import os
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import maske_aus_png                                            # noqa: E402

GRUEN = (34, 177, 76)
NUTZER = "analysis/befunde_2026-09-21/pri-runde19c-saum/nutzer_gruen_2026-09-21.png"
ALT = "pri/STAGE1/10D0/07_01.png"
NEU = "pri/STAGE1/10D0/07_01_mit_saum.png"
OX, OY = 0, 123

a = np.asarray(Image.open(NUTZER).convert("RGB")).astype(int)
print("Nutzer-Bild %dx%d" % (a.shape[1], a.shape[0]))
m4 = (a[..., 0] == GRUEN[0]) & (a[..., 1] == GRUEN[1]) & (a[..., 2] == GRUEN[2])
print("gruene Quellpunkte (genau %s): %d" % (GRUEN, m4.sum()))
for th in (10, 20, 30, 50, 80, 100):
    print("   Schwelle g>r+%-3d und g>b+%-3d: %d" % (th, th, int(((a[..., 1] > a[..., 0] + th) &
                                                                  (a[..., 1] > a[..., 2] + th)).sum())))
gruen = m4.reshape(240, 4, 320, 4).any(axis=(1, 3))
print("Zielpunkte 320x240 (4x4, 'einer genuegt'): %d" % int(gruen.sum()))

alt = maske_aus_png.setze(ALT, OX, OY, 1, alpha_schwelle=110)
neu = maske_aus_png.setze(NEU, OX, OY, 1, alpha_schwelle=110)
print("Silhouette alt %d / neu %d Punkte (ueber den ECHTEN Leser maske_aus_png.setze)"
      % (int(alt.sum()), int(neu.sum())))
print("gruen schon gedeckt %d, NEU %d" % (int((gruen & alt).sum()), int((gruen & ~alt).sum())))
print("neu == alt | gruen?  %s   (Differenz %d / %d)"
      % (bool(np.array_equal(neu, alt | gruen)), int((neu & ~(alt | gruen)).sum()),
         int(((alt | gruen) & ~neu).sum())))
fen = np.zeros((240, 320), bool); fen[OY:OY + 117, OX:OX + 73] = True
print("gruene Punkte AUSSERHALB des 73x117-Fensters bei (%d,%d): %d" % (OX, OY, int((gruen & ~fen).sum())))

# Nachbarschaft / Abstand zur alten Silhouette
from scipy import ndimage as nd
rand = nd.binary_dilation(alt, np.ones((3, 3), bool)) & ~alt
sn = gruen & ~alt
print("davon direkte 8er-Nachbarn der alten Silhouette: %d von %d" % (int((sn & rand).sum()), int(sn.sum())))
dist = nd.distance_transform_edt(~alt)
d = dist[sn]
print("Abstand zur alten Form: Median %.1f, 90-%%-Quantil %.1f, Maximum %.1f"
      % (np.median(d), np.percentile(d, 90), d.max()))

# Farbklasse b-r am Hintergrund
bg = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB")).astype(int)
br = bg[..., 2] - bg[..., 0]
print("b-r der 187 neuen Punkte:      Quartile %+d / %+d / %+d"
      % tuple(int(x) for x in np.percentile(br[sn], (25, 50, 75))))
print("b-r INNERHALB der Freistellung: Quartile %+d / %+d / %+d"
      % tuple(int(x) for x in np.percentile(br[alt], (25, 50, 75))))
beh = np.zeros((240, 320), bool); beh[118:135, 41:65] = True      # Behaelterrand, §3.2 des 19b-Dossiers
hell = (bg.sum(2) >= 150)
sel = beh & hell & ~alt & ~sn
print("b-r des hellen BEHAELTERS (x41..64 y118..134, hell, ungedeckt): Quartile %+d / %+d / %+d (%d Punkte)"
      % (tuple(int(x) for x in np.percentile(br[sel], (25, 50, 75))) + (int(sel.sum()),)))

# Farbe des Saum-PNG == Hintergrund?
pn = np.asarray(Image.open(NEU).convert("RGBA")).astype(int)
gl = ab = 0
for (y, x) in zip(*np.nonzero(sn)):
    if tuple(pn[y - OY, x - OX, :3]) == tuple(bg[y, x]):
        gl += 1
    else:
        ab += 1
print("Saum-PNG: Farbe der neuen Punkte == Hintergrundbild an %d von %d (abweichend %d)" % (gl, gl + ab, ab))
alp = np.unique(pn[:, :, 3][np.nonzero(np.asarray(Image.open(NEU).convert("RGBA"))[:, :, 3] > 0)])
print("Alphawerte im Saum-PNG (nur > 0): %s" % alp)
# Die 2677 Punkte des Nutzers unveraendert?
po = np.asarray(Image.open(ALT).convert("RGBA")).astype(int)
gl = int((po[:, :, 3] > 110).sum())
same = sum(1 for (y, x) in zip(*np.nonzero(po[:, :, 3] > 110)) if tuple(pn[y, x]) == tuple(po[y, x]))
print("die %d Punkte des Nutzers im Saum-PNG bitgleich: %d" % (gl, same))

# Die 12 offenen Punkte aus pri-runde19b-marke4.md §3.3
OFFEN = [(61, 142), (61, 143), (61, 144), (61, 145), (60, 148), (60, 149), (60, 150), (60, 151),
         (55, 132), (56, 132), (57, 132), (57, 133)]
drin = [p for p in OFFEN if gruen[p[1], p[0]]]
print("die 12 offenen Punkte aus 19b §3.3 in der Markierung: %d von 12  %s"
      % (len(drin), "" if len(drin) == 12 else "FEHLEN: %s" % [p for p in OFFEN if p not in drin]))
np.save("build/r19c/gruen.npy", gruen)
np.save("build/r19c/alt.npy", alt)
np.save("build/r19c/neu.npy", neu)
