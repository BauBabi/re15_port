"""Messung 5: Beschreibt die SCA-Zelle x16100..17200 z1500..5900 die Freistellung?
(Die Zuordnung in bau_p2 wurde mit 37 % der Standpunkte VERWORFEN — aber die
Standpunkte selbst kommen aus der Spaltenregel, die bei einer Tischplatte nachweislich
falsch ist. Also die Zelle DIREKT messen: Quaderhoehe gegen die Freistellung.)
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
PX, PZ = 16732, 6350


def proj(x, y, z):
    vx = (x * R[0] + y * R[1] + z * R[2]) / 4096.0 + t[0]
    vy = (x * R[3] + y * R[4] + z * R[5]) / 4096.0 + t[1]
    vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
    if vz <= 64:
        return None
    return 160 + vx * H / vz, 120 + vy * H / vz, vz


Z = (16100, 1500, 1100, 4400, 1)
print("Zelle x%d..%d z%d..%d" % (Z[0], Z[0] + Z[2], Z[1], Z[1] + Z[3]))
print()
print("Wo liegt die FERNE Stirnkante der Zelle (z=5900) im Bild, je Hoehe?")
for hoehe in (0, -400, -700, -750, -800, -1000):
    e = [proj(x, hoehe, 5900) for x in (16100, 17200)]
    print("  y=%5d: Bild (%.1f,%.1f) .. (%.1f,%.1f)  vz %.0f / %.0f"
          % (hoehe, e[0][0], e[0][1], e[1][0], e[1][1], e[0][2], e[1][2]))
print()
print("Leon: Fuss (%.1f,%.1f) vz=%.0f | Kopf %s"
      % (proj(PX, 0, PZ)[0], proj(PX, 0, PZ)[1], proj(PX, 0, PZ)[2],
         "(%.1f,%.1f) vz=%.0f" % proj(PX, -1500, PZ)))

print()
print("hoehe_messen (IoU der Quader-Silhouette gegen die Freistellung):")
hm = geometrie.hoehe_messen(region, R, t, H, Z, 0)
if hm is None:
    print("  KEIN Treffer")
else:
    hoehe, iou, kante, saeule = hm
    print("  Hoehe %d, IoU %.3f, Oberkanten-Abstand %.1f px, Saeule=%s" % (hoehe, iou, kante, saeule))
    vzq, tr = geometrie.quader_auf_band(R, t, H, Z[0], Z[0] + Z[2], Z[1], Z[1] + Z[3], hoehe, 0)
    print("  Quader-Silhouette %d Punkte, Freistellung %d Punkte, Schnitt %d"
          % (tr.sum(), region.sum(), int((tr & region).sum())))
    depq = geometrie.rastern(np.where(region & tr, vzq, 0))
    ys, xs = np.nonzero(tr)
    print("  Quader-Silhouette Kasten x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
    # Tiefe der Zelle in den Spalten, die den Spieler beruehren
    print()
    print("  Spalte | Quadertiefe in den Freistellungspunkten | Modell (Spaltenregel)")
    dep_mod = np.load(os.path.join(D, "dep_modell.npy"))
    vzz, trz = geometrie.tiefe_zelle(region, R, t, H, Z, hoehe, saeule, 0)
    depz = geometrie.rastern(vzz) * region
    for x in range(180, 232, 2):
        m = region[:, x]
        if not m.any():
            continue
        a = depz[m, x]; b = dep_mod[m, x]
        print("   %4d  | %4d..%-4d  | %4d..%-4d" % (x, a.min(), a.max(), b.min(), b.max()))
    np.save(os.path.join(D, "dep_zelle.npy"), depz)
    np.save(os.path.join(D, "tr_zelle.npy"), tr)

# Grobkurve der IoU, damit die Hoehenmessung nicht nur ihr Maximum zeigt (Nullmodell)
print()
print("IoU-Kurve (Nullmodell: wie stark haengt sie von der Hoehe ab?):")
for h in range(-100, -2001, -100):
    _, tr = geometrie.quader_auf_band(R, t, H, Z[0], Z[0] + Z[2], Z[1], Z[1] + Z[3], h, 0)
    u = float((tr & region).sum()); v = float((tr | region).sum())
    print("   h=%5d  IoU %.3f  (Quader %5d Punkte, Schnitt %5d)" % (h, u / v if v else 0, tr.sum(), u))
