# ROOM10D0 C1, Farbtrennung Platte/Gestell: hat das GESTELL allein eine saubere Fusslinie?
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, bau_p2, raum as R

ROOM, CUT = "ROOM10D0", 1
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
Rm, t, H = geom.cut_view(rdt, cam, CUT)
o = aus[ROOM][str(CUT)]["objekte"][0]
r, _ = bau_p2.objekt_region(o, bg)
b = bg.astype(int)
gruen = r & (b[:, :, 1] > b[:, :, 0] + 10) & (b[:, :, 1] > b[:, :, 2] + 10)
gest = r & ~gruen
for name, reg in (("GESTELL", gest), ("PLATTE", gruen)):
    print("== %s: %d Punkte" % (name, int(reg.sum())))
    print("  x   unten   Weltpunkt(unten)        vz   Bucket  Schritt zum Vorgaenger")
    prev = None
    for x in range(320):
        rows = np.nonzero(reg[:, x])[0]
        if not len(rows):
            continue
        yb = int(rows.max())
        vz = geom.vz_at_floor(Rm, t, H, x + 0.5, yb + 0.5, 0)
        P = geom.welt_am_boden(Rm, t, H, x + 0.5, yb + 0.5, 0)
        d = "-" if (P is None or prev is None) else "%.0f" % np.hypot(P[0] - prev[0], P[1] - prev[1])
        print("%3d  %5d   %-22s %7s  %6s  %s"
              % (x, yb, "-" if P is None else "(%d, %d)" % (P[0], P[1]),
                 "-" if not vz else "%.0f" % vz,
                 "-" if not vz else int(geometrie.bucket(vz)), d))
        prev = P
