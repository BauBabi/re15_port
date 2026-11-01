"""Perspektivische Pruefbuehne: Modell in echter Spielkamera gegen Videobild. Nur Analyse."""
import sys, os, json, struct, math
sys.path.insert(0, r'C:/workspace/git/reAi_v2/analysis/befunde_runde41/herkunft/birkin_modelle/werkzeuge')
from lib_fassungen import *
import cv2

ROOMS = {'50A': ('C:/workspace/git/reAi_v2/info/Re1.5/PSX/STAGE5/ROOM50A0.RDT', 12),
         '307c5': ('C:/workspace/git/reAi_v2/Version_40%/PSX/STAGE3/ROOM3070.RDT', 5),
         '307c8': ('C:/workspace/git/reAi_v2/Version_40%/PSX/STAGE3/ROOM3070.RDT', 8),
         '307c4': ('C:/workspace/git/reAi_v2/Version_40%/PSX/STAGE3/ROOM3070.RDT', 4)}


def kamera(key):
    p, c = ROOMS[key]
    d = open(p, 'rb').read()
    cs = struct.unpack_from('<I', d, 0x24)[0]
    o = cs + c*32
    flag, fov = struct.unpack_from('<HH', d, o)
    ex, ey, ez, tx, ty, tz = struct.unpack_from('<6i', d, o+4)
    return np.array([ex, ey, ez], float), np.array([tx, ty, tz], float), fov >> 7


def blick(auge, ziel):
    d = ziel - auge
    dist, horiz = np.linalg.norm(d), np.hypot(d[0], d[2])
    sp, cp = -d[1]/dist, horiz/dist
    sy, cy = d[0]/horiz, d[2]/horiz
    return np.array([[cy, 0.0, -sy], [sp*sy, cp, sp*cy], [cp*sy, -sp, cp*cy]])


class Cam:
    def __init__(s, key, hjson):
        s.auge, s.ziel, s.Hf = kamera(key)
        s.R = blick(s.auge, s.ziel)
        s.B = np.array(json.load(open(hjson))['H'], float)
        s.Bi = np.linalg.inv(s.B)

    def welt2port(s, W):
        v = (W - s.auge) @ s.R.T
        z = v[:, 2]
        return np.c_[160 + s.Hf*v[:, 0]/z, 120 + s.Hf*v[:, 1]/z], z

    def port2vid(s, P):
        h = np.c_[P, np.ones(len(P))] @ s.B.T
        return h[:, :2] / h[:, 2:3]

    def welt2vid(s, W):
        P, z = s.welt2port(W)
        return s.port2vid(P), z

    def vid2boden(s, xv, yv, y0=0.0):
        p = np.array([xv, yv, 1.0]) @ s.Bi.T
        p = p[:2]/p[2]
        d = np.array([(p[0]-160)/s.Hf, (p[1]-120)/s.Hf, 1.0]) @ s.R   # Blick->Welt
        t = (y0 - s.auge[1]) / d[1]
        return s.auge + t*d


def _geo(self, keyframe):
    R, T = self.pose(keyframe)
    V, N, F = [], [], []
    for b in range(min(self.skel.bone_count, len(self.meshes))):
        m = self.meshes[b]
        rot = R[b].astype(np.float64)/4096.0
        tr = T[b].astype(np.float64)

        def xf(vs):
            return np.zeros((0, 3)) if not vs else np.array([[v[0], v[1], v[2]] for v in vs], float) @ rot.T + tr

        def xn(ns):
            if not ns:
                return np.zeros((0, 3))
            w = np.array([[v[0], v[1], v[2]] for v in ns], float) @ rot.T
            ln = np.linalg.norm(w, axis=1, keepdims=True)
            ln[ln == 0] = 1
            return w/ln
        tvb = len(V)
        tnb = len(N)
        V.extend(xf(m.tv))
        N.extend(xn(m.tn))
        qvb = len(V)
        qnb = len(N)
        V.extend(xf(m.qv))
        N.extend(xn(m.qn))
        for i, (n0, v0, n1, v1, n2, v2) in enumerate(m.tf):
            uv = m.tuv[i]
            F.append(dict(v=(tvb+v0, tvb+v1, tvb+v2), n=(tnb+n0, tnb+n1, tnb+n2),
                          uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])), clut=uv[2], page=uv[5], bone=b))
        for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m.qf):
            uv = m.quv[i]
            a, bb = (uv[0], uv[1]), (uv[3], uv[4])
            c, dd = (uv[6], uv[7]), (uv[9], uv[10])
            F.append(dict(v=(qvb+v0, qvb+v1, qvb+v2), n=(qnb+n0, qnb+n1, qnb+n2), uv=(a, bb, c), clut=uv[2], page=uv[5], bone=b))
            F.append(dict(v=(qvb+v1, qvb+v3, qvb+v2), n=(qnb+n1, qnb+n3, qnb+n2), uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=b))
    return np.array(V), np.array(N), F


EA.Modell.geometrie = _geo


def lade(name):
    if name in FASSUNGEN:
        b = blob(name)
        t = None
    else:
        kind = int(name[1:], 16)
        b, _, _ = re2_rec(kind, 3)
        t, _, _ = re2_rec(kind, 2)
    mo = EA.Modell.__new__(EA.Modell)
    d = EA.emd_dir(b)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = b, 0x30, 0, len(b)
    if len(d) >= 9:
        mo.meshes = EA.md1_parse(b[d[7]:d[8]])
        tim = b[d[8]:]
    else:
        mo.meshes = EA.md1_parse(b[d[7]:])
        tim = t
    mo.skel = EA.emr_parse(b[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(b[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    return mo


def dreh_y(V, yaw):
    a = yaw*2*math.pi/4096
    c, s = math.cos(a), math.sin(a)
    x, z = V[:, 0].copy(), V[:, 2].copy()
    out = V.copy()
    out[:, 0] = c*x + s*z
    out[:, 2] = -s*x + c*z
    return out


def bild(mo, cam, V, N, F, W, H, ss=3, boost=1.45):
    """Affin texturiert wie PSX, Z-Buffer. W,H Videobild; Rueckgabe RGB float + Alpha bei ss-fach."""
    pv, z = cam.welt2vid(V)
    sx, sy = pv[:, 0]*ss, pv[:, 1]*ss
    w, h = W*ss, H*ss
    col = np.zeros((h, w, 3), np.float32)
    zbf = np.full((h, w), 1e18)
    alp = np.zeros((h, w), np.float32)
    l = (cam.auge - cam.ziel)
    l = l/np.linalg.norm(l)
    sh = 0.45 + 0.55*np.clip(N @ l, 0, 1)
    for f in F:
        i0, i1, i2 = f['v']
        x = np.array([sx[i0], sx[i1], sx[i2]])
        y = np.array([sy[i0], sy[i1], sy[i2]])
        zz0 = np.array([z[i0], z[i1], z[i2]])
        ar = (x[1]-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(y[1]-y[0])
        if abs(ar) < 1e-9:
            continue
        x0 = max(int(x.min()), 0)
        x1 = min(int(x.max())+2, w)
        y0 = max(int(y.min()), 0)
        y1 = min(int(y.max())+2, h)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1)+.5, np.arange(y0, y1)+.5)
        w0 = ((x[1]-x[0])*(PY-y[0]) - (PX-x[0])*(y[1]-y[0]))/ar
        w1 = ((PX-x[0])*(y[2]-y[0]) - (x[2]-x[0])*(PY-y[0]))/ar
        w2 = 1-w0-w1
        ins = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        if not ins.any():
            continue
        zz = w2*zz0[0] + w1*zz0[1] + w0*zz0[2]
        sub = zbf[y0:y1, x0:x1]
        hit = ins & (zz < sub)
        if not hit.any():
            continue
        n0, n1, n2 = f['n']
        s_ = w2*sh[n0] + w1*sh[n1] + w0*sh[n2]
        (u0, v0), (u1, v1), (u2, v2) = f['uv']
        ub = mo.page_ubase(f['page'])
        uu = np.clip((w2*(u0+ub)+w1*(u1+ub)+w0*(u2+ub)).astype(int), 0, mo.texw-1)
        vv = np.clip((w2*v0+w1*v1+w0*v2).astype(int), 0, mo.texh-1)
        rgb = mo.pal[mo.clut_row(f['clut'])][mo.tex[vv, uu]].astype(np.float32)/255
        c = rgb*s_[..., None]*boost
        cs = col[y0:y1, x0:x1]
        cs[hit] = c[hit]
        col[y0:y1, x0:x1] = cs
        sub[hit] = zz[hit]
        zbf[y0:y1, x0:x1] = sub
        a_ = alp[y0:y1, x0:x1]
        a_[hit] = 1
        alp[y0:y1, x0:x1] = a_
    return np.clip(col, 0, 1), alp


def tris(F):
    return np.array([f['v'] for f in F], int)


def silhouette(cam, V, tri, W, H, s=0.5):
    pv, z = cam.welt2vid(V)
    p = (pv*s).astype(np.int32)
    m = np.zeros((int(H*s), int(W*s)), np.uint8)
    for t in tri:
        cv2.fillConvexPoly(m, p[t], 1)
    return m
