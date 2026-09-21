# -*- coding: utf-8 -*-
"""Messung 14: Trennregel Gegenstand / Boden im Fenster rechts der Deckung.

Referenzflaechen werden aus dem Bild GENOMMEN, nicht behauptet:
  BODEN  = x72..100, y150..170 (freier Flur, im Render von niemandem verdeckt)
  KISTE  = x60..66, y136..152 (der senkrechte Teil des blaugrauen Kastens)
  KANTE  = x66..75, y128..140 (der graue Bogen des Stuhls dahinter)
Ausgegeben: Verteilung von Helligkeit und b-r je Flaeche und die Trennguete.
"""
import numpy as np
from PIL import Image

BG = np.asarray(Image.open("build/bg_ppm/ROOM10D07.ppm").convert("RGB"), int)
lum = BG.sum(2)
br = BG[..., 2] - BG[..., 0]
gr = BG[..., 1] - (BG[..., 0] + BG[..., 2]) / 2.0

FL = {"BODEN x72..100 y150..170": (72, 101, 150, 171),
      "KISTE x60..66 y136..152": (60, 67, 136, 153),
      "BOGEN x66..75 y128..140": (66, 76, 128, 141),
      "KISTE-DECKEL x52..70 y127..132": (52, 71, 127, 133)}
for tag, (x0, x1, y0, y1) in FL.items():
    L = lum[y0:y1, x0:x1].ravel()
    B = br[y0:y1, x0:x1].ravel()
    G = gr[y0:y1, x0:x1].ravel()
    print("%-28s n=%4d  Helligkeit %3d/%3d/%3d (25/50/75%%)  b-r %+.1f/%+.1f/%+.1f  g-rb %+.1f"
          % (tag, L.size, np.percentile(L, 25), np.percentile(L, 50), np.percentile(L, 75),
             np.percentile(B, 25), np.percentile(B, 50), np.percentile(B, 75), np.median(G)))

# Trennguete: Schwelle auf b-r und auf Helligkeit, Boden gegen Kiste+Bogen
boden = np.concatenate([br[150:171, 72:101].ravel()])
obj = np.concatenate([br[136:153, 60:67].ravel(), br[128:141, 66:76].ravel()])
print()
print("Schwelle b-r >= t : Boden-Fehlalarm / Gegenstand-Treffer")
for t in range(0, 20, 2):
    print("  t=%2d  Boden %5.1f%%   Gegenstand %5.1f%%"
          % (t, 100.0 * (boden >= t).mean(), 100.0 * (obj >= t).mean()))
bl = lum[150:171, 72:101].ravel()
ol = np.concatenate([lum[136:153, 60:67].ravel(), lum[128:141, 66:76].ravel()])
print("Schwelle Helligkeit >= t : Boden-Fehlalarm / Gegenstand-Treffer")
for t in range(80, 220, 10):
    print("  t=%3d  Boden %5.1f%%   Gegenstand %5.1f%%"
          % (t, 100.0 * (bl >= t).mean(), 100.0 * (ol >= t).mean()))
