"""Messung 6: Sieht der gemessene Quader wirklich wie der Tisch aus?
Links Hintergrund, Mitte Freistellung, rechts Quader-Silhouette (Hoehe -1575) und der
Quader als Drahtgitter. Dazu die Tiefe je Spalte aus beiden Modellen als Kurve.
"""
import os
import struct
import sys
import numpy as np
from PIL import Image, ImageDraw

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
bg = np.asarray(Image.open("re15_port/build_r19_marke310501/bg_ppm/ROOM10501.ppm").convert("RGB"), np.int16)
region = maske_aus_png.setze("pri/STAGE1/1050/01.png", 179, 139, 1, alpha_schwelle=110)
Zc = (16100, 1500, 1100, 4400)
HOEHE = -1575
vzq, tr = geometrie.quader_auf_band(R, t, H, Zc[0], Zc[0] + Zc[2], Zc[1], Zc[1] + Zc[3], HOEHE, 0)


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    if vz <= 64:
        return None
    return 160 + vx * H / vz, 120 + vy * H / vz


Zf = 3


def kachel(rgb, titel, gitter=None):
    im = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8)).resize((320 * Zf, 240 * Zf), Image.NEAREST)
    d = ImageDraw.Draw(im)
    if gitter:
        X0, X1, Z0, Z1 = Zc[0], Zc[0] + Zc[2], Zc[1], Zc[1] + Zc[3]
        for yy, col in ((0, (0, 255, 0)), (HOEHE, (255, 255, 0))):
            pts = [proj(X0, yy, Z0), proj(X1, yy, Z0), proj(X1, yy, Z1), proj(X0, yy, Z1)]
            for i in range(4):
                a, b = pts[i], pts[(i + 1) % 4]
                if a and b:
                    d.line([a[0] * Zf, a[1] * Zf, b[0] * Zf, b[1] * Zf], fill=col, width=2)
    for g in range(0, 320, 20):
        d.line([g * Zf, 0, g * Zf, 240 * Zf], fill=(120, 60, 0)); d.text((g * Zf + 1, 1), str(g), fill=(255, 220, 0))
    for g in range(0, 240, 20):
        d.line([0, g * Zf, 320 * Zf, g * Zf], fill=(120, 60, 0)); d.text((1, g * Zf + 1), str(g), fill=(255, 220, 0))
    d.text((4, 240 * Zf - 14), titel, fill=(255, 255, 255))
    return im


a = kachel(bg.copy(), "A Hintergrund + Quader (gruen = Boden y=0, gelb = Deckel y=-1575)", gitter=True)
ov = bg.astype(float) * 0.5
ov[region] = ov[region] * 0.3 + np.array([255, 0, 200]) * 0.7
b = kachel(ov, "B Freistellung des Nutzers (magenta)")
ov2 = bg.astype(float) * 0.5
both = region & tr
ov2[both] = ov2[both] * 0.3 + np.array([255, 0, 200]) * 0.7
ov2[region & ~tr] = np.array([255, 40, 40])
ov2[tr & ~region] = np.array([255, 230, 0]) * 0.8
c = kachel(ov2, "C magenta=beides  rot=Freistellung ohne Quader  gelb=Quader ohne Freistellung")
sheet = Image.new("RGB", (320 * Zf, 240 * Zf * 3 + 16), (25, 25, 25))
sheet.paste(a, (0, 0)); sheet.paste(b, (0, 240 * Zf + 8)); sheet.paste(c, (0, 480 * Zf + 16))
sheet.save(os.path.join(D, "09_quader.png"))
print("-> 09_quader.png")
print("Quader %d Punkte, Freistellung %d, beides %d, Freistellung ohne Quader %d, Quader ohne Freistellung %d"
      % (tr.sum(), region.sum(), both.sum(), int((region & ~tr).sum()), int((tr & ~region).sum())))
ys, xs = np.nonzero(region & ~tr)
if len(ys):
    print("  Freistellung ohne Quader: Kasten x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
