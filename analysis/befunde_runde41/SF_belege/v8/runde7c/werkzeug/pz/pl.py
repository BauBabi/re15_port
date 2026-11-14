"""Pruefer V8 r1 (Zahlen) - eigener EMD/TIM-Leser, eigene Pose, eigene Projektion, z-Puffer-Raster. Kein Import aus re15_port/tools."""
import struct, math, os, json
import numpy as np
W8 = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8'
PSX = W8 + '/re15_port/shared_assets/PSX'
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]


def ems_blob(typ=0x30):
    order = [0x10, 0x11, 0x12, 0x13, 0x16, 0x18, 0x1A, 0x20, 0x21, 0x24, 0x25, 0x26, 0x27, 0x29, 0x2B, 0x2D, 0x30, 0x36,
             0x40, 0x42, 0x45, 0x47, 0x49, 0x4B, 0x4D]
    e = open(PSX + '/EMD/CDEMD0.EMS', 'rb').read()
    idx = order.index(typ); c = 0; i = 0
    while True:
        d = u32(e, c); L = d + 36
        if i == idx:
            return e[c:c + L]
        i += 1; n = (c + L + 2047) & ~2047
        k = 0
        while u32(e, n) == 0:
            n += 4; k += 1
        if k:
            n = (n + 2047) & ~2047
        c = n


class EMD:
    def __init__(self, blob):
        self.blob = b = bytes(blob)
        do = u32(b, 0); n = u32(b, 4)
        self.dir = [u32(b, do + 4 * i) for i in range(n)]
        d = self.dir
        self.edd = b[d[1]:d[2]]; self.emr = b[d[2]:d[3]]; self.md1 = b[d[7]:d[8]]; self.tim = b[d[8]:do]
        self._md1(); self._emr(); self._edd(); self._timp()

    def _md1(self):
        d = self.md1; nobj = u32(d, 8); self.meshes = []; self.mesh_raw = []
        for i in range(nobj // 2):
            f = struct.unpack_from('<14I', d, 12 + 56 * i); B = 12
            tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = f
            m = dict(
                tv=np.array([struct.unpack_from('<4h', d, B + tvo + 8 * k)[:3] for k in range(tvc)], np.int64).reshape(-1, 3),
                tn=np.array([struct.unpack_from('<4h', d, B + tno + 8 * k)[:3] for k in range(tnc)], np.int64).reshape(-1, 3),
                tf=[struct.unpack_from('<6H', d, B + tfo + 12 * k) for k in range(tfc)],
                tuv=[struct.unpack_from('<BBHBBHBBH', d, B + tuo + 12 * k) for k in range(tfc)],
                qv=np.array([struct.unpack_from('<4h', d, B + qvo + 8 * k)[:3] for k in range(qvc)], np.int64).reshape(-1, 3),
                qn=np.array([struct.unpack_from('<4h', d, B + qno + 8 * k)[:3] for k in range(qnc)], np.int64).reshape(-1, 3),
                qf=[struct.unpack_from('<8H', d, B + qfo + 16 * k) for k in range(qfc)],
                quv=[struct.unpack_from('<BBHBBHBBHBBH', d, B + quo + 16 * k) for k in range(qfc)])
            raw = b''.join([d[B + tvo:B + tvo + 8 * tvc], d[B + tno:B + tno + 8 * tnc], d[B + tfo:B + tfo + 12 * tfc],
                            d[B + tuo:B + tuo + 12 * tfc], d[B + qvo:B + qvo + 8 * qvc], d[B + qno:B + qno + 8 * qnc],
                            d[B + qfo:B + qfo + 16 * qfc], d[B + quo:B + quo + 16 * qfc]])
            self.meshes.append(m); self.mesh_raw.append(raw)

    def _emr(self):
        e = self.emr; bt, ko, nb, ks = struct.unpack_from('<4H', e, 0)
        self.nb, self.ks = nb, ks
        self.rel = np.array([struct.unpack_from('<3h', e, 8 + 6 * i) for i in range(nb)], np.int64)
        self.parent = [-1] * nb
        for b in range(nb):
            cc, co = struct.unpack_from('<2H', e, bt + 4 * b)
            for i in range(cc):
                if bt + co + i >= len(e):
                    break
                ch = e[bt + co + i]
                if ch < nb and ch != b and self.parent[ch] == -1:
                    self.parent[ch] = b
        self.kfd = e[ko:]; self.nkf = len(self.kfd) // ks

    def _edd(self):
        d = self.edd; c0, o0 = struct.unpack_from('<2H', d, 0)
        self.clips = [(0, c0)]; f = c0
        for k in range(1, o0 // 4):
            c = u16(d, 4 * k); self.clips.append((f, c)); f += c
        self.frames = [u32(d, o0 + 4 * i) for i in range(f)]

    def _timp(self):
        t = self.tim; o = 8; cs = u32(t, o); self.clut_x, self.clut_y, cw, ch = struct.unpack_from('<4H', t, o + 4)
        self.clut = np.frombuffer(t[o + 12:o + 12 + 2 * cw * ch], '<u2').reshape(ch, cw).copy()
        o += cs; iw, ih = struct.unpack_from('<2H', t, o + 8)
        self.tex = np.frombuffer(t[o + 12:o + 12 + 2 * iw * ih], np.uint8).reshape(ih, 2 * iw).copy()

    def kf_of(self, clip, bild):
        f0, n = self.clips[clip]
        return self.frames[f0 + bild % n] & 0xFFF

    def angles(self, kf, b):
        base = kf * self.ks + 12
        bits = int.from_bytes(self.kfd[base:base + self.ks - 12], 'little')
        r = []
        for k in range(3):
            v = (bits >> (36 * b + 12 * k)) & 0xFFF
            r.append(v - 4096 if v & 0x800 else v)
        return r

    def rootpos(self, kf):
        return np.array(struct.unpack_from('<3h', self.kfd, kf * self.ks), np.int64)

    def pose(self, kf):
        R = [None] * self.nb; T = [None] * self.nb
        for b in range(self.nb):
            L = rot(*self.angles(kf, b)); p = self.parent[b]
            if p < 0 or p >= b:
                R[b], T[b] = L, self.rootpos(kf)
            else:
                R[b] = (R[p] @ L) >> 12; T[b] = ((R[p] @ self.rel[b]) >> 12) + T[p]
        return R, T


def q12s(a): return int(round(math.sin(a * 2 * math.pi / 4096) * 4096))
def q12c(a): return int(round(math.cos(a * 2 * math.pi / 4096) * 4096))


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def rot(ax, ay, az):
    sx, cx, sy, cy, sz, cz = q12s(ax), q12c(ax), q12s(ay), q12c(ay), q12s(az), q12c(az)
    m = np.zeros(9, np.int64)
    m[0] = s16((cz * cy) >> 12); m[1] = s16((-(sz * cy)) >> 12); m[2] = s16(sy)
    a = (cz * -sy) >> 12; bq = (sz * -sy) >> 12
    m[3] = s16(s16((sz * cx) >> 12) - s16((a * sx) >> 12)); m[4] = s16(s16((cz * cx) >> 12) + s16((bq * sx) >> 12))
    m[5] = s16((-(cy * sx)) >> 12)
    m[6] = s16(s16((sz * sx) >> 12) + s16((a * cx) >> 12)); m[7] = s16(s16((cz * sx) >> 12) - s16((bq * cx) >> 12))
    m[8] = s16((cy * cx) >> 12)
    return m.reshape(3, 3)


def rdt_cam(room='50A0', cut=12):
    b = open(PSX + '/STAGE%s/ROOM%s.RDT' % (room[0], room), 'rb').read()
    rid = u32(b, 0x24)
    _f, fov, px, py, pz, tx, ty, tz, _p = struct.unpack_from('<HH6iI', b, rid + 32 * cut)

    def td(a, c):
        q = abs(a) // abs(c)
        return q if (a < 0) == (c < 0) else -q
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = math.isqrt(dx * dx + dy * dy + dz * dz); hz = math.isqrt(dx * dx + dz * dz)
    sp, cp, sy, cy = td(-dy * 4096, dist), td(hz * 4096, dist), td(dx * 4096, hz), td(dz * 4096, hz)
    V = np.array([[cy, 0, -sy], [(sp * sy) >> 12, cp, (sp * cy) >> 12], [(cp * sy) >> 12, -sp, (cp * cy) >> 12]], np.int64)
    t = (V @ np.array([-px, -py, -pz], np.int64)) >> 12
    M = np.array(json.load(open(W8 + '/analysis/befunde_runde41/SF_belege/kamera_50A0_12.json'))['M'], float)
    return V, t, fov >> 7, M, (px, py, pz)


CAM = None
def cam():
    global CAM
    if CAM is None:
        CAM = rdt_cam()
    return CAM


def to_video(sx, sy, M):
    x = np.asarray(sx, float) + 0.375; y = np.asarray(sy, float) + 0.375
    w = M[2, 0] * x + M[2, 1] * y + M[2, 2]
    return (M[0, 0] * x + M[0, 1] * y + M[0, 2]) / w, (M[1, 0] * x + M[1, 1] * y + M[1, 2]) / w


def world_tris(E, pose, bones=None, R_T=None):
    V, t, h, M, _ = cam()
    g = pose['gier']; s, c = q12s(g), q12c(g)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], np.int64)
    lage = np.array([pose['x'], pose.get('y', 0), pose['z']], np.int64)
    R, T = R_T if R_T is not None else E.pose(E.kf_of(pose['clip'], pose['bild']))
    out = []
    for b in range(min(E.nb, len(E.meshes))):
        if bones is not None and b not in bones:
            continue
        m = E.meshes[b]; Wm = (Y @ R[b]) >> 12; tw = ((Y @ T[b]) >> 12) + lage
        VW = (V @ Wm) >> 12; vT = ((V @ tw) >> 12) + t
        for kind, verts, faces, uvs in (('t', m['tv'], m['tf'], m['tuv']), ('q', m['qv'], m['qf'], m['quv'])):
            if not faces:
                continue
            q = ((VW @ verts.T) >> 12) + vT[:, None]
            vz = q[2]; zc = np.clip(vz, 1, 0xFFFF)
            n = np.minimum(((h * 0x20000) // zc + 1) // 2, 0x1FFFF)
            sx = 160 + ((np.clip(q[0], -0x8000, 0x7FFF) * n) >> 16); sy = 120 + ((np.clip(q[1], -0x8000, 0x7FFF) * n) >> 16)
            X, Yv = to_video(sx, sy, M)
            for fi, (fc, uv) in enumerate(zip(faces, uvs)):
                ie = fc[1::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = p[0][0] * p[1][1] + p[1][0] * p[2][1] + p[2][0] * p[0][1] - p[0][0] * p[2][1] - p[1][0] * p[0][1] - p[2][0] * p[1][1]
                if (m0 < 0) if kind == 'q' else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128; zeile = (uv[2] >> 6) & 0x1FF
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if kind == 'q' else [])
                sets = ((0, 1, 3), (0, 3, 2)) if kind == 'q' else ((0, 1, 2),)
                for a_, b_, c_ in sets:
                    ii = [ie[a_], ie[b_], ie[c_]]
                    out.append((b, kind, fi, np.array([[X[i], Yv[i]] for i in ii]), np.array([vz[i] for i in ii], float),
                                np.array([tx[a_], tx[b_], tx[c_]], float), zeile, verts[ii].astype(float)))
    return out


def raster(E, tris, Wd=640, Hd=480, alpha=True):
    zb = np.full((Hd, Wd), np.inf); bid = np.full((Hd, Wd), -1, np.int16)
    U = np.full((Hd, Wd), -1, np.int32); Vv = np.full((Hd, Wd), -1, np.int32); Z = np.full((Hd, Wd), -1, np.int32)
    LOK = np.full((Hd, Wd, 3), np.nan, np.float32); FACE = np.full((Hd, Wd), -1, np.int32)
    for k, (b, kind, fi, P, z, uv, zeile, loc) in enumerate(tris):
        x, y = P[:, 0], P[:, 1]
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, Wd)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, Hd)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1 - w1 - w2
        inn = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not inn.any():
            continue
        uu = np.clip((w0 * uv[0, 0] + w1 * uv[1, 0] + w2 * uv[2, 0]).astype(int), 0, E.tex.shape[1] - 1)
        vv = np.clip((w0 * uv[0, 1] + w1 * uv[1, 1] + w2 * uv[2, 1]).astype(int), 0, E.tex.shape[0] - 1)
        if alpha:
            r = zeile - E.clut_y
            row = E.clut[r] if 0 <= r < E.clut.shape[0] else E.clut[0]
            inn &= row[E.tex[vv, uu]] != 0
        zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
        sl = (slice(y0, y1), slice(x0, x1))
        win = inn & (zz < zb[sl])
        if not win.any():
            continue
        zb[sl][win] = zz[win]; bid[sl][win] = b; U[sl][win] = uu[win]; Vv[sl][win] = vv[win]; Z[sl][win] = zeile
        FACE[sl][win] = k
        lo = w0[..., None] * loc[0] + w1[..., None] * loc[1] + w2[..., None] * loc[2]
        LOK[sl][win] = lo[win]
    return dict(bone=bid, u=U, v=Vv, zeile=Z, lok=LOK, face=FACE, z=zb)


def proj_local(E, pose, b, P, R_T=None):
    V, t, h, M, _ = cam()
    R, T = R_T if R_T is not None else E.pose(E.kf_of(pose['clip'], pose['bild']))
    g = pose['gier']; s, c = q12s(g) / 4096, q12c(g) / 4096
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    Pm = np.atleast_2d(np.asarray(P, float)) @ (R[b] / 4096.0).T + T[b]
    Pw = Pm @ Y.T + np.array([pose['x'], pose.get('y', 0), pose['z']], float)
    q = Pw @ (V / 4096.0).T + t
    sx = 160 + q[:, 0] * h / q[:, 2]; sy = 120 + q[:, 1] * h / q[:, 2]
    X, Yv = to_video(sx, sy, M)
    return np.stack([X, Yv], 1)


PV = dict(clip=17, bild=47, gier=587, x=-8246, z=-21266)


def raster_maler(E, tris, Wd=640, Hd=480, alpha=True):
    """Pruefer P5: Maler-Reihenfolge wie der Port (platform/pc/src/render_pc.c:1171-1182 stabile Einfuegesortierung, Tiefe absteigend;
    main.c:10992 / 11080-11081 Tiefe = (int) Mittel der drei z je Dreieck bzw. je Viereck-Haelfte), KEIN Tiefenpuffer.
    Gleichstand: frueher eingereiht zuerst gemalt (darunter)."""
    keys = [int(sum(t[4]) / 3.0) for t in tris]
    order = sorted(range(len(tris)), key=lambda i: -keys[i])
    bid = np.full((Hd, Wd), -1, np.int16); FACE = np.full((Hd, Wd), -1, np.int32)
    LOK = np.full((Hd, Wd, 3), np.nan, np.float32); U = np.full((Hd, Wd), -1, np.int32); Vv = np.full((Hd, Wd), -1, np.int32)
    Z = np.full((Hd, Wd), -1, np.int32)
    for k in order:
        b, kind, fi, P, z, uv, zeile, loc = tris[k]
        x, y = P[:, 0], P[:, 1]
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, Wd)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, Hd)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1 - w1 - w2
        inn = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not inn.any():
            continue
        uu = np.clip((w0 * uv[0, 0] + w1 * uv[1, 0] + w2 * uv[2, 0]).astype(int), 0, E.tex.shape[1] - 1)
        vv = np.clip((w0 * uv[0, 1] + w1 * uv[1, 1] + w2 * uv[2, 1]).astype(int), 0, E.tex.shape[0] - 1)
        if alpha:
            r = zeile - E.clut_y
            row = E.clut[r] if 0 <= r < E.clut.shape[0] else E.clut[0]
            inn &= row[E.tex[vv, uu]] != 0
        sl = (slice(y0, y1), slice(x0, x1))
        bid[sl][inn] = b; FACE[sl][inn] = k; U[sl][inn] = uu[inn]; Vv[sl][inn] = vv[inn]; Z[sl][inn] = zeile
        lo = w0[..., None] * loc[0] + w1[..., None] * loc[1] + w2[..., None] * loc[2]
        LOK[sl][inn] = lo[inn]
    return dict(bone=bid, u=U, v=Vv, zeile=Z, lok=LOK, face=FACE)
