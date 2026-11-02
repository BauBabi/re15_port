# Runde 41 Spur H: Rueckprojektion durch die RID-Kamera ROOM50A0 Cut 12. Aufruf: rueck.py sx,sy[,y] ... -> Weltpunkt auf Hoehe y.
import struct, math, sys
# Rueckprojektion durch die RID-Kamera ROOM50A0 Cut 12 (Matrix wie camera_common.c re15_camera_build_view = FUN_80053ca4)
d = open(__import__("os").path.join(__import__("os").path.dirname(__import__("os").path.abspath(__file__)), "..", "..", "shared_assets", "PSX", "STAGE5", "ROOM50A0.RDT"), 'rb').read()
cs = struct.unpack_from('<I', d, 0x24)[0]
fl, fov, px, py, pz, tx, ty, tz, pri = struct.unpack_from('<HHiiiiiiI', d, cs + 32 * 12)
def tdiv(a, b): q = abs(a) // abs(b); return q if (a >= 0) == (b >= 0) else -q
dx, dy, dz = tx - px, ty - py, tz - pz
dist = math.isqrt(dx*dx + dy*dy + dz*dz); hor = math.isqrt(dx*dx + dz*dz)
sp = tdiv(-dy * 4096, dist); cp = tdiv(hor * 4096, dist); sy_ = tdiv(dx * 4096, hor); cy = tdiv(dz * 4096, hor)
R = [cy, 0, -sy_, (sp*sy_) >> 12, cp, (sp*cy) >> 12, (cp*sy_) >> 12, -sp, (cp*cy) >> 12]
T = [(R[0]*-px + R[1]*-py + R[2]*-pz) >> 12, (R[3]*-px + R[4]*-py + R[5]*-pz) >> 12, (R[6]*-px + R[7]*-py + R[8]*-pz) >> 12]
H = fov >> 7
def proj(x, y, z):
    v = [(R[3*i]*x + R[3*i+1]*y + R[3*i+2]*z) / 4096 + T[i] for i in range(3)]
    return 160 + H * v[0] / v[2], 120 + H * v[1] / v[2], v[2]
def boden(sx, sy, y=0.0):
    # (sx-160)*vz = H*vx ; (sy-120)*vz = H*vy ; v = R p / 4096 + T ; unbekannt x,z
    a = sx - 160; b = sy - 120
    r = [c / 4096 for c in R]
    # Gleichungen: a*(r6 x + r7 y + r8 z + T2) - H*(r0 x + r1 y + r2 z + T0) = 0
    A1 = a*r[6] - H*r[0]; B1 = a*r[8] - H*r[2]; C1 = -(a*(r[7]*y + T[2]) - H*(r[1]*y + T[0]))
    A2 = b*r[6] - H*r[3]; B2 = b*r[8] - H*r[5]; C2 = -(b*(r[7]*y + T[2]) - H*(r[4]*y + T[1]))
    det = A1*B2 - A2*B1
    return (C1*B2 - C2*B1) / det, (A1*C2 - A2*C1) / det
if __name__ == '__main__':
    print('Cut 12 pos', (px, py, pz), 'tgt', (tx, ty, tz), 'H', H)
    for a in sys.argv[1:]:
        p = [float(t) for t in a.split(',')]
        y = p[2] if len(p) > 2 else 0.0
        x, z = boden(p[0], p[1], y)
        print('screen (%g,%g) y=%g -> world (%.0f, %.0f)  check %s' % (p[0], p[1], y, x, z, tuple(round(c) for c in proj(x, y, z)[:2])))
