#!/usr/bin/env python3
"""kameras.py - Spur D (Runde 34 Nacht, Gegenpruefung Auflage 7): Kameratabelle eines Raums und von
wo jede Kamera auf einen Punkt (Standard: Leon nach dem Rueckschritt, (16000,-13700)) schaut.

Kamerasatz (include/re15_camera.h re15_camera_cut_t, 0x20 Bytes, Tabelle ab RDT+0x60, Anzahl = RDT
Byte 1): +0 u16 flag, +2 u16 fov, +4 s32 pos_x, +8 pos_y, +0xC pos_z, +0x10 target_x, +0x14 target_y,
+0x18 target_z, +0x1C pri. Winkel in Grad, atan2(dz, dx) (+X = 0, +Z = 90).

Aufruf: kameras.py [RAUM] [x z]
"""
import math, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))


def main():
    raum = sys.argv[1] if len(sys.argv) > 1 else "1050"
    px, pz = (int(sys.argv[2]), int(sys.argv[3])) if len(sys.argv) > 3 else (16000, -13700)
    p = os.path.join(HIER, "..", "..", "shared_assets", "PSX", "STAGE%s" % raum[0], "ROOM%s.RDT" % raum)
    d = open(p, "rb").read()
    print("ROOM%s: %d Kameras (RDT Byte 1), Punkt (%d,%d)" % (raum, d[1], px, pz))
    for c in range(d[1]):
        b = 0x60 + c * 0x20
        fl, fov, cx, cy, cz, tx, ty, tz = struct.unpack_from("<HHiiiiii", d, b)
        blick = math.degrees(math.atan2(tz - cz, tx - cx))
        zu = math.degrees(math.atan2(pz - cz, px - cx))
        print("Cut %d @0x%05X: pos (%d,%d,%d) ziel (%d,%d,%d) Blick %.0f Grad | zum Punkt %.0f Grad, %.0f entfernt"
              % (c, b, cx, cy, cz, tx, ty, tz, blick, zu, math.hypot(px - cx, pz - cz)))


if __name__ == "__main__":
    main()
