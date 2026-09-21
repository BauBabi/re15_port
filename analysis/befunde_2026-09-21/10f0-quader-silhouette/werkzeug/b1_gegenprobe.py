"""GEGENPROBE: stimmt das Modell der Sonde mit dem BILD des Nutzers ueberein?

Sichtbar laut Sonde  = gezeichnete Figurpunkte, die keine Maske verdeckt.
Sichtbar laut Bild   = Punkte, an denen sich der Abzug vom reinen Hintergrund
                       desselben Winkels unterscheidet.
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image
import msk

SC = os.path.dirname(os.path.abspath(__file__)) + "/"
R_ = "C:/workspace/git/reAi_v2/"
W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"

shot = np.asarray(Image.open(R_ + 're15_port/build/platform/pc/befund_10F0_F335_marke1.bmp').convert('RGB'), np.float32)
sh = shot.reshape(240, 3, 320, 3, 3).mean(axis=(1, 3))
img = np.asarray(Image.open(R_ + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C4.png').convert('RGB'), np.float32)
bg = img[:, :640][0::2, 0::2]
d = np.abs(sh - bg).sum(2)

fig = np.fromfile(W_ + "fig_marke.bin", np.int32).reshape(240, 320)
gez = fig > 0
bk = (1023 * fig.astype(np.int64)) >> 16
masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
sicht = gez & ~(tief < bk)

print("Sonde: gezeichnet %d, davon sichtbar %d" % (gez.sum(), sicht.sum()))
for s in (6, 10, 16, 24):
    b = d > s
    tp = int((b & sicht).sum()); fn = int((~b & sicht).sum()); fp = int((b & ~sicht).sum())
    print("  Bild-Schwelle %2d: Bildpunkte %5d | Treffer %4d  fehlend %4d  ueberzaehlig %4d"
          % (s, int(b.sum()), tp, fn, fp))

b = d > 10
print()
print(" Zeile | Sonde sichtbar | Bild veraendert")
for y in range(52, 122):
    a1 = int(sicht[y].sum()); a2 = int(b[y, 85:130].sum())
    if a1 or a2:
        print("  %4d | %14d | %d" % (y, a1, a2))
