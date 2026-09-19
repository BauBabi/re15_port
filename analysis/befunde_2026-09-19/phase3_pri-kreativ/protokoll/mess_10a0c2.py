# ROOM10A0 C2: der EINE VOR-Standplatz (Band 3, Welt 22250/22150) — steht die Figur dort
# wirklich VOR dem Gelaender, oder taeuscht die Bezugsebene?
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image, ImageDraw
import geom, geometrie, bau_p2, abnahme, raum as R

ROOM, CUT = "ROOM10A0", 2
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
Rm, t, H = geom.cut_view(rdt, cam, CUT)
o = aus[ROOM][str(CUT)]["objekte"][0]
r, _ = bau_p2.objekt_region(o, bg)
y0 = int(o["ebene"])
band = int(round(-y0 / float(geom.BAND_HOEHE)))
print("Ebene y0=%d -> Band %d; BAND_HOEHE=%d" % (y0, band, geom.BAND_HOEHE))
vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, r, y0, band, o, None, [])
dep = geometrie.rastern(vzm) * r
stand, stand_y0 = abnahme.standlinie([(r, vzm, y0)])

floor = abnahme.floor_aus_dump("build/p2/floor_p2.txt", rid)
print("Baender im Dump: %s" % sorted(floor))
for b, pts in sorted(floor.items()):
    print("   Band %d: %d Punkte, y=%d" % (b, len(pts), -b * 0x708))

WX, WZ, BAND = 22250, 22150, 3
yf = -BAND * 0x708
pf = abnahme.proj(Rm, t, H, WX, yf, WZ)
pk = abnahme.proj(Rm, t, H, WX, yf - 1500, WZ)
print("Standplatz Band %d y=%d: Fuss Bild (%.0f,%.0f) vz %.0f | Kopf (%.0f,%.0f) vz %.0f"
      % (BAND, yf, pf[0], pf[1], pf[2], pk[0], pk[1], pk[2]))
hw = 450 * H / pf[2]
x0, x1 = int(max(0, pf[0] - hw)), int(min(320, pf[0] + hw))
yy0, yy1 = int(max(0, min(pk[1], pf[1]))), int(min(240, max(pk[1], pf[1])))
print("Koerperkasten x%d..%d y%d..%d" % (x0, x1, yy0, yy1))
basis = (WX * Rm[6] + WZ * Rm[8]) / 4096.0 + t[2]
print("Spalte | Standlinie(vz) | Standplatz auf y0 (vz) | Urteil | Maskentiefe im Kasten")
for x in range(x0, x1):
    sv = stand[x]
    ref = basis + y0 * Rm[7] / 4096.0
    col = dep[yy0:yy1, x]
    col = col[col > 0]
    print("  %3d  | %14s | %21.0f | %-7s | %s"
          % (x, "-" if np.isnan(sv) else "%.0f" % sv, ref,
             "-" if np.isnan(sv) else ("VOR" if ref < sv - 1 else ("HINTER" if ref > sv + 1 else "gleich")),
             "-" if not col.size else "%d..%d" % (col.min(), col.max())))
# Bild
ov = bg.astype(float) * 0.6
ov[r] = ov[r] * 0.4 + np.array([255, 0, 200]) * 0.6
Z = 3
im = Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST)
d = ImageDraw.Draw(im)
d.rectangle([x0 * Z, yy0 * Z, x1 * Z, yy1 * Z], outline=(0, 255, 0), width=2)
d.text((x0 * Z, yy0 * Z - 14), "Standplatz Band 3 (22250,22150)", fill=(0, 255, 0))
im.save("build/p3/10A0_C2_standplatz.png")
print("build/p3/10A0_C2_standplatz.png")
