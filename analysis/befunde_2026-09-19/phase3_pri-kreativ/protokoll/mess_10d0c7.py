# ROOM10D0 C7: Nutzer-Original 07_01 gegen meine Fassung; wo liegen die 168 Luecken-
# Punkte, und welche Tiefe gibt ihnen das Modell gegen die Bodentiefe an derselben Stelle?
import os, sys, json, struct
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
from PIL import Image, ImageDraw
import geom, geometrie, bau_p2, maske_aus_png, raum as R

ROOM, CUT = "ROOM10D0", 7
rid = int(ROOM[4:], 16)
aus = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt, _ = R.load_rdt(R.CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
bg = R.load_bg("build/bg_ppm", rid, CUT)
Rm, t, H = geom.cut_view(rdt, cam, CUT)
o = aus[ROOM][str(CUT)]["objekte"][0]
meins = maske_aus_png.setze("pri/STAGE1/10D0/07_01.png", o["x"], o["y"], 1, alpha_schwelle=110)
nutzer = maske_aus_png.setze("build/p3/10D0_07_01_nutzer.png", o["x"], o["y"], 1, alpha_schwelle=110)
print("meins %d, nutzer %d, nur beim Nutzer %d, nur bei mir %d"
      % (meins.sum(), nutzer.sum(), (nutzer & ~meins).sum(), (meins & ~nutzer).sum()))
luecke = nutzer & ~meins
ys, xs = np.nonzero(luecke)
print("Luecken x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
# Tiefe des Objekts an diesen Punkten (Modell des Nutzer-PNGs als EIN Objekt, aufrecht)
ber = []
vzm, info = geometrie.tiefe_geometrie(rdt, Rm, t, H, nutzer, 0, 0, o, None, ber)
for b in ber:
    print("  ", b)
dep = geometrie.rastern(vzm) * nutzer
print("Objekt-Tiefe an den Luecken: %d..%d" % (dep[luecke].min(), dep[luecke].max()))
# Bodentiefe an denselben Bildpunkten
bod = np.zeros((240, 320))
for y in range(240):
    for x in range(320):
        if luecke[y, x]:
            v = geom.vz_at_floor(Rm, t, H, x + 0.5, y + 0.5, 0)
            bod[y, x] = v or 0
b2 = geometrie.rastern(bod)
gut = luecke & (b2 > 0)
print("Bodentiefe an den Luecken: %d..%d (%d von %d Punkten mit Bodentreffer)"
      % (b2[gut].min(), b2[gut].max(), gut.sum(), luecke.sum()))
ov = bg.astype(float) * 0.6
ov[meins] = ov[meins] * 0.4 + np.array([255, 0, 200]) * 0.6
ov[luecke] = ov[luecke] * 0.2 + np.array([255, 230, 0]) * 0.8
Z = 4
Image.fromarray(ov.astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST).crop(
    (0, 110 * Z, 90 * Z, 200 * Z)).save("build/p3/10D0_C7_luecken.png")
print("build/p3/10D0_C7_luecken.png")
