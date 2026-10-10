import struct, sys, math
# kam.py <rdt> x,y,z [x,y,z ...]  -> projiziert Weltpunkte in alle Kameras (RID @[0x24], 0x20 B je Cut)
d = open(sys.argv[1], "rb").read()
ncut = d[1]
rid = struct.unpack_from("<I", d, 0x24)[0]
pts = [tuple(int(v) for v in a.split(",")) for a in sys.argv[2:]]
for c in range(ncut):
    o = rid + c * 0x20
    flag, persp, vx, vy, vz, tx, ty, tz, pri = struct.unpack_from("<hHiiiiiiI", d, o); persp >>= 7
    # Kamerabasis
    fx, fy, fz = tx - vx, ty - vy, tz - vz
    n = math.sqrt(fx*fx + fy*fy + fz*fz); fx, fy, fz = fx/n, fy/n, fz/n
    # rechts = f x up (up = 0,-1,0 in PSX: y nach unten)
    ux, uy, uz = 0.0, -1.0, 0.0
    rx, ry, rz = fy*uz - fz*uy, fz*ux - fx*uz, fx*uy - fy*ux
    rn = math.sqrt(rx*rx + ry*ry + rz*rz); rx, ry, rz = rx/rn, ry/rn, rz/rn
    # unten = f x rechts
    dx_, dy_, dz_ = fy*rz - fz*ry, fz*rx - fx*rz, fx*ry - fy*rx
    res = []
    for (px, py, pz) in pts:
        qx, qy, qz = px - vx, py - vy, pz - vz
        zc = qx*fx + qy*fy + qz*fz
        xc = qx*rx + qy*ry + qz*rz
        yc = qx*dx_ + qy*dy_ + qz*dz_
        if zc <= 0: res.append("hinter"); continue
        sx = 160 + persp * xc / zc; sy = 120 + persp * yc / zc
        tag = "IM BILD" if (0 <= sx < 320 and 0 <= sy < 240) else "aussen"
        res.append("(%4d,%4d) %s" % (sx, sy, tag))
    print("cut %2d @0x%04X persp=%d pos=(%d,%d,%d) tgt=(%d,%d,%d) | %s" % (c, o, persp, vx, vy, vz, tx, ty, tz, " | ".join(res)))
