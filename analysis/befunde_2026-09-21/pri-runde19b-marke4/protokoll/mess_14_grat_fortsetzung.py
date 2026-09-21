"""Der SAUM, zweiter Versuch — ohne Farbschwelle, weil die Farbe NICHT trennt.

GEMESSEN (mess_14, Tabelle x54..68 / y128..155): der helle Pfosten, den die
Nutzer-Freistellung in den Zeilen 134..141 selbst enthaelt, traegt dort b-r +5..+18.
Die Fortsetzung desselben Pfostens in den Zeilen 142..151 traegt b-r +3..+22. Eine
b-r-Schwelle, die die Fortsetzung verwirft, verwirft also auch die Punkte, die der
Nutzer selbst genommen hat. Farbe ist hier kein Kriterium.

Was trennt: der Pfosten ist ein zusammenhaengender heller GRAT, und die Freistellung
hoert mitten darin auf. Regel:
  Grat      = White-Top-Hat (grey_opening SE) >= SCHWELLE  — duenne helle Struktur.
  Keim      = Gratpunkte INNERHALB der Freistellung.
  Saum      = Gratpunkte AUSSERHALB, die mit einem Keim 8-zusammenhaengend sind,
              und deren Helligkeit nicht unter das 10-%-Quantil der Keime faellt.
Der Saum kann also nur dort entstehen, wo der Nutzer denselben Grat schon selbst
genommen hat. Ein neuer Gegenstand kann so nicht entstehen.
"""
import numpy as np
from scipy import ndimage as nd

bg = np.load("build/r19b/bg10d07.npy").astype(np.int32)
reg = np.load("build/r19b/reg0701.npy"); deck = np.load("build/r19b/deck.npy")
L = bg.sum(2); BR = bg[:, :, 2] - bg[:, :, 0]

print("=== Belegt: Farbe trennt hier nicht ===")
pf_in = [(61, 138), (62, 138), (61, 139), (62, 139), (61, 140), (62, 140), (61, 141), (62, 141),
         (59, 134), (60, 135), (60, 136), (61, 136), (61, 137)]
pf_out = [(61, 142), (62, 142), (61, 143), (62, 143), (61, 144), (61, 145), (61, 146),
          (61, 147), (60, 148), (61, 148), (60, 149), (60, 150), (60, 151)]
print("  Pfostenpunkte IN der Freistellung   : b-r %s" % [int(BR[y, x]) for x, y in pf_in])
print("  Pfostenpunkte AUSSERHALB (Fortsetz.): b-r %s" % [int(BR[y, x]) for x, y in pf_out])
print("  Helligkeit innen %s" % [int(L[y, x]) for x, y in pf_in])
print("  Helligkeit aussen %s" % [int(L[y, x]) for x, y in pf_out])

fen = np.zeros_like(reg); fen[118:239, 0:76] = True
for SE in (5, 7):
    for SCHW in (30, 45, 60):
        th = L - nd.grey_opening(L, size=(SE, SE))
        grat = (th >= SCHW) & fen
        keim = grat & reg
        if not keim.any():
            continue
        q10 = np.percentile(L[keim], 10)
        lab, n = nd.label(grat, np.ones((3, 3), bool))
        gute = set(np.unique(lab[keim])) - {0}
        saum = grat & ~reg & ~deck & np.isin(lab, list(gute)) & (L >= q10)
        print("SE=%d Schwelle=%d: Grat %4d, Keim %4d (q10 Hell %3d), SAUM %3d Punkte in %d Komponenten"
              % (SE, SCHW, int(grat.sum()), int(keim.sum()), q10, int(saum.sum()),
                 len(set(np.unique(nd.label(saum, np.ones((3, 3), bool))[0])) - {0})))
        if (SE, SCHW) == (5, 45):
            np.save("build/r19b/saum2.npy", saum)

s = np.load("build/r19b/saum2.npy")
print("\n=== gewaehlt SE=5, Schwelle=45 (s.u. Stabilitaet): %d Punkte ===" % int(s.sum()))
lab, n = nd.label(s, np.ones((3, 3), bool))
for i in range(1, n + 1):
    m = lab == i; yy, xx = np.nonzero(m)
    print("   %2d Punkte  x%2d..%2d y%3d..%3d  Hell %3d..%3d  b-r %+d..%+d"
          % (int(m.sum()), xx.min(), xx.max(), yy.min(), yy.max(),
             L[m].min(), L[m].max(), BR[m].min(), BR[m].max()))
