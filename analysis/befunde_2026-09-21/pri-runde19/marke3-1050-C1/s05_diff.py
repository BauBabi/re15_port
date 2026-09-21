"""Messung 3: was der Nutzer SIEHT, gegen den unberuehrten Hintergrund.
- Diff Screenshot(F239) gegen ROOM10501.ppm -> die Punkte, die der Zeichner ueber den
  Hintergrund gemalt hat (= die sichtbare Figur).
- Darueber die geschriebene Maskendeckung und ihre Tiefe.
- Und: wo verdeckt die Maske die Figur an dieser Standposition (Original-Regel).
"""
import os
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
dd = np.abs(shot - bg).max(2)
gemalt = dd > 18
print("Vom Hintergrund abweichende Punkte (Schwelle 18): %d" % gemalt.sum())
ys, xs = np.nonzero(gemalt)
print("  Kasten der Abweichung: x%d..%d  y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))

deck = np.load(os.path.join(D, "deck.npy"))
tief = np.load(os.path.join(D, "tief.npy"))

# --- Kamera + Standposition -------------------------------------------------
rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
import struct
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
print("Kamera Cut 1: H=%d  t=%s" % (H, t))

PX, PZ = 16732, 6350


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


for nm, y in (("Fuss y=0", 0), ("Huefte y=-750", -750), ("Kopf y=-1500", -1500)):
    sx, sy, vz = proj(PX, y, PZ)
    print("  %-14s -> Bild (%.1f,%.1f) vz=%.0f bucket=%d" % (nm, sx, sy, vz, geometrie.bucket(vz)))

# Figur-Tiefe JE BILDZEILE (Senkrechte durch den Standpunkt) — so rechnet abnahme.py
ys_all = np.arange(240)
vzs, ok = geometrie.profil_spalte(R, t, H, PX, PZ, ys_all)
vzs = np.where(ok, vzs, np.nan)

# --- Verdeckung im Spielerkasten -------------------------------------------
kx0, kx1, ky0, ky1 = 181, 219, 122, 190
verd = np.zeros((240, 320), bool)
for y in range(240):
    if not np.isfinite(vzs[y]):
        continue
    verd[y] = deck[y] & (tief[y] < geometrie.bucket(vzs[y]))

# --- Bild: vier Kacheln ----------------------------------------------------
Z = 6
x0, x1, y0, y1 = 170, 250, 110, 240
W, Hh = (x1 - x0) * Z, (y1 - y0) * Z


def kachel(rgb, titel, marks=()):
    im = Image.fromarray(rgb.astype(np.uint8)).crop((x0, y0, x1, y1)).resize((W, Hh), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for g in range(x0, x1, 10):
        d.line([(g - x0) * Z, 0, (g - x0) * Z, Hh], fill=(255, 100, 0))
        d.text(((g - x0) * Z + 1, 1), str(g), fill=(255, 220, 0))
    for g in range(y0, y1, 10):
        d.line([0, (g - y0) * Z, W, (g - y0) * Z], fill=(255, 100, 0))
        d.text((1, (g - y0) * Z + 1), str(g), fill=(255, 220, 0))
    d.rectangle([(kx0 - x0) * Z, (ky0 - y0) * Z, (kx1 - x0 + 1) * Z, (ky1 - y0 + 1) * Z],
                outline=(0, 255, 255))
    d.text((3, Hh - 14), titel, fill=(255, 255, 255))
    return im


a = kachel(bg.copy(), "A Hintergrund (Original BSS)")
b = kachel(shot.copy(), "B Screenshot F239 (was der Nutzer sieht)")
ov = bg.astype(float) * 0.5
ov[deck] = ov[deck] * 0.3 + np.array([255, 0, 200]) * 0.7
c = kachel(ov, "C Maskendeckung (Texel) magenta")
ov2 = shot.astype(float) * 0.55
ov2[verd] = ov2[verd] * 0.25 + np.array([255, 40, 0]) * 0.75
ov2[deck & ~verd] = ov2[deck & ~verd] * 0.5 + np.array([0, 160, 255]) * 0.5
e = kachel(ov2, "D rot=verdeckt an dieser Lage, blau=Maske ohne Wirkung")

sheet = Image.new("RGB", (W * 2 + 12, Hh * 2 + 12), (25, 25, 25))
sheet.paste(a, (0, 0)); sheet.paste(b, (W + 12, 0))
sheet.paste(c, (0, Hh + 12)); sheet.paste(e, (W + 12, Hh + 12))
sheet.save(os.path.join(D, "05_vier_kacheln.png"))
print("-> 05_vier_kacheln.png")

# --- Zeilenweise Tabelle: wo ist die Figur gemalt, wo verdeckt die Maske ----
print()
print("Spalten x181..219, Zeile fuer Zeile (Figur = Abweichung vom Hintergrund):")
print(" y   Figurpunkte  davon mit Maskentexel  davon verdeckt  Tiefen der Maske   Figur-bucket")
for y in range(120, 200, 4):
    f = gemalt[y, kx0:kx1 + 1]
    dk = deck[y, kx0:kx1 + 1]
    vd = verd[y, kx0:kx1 + 1]
    tt = tief[y, kx0:kx1 + 1][dk]
    bk = geometrie.bucket(vzs[y]) if np.isfinite(vzs[y]) else -1
    print("%3d  %4d %11d %15d       %-16s %d"
          % (y, f.sum(), int((f & dk).sum()), int((f & vd).sum()),
             ("%d..%d" % (tt.min(), tt.max())) if tt.size else "-", bk))
np.save(os.path.join(D, "gemalt.npy"), gemalt)
np.save(os.path.join(D, "verd.npy"), verd)
