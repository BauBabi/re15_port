# kamsicht.py <ROOM.RDT> x,y,z [x,y,z ...] : je Cut, ob die Weltpunkte im 320x240-Bild liegen (LookAt pos->target,
# H = fov>>7 (camera_common.c fov_to_screen_dist), sx = 160 + H*x/z, sy = 120 + H*y/z). Naeherung (float), nur zur Cut-Wahl.
import sys, struct, math
d = open(sys.argv[1], 'rb').read()
ncut = d[1]; cs = struct.unpack_from('<I', d, 0x24)[0]
pts = [tuple(float(v) for v in a.split(',')) for a in sys.argv[2:]]
for c in range(ncut):
    flag, fov, px, py, pz, tx, ty, tz, pri = struct.unpack_from('<HHiiiiiiI', d, cs + 32 * c)
    fx, fy, fz = tx - px, ty - py, tz - pz
    n = math.sqrt(fx*fx + fy*fy + fz*fz) or 1
    fx, fy, fz = fx/n, fy/n, fz/n
    # right = up x fwd with PSX up = (0,-1,0) -> screen x right, screen y down
    rx, ry, rz = fz, 0.0, -fx
    rn = math.hypot(rx, rz) or 1; rx, rz = rx/rn, rz/rn
    ux, uy, uz = fy*rz - fz*ry, fz*rx - fx*rz, fx*ry - fy*rx   # fwd x right
    H = fov >> 7
    out = []
    for (x, y, z) in pts:
        vx, vy, vz = x - px, y - py, z - pz
        cx = vx*rx + vy*ry + vz*rz; cy = vx*ux + vy*uy + vz*uz; cz = vx*fx + vy*fy + vz*fz
        if cz <= 100: out.append('hinter'); continue
        sx = 160 + H*cx/cz; sy = 120 + H*cy/cz
        ok = 0 <= sx < 320 and 0 <= sy < 240
        out.append('%s(%d,%d)' % ('IM BILD' if ok else 'aussen', sx, sy))
    print('cut %2d pos=(%d,%d,%d) ziel=(%d,%d,%d) H=%d : %s' % (c, px, py, pz, tx, ty, tz, H, ' | '.join(out)))
