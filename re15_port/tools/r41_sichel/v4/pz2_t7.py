"""Zahlen-Pruefer V4 r2: T7 unabhaengig vom b2-Umhaengen - Form der Kette im Lauf (debug.log) gegen die Ruhe-Form der Gelenkliste
SCHWANZ_V4 gelenke (Bild der Vorlage-2-Kamera, H): Hoehenabfall Biegung (Gelenk 3) -> Knick (Gelenk 5) und -> Ende (Punkt 7) in H.
H in Welt-E. aus den Gliedern 20-22 (ohne Tiefenversatz, Bildebene): Summe Gliedlaengen / Summe der Bildlaengen der Gelenkliste."""
import sys, os, numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz2_lauf import K, B
g = np.array([(0.12, -0.080), (-0.037, -0.040), (-0.135, 0.003), (-0.245, 0.035), (-0.256, 0.165), (-0.259, 0.280), (-0.238, 0.337), (-0.218, 0.393)])
bild = np.linalg.norm(np.diff(g[3:], axis=0), axis=1).sum()
welt = np.mean([np.linalg.norm(np.diff(K[h][3:], axis=0), axis=1).sum() for h in K])
HE = welt / bild
print('H in Welt-E. ~ %.0f (Glieder 3-7 Welt %.0f / Bild %.4f H)' % (HE, welt, bild))
soll35, soll37 = g[5][1] - g[3][1], g[7][1] - g[3][1]
print('Ruhe: Abfall Biegung->Knick %.3f H, Biegung->Ende %.3f H' % (soll35, soll37))
ok = []
for h in sorted(K):
    if h < 85: continue
    P = K[h]
    a35 = (P[5][1] - P[3][1]) / HE; a37 = (P[7][1] - P[3][1]) / HE
    abw = max(abs(a35 - soll35), abs(a37 - soll37))
    ok.append((h, abw))
    if h in (85, 90, 95, 100, 105, 110, 115, 118, 120, 125, 130, 135):
        print('h%3d clip %s: Abfall Biegung->Knick %.3f H, ->Ende %.3f H; Abweichung max %.3f H %s' % (h, B[h]['clip'], a35, a37, abw, 'OK' if abw <= 0.06 else 'ROT'))
gang = [o for o in ok if B[o[0]]['clip'] == 1]
print('Gangtakte (clip 1) mit Abweichung > 0,06 H: %d von %d' % (sum(1 for o in gang if o[1] > 0.06), len(gang)))
print('alle Takte h85-h135 > 0,06 H: %d von %d' % (sum(1 for o in ok if o[1] > 0.06), len(ok)))
