"""Mass fuer Spur PF Nachbesserung 1: Scheiben und Zeiger-Instrument im Fensterbild (gdigrab, 960x720 -> logisch ueber logisch.lade).
scheiben_mass.py <bild> <vor|nach>
1. Querbalken der vier Drehscheiben: Texel auf der Seite OHNE Schatten (oben links, Abstand 0..3 von der Balkenmitte, laengs |t| <= 0,8 R): Anteil der
   Zwischentoene (Helligkeit zwischen 20 % und 80 % von Balken -> Blatt). Weiche Kante = Zwischentoene, ganze Texel = keine.
2. Teilstriche des Instruments (0..70, ohne das rote Band): Strichkontrast = (Blatt - dunkelster Texel des Strichs) / (Blatt - Tinte nach Gamma).
3. Blatt des Instruments ohne Teilstriche/Band: Zwischentoene (Helligkeit zwischen 20 % und 80 % von Zeiger -> Blatt) = weicher Zeiger."""
import sys, numpy as np, logisch
a, s, ein = logisch.lade(sys.argv[1])
L = a @ np.array([.299, .587, .114])
stand = sys.argv[2]
if stand == 'vor':
    D0, DR, winkel = (124.0, 87.0), [(125, 160), (163, 160), (200.5, 160), (237.5, 160)], 30.0
else:
    D0, DR, winkel = (124.5, 87.5), [(125.5, 160.5), (163.5, 160.5), (200.5, 160.5), (237.5, 160.5)], np.degrees(np.arctan2(1, 2))
R = 13.9
yy, xx = np.mgrid[0:240, 0:320]
X, Y = xx + .5, yy + .5
# 1. Balken
w = np.radians(winkel)
e = np.array([np.cos(w), -np.sin(w)])          # laengs (nach rechts oben)
n = np.array([-np.sin(w), -np.cos(w)])         # quer, nach oben links (Schatten liegt unten rechts)
zw_b, alle_b = 0, 0
for cx, cy in DR:
    t = (X - cx) * e[0] + (Y - cy) * e[1]
    q = (X - cx) * n[0] + (Y - cy) * n[1]
    d = np.hypot(X - cx, Y - cy)
    blatt = L[(d < R - 1) & (np.abs(q) > 4)]
    F = np.median(blatt)
    B = L[(np.abs(q) < .6) & (np.abs(t) < .8 * R)].min()
    zone = (q >= 0) & (q <= 3) & (np.abs(t) <= .8 * R)
    lo, hi = B + .2 * (F - B), B + .8 * (F - B)
    zw_b += int(((L > lo) & (L < hi) & zone).sum()); alle_b += int(zone.sum())
# 2. Teilstriche
cx, cy = D0
d = np.hypot(X - cx, Y - cy)
wk = np.arctan2(X - cx, -(Y - cy))
F = np.median(L[(d < R * .6) & (d > 4)])
tinte = 255 * (30 / 255) ** 1.16 * .9 + F * .1
kon = []
for v in range(0, 71, 10):
    ph = np.radians(-135 + 270 * v / 100)
    m = (d >= R * .66) & (d <= R * .95) & (np.abs(np.angle(np.exp(1j * (wk - ph)))) < np.radians(6))
    kon.append((F - L[m].min()) / (F - tinte))
# 3. Zeiger: Richtung = dunkelster Strahl (r 4..10), dann die Seite OHNE Schatten (Schatten liegt um (+1,+1) versetzt): Texel mit Abstand 0,3..3 von der
#    Zeigerlinie, r 3..0,66 R, ohne Nabe: Zwischentoene zwischen Zeiger und Blatt
Z = 16 * .299 + 15 * .587 + 15 * .114
best = None
for g in np.arange(-135, 135.1, 0.5):
    ph = np.radians(g)
    ls = [L[int(cy + r * -np.cos(ph)), int(cx + r * np.sin(ph))] for r in np.arange(4, 10, .5)]
    if best is None or np.mean(ls) < best[0]: best = (np.mean(ls), ph)
ph = best[1]
e2 = np.array([np.sin(ph), -np.cos(ph)]); n2 = np.array([-e2[1], e2[0]])
if n2 @ np.array([-1, -1]) < 0: n2 = -n2
q2 = (X - cx) * n2[0] + (Y - cy) * n2[1]; t2 = (X - cx) * e2[0] + (Y - cy) * e2[1]
zone = (q2 >= .3) & (q2 <= 3) & (t2 >= 3) & (t2 <= R * .66)
lo, hi = Z + .2 * (F - Z), Z + .8 * (F - Z)
zw_z = int(((L > lo) & (L < hi) & zone).sum())
zw_n = int(zone.sum())
print('%s: Bloecke einfarbig %.4f | Balken: Zwischentoene %d von %d (%.1f %%) | Teilstriche 0..70 Kontrast %s Mittel %.2f | Zeiger %.0f Grad: Zwischentoene %d von %d'
      % (sys.argv[1].split('/')[-1], ein, zw_b, alle_b, 100.0 * zw_b / alle_b, ' '.join('%.2f' % k for k in kon), np.mean(kon), np.degrees(ph), zw_z, zw_n))
