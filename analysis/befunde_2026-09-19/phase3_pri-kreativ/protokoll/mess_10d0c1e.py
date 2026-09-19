# Hypothese: die Spalten mit Phantom-"Bodenkante" sind die, deren Sehstrahl ERST eine
# SCA-Sperrzelle trifft und dann erst die Bodenebene. Messung fuer ROOM10D0 C1.
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
zellen = geom.sca_sperrzellen(rdt, 0) or []
print("%d Sperrzellen" % len(zellen))

# Sehstrahl gegen die Zellen, als SAEULE (unbegrenzt hoch) — genau die Waende.
Rm_ = np.array(Rm, float).reshape(3, 3) / 4096.0
Ri = np.linalg.inv(Rm_)
c = Ri.dot(-np.array(t, float))
print(" x  unten   vz(Boden)  vz(naechste Zelle)   Verhaeltnis  Urteil")
for x in range(126, 177):
    rows = np.nonzero(r[:, x])[0]
    if not len(rows):
        continue
    yb = int(rows.max())
    vzf = geom.vz_at_floor(Rm, t, H, x + 0.5, yb + 0.5, 0)
    dw = Ri.dot(np.array([x + 0.5 - 160.0, yb + 0.5 - 120.0, float(H)]))
    best = None
    for (X, Z, W, D, *rest) in zellen:
        for (achse, wert, lo, hi, oa) in ((0, X, Z, Z + D, 2), (0, X + W, Z, Z + D, 2),
                                          (2, Z, X, X + W, 0), (2, Z + D, X, X + W, 0)):
            if abs(dw[achse]) < 1e-9:
                continue
            s = (wert - c[achse]) / dw[achse]
            if s <= 0:
                continue
            q = c + s * dw
            if not (lo <= q[oa] <= hi):
                continue
            if q[1] > 0:            # unter dem Boden
                continue
            vz = (q[0] * Rm[6] + q[1] * Rm[7] + q[2] * Rm[8]) / 4096.0 + t[2]
            if vz > 64 and (best is None or vz < best):
                best = vz
    urteil = "-"
    verh = "-"
    if best and vzf:
        verh = "%.2f" % (best / vzf)
        urteil = "PHANTOM (Zelle davor)" if best < vzf * 0.98 else "Boden sichtbar"
    print("%3d %5d  %9s  %17s  %11s  %s"
          % (x, yb, "%.0f" % vzf if vzf else "-", "%.0f" % best if best else "-", verh, urteil))
