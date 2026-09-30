# plan.py <zx> <zz> <rx> <rz> [abstand=2513] : Leon-Stellung fuer einen TIEF-Wurf auf das Ziel (zx,zz),
# Leon auf der Linie Ziel -> Referenzpunkt (rx,rz) im Abstand <abstand>, Blick auf das Ziel.
# Landung (gemessen ROOM1140 W9, Gier r im Spawnbild): Leon + 2513*(cos r, -sin r) - 313*(sin r, cos r).
import math, sys
zx, zz, rx, rz = (float(a) for a in sys.argv[1:5])
ab = float(sys.argv[5]) if len(sys.argv) > 5 else 2513.0
ux, uz = rx - zx, rz - zz; n = math.hypot(ux, uz); ux /= n; uz /= n
lx, lz = zx + ab * ux, zz + ab * uz
fx, fz = -ux, -uz                       # Blick auf das Ziel = (cos r, -sin r)
r = math.atan2(-fz, fx)                 # sin r = -fz, cos r = fx
ri = int(round(r / (2 * math.pi) * 4096)) & 0xfff
sx, sz = math.sin(r), math.cos(r)
landx = lx + 2513 * fx - 313 * sx; landz = lz + 2513 * fz - 313 * sz
print('POS=%d,%d,%d  Landung ~(%d,%d)  Abstand Landung-Ziel %d' % (round(lx), round(lz), ri, landx, landz, math.hypot(landx - zx, landz - zz)))
