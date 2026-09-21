"""Was genau ist an Marke F3843 im Bild verloren — und gehoert es zum Stuhl (neutral)
oder zum blauen Gegenstand dahinter (b-r >= +10)?"""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
rn = np.load("build/r19b/render_F3843.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); deck = np.load("build/r19b/deck.npy")
kand = np.load("build/r19b/chrom_fehl.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]
d = np.abs(rn - bg).sum(2)
ueber = d > 30

print("=== Marke F3843, Fenster des Stuhls x0..75 y118..239 ===")
fen = np.zeros_like(reg); fen[118:240, 0:76] = True
print("  uebermalte Punkte im Fenster: %d (davon unter deckender Maske %d = Renderer-Leck)"
      % (int((ueber & fen).sum()), int((ueber & fen & deck).sum())))
u = ueber & fen & ~deck
for schw in (150, 200, 250):
    m = u & (L >= schw)
    if m.any():
        neu = m & (np.abs(BR) < 8); bl = m & (BR >= 10)
        print("  uebermalt+ungedeckt mit Helligkeit>=%3d: %3d   davon NEUTRAL (Chrom) %3d, BLAU %3d"
              % (schw, int(m.sum()), int(neu.sum()), int(bl.sum())))

print("\n=== Die im Auftrag genannte 'obere Rahmenschiene' y129..133 x17..58 ===")
m = np.zeros_like(reg); m[129:134, 17:59] = True
for name, sel in (("alle Punkte", m), ("ungedeckt", m & ~deck),
                  ("ungedeckt und Hell>=150", m & ~deck & (L >= 150))):
    if sel.any():
        print("  %-24s n=%4d  b-r Quartile %+d / %+d / %+d   Hell Median %d"
              % (name, int(sel.sum()), np.percentile(BR[sel], 25), np.median(BR[sel]),
                 np.percentile(BR[sel], 75), int(np.median(L[sel]))))
s = m & ~deck & (L >= 150)
print("  -> NEUTRAL (|b-r|<8): %d ; BLAU (b-r>=10): %d" % (int((s & (np.abs(BR) < 8)).sum()),
                                                            int((s & (BR >= 10)).sum())))

print("\n=== Koerperkasten der Marke (befund.log: x59..95 y118..182) ===")
box = np.zeros_like(reg); box[118:183, 59:96] = True
print("  Chrom-Zusatzpunkte im Kasten: %d" % int((kand & box).sum()))
print("  uebermalte ungedeckte Punkte im Kasten: %d, davon Hell>=200: %d (neutral %d, blau %d)"
      % (int((ueber & box & ~deck).sum()), int((ueber & box & ~deck & (L >= 200)).sum()),
         int((ueber & box & ~deck & (L >= 200) & (np.abs(BR) < 8)).sum()),
         int((ueber & box & ~deck & (L >= 200) & (BR >= 10)).sum())))
ys, xs = np.nonzero(kand)
print("\n=== alle 66 Chrom-Zusatzpunkte, Zeile fuer Zeile ===")
for y in sorted(set(ys)):
    c = np.nonzero(kand[y])[0]
    print("  y%3d  x %s" % (y, " ".join("%d" % v for v in c)))
