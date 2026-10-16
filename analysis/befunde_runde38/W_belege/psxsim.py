"""PSX-Zeichenweg eines Raum-Objekts (FUN_8002c18c -> FUN_80023098 Quads, Flach-Bit 0x10) nachgerechnet:
GTE RTPT/RTPS (Saettigung SX/SY [-0x400,0x3FF], SZ [0,0xFFFF], Division UNR ~ exakt), NCLIP (OPZ<0 -> weg),
AVSZ4 OTZ = 256*sum(SZ)>>12, Gate OTZ>>6 != 0, GPU-Groessengrenze je Dreieck (dx>=1024 / dy>=512 -> weg)
und Zeichenflaeche 320x240. Aufruf: psxsim.py <rdt> <obj> <cut> <prop_y> [rot_y=1024] [x z]"""
import struct, sys, math

def isqrt(v): return math.isqrt(v)

def view(cam):
    flag, fov, px, py, pz, tx, ty, tz = cam
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx*dx + dy*dy + dz*dz); horiz = isqrt(dx*dx + dz*dz)
    def tdiv(a, b):  # C-Division (Richtung 0)
        q = abs(a) // abs(b); return q if (a >= 0) == (b >= 0) else -q
    sp = tdiv(-dy*4096, dist); cp = tdiv(horiz*4096, dist)
    sy = tdiv(dx*4096, horiz); cy = tdiv(dz*4096, horiz)
    r = [cy, 0, -sy, (sp*sy) >> 12, cp, (sp*cy) >> 12, (cp*sy) >> 12, -sp, (cp*cy) >> 12]
    t = [(r[0]*-px + r[1]*-py + r[2]*-pz) >> 12, (r[3]*-px + r[4]*-py + r[5]*-pz) >> 12,
         (r[6]*-px + r[7]*-py + r[8]*-pz) >> 12]
    return r, t, fov >> 7

def main():
    rdt, obj, cut, prop_y = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])
    rot_y = int(sys.argv[5]) if len(sys.argv) > 5 else 1024
    px = int(sys.argv[6]) if len(sys.argv) > 6 else None
    pz = int(sys.argv[7]) if len(sys.argv) > 7 else None
    d = open(rdt, 'rb').read()
    camoff = struct.unpack_from('<I', d, 0x24)[0]
    cam = struct.unpack_from('<HH6i', d, camoff + cut*0x20)
    R, T, H = view(cam)
    tbl = struct.unpack_from('<I', d, 0x30)[0]
    md1 = struct.unpack_from('<I', d, tbl + obj*8 + 4)[0]
    m = d[md1:]
    h = struct.unpack_from('<14I', m, 12)
    qv_off, qv_cnt, qf_off, qf_cnt = h[7], h[8], h[11], h[12]
    V = [struct.unpack_from('<hhh', m, 12 + qv_off + k*8) for k in range(qv_cnt)]
    s = {0: 0, 1024: 4096, 2048: 0, 3072: -4096}[rot_y]; c = {0: 4096, 1024: 0, 2048: -4096, 3072: 0}[rot_y]
    W = [c, 0, s, 0, 4096, 0, -s, 0, c]
    pos = [px if px is not None else 7942, prop_y, pz if pz is not None else -20510]
    # composite = R*W, t = R*pos + T (FUN_80022da0 CompMatrix)
    C = [sum(R[i*3+k]*W[k*3+j] for k in range(3)) >> 12 for i in range(3) for j in range(3)]
    Ct = [((R[i*3]*pos[0] + R[i*3+1]*pos[1] + R[i*3+2]*pos[2]) >> 12) + T[i] for i in range(3)]
    def rtps(v):
        mac = [((Ct[i] << 12) + C[i*3]*v[0] + C[i*3+1]*v[1] + C[i*3+2]*v[2]) >> 12 for i in range(3)]
        ir = [max(-0x8000, min(0x7FFF, mac[0])), max(-0x8000, min(0x7FFF, mac[1]))]
        sz = max(0, min(0xFFFF, mac[2]))
        n = 0x1FFFF if not (H < sz*2) else min(0x1FFFF, ((H*0x20000 // sz) + 1) // 2)
        sx = max(-0x400, min(0x3FF, (n*ir[0] + (160 << 16)) >> 16))
        sy = max(-0x400, min(0x3FF, (n*ir[1] + (120 << 16)) >> 16))
        return sx, sy, sz, mac[2]
    print('cut', cut, 'cam', cam, 'H', H, 'prop pos', pos)
    gez = 0; px_sum = 0
    for k in range(qf_cnt):
        f = struct.unpack_from('<8H', m, 12 + qf_off + k*16)
        vi = [f[1], f[3], f[5], f[7]]
        P = [rtps(V[i]) for i in vi]
        (x0, y0, z0, _), (x1, y1, z1, _), (x2, y2, z2, _), (x3, y3, z3, _) = P
        opz = x0*y1 + x1*y2 + x2*y0 - x0*y2 - x1*y0 - x2*y1
        otz = (256 * (z0 + z1 + z2 + z3)) >> 12
        grund = []
        if opz < 0: grund.append('NCLIP')
        elif (otz >> 6) == 0: grund.append('OTZ<64')
        tri_ok = []
        for tri in ((P[0], P[1], P[2]), (P[1], P[2], P[3])):
            xs = [p[0] for p in tri]; ys = [p[1] for p in tri]
            ok = (max(xs) - min(xs)) < 1024 and (max(ys) - min(ys)) < 512
            onscr = not (max(xs) < 0 or min(xs) > 319 or max(ys) < 0 or min(ys) > 239)
            tri_ok.append((ok, onscr))
        if not grund:
            for i, (ok, on) in enumerate(tri_ok):
                if not ok: grund.append('GPU-Groesse T%d' % i)
        sichtbar = (not any(g in ('NCLIP', 'OTZ<64') for g in grund)) and any(ok and on for ok, on in tri_ok)
        if sichtbar: gez += 1
        print('Q%02d vz=%s sx=%s sy=%s opz=%d otz=%d %s %s' % (k, [p[3] for p in P], [p[0] for p in P],
              [p[1] for p in P], opz, otz, 'ZEICHNET' if sichtbar else 'weg', ','.join(grund) or '-'))
    print('Quads gezeichnet (auf dem Schirm):', gez, 'von', qf_cnt)

main()
