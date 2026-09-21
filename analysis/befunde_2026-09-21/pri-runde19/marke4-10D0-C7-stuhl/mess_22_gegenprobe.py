# -*- coding: utf-8 -*-
"""Messung 22: Gegenprobe der Tiefen-Rechnung.

(1) Projiziert die Kameramatrix den Spieler an Marke 4 auf die Werte, die
    befund.log gemeldet hat (scr 77/179, vz 6073)? Dann stimmt die Matrix.
(2) Rueckprojektion der Sockelzeile des Kastens auf den Boden und wieder
    vorwaerts: kommt dieselbe Bildzeile heraus?
(3) Nullmodell: welche Bildzeile haette ein Gegenstand, der GENAUSO nah waere
    wie der Spieler?
"""
import os
import struct
import sys

sys.path.insert(0, "re15_port/tools/maske")
import geom                                     # noqa: E402

CD = os.path.join("re15_port", "shared_assets", "PSX")
rdt, _ = geom.load_rdt(CD, "ROOM10D0")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 7)


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


print("(1) Spieler (418,0,26497):")
for hy, tag in ((0, "Fuss"), (-750, "Huefte"), (-1500, "Kopf")):
    sx, sy, vz = proj(418, hy, 26497)
    print("    %-6s -> scr (%.1f, %.1f)  vz %.0f   [befund.log: scr (77,179), vz 6073/5883/5692]"
          % (tag, sx, sy, vz))

print()
print("(2) Sockelzeilen des Kastens, hin und zurueck:")
for x, sy in ((58, 158), (62, 155), (67, 154), (59, 157)):
    w = geom.welt_am_boden(R, t, H, x, sy, 0)
    bx, by, bvz = proj(w[0], 0, w[1])
    print("    Bild(%d,%d) -> Welt(%d,%d) -> Bild(%.1f,%.1f) vz %.0f"
          % (x, sy, w[0], w[1], bx, by, bvz))

print()
print("(3) Nullmodell: Bodenpunkte mit demselben vz wie der Fuss des Spielers")
for x in (58, 64, 70):
    beste = None
    for sy in range(120, 240):
        vz = geom.vz_at_floor(R, t, H, x, sy, 0)
        if vz and (beste is None or abs(vz - 6073) < abs(beste[1] - 6073)):
            beste = (sy, vz)
    print("    Spalte %d: vz 6073 liegt auf Bildzeile %d (vz %.0f)" % (x, beste[0], beste[1]))
print("    -> ein Gegenstand, der am Fuss des Spielers steht, haette in diesen Spalten")
print("       seinen Sockel auf ~dieser Zeile. Der Kasten hat ihn bei y154..158.")
