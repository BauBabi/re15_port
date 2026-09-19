# ROOM10D0 C1, Variante: Platte = gruene Plattenfarbe UM 1 PUNKT GEWEITET (der
# Antialias-Saum der Platte ist nicht gruen und landete sonst beim Gestell).
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from scipy import ndimage
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
platte = r & ndimage.binary_dilation(gruen, np.ones((3, 3), bool))
gest = r & ~platte
print("Platte %d, Gestell %d, Summe %d (Vorlage %d)"
      % (platte.sum(), gest.sum(), platte.sum() + gest.sum(), r.sum()))
for name, reg in (("PLATTE", platte), ("GESTELL", gest)):
    xs = np.nonzero(reg.any(0))[0]
    print("== %s x%d..%d" % (name, xs.min(), xs.max()))
    prev = None
    zeilen = []
    for x in xs:
        rows = np.nonzero(reg[:, x])[0]
        yb = int(rows.max())
        vz = geom.vz_at_floor(Rm, t, H, x + 0.5, yb + 0.5, 0)
        P = geom.welt_am_boden(Rm, t, H, x + 0.5, yb + 0.5, 0)
        d = None if (P is None or prev is None) else float(np.hypot(P[0] - prev[0], P[1] - prev[1]))
        zeilen.append((int(x), yb, P, vz, d))
        prev = P
    schritte = sorted(z[4] for z in zeilen if z[4] is not None)
    print("   Schritte: min %.0f  median %.0f  p90 %.0f  max %.0f"
          % (schritte[0], np.median(schritte), np.percentile(schritte, 90), schritte[-1])
          if schritte else "   keine Schritte")
    print("   groesste fuenf Schritte: %s"
          % ", ".join("x%d->%d: %.0f" % (zeilen[i - 1][0], zeilen[i][0], zeilen[i][4])
                      for i in sorted(range(1, len(zeilen)),
                                      key=lambda j: -(zeilen[j][4] or 0))[:5]))
    print("   Tiefe je Spalte (Bucket): %s"
          % ", ".join("%d:%s" % (z[0], "-" if not z[3] else int(geometrie.bucket(z[3])))
                      for z in zeilen))
