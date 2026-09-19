# ROOM10D0 C1: unterste Silhouettenzeile je Spalte + Tiefe der Bodenregel.
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
hull = geometrie.huelle(rdt)
print("Huelle x%d..%d z%d..%d" % hull)
print(" x   oben unten  n   Weltpunkt(unten)        vz     Bucket")
for x in range(126, 177):
    rows = np.nonzero(r[:, x])[0]
    if not len(rows):
        continue
    yb = int(rows.max()); yo = int(rows.min())
    vz = geom.vz_at_floor(Rm, t, H, x + 0.5, yb + 0.5, 0)
    P = geom.welt_am_boden(Rm, t, H, x + 0.5, yb + 0.5, 0)
    print("%3d  %4d %5d %3d   %-22s %7s  %s"
          % (x, yo, yb, len(rows),
             "-" if P is None else "(%d, %d)" % (P[0], P[1]),
             "-" if not vz else "%.0f" % vz,
             "-" if not vz else int(geometrie.bucket(vz))))
