"""Messung 10: der MECHANISMUS in Zahlen. Die Spaltenregel deutet die unterste opake
Zeile einer Spalte als BODENkontakt. Bei einer Tischplatte ist diese Zeile die eigene
Plattenkante auf Tischhoehe. Derselbe Bildpunkt, einmal auf y=0 und einmal auf der
gemessenen Plattenhoehe y=-1575 in die Welt zurueckgerechnet.
"""
import os
import struct
import sys
import numpy as np

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import geom
import geometrie
import maske_aus_png

rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
region = maske_aus_png.setze("pri/STAGE1/1050/01.png", 179, 139, 1, alpha_schwelle=110)
ZZ1 = 5900          # ferne Stirnkante der Zelle
PLATTE = -1575      # gemessene Plattenhoehe

print("Spalte | unterste opake Zeile | zurueckgerechnet auf y=0 (Spaltenregel) | "
      "auf y=-1575 (Plattenhoehe) | Fehler in z")
print("       |                      |    (wx,   wz)   vz  Tiefe             | "
      "   (wx,   wz)   vz  Tiefe |")
for x in (180, 181, 182, 183, 184, 185, 188, 190, 193, 196, 198, 200, 204, 210, 220, 226):
    rows = np.nonzero(region[:, x])[0]
    if len(rows) == 0:
        continue
    yb = int(rows.max())
    sy = min(yb, 239) + 0.5
    P0 = geom.welt_am_boden(R, t, H, x + 0.5, sy, 0)
    v0 = geom.vz_at_floor(R, t, H, x + 0.5, sy, 0)
    P1 = geom.welt_am_boden(R, t, H, x + 0.5, sy, PLATTE)
    v1 = geom.vz_at_floor(R, t, H, x + 0.5, sy, PLATTE)
    if P0 is None or P1 is None:
        continue
    print("  %3d  |        y%3d          | (%6.0f,%6.0f) %5.0f %4d             | "
          "(%6.0f,%6.0f) %5.0f %4d | %+6.0f  %s"
          % (x, yb, P0[0], P0[1], v0, geometrie.bucket(v0),
             P1[0], P1[1], v1, geometrie.bucket(v1), P0[1] - ZZ1,
             "HINTER dem Tisch" if P0[1] > ZZ1 else ""))

print()
print("Leon steht bei z=6350. Die Zelle endet bei z=%d." % ZZ1)
print("Eine Spalte, deren Spaltenregel-Standpunkt z > 6350 liefert, kann Leon nie")
print("verdecken — sie behauptet, weiter weg zu sein als er.")
zu_weit = 0
spalten = 0
for x in range(320):
    rows = np.nonzero(region[:, x])[0]
    if len(rows) == 0:
        continue
    spalten += 1
    P0 = geom.welt_am_boden(R, t, H, x + 0.5, min(int(rows.max()), 239) + 0.5, 0)
    if P0 is not None and P0[1] > 6350:
        zu_weit += 1
print("  Spalten mit Maske: %d | davon Standpunkt hinter Leon: %d (%.0f %%)"
      % (spalten, zu_weit, 100.0 * zu_weit / spalten))
print("  Spalten, deren Standpunkt HINTER der Zelle liegt (z > %d):" % ZZ1)
hinter = [x for x in range(320) if region[:, x].any()
          and (lambda P: P is not None and P[1] > ZZ1)(
              geom.welt_am_boden(R, t, H, x + 0.5,
                                 min(int(np.nonzero(region[:, x])[0].max()), 239) + 0.5, 0))]
print("    %d Spalten: %s" % (len(hinter), geom._bereiche(hinter)))
