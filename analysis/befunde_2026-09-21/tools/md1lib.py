#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""MD1 parser + tiny software renderer (flat shaded, painter's algorithm).

MD1 layout is taken verbatim from the repo's own parser contract
re15_port/include/re15_md1.h (which in turn mirrors
src/main/java/de/re15/extractors/md1/MD1File.java):

  [0x00] u32 length
  [0x04] u32 unknown
  [0x08] u32 object_count      mesh_count = object_count / 2
  per mesh i: header at 12 + i*56, 14 u32:
     t_vtx_off,t_vtx_cnt,t_nrm_off,t_nrm_cnt,t_face_off,t_face_cnt,t_uv_off,
     q_vtx_off,q_vtx_cnt,q_nrm_off,q_nrm_cnt,q_face_off,q_face_cnt,q_uv_off
  All block offsets are absolute from (file_start + 12).
  Vertex/Normal 8B: s16 x,y,z,pad ; Tri 12B: u16 n0,v0,n1,v1,n2,v2
  Quad 16B: u16 n0,v0,n1,v1,n2,v2,n3,v3
"""
import struct


def parse(d):
    if len(d) < 12:
        return None
    length, unk, objc = struct.unpack_from('<III', d, 0)
    nmesh = objc // 2
    if nmesh <= 0 or nmesh > 64:
        return None
    base = 12
    meshes = []
    for i in range(nmesh):
        ho = 12 + i * 56
        if ho + 56 > len(d):
            return None
        f = struct.unpack_from('<14I', d, ho)
        (tvo, tvc, tno, tnc, tfo, tfc, tuo,
         qvo, qvc, qno, qnc, qfo, qfc, quo) = f
        if tvc > 8000 or qvc > 8000 or tfc > 8000 or qfc > 8000:
            return None
        def vts(off, cnt):
            o = base + off
            if cnt == 0:
                return []
            if o + cnt * 8 > len(d):
                return None
            return [struct.unpack_from('<3h', d, o + k * 8) for k in range(cnt)]
        tv = vts(tvo, tvc)
        qv = vts(qvo, qvc)
        if tv is None or qv is None:
            return None
        tris = []
        o = base + tfo
        if tfc and o + tfc * 12 <= len(d):
            for k in range(tfc):
                n0, v0, n1, v1, n2, v2 = struct.unpack_from('<6H', d, o + k * 12)
                tris.append((v0, v1, v2))
        quads = []
        o = base + qfo
        if qfc and o + qfc * 16 <= len(d):
            for k in range(qfc):
                vals = struct.unpack_from('<8H', d, o + k * 16)
                quads.append((vals[1], vals[3], vals[5], vals[7]))
        meshes.append({'tv': tv, 'qv': qv, 'tris': tris, 'quads': quads})
    return {'length': length, 'nmesh': nmesh, 'meshes': meshes}


def faces_world(md1):
    """Return list of (p0,p1,p2[,p3]) polygons in model space, plus vertex list."""
    polys = []
    for m in md1['meshes']:
        tv, qv = m['tv'], m['qv']
        for (a, b, c) in m['tris']:
            if a < len(tv) and b < len(tv) and c < len(tv):
                polys.append((tv[a], tv[b], tv[c]))
        for (a, b, c, e) in m['quads']:
            if a < len(qv) and b < len(qv) and c < len(qv) and e < len(qv):
                polys.append((qv[a], qv[b], qv[c], qv[e]))
    return polys


def bbox(polys):
    xs = []
    ys = []
    zs = []
    for p in polys:
        for v in p:
            xs.append(v[0]); ys.append(v[1]); zs.append(v[2])
    if not xs:
        return None
    return (min(xs), max(xs), min(ys), max(ys), min(zs), max(zs))


def render(polys, W, H, view='front', pad=4):
    """Orthographic flat render. view 'front' = XY (Y flipped), 'side' = ZY."""
    from PIL import Image, ImageDraw
    img = Image.new('RGB', (W, H), (20, 20, 26))
    if not polys:
        return img
    if view == 'front':
        ax, ay, az = 0, 1, 2
    elif view == 'side':
        ax, ay, az = 2, 1, 0
    else:  # top
        ax, ay, az = 0, 2, 1
    xs = [v[ax] for p in polys for v in p]
    ys = [v[ay] for p in polys for v in p]
    x0, x1 = min(xs), max(xs)
    y0, y1 = min(ys), max(ys)
    sx = (W - 2 * pad) / float(max(1, x1 - x0))
    sy = (H - 2 * pad) / float(max(1, y1 - y0))
    s = min(sx, sy)
    cx = (x0 + x1) / 2.0
    cy = (y0 + y1) / 2.0

    def proj(v):
        return (W / 2.0 + (v[ax] - cx) * s, H / 2.0 - (v[ay] - cy) * s)

    # sort back to front by mean depth; PSX: larger Z = further?  We just need
    # a stable silhouette, so paint far first using mean of the depth axis.
    order = sorted(range(len(polys)),
                   key=lambda i: -sum(v[az] for v in polys[i]) / float(len(polys[i])))
    dr = ImageDraw.Draw(img)
    for i in order:
        p = polys[i]
        pts = [proj(v) for v in p]
        # flat shade from face normal (cross of first two edges)
        ux = p[1][0] - p[0][0]; uy = p[1][1] - p[0][1]; uz = p[1][2] - p[0][2]
        vx = p[2][0] - p[0][0]; vy = p[2][1] - p[0][1]; vz = p[2][2] - p[0][2]
        nx = uy * vz - uz * vy
        ny = uz * vx - ux * vz
        nz = ux * vy - uy * vx
        ln = (nx * nx + ny * ny + nz * nz) ** 0.5 or 1.0
        # light from upper-left-front
        lz = (-0.4 * nx / ln) + (-0.5 * ny / ln) + (-0.76 * nz / ln)
        g = int(70 + 150 * abs(lz))
        g = max(40, min(235, g))
        dr.polygon(pts, fill=(g, g, int(g * 0.93)), outline=(15, 15, 18))
    return img


def render_z(polys, W, H, view='front', pad=6, yaw=0.0, pitch=0.0):
    """Z-buffered flat-shaded orthographic render (no painter-order artefacts).

    view sets the base axis mapping; yaw/pitch (radians) rotate the model so a
    silhouette can be judged from an angle as well.
    """
    import numpy as np
    from PIL import Image
    if not polys:
        return Image.new('RGB', (W, H), (20, 20, 26))
    if view == 'front':
        ax, ay, az = 0, 1, 2
    elif view == 'side':
        ax, ay, az = 2, 1, 0
    else:
        ax, ay, az = 0, 2, 1
    tris = []
    for p in polys:
        if len(p) == 3:
            tris.append((p[0], p[1], p[2]))
        else:
            tris.append((p[0], p[1], p[2]))
            tris.append((p[0], p[2], p[3]))
    import math
    cy_, sy_ = math.cos(yaw), math.sin(yaw)
    cp_, sp_ = math.cos(pitch), math.sin(pitch)

    def xf(v):
        a = float(v[ax]); b = float(v[ay]); c = float(v[az])
        a2 = a * cy_ + c * sy_
        c2 = -a * sy_ + c * cy_
        b2 = b * cp_ - c2 * sp_
        c3 = b * sp_ + c2 * cp_
        return (a2, b2, c3)

    T = [tuple(xf(v) for v in t) for t in tris]
    xs = [v[0] for t in T for v in t]
    ys = [v[1] for t in T for v in t]
    x0, x1 = min(xs), max(xs)
    y0, y1 = min(ys), max(ys)
    s = min((W - 2 * pad) / max(1e-6, x1 - x0), (H - 2 * pad) / max(1e-6, y1 - y0))
    cx = (x0 + x1) / 2.0
    cyy = (y0 + y1) / 2.0

    zbuf = np.full((H, W), 1e18, dtype=np.float64)
    col = np.zeros((H, W, 3), dtype=np.uint8)
    col[:, :] = (20, 20, 26)
    xg, yg = np.meshgrid(np.arange(W), np.arange(H))
    for t in T:
        P = []
        for v in t:
            P.append((W / 2.0 + (v[0] - cx) * s, H / 2.0 - (v[1] - cyy) * s, v[2]))
        (X0, Y0, Z0), (X1, Y1, Z1), (X2, Y2, Z2) = P
        minx = max(0, int(min(X0, X1, X2)) - 1)
        maxx = min(W - 1, int(max(X0, X1, X2)) + 1)
        miny = max(0, int(min(Y0, Y1, Y2)) - 1)
        maxy = min(H - 1, int(max(Y0, Y1, Y2)) + 1)
        if minx > maxx or miny > maxy:
            continue
        den = (X1 - X0) * (Y2 - Y0) - (X2 - X0) * (Y1 - Y0)
        if abs(den) < 1e-9:
            continue
        sub = (slice(miny, maxy + 1), slice(minx, maxx + 1))
        px = xg[sub] + 0.5
        py = yg[sub] + 0.5
        w1 = ((px - X0) * (Y2 - Y0) - (X2 - X0) * (py - Y0)) / den
        w2 = ((X1 - X0) * (py - Y0) - (px - X0) * (Y1 - Y0)) / den
        w0 = 1.0 - w1 - w2
        inside = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        if not inside.any():
            continue
        z = w0 * Z0 + w1 * Z1 + w2 * Z2
        # flat shade from the (unrotated) geometric normal of the screen-space tri
        ux, uy, uz = X1 - X0, Y1 - Y0, Z1 - Z0
        vx, vy, vz = X2 - X0, Y2 - Y0, Z2 - Z0
        nx = uy * vz - uz * vy
        ny = uz * vx - ux * vz
        nz = ux * vy - uy * vx
        ln = (nx * nx + ny * ny + nz * nz) ** 0.5 or 1.0
        lam = abs(-0.35 * nx / ln - 0.45 * ny / ln - 0.82 * nz / ln)
        g = int(max(45, min(240, 60 + 175 * lam)))
        cur = zbuf[sub]
        m = inside & (z < cur)
        cur[m] = z[m]
        zbuf[sub] = cur
        c = col[miny:maxy + 1, minx:maxx + 1]
        c[m] = (g, g, int(g * 0.94))
        col[miny:maxy + 1, minx:maxx + 1] = c
    return Image.fromarray(col, 'RGB')
