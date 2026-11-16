"""S7 b/c/d selbst: Rendern mit RDT-Licht (eigene Umsetzung), Hintergrund BSS, Videoraum-Regel, a* >= 10 je Teil."""
import sys, struct, math, json, numpy as np, cv2
from PIL import Image
from scipy import ndimage
from skimage.color import rgb2lab
import pl

SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'


def licht(room='50A0', cut=12):
    b = open(pl.PSX + '/STAGE5/ROOM%s.RDT' % room, 'rb').read(); p = pl.u32(b, 0x2C) + cut * 40
    return dict(skala=b[p], typ=tuple(b[p + 1:p + 4]), farbe=[tuple(b[p + 4 + 3 * k:p + 7 + 3 * k]) for k in range(3)],
                umg=tuple(b[p + 13:p + 16]), lage=[struct.unpack_from('<3h', b, p + 0x10 + 6 * k) for k in range(3)],
                hell=struct.unpack_from('<3H', b, p + 0x22))


def kontext(c, akt):
    L = np.zeros((3, 3), np.int64); C = np.zeros((3, 3), np.int64)
    for i in range(3):
        if c['typ'][i] == 0:
            d = [(int(akt[k]) - int(c['lage'][i][k])) >> 4 for k in range(3)]; rng = c['hell'][i] >> 4
            if rng <= 0:
                continue
            ab = rng - math.isqrt(d[0] * d[0] + d[2] * d[2])
            if ab <= 0:
                continue
            f = [c['farbe'][i][k] * ab // rng for k in range(3)]
        else:
            d, f = list(c['lage'][i]), list(c['farbe'][i])
        if not any(f) or not any(d):
            continue
        n = np.array(d, float); L[i] = -c['skala'] * np.rint(n / np.linalg.norm(n) * 4096).astype(np.int64)
        C[i] = np.array(f, np.int64) << 4
    return L, C, np.array(c['umg'], np.int64)


def render(E, pose, mit_licht=True):
    V, t, h, M, _ = pl.cam()
    g = pose['gier']; s, c = pl.q12s(g), pl.q12c(g)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], np.int64)
    lage = np.array([pose['x'], 0, pose['z']], np.int64)
    R, T = E.pose(E.kf_of(pose['clip'], pose['bild']))
    Lc, Cc, amb = kontext(licht(), lage)
    img = np.zeros((480, 640, 3)); zb = np.full((480, 640), np.inf); bid = np.full((480, 640), -1, np.int16)
    lok = np.full((480, 640, 3), np.nan)
    lut = np.stack([(E.clut & 31) << 3, ((E.clut >> 5) & 31) << 3, ((E.clut >> 10) & 31) << 3], -1).astype(float)
    for b in range(min(E.nb, len(E.meshes))):
        m = E.meshes[b]; Wm = (Y @ R[b]) >> 12; tw = ((Y @ T[b]) >> 12) + lage
        VW = (V @ Wm) >> 12; vT = ((V @ tw) >> 12) + t
        for kind, verts, norms, faces, uvs in (('t', m['tv'], m['tn'], m['tf'], m['tuv']), ('q', m['qv'], m['qn'], m['qf'], m['quv'])):
            if not faces:
                continue
            q = ((VW @ verts.T) >> 12) + vT[:, None]; vz = q[2]; zc = np.clip(vz, 1, 0xFFFF)
            n = np.minimum(((h * 0x20000) // zc + 1) // 2, 0x1FFFF)
            sx = 160 + ((np.clip(q[0], -0x8000, 0x7FFF) * n) >> 16); sy = 120 + ((np.clip(q[1], -0x8000, 0x7FFF) * n) >> 16)
            X, Yv = pl.to_video(sx, sy, M)
            if mit_licht:
                Ll = (Wm.T @ Lc.T).T >> 12
                d = np.clip((Ll @ norms.T) >> 12, 0, 0x7FFF)
                ir = np.clip(((amb[:, None] << 16) + Cc.T @ d) >> 12, 0, 0x7FFF)
                col = np.clip((((128 * ir) << 4) >> 12) >> 4, 0, 255).T
                f = np.minimum((col * 0xFF + 0x40) // 0x80, 0xFF).astype(float)
            else:
                f = np.full((len(norms), 3), 255.0)
            for fc, uv in zip(faces, uvs):
                ie, inn = fc[1::2], fc[0::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = p[0][0] * p[1][1] + p[1][0] * p[2][1] + p[2][0] * p[0][1] - p[0][0] * p[2][1] - p[1][0] * p[0][1] - p[2][0] * p[1][1]
                if (m0 < 0) if kind == 'q' else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128; zl = ((uv[2] >> 6) & 0x1FF) - E.clut_y
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if kind == 'q' else [])
                for a_, b_, c_ in (((0, 1, 3), (0, 3, 2)) if kind == 'q' else ((0, 1, 2),)):
                    ii = [ie[a_], ie[b_], ie[c_]]; nn = [inn[a_], inn[b_], inn[c_]]
                    x = np.array([X[i] for i in ii]); y = np.array([Yv[i] for i in ii]); z = np.array([vz[i] for i in ii], float)
                    uvt = np.array([tx[a_], tx[b_], tx[c_]], float); fl_ = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
                    if abs(fl_) < 1e-9:
                        continue
                    x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, 640)
                    y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, 480)
                    if x0 >= x1 or y0 >= y1:
                        continue
                    PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
                    w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl_
                    w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl_
                    w0 = 1 - w1 - w2; ins = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
                    uu = np.clip((w0 * uvt[0, 0] + w1 * uvt[1, 0] + w2 * uvt[2, 0]).astype(int), 0, E.tex.shape[1] - 1)
                    vv = np.clip((w0 * uvt[0, 1] + w1 * uvt[1, 1] + w2 * uvt[2, 1]).astype(int), 0, E.tex.shape[0] - 1)
                    row = zl if 0 <= zl < E.clut.shape[0] else 0
                    idx = E.tex[vv, uu]; ins &= E.clut[row][idx] != 0
                    zz = w0 * z[0] + w1 * z[1] + w2 * z[2]
                    sl = (slice(y0, y1), slice(x0, x1)); win = ins & (zz < zb[sl])
                    if not win.any():
                        continue
                    ff = w0[..., None] * f[nn[0]] + w1[..., None] * f[nn[1]] + w2[..., None] * f[nn[2]]
                    rgb = np.clip(lut[row][idx] * ff / 255.0, 0, 255).astype(np.uint8).astype(float)
                    zb[sl][win] = zz[win]; img[sl][win] = rgb[win]; bid[sl][win] = b
                    lv = verts[ii].astype(float); lo = w0[..., None] * lv[0] + w1[..., None] * lv[1] + w2[..., None] * lv[2]
                    lok[sl][win] = lo[win]
    return img, bid, lok


def bss_bg():
    bss = np.asarray(Image.open(SP + '/sfv4/bg/ROOM50A12.ppm').convert('RGB')).astype(float)
    M = pl.cam()[3]; Mi = np.linalg.inv(M); yy, xx = np.mgrid[0:480, 0:640]
    P = np.stack([xx.ravel() + 0.5, yy.ravel() + 0.5, np.ones(xx.size)]); q = Mi @ P
    u = np.clip(np.round(q[0] / q[2] - 0.375).astype(int), 0, 319); v = np.clip(np.round(q[1] / q[2] - 0.375).astype(int), 0, 239)
    return bss[v, u].reshape(480, 640, 3)


if __name__ == '__main__':
    import os
    M = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'
    bg = bss_bg()
    for name in ('V7', 'R4', 'R5D', 'R6', 'R2', 'R3'):
        E = pl.EMD(open(M + name + '.EMD', 'rb').read())
        img, bid, lok = render(E, pl.PV)
        comp = np.where((bid >= 0)[..., None], img, bg)
        vid = np.clip(np.array([9.42, 11.96, 10.94]) + np.array([0.9979, 0.9901, 0.9964]) * cv2.GaussianBlur(comp.astype(np.float32), (0, 0), 1.5), 0, 255)
        Lab = rgb2lab(vid / 255.0); v = []; kn = []
        for x in range(428, 446):
            ys = np.nonzero(bid[:, x] >= 0)[0]
            if len(ys): v += [Lab[y, x, 1] >= 10 for y in range(ys.min(), ys.min() + 8) if bid[y, x] >= 0]
        print('%-4s Huegel-Rot beleuchtet (Videoraum, PV, oberste 8 px x428-445): %.1f %% (%d px)' % (name, 100 * np.mean(v), len(v)))
        Image.fromarray(comp.astype(np.uint8)).save(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/p5/pruef_zahlen-r3/k/lit_PV_%s.png' % name)
