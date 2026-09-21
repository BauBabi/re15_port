# -*- coding: utf-8 -*-
"""Messung 11: Ist 07_01.png eine Farb-Heuristik oder eine Handauswahl?

Wenn die Alpha-Maske sich durch EINE Helligkeitsschwelle auf dem Hintergrund
reproduzieren laesst, ist sie das Ergebnis einer Farbregel. Nullmodell: die
Jaccard-Verteilung ueber ALLE Schwellen, nicht nur der Bestwert.
"""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
a = np.asarray(Image.open("pri/STAGE1/10D0/07_01.png").convert("RGBA"), int)
PY = 123
soll = np.zeros((240, 320), bool)
soll[PY:PY + a.shape[0], 0:a.shape[1]] = a[..., 3] >= 110

# Fenster = Bounding-Box der Freistellung
ys, xs = np.nonzero(soll)
Y0, Y1, X0, X1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
fen = np.zeros((240, 320), bool)
fen[Y0:Y1, X0:X1] = True
s = soll & fen
print("Fenster x%d..%d y%d..%d, %d Punkte, davon %d opak (%.1f%%)"
      % (X0, X1 - 1, Y0, Y1 - 1, fen.sum(), s.sum(), 100.0 * s.sum() / fen.sum()))

best = None
werte = []
for thr in range(0, 766, 5):
    p = (lum < thr) & fen                      # dunkel = Gestell
    j = (p & s).sum() / float((p | s).sum() or 1)
    werte.append(j)
    if best is None or j > best[0]:
        best = (j, thr)
print("beste Helligkeitsschwelle < %d: Jaccard %.3f" % (best[1], best[0]))
print("Nullmodell ueber alle Schwellen: Median %.3f, 90%%-Quantil %.3f, Max %.3f"
      % (np.median(werte), np.percentile(werte, 90), max(werte)))

# Vergleich: die eigene TIM/Deckung des Ports ist per Definition gleich - also
# statt dessen: wie gut reproduziert eine FLAECHE (Bbox) die Freistellung?
print("Nullmodell Bbox-Vollflaeche: Jaccard %.3f" % (s.sum() / float(fen.sum())))

# Wie sieht die Grenze aus? Helligkeit direkt innen/aussen an der rechten Kante
innen, aussen = [], []
for y in range(Y0, Y1):
    xr = np.nonzero(s[y])[0]
    if len(xr) == 0:
        continue
    x = xr.max()
    innen.append(lum[y, x])
    if x + 1 < 320 and not s[y, x + 1]:
        aussen.append(lum[y, x + 1])
print("rechte Kante: Helligkeit INNEN Median %.0f (n=%d), direkt AUSSEN Median %.0f (n=%d)"
      % (np.median(innen), len(innen), np.median(aussen), len(aussen)))
