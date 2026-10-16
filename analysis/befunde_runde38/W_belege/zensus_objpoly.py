"""zensus_objpoly.py - Runde 38 Spur W: wo aendert der Fix (Original-Polygon-Schranken im PC-Prop-Weg) das Bild?

Fuer jeden Raum (alle RDTs, inhaltsgleiche Elza-Varianten einmal), jedes Obj_model_set (0x2D) aus main/sub (alle
Lagen, die das Skript setzt), Typ != 3, ohne Elternobjekt, und jeden Kamera-Cut, dessen Regions-Viereck (erste RVD-Zone
mit cam_from == Cut, FUN_80014368) die Objektlage enthaelt: Bildbedeckung des Objekts
  ALT  = Port bis df08d83a: Scheitel vz < 64 -> Polygon weg, sonst ungesaettigt, ohne NCLIP/OTZ/GPU-Grenze,
  NEU  = engine/src/objpoly_r38.c: gesaettigte SXY, NCLIP < 0 weg, OTZ < 64 weg, GPU-Dreieck > 1023 x 511 weg.
Ausgabe: alle Faelle, in denen ALT mehr als 2 % des Bilds bedeckt, das NEU nicht bedeckt (Objekt "vor der Kamera")
oder NEU nennenswert weniger zeigt; dazu die Summen.
Kamera-Matrix wie re15_camera_build_view mit exakter Wurzel statt SquareRoot0, Drehung mit float-sin/cos (Q12 gerundet)
statt der Port-Tabelle — fuer einen Zensus genau genug (der Rolladen wird damit 48/48 wie im Original entschieden).
Aufruf: python zensus_objpoly.py <PSX-Wurzel> [ausgabe.txt]"""
import os, sys, struct, math, hashlib
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402


def tdiv(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def view(cam):
    flag, fov, px, py, pz, tx, ty, tz = cam
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = math.isqrt(dx * dx + dy * dy + dz * dz)
    horiz = math.isqrt(dx * dx + dz * dz)
    if dist == 0:
        return None
    sp = tdiv(-dy * 4096, dist); cp = tdiv(horiz * 4096, dist)
    if horiz:
        sy = tdiv(dx * 4096, horiz); cy = tdiv(dz * 4096, horiz)
        r = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    else:
        r = [4096, 0, 0, 0, cp, sp, 0, -sp, cp]
    t = [(r[0] * -px + r[1] * -py + r[2] * -pz) >> 12, (r[3] * -px + r[4] * -py + r[5] * -pz) >> 12,
         (r[6] * -px + r[7] * -py + r[8] * -pz) >> 12]
    return r, t, fov >> 7


def q12(a, f):
    return int(round(4096 * f(a * 2 * math.pi / 4096)))


def prop_rot(rx, ry, rz):
    """main.c pc_prop_rot_q12."""
    sx, cx = q12(rx, math.sin), q12(rx, math.cos)
    sy, cy = q12(ry, math.sin), q12(ry, math.cos)
    sz, cz = q12(rz, math.sin), q12(rz, math.cos)
    m = [0] * 9
    m[0] = (cz * cy) >> 12; m[1] = -((sz * cy) >> 12); m[2] = sy
    m[3] = ((sz * cx << 12) + cz * sy * sx) >> 24
    m[4] = ((cz * cx << 12) - sz * sy * sx) >> 24
    m[5] = -((cy * sx) >> 12)
    m[6] = ((sz * sx << 12) - cz * sy * cx) >> 24
    m[7] = ((cz * sx << 12) + sz * sy * cx) >> 24
    m[8] = (cy * cx) >> 12
    return m


def compose(R, T, W, pos):
    C = [sum(R[i * 3 + k] * W[k * 3 + j] for k in range(3)) >> 12 for i in range(3) for j in range(3)]
    Ct = [((R[i * 3] * pos[0] + R[i * 3 + 1] * pos[1] + R[i * 3 + 2] * pos[2]) >> 12) + T[i] for i in range(3)]
    return C, Ct


def mac(C, Ct, v):
    return [((C[i * 3] * v[0] + C[i * 3 + 1] * v[1] + C[i * 3 + 2] * v[2]) >> 12) + Ct[i] for i in range(3)]


def rtps(m, H):
    ir1 = max(-0x8000, min(0x7FFF, m[0])); ir2 = max(-0x8000, min(0x7FFF, m[1]))
    sz = max(0, min(0xFFFF, m[2]))
    n = 0x1FFFF if (sz == 0 or H >= sz * 2) else min(0x1FFFF, ((H * 0x20000 // sz) + 1) // 2)
    sx = max(-0x400, min(0x3FF, 160 + ((n * ir1) >> 16)))
    sy = max(-0x400, min(0x3FF, 120 + ((n * ir2) >> 16)))
    return sx, sy, sz


def alt_proj(m, H):
    if m[2] < 64:
        return None
    ir1 = max(-0x8000, min(0x7FFF, m[0])); ir2 = max(-0x8000, min(0x7FFF, m[1]))
    sz = min(0xFFFF, m[2])
    n = 0x1FFFF if (H >= sz * 2) else min(0x1FFFF, ((H * 0x20000 // sz) + 1) // 2)
    return 160 + ((n * ir1) >> 16), 120 + ((n * ir2) >> 16)


def gpu_ok(p):
    xs = [q[0] for q in p]; ys = [q[1] for q in p]
    return (max(xs) - min(xs)) <= 1023 and (max(ys) - min(ys)) <= 511


def md1_polys(d, off):
    m = d[off:]
    objc = struct.unpack_from('<I', m, 8)[0]
    out = []
    for i in range(objc // 2):
        h = struct.unpack_from('<14I', m, 12 + i * 56)
        tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu = h
        TV = [struct.unpack_from('<hhh', m, 12 + tv + k * 8) for k in range(tvc)]
        QV = [struct.unpack_from('<hhh', m, 12 + qv + k * 8) for k in range(qvc)]
        for k in range(tfc):
            f = struct.unpack_from('<6H', m, 12 + tf + k * 12)
            if max(f[1], f[3], f[5]) < len(TV):
                out.append([TV[f[1]], TV[f[3]], TV[f[5]]])
        for k in range(qfc):
            f = struct.unpack_from('<8H', m, 12 + qf + k * 16)
            if max(f[1], f[3], f[5], f[7]) < len(QV):
                out.append([QV[f[1]], QV[f[3]], QV[f[5]], QV[f[7]]])
    return out


def inside_quad(x, z, q):
    """Punkt-in-Viereck (konvex, Reihenfolge der RVD-Ecken)."""
    s = 0
    for i in range(4):
        x0, z0 = q[i]; x1, z1 = q[(i + 1) % 4]
        c = (x1 - x0) * (z - z0) - (z1 - z0) * (x - x0)
        if c != 0:
            if s == 0:
                s = 1 if c > 0 else -1
            elif (c > 0) != (s > 0):
                return False
    return True


def regions(d):
    p = struct.unpack_from('<I', d, 0x28)[0]
    res = {}
    while p + 20 <= len(d):
        m, cf, ct = struct.unpack_from('<HBB', d, p)
        if m == 0xFFFF and cf == 0xFF and ct == 0xFF:
            break
        pts = struct.unpack_from('<8h', d, p + 4)
        if cf not in res:
            res[cf] = [(pts[k], pts[k + 1]) for k in range(0, 8, 2)]
        p += 20
    return res


def bedeckung(polys, C, Ct, H, neu):
    img = Image.new('1', (320, 240), 0)
    dr = ImageDraw.Draw(img)
    n_poly = 0
    for P in polys:
        M = [mac(C, Ct, v) for v in P]
        if neu:
            S = [rtps(m, H) for m in M]
            (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = S[0], S[1], S[2]
            opz = x0 * y1 + x1 * y2 + x2 * y0 - x0 * y2 - x1 * y0 - x2 * y1
            if opz < 0:
                continue
            if len(P) == 3:
                otz = (341 * (z0 + z1 + z2)) >> 12
            else:
                otz = (256 * (z0 + z1 + z2 + S[3][2])) >> 12
            if otz < 64:
                continue
            pts = [(s[0], s[1]) for s in S]
            if len(P) == 3:
                if gpu_ok(pts):
                    dr.polygon(pts, fill=1); n_poly += 1
            else:
                ok0, ok1 = gpu_ok(pts[0:3]), gpu_ok(pts[1:4])
                if ok0 and ok1:
                    dr.polygon([pts[0], pts[1], pts[3]], fill=1); dr.polygon([pts[0], pts[3], pts[2]], fill=1)
                    n_poly += 1
                elif ok0 or ok1:
                    t = 0 if ok0 else 1
                    dr.polygon(pts[t:t + 3], fill=1); n_poly += 1
        else:
            A = [alt_proj(m, H) for m in M]
            if any(a is None for a in A):
                continue
            A = [(max(-100000, min(100000, a[0])), max(-100000, min(100000, a[1]))) for a in A]
            if len(P) == 3:
                dr.polygon(A, fill=1)
            else:
                dr.polygon([A[0], A[1], A[3]], fill=1); dr.polygon([A[0], A[3], A[2]], fill=1)
            n_poly += 1
    return img, n_poly


def main():
    root = sys.argv[1]
    out = open(sys.argv[2], 'w') if len(sys.argv) > 2 else sys.stdout
    seen = {}
    faelle = []
    summe = dict(kombis=0, alt_px=0, neu_px=0)
    for st in range(1, 7):
        sd = os.path.join(root, 'STAGE%d' % st)
        if not os.path.isdir(sd):
            continue
        for fn in sorted(os.listdir(sd)):
            if not (fn.upper().startswith('ROOM') and fn.upper().endswith('.RDT')):
                continue
            d = open(os.path.join(sd, fn), 'rb').read()
            if len(d) < 0x60:
                continue
            h = hashlib.md5(d).hexdigest()
            if h in seen:
                continue
            seen[h] = fn
            rid = fn[4:8]
            ncut = d[1]
            camoff = struct.unpack_from('<I', d, 0x24)[0]
            tbl = struct.unpack_from('<I', d, 0x30)[0]
            nO = d[2]
            regs = regions(d)
            recs = set()
            for (tag, idx), ops in L.regionen(d).items():
                for (pc, op, sz) in ops:
                    if op == 0x2D and pc + 34 <= len(d):
                        recs.add(d[pc:pc + 34])
            modelle = {}
            for rec in sorted(recs):
                oid, typ, par = rec[1], rec[2], rec[5]
                if typ == 3 or (par & 0xC0) == 0xC0 or oid >= nO:
                    continue
                x, y, z, rx, ry, rz = struct.unpack_from('<6h', rec, 10)
                if typ == 4:
                    y -= 900
                if oid not in modelle:
                    try:
                        mo = struct.unpack_from('<I', d, tbl + oid * 8 + 4)[0]
                        modelle[oid] = md1_polys(d, mo) if mo else []
                    except Exception:
                        modelle[oid] = []
                polys = modelle[oid]
                if not polys:
                    continue
                W = prop_rot(rx, ry, rz)
                for cut in range(ncut):
                    if cut in regs and not inside_quad(x, z, regs[cut]):
                        continue
                    cam = struct.unpack_from('<HH6i', d, camoff + cut * 0x20)
                    v = view(cam)
                    if v is None:
                        continue
                    R, T, H = v
                    C, Ct = compose(R, T, W, [x, y, z])
                    ia, na = bedeckung(polys, C, Ct, H, False)
                    ib, nb = bedeckung(polys, C, Ct, H, True)
                    pa = sum(ia.getdata()); pb = sum(ib.getdata())
                    summe['kombis'] += 1; summe['alt_px'] += pa; summe['neu_px'] += pb
                    if pa - pb > 1536 or (pa > 1000 and pb < pa * 0.5):
                        faelle.append((pa - pb, rid, oid, cut, x, y, z, pa, pb, na, nb))
    out.write('Raeume (inhaltsgleich zusammengefasst): %d, Kombinationen Objekt x Cut im Regions-Viereck: %d\n'
              % (len(seen), summe['kombis']))
    out.write('Summe Bildbedeckung ALT %d px, NEU %d px\n' % (summe['alt_px'], summe['neu_px']))
    out.write('Faelle mit > 2 %% des Bilds weniger (oder NEU < 50 %% von ALT bei ALT > 1000 px): %d\n' % len(faelle))
    for f in sorted(faelle, reverse=True):
        dpx, rid, oid, cut, x, y, z, pa, pb, na, nb = f
        out.write('  ROOM%s obj %2d Cut %2d @(%d,%d,%d): ALT %5d px (%3d Poly) -> NEU %5d px (%3d Poly), weniger %5d px'
                  ' (%.1f %%)\n' % (rid, oid, cut, x, y, z, pa, na, pb, nb, dpx, dpx * 100.0 / 76800))
    out.flush()


main()
