# ROOM10D0 C1: warum gibt die reine Geometrie der Spalte 176 eine Tiefe >= 180?
# Die Marke F423 (Welt 3228/-4568, Kopf-vz 11579) verlangt wirksam < 180.
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image
import geom, geometrie, bau_p2, raum as R

ROOM, CUT = "ROOM10D0", 1
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
e = aus[ROOM][str(CUT)]
Rm, t, H = geom.cut_view(rdt, cam, CUT)
o = e["objekte"][0]
r, q = bau_p2.objekt_region(o, bg)
y0 = bau_p2.ebene_des_objekts(o, rdt, cam, CUT, rid, [])
print("Objekt %s: %d Punkte, Ebene y=%d" % (o["name"], int(r.sum()), y0))
ys, xs = np.nonzero(r)
print("Ausdehnung x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
band = int(round(-y0 / float(geom.BAND_HOEHE)))
ber = []
vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, r, y0, band, o, None, ber)
for b in ber:
    print("   ", b)
dep = geometrie.rastern(vzm) * r
print("Tiefe je Spalte (x: min..max je Spalte), Marke verlangt < 180 in x156..176:")
for x in range(int(xs.min()), int(xs.max()) + 1):
    col = dep[:, x][r[:, x]]
    if col.size == 0:
        continue
    mark = " <== Marke" if 156 <= x <= 176 else ""
    print("   x=%3d  n=%3d  Tiefe %3d..%3d%s" % (x, col.size, col.min(), col.max(), mark))

# Farbtrennung: die gruene Plattenfarbe (Messung 2026-09-08: g > r+10 und g > b+10)
b = bg.astype(int)
gruen = r & (b[:, :, 1] > b[:, :, 0] + 10) & (b[:, :, 1] > b[:, :, 2] + 10)
print("\nFarbtrennung am Hintergrund: Platte (gruen) %d Punkte, Gestell %d Punkte"
      % (int(gruen.sum()), int((r & ~gruen).sum())))
ys2, xs2 = np.nonzero(gruen)
if len(xs2):
    print("   Platte x%d..%d y%d..%d" % (xs2.min(), xs2.max(), ys2.min(), ys2.max()))
ys3, xs3 = np.nonzero(r & ~gruen)
print("   Gestell x%d..%d y%d..%d" % (xs3.min(), xs3.max(), ys3.min(), ys3.max()))
# Wie sieht die Tiefe aus, wenn beide Teile EIGENE Objekte sind?
for name, reg, opt in (("Platte", gruen, {"aufrecht": True}),
                       ("Platte-flach", gruen, {}),
                       ("Gestell", r & ~gruen, {}),
                       ("Gestell-aufrecht", r & ~gruen, {"aufrecht": True})):
    if not reg.any():
        continue
    ber = []
    v2, i2 = geometrie.tiefe_geometrie(rdt, Rm, t, H, reg, y0, band, opt, None, ber)
    if v2 is None:
        print("%s: keine Tiefe (%s)" % (name, i2.get("quelle"))); continue
    d2 = geometrie.rastern(v2) * reg
    sp = [x for x in range(156, 177) if reg[:, x].any()]
    if sp:
        mm = [int(d2[:, x][reg[:, x]].max()) for x in sp]
        print("%-18s Quelle %-7s Tiefe gesamt %3d..%3d | Spalten 156..176: max je Spalte %d..%d"
              % (name, i2.get("quelle"), d2[reg].min(), d2[reg].max(), min(mm), max(mm)))
    else:
        print("%-18s Quelle %-7s Tiefe gesamt %3d..%3d | keine Punkte in 156..176"
              % (name, i2.get("quelle"), d2[reg].min(), d2[reg].max()))
