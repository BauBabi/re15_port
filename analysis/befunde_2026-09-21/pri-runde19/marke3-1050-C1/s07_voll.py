"""Vollbild A/B/C/D + alle SCA-Zellen projiziert + die Figurpunkte je Zeile mit x-Bereichen."""
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

BG = "re15_port/build_r19_marke310501/bg_ppm/ROOM10501.ppm"
SHOT = "C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund_1050_F239_marke3.bmp"
bg = np.asarray(Image.open(BG).convert("RGB"), np.int16)
shot = np.asarray(Image.open(SHOT).convert("RGB").resize((320, 240), Image.BOX), np.int16)
deck = np.load(os.path.join(D, "deck.npy"))
tief = np.load(os.path.join(D, "tief.npy"))
gemalt = np.load(os.path.join(D, "gemalt.npy"))
verd = np.load(os.path.join(D, "verd.npy"))

Z = 3
def kachel(rgb, titel):
    im = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8)).resize((320 * Z, 240 * Z), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for g in range(0, 320, 20):
        d.line([g * Z, 0, g * Z, 240 * Z], fill=(255, 100, 0)); d.text((g * Z + 1, 1), str(g), fill=(255, 220, 0))
    for g in range(0, 240, 20):
        d.line([0, g * Z, 320 * Z, g * Z], fill=(255, 100, 0)); d.text((1, g * Z + 1), str(g), fill=(255, 220, 0))
    d.text((4, 240 * Z - 14), titel, fill=(255, 255, 255))
    return im

ov = bg.astype(float) * 0.5
ov[deck] = ov[deck] * 0.3 + np.array([255, 0, 200]) * 0.7
ov2 = shot.astype(float) * 0.55
ov2[verd] = ov2[verd] * 0.25 + np.array([255, 40, 0]) * 0.75
ov2[deck & ~verd] = ov2[deck & ~verd] * 0.5 + np.array([0, 160, 255]) * 0.5
sheet = Image.new("RGB", (320 * Z * 2 + 12, 240 * Z * 2 + 12), (25, 25, 25))
sheet.paste(kachel(bg.copy(), "A Hintergrund"), (0, 0))
sheet.paste(kachel(shot.copy(), "B Screenshot F239"), (320 * Z + 12, 0))
sheet.paste(kachel(ov, "C Maskendeckung magenta"), (0, 240 * Z + 12))
sheet.paste(kachel(ov2, "D rot=verdeckt hier, blau=Maske wirkungslos"), (320 * Z + 12, 240 * Z + 12))
sheet.save(os.path.join(D, "07_voll_ABCD.png"))
print("-> 07_voll_ABCD.png")

# --- Figurpunkte je Zeile mit x-Bereichen ---------------------------------
print()
print("Sichtbare Figur (Screenshot != Hintergrund) je Zeile, x-Bereiche:")
for y in range(70, 200):
    xs = np.nonzero(gemalt[y])[0]
    if len(xs) == 0:
        continue
    if y % 3:
        continue
    dk = deck[y][xs]
    print("  y%3d  x: %-34s  %2d Punkte, %2d mit Maskentexel, %2d verdeckt"
          % (y, geom._bereiche(list(xs)), len(xs), int(dk.sum()), int(verd[y][xs].sum())))

# --- alle SCA-Zellen ------------------------------------------------------
rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
PX, PZ = 16732, 6350


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    if vz <= 64:
        return None
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


print()
print("ALLE SCA-Sperrzellen Band 0 (Boden y=0):")
for (zx, zz, zw, zd, typ) in geom.sca_sperrzellen(rdt, 0):
    ecken = [proj(zx + a * zw, 0, zz + b * zd) for a in (0, 1) for b in (0, 1)]
    if any(e is None for e in ecken):
        print("  Typ %d x%6d..%6d z%6d..%6d -> hinter der Kamera" % (typ, zx, zx + zw, zz, zz + zd)); continue
    sxs = [e[0] for e in ecken]; sy = [e[1] for e in ecken]; vz = [e[2] for e in ecken]
    print("  Typ %d x%6d..%6d z%6d..%6d | Bild x%5.0f..%5.0f y%5.0f..%5.0f | vz %6.0f..%6.0f%s"
          % (typ, zx, zx + zw, zz, zz + zd, min(sxs), max(sxs), min(sy), max(sy), min(vz), max(vz),
             "   <== LEON STEHT DRIN" if (zx <= PX <= zx + zw and zz <= PZ <= zz + zd) else ""))
print()
print("Leon (%d,0,%d) -> Bild (%.1f,%.1f) vz=%.0f" % ((PX, PZ) + proj(PX, 0, PZ)))
