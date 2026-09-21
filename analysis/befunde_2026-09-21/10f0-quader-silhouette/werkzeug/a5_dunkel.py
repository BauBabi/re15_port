import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image
import msk

SC = os.path.dirname(os.path.abspath(__file__)) + "/"
R = "C:/workspace/git/reAi_v2/"
img = np.asarray(Image.open(R + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C4.png').convert('RGB'), np.uint8)
bg = img[:, :640][0::2, 0::2].copy()
summe = bg.astype(int).sum(2)
dunkel = summe < 45                                    # raum.py:327 dunkel_label

lag = {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)}
lasso = np.zeros((240, 320), bool)
for n, (x0, y0) in lag.items():
    a = np.asarray(Image.open(R + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
    h, w = a.shape
    lasso[y0:y0 + h, x0:x0 + w] |= (a > 110)

masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
deck = tief < (1 << 30)

print("AUSGELIEFERT Deckung gesamt      %6d" % deck.sum())
print("NUTZER-LASSO  (Alpha>110)        %6d" % lasso.sum())
print("davon in der Deckung             %6d" % (deck & lasso).sum())
print("Deckung OHNE Lasso               %6d" % (deck & ~lasso).sum())
print("  davon Tiefschwarz (Summe<45)   %6d  = %.1f %%"
      % ((deck & ~lasso & dunkel).sum(), 100.0 * (deck & ~lasso & dunkel).sum() / max(1, (deck & ~lasso).sum())))
print("  davon HELLER als 45            %6d" % (deck & ~lasso & ~dunkel).sum())
print("Tiefschwarzflaeche im ganzen Bild%6d" % dunkel.sum())

# nur der Block x76..115 / y80..124
B = np.zeros((240, 320), bool); B[80:125, 76:116] = True
print()
print("Block x76..115 / y80..124:")
print("  Deckung                        %6d" % (deck & B).sum())
print("  davon Nutzer-Lasso             %6d" % (deck & B & lasso).sum())
print("  davon NICHT Lasso              %6d" % (deck & B & ~lasso).sum())
print("  davon NICHT Lasso, Tiefschwarz %6d" % (deck & B & ~lasso & dunkel).sum())
print("  davon NICHT Lasso, HELL        %6d" % (deck & B & ~lasso & ~dunkel).sum())

# Bild: was ist Lasso (gruen), was Tiefschwarz-Zugabe (rot), was sonst (gelb)
ov = bg.copy()
ov[deck & lasso] = [0, 255, 0]
ov[deck & ~lasso & dunkel] = [255, 0, 0]
ov[deck & ~lasso & ~dunkel] = [255, 255, 0]
Image.fromarray(ov).resize((640, 480), Image.NEAREST).save(SC + 'herkunft_c4.png')
c = ov[70:135, 66:126]
Image.fromarray(c).resize((c.shape[1] * 10, c.shape[0] * 10), Image.NEAREST).save(SC + 'herkunft_block.png')
