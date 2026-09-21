"""Messung 4: woher kommen die Tiefen? Das Modell von bau_p2 fuer ROOM1050 C1
nachgefahren, mit Bericht, Standpunkten je Spalte und der Frage, ob die Zellen-
Zuordnung oder die Spaltenregel gegriffen hat.
"""
import os
import struct
import sys
import numpy as np

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
region = maske_aus_png.setze("pri/STAGE1/1050/01.png", 179, 139, 1, alpha_schwelle=110)
print("Region: %d Punkte" % region.sum())

baender = sorted(geom.begehbare_baender(0x1050))
print("begehbare Baender:", baender)
y0 = -baender[0] * geom.BAND_HOEHE if len(baender) == 1 else geom.ebene_aus_kamera(rdt, cam, 1, 0x1050)
print("Bodenebene y0 =", y0)
if not isinstance(y0, int):
    y0 = int(y0[0])
band = int(round(-y0 / float(geom.BAND_HOEHE)))

ber = []
vzm, info = geometrie.tiefe_geometrie(rdt, R, t, H, region, y0, band, {}, None, ber)
print("INFO:", info)
for b in ber:
    print("  ", b)

dep = geometrie.rastern(vzm) * region
hull = geometrie.huelle(rdt)
print("SCA-Huelle x%d..%d z%d..%d" % hull)
stand, eigene, ohne = geometrie.standpunkte(region, R, t, H, y0, hull)
print("Spalten %d, eigene %d, geerbt %d" % (len(stand), len(eigene), len(ohne)))
print("geerbte Spalten:", geom._bereiche(sorted(ohne)))

print()
print("Spalte | unterste Maskenzeile | Standpunkt (wx,wz) | vz | Tiefe unten | Tiefe oben | eigen?")
for x in list(range(179, 232)) + [240, 260, 280, 300, 319]:
    if x not in stand:
        continue
    rows = np.nonzero(region[:, x])[0]
    wx, wz, vz = stand[x]
    dcol = dep[rows, x]
    print("  %3d  |  y%3d..%3d          | (%7.0f,%7.0f) | %6.0f | %4d | %4d | %s"
          % (x, rows.min(), rows.max(), wx, wz, vz, dcol[-1], dcol[0],
             "ja" if x in eigene else "GEERBT"))

# Leon
PX, PZ = 16732, 6350
vzP = (PX * R[6] + 0 * R[7] + PZ * R[8]) / 4096.0 + t[2]
print()
print("Leon Standpunkt (%d,%d) vz=%.0f bucket=%d" % (PX, PZ, vzP, geometrie.bucket(vzP)))
np.save(os.path.join(D, "dep_modell.npy"), dep)
np.save(os.path.join(D, "vzm_modell.npy"), vzm)

# --- SCA-Sperrzellen Band 0, auf den Boden projiziert ---------------------
print()
zs = geom.sca_sperrzellen(rdt, band)
print("SCA-Sperrzellen Band %d: %d" % (band, len(zs)))


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    if vz <= 64:
        return None
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


for (zx, zz, zw, zd, typ) in zs:
    ecken = [proj(zx + a * zw, y0, zz + b * zd) for a in (0, 1) for b in (0, 1)]
    if any(e is None for e in ecken):
        continue
    sxs = [e[0] for e in ecken]; sys_ = [e[1] for e in ecken]; vzs = [e[2] for e in ecken]
    if max(sxs) < 150 or min(sxs) > 260 or max(sys_) < 100:
        continue
    print("  Typ %d x%6d..%6d z%6d..%6d -> Bild x%.0f..%.0f y%.0f..%.0f vz %.0f..%.0f %s"
          % (typ, zx, zx + zw, zz, zz + zd, min(sxs), max(sxs), min(sys_), max(sys_),
             min(vzs), max(vzs),
             "<== LEON DRIN" if (zx <= PX <= zx + zw and zz <= PZ <= zz + zd) else ""))
