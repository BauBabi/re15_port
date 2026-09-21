"""NAHT-PROBE: darf ein starrer Gegenstand an seiner eigenen Naht in der Tiefe springen?
Und: wie weit traegt die 'Fussabdruck-Schranke' wirklich (ist sie unabhaengig oder
selbstbestaetigend)?"""
import os, sys, struct
import numpy as np
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom, geometrie

rdt, _ = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)
reg = np.load("build/r19b/reg0701.npy")
tief = np.load("build/r19b/tief.npy")
B = np.load("build/r19b/depB.npy")       # Spaltenregel
C = np.load("build/r19b/depC.npy")       # Spaltenregel + bodenkante


def naht(D, name):
    mx = 0; n1 = 0; paare = 0; wo = None
    for dy, dx in ((0, 1), (1, 0), (1, 1), (1, -1)):
        a = reg.copy()
        b = np.roll(np.roll(reg, dy, 0), dx, 1)
        m = a & b
        if dy: m[:dy] = False
        ys, xs = np.nonzero(m)
        for y, x in zip(ys, xs):
            y2, x2 = y - dy, x - dx
            if not (0 <= y2 < 240 and 0 <= x2 < 320):
                continue
            paare += 1
            d = abs(int(D[y, x]) - int(D[y2, x2]))
            if d > 1:
                n1 += 1
            if d > mx:
                mx = d; wo = (x2, y2, int(D[y2, x2]), x, y, int(D[y, x]))
    print("%-28s %6d Nachbarpaare, groesster Tiefensprung %2d, Paare mit Sprung > 1: %5d  %s"
          % (name, paare, mx, n1, "" if wo is None else
             "(%d,%d)=%d -> (%d,%d)=%d" % wo))
    return mx, n1


naht(tief, "AUSGELIEFERT (aufrecht)")
naht(B, "Spaltenregel")
naht(C, "Spaltenregel + bodenkante")

print("\n=== Ist die 'Fussabdruck-Schranke' unabhaengig? ===")
ys, xs = np.nonzero(reg)
vz_alle = []
for y, x in zip(ys, xs):
    v = geometrie.vz_at_floor(R, t, H, x + 0.5, y + 0.5, 0)
    if v:
        vz_alle.append(v)
vz_alle = np.array(vz_alle)
print("Bodenpunkt-Kamera-z UNTER ALLEN Silhouettenpunkten: %.0f .. %.0f -> Eimer %d .. %d"
      % (vz_alle.min(), vz_alle.max(), int(np.floor(1023 * vz_alle.min() / 65536)),
         int(np.floor(1023 * vz_alle.max() / 65536))))
print("  -> die LOCKERE, modellfreie Schranke (der Fussabdruck liegt irgendwo unter der")
print("     Silhouette) ist damit %d, nicht 62. Sie hat gegen die Spaltenregel (max 86)"
      % int(np.floor(1023 * vz_alle.max() / 65536)))
print("     KEINE Trennkraft. Die Schranke 62 setzt den EINEN Standpunkt schon voraus")
print("     und ist deshalb selbstbestaetigend — sie kommt NICHT in den Riegel.")
