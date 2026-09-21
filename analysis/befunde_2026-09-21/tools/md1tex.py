#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""MD1 UV parsing + textured z-buffer render (PSX-accurate texel lookup).

UV record layout per re15_port/include/re15_md1.h:
  TriangleUV 12B: u8 u0,v0; u16 clut; u8 u1,v1; u16 page; u8 u2,v2; u16 pad
  QuadUV     16B: ... + u8 u3,v3; u16 pad
tpage / CLUT bit layout per psx-spx (GP0 texpage + CLUT attribute):
  page: bits0-3 = tpage X in 64-halfword steps, bit4 = tpage Y (*256),
        bits7-8 = colour mode (0=4bpp 1=8bpp 2=16bpp)
  clut: bits0-5 = CLUT X / 16 (halfwords), bits6-14 = CLUT Y (lines)
"""
import struct, math
import timlib


def parse_uv(d):
    if len(d) < 12:
        return None
    length, unk, objc = struct.unpack_from('<III', d, 0)
    nmesh = objc // 2
    if nmesh <= 0 or nmesh > 64:
        return None
    base = 12
    faces = []
    for i in range(nmesh):
        ho = 12 + i * 56
        if ho + 56 > len(d):
            return None
        (tvo, tvc, tno, tnc, tfo, tfc, tuo,
         qvo, qvc, qno, qnc, qfo, qfc, quo) = struct.unpack_from('<14I', d, ho)

        def vl(off, cnt):
            o = base + off
            if cnt == 0 or o + cnt * 8 > len(d):
                return []
            return [struct.unpack_from('<3h', d, o + k * 8) for k in range(cnt)]
        tv = vl(tvo, tvc)
        qv = vl(qvo, qvc)
        of, ou = base + tfo, base + tuo
        for k in range(tfc):
            if of + (k + 1) * 12 > len(d) or ou + (k + 1) * 12 > len(d):
                break
            a = struct.unpack_from('<6H', d, of + k * 12)
            v0, v1, v2 = a[1], a[3], a[5]
            o = ou + k * 12
            uv = [(d[o], d[o + 1]), (d[o + 4], d[o + 5]), (d[o + 8], d[o + 9])]
            clut = struct.unpack_from('<H', d, o + 2)[0]
            page = struct.unpack_from('<H', d, o + 6)[0]
            if max(v0, v1, v2) < len(tv):
                faces.append(([tv[v0], tv[v1], tv[v2]], uv, clut, page))
        of, ou = base + qfo, base + quo
        for k in range(qfc):
            if of + (k + 1) * 16 > len(d) or ou + (k + 1) * 16 > len(d):
                break
            a = struct.unpack_from('<8H', d, of + k * 16)
            v0, v1, v2, v3 = a[1], a[3], a[5], a[7]
            o = ou + k * 16
            uv = [(d[o], d[o + 1]), (d[o + 4], d[o + 5]),
                  (d[o + 8], d[o + 9]), (d[o + 12], d[o + 13])]
            clut = struct.unpack_from('<H', d, o + 2)[0]
            page = struct.unpack_from('<H', d, o + 6)[0]
            if max(v0, v1, v2, v3) < len(qv):
                faces.append(([qv[v0], qv[v1], qv[v2], qv[v3]], uv, clut, page))
    return faces


def texel(t, page, clut, u, v):
    mode = (page >> 7) & 0x03
    tpx = (page & 0x0F) * 64
    tpy = ((page >> 4) & 0x01) * 256
    if mode == 0:
        hx = tpx + (u >> 2); sub = u & 3
    elif mode == 1:
        hx = tpx + (u >> 1); sub = u & 1
    else:
        hx = tpx + u; sub = 0
    vy = tpy + v
    col = hx - t['ix']
    row = vy - t['iy']
    if row < 0 or row >= t['h'] or col < 0:
        return None
    if t['bpp'] == 4:
        stride = t['w'] // 2
        bo = row * stride + col * 2 + (sub >> 1)
        if bo >= len(t['px']):
            return None
        b = t['px'][bo]
        idx = (b & 0x0F) if (sub & 1) == 0 else (b >> 4)
    elif t['bpp'] == 8:
        bo = row * t['w'] + col * 2 + sub
        if bo >= len(t['px']):
            return None
        idx = t['px'][bo]
    else:
        bo = (row * t['w'] + col) * 2
        if bo + 2 > len(t['px']):
            return None
        return timlib.c16(struct.unpack_from('<H', t['px'], bo)[0])
    if not t['cluts']:
        return None
    cx = (clut & 0x3F) * 16
    cy = (clut >> 6) & 0x1FF
    prow = cy - t.get('cy', 0)
    pcol = cx - t.get('cx', 0)
    if prow < 0 or prow >= len(t['cluts']):
        prow = 0
    pal = t['cluts'][prow]
    j = pcol + idx
    if j < 0 or j >= len(pal):
        j = idx if idx < len(pal) else 0
    return pal[j]


def render_tex(faces, tim, W, H, view='front', pad=6, yaw=0.0, pitch=0.0):
    import numpy as np
    from PIL import Image
    if not faces:
        return Image.new('RGB', (W, H), (20, 20, 26))
    if view == 'front':
        ax, ay, az = 0, 1, 2
    elif view == 'side':
        ax, ay, az = 2, 1, 0
    else:
        ax, ay, az = 0, 2, 1
    cyw, syw = math.cos(yaw), math.sin(yaw)
    cpw, spw = math.cos(pitch), math.sin(pitch)

    def xf(v):
        a = float(v[ax]); b = float(v[ay]); c = float(v[az])
        a2 = a * cyw + c * syw
        c2 = -a * syw + c * cyw
        b2 = b * cpw - c2 * spw
        c3 = b * spw + c2 * cpw
        return (a2, b2, c3)

    tri = []
    for (vs, uvs, clut, page) in faces:
        cols = []
        if tim:
            for (u, v) in uvs:
                c = texel(tim, page, clut, u, v)
                if c:
                    cols.append(c)
        fc = (tuple(int(sum(c[i] for c in cols) / len(cols)) for i in range(3))
              if cols else (140, 140, 140))
        P = [xf(v) for v in vs]
        tri.append((P[0], P[1], P[2], fc))
        if len(P) == 4:
            tri.append((P[0], P[2], P[3], fc))
    xs = [p[i][0] for p in tri for i in range(3)]
    ys = [p[i][1] for p in tri for i in range(3)]
    x0, x1 = min(xs), max(xs)
    y0, y1 = min(ys), max(ys)
    s = min((W - 2 * pad) / max(1e-6, x1 - x0), (H - 2 * pad) / max(1e-6, y1 - y0))
    cx = (x0 + x1) / 2.0
    cyy = (y0 + y1) / 2.0
    zb = np.full((H, W), 1e18)
    col = np.zeros((H, W, 3), dtype=np.uint8)
    col[:, :] = (20, 20, 26)
    xg, yg = np.meshgrid(np.arange(W), np.arange(H))
    for t in tri:
        P = []
        for k in range(3):
            v = t[k]
            P.append((W / 2.0 + (v[0] - cx) * s, H / 2.0 - (v[1] - cyy) * s, v[2]))
        fc = t[3]
        (X0, Y0, Z0), (X1, Y1, Z1), (X2, Y2, Z2) = P
        minx = max(0, int(min(X0, X1, X2)) - 1); maxx = min(W - 1, int(max(X0, X1, X2)) + 1)
        miny = max(0, int(min(Y0, Y1, Y2)) - 1); maxy = min(H - 1, int(max(Y0, Y1, Y2)) + 1)
        if minx > maxx or miny > maxy:
            continue
        den = (X1 - X0) * (Y2 - Y0) - (X2 - X0) * (Y1 - Y0)
        if abs(den) < 1e-9:
            continue
        sub = (slice(miny, maxy + 1), slice(minx, maxx + 1))
        px = xg[sub] + 0.5; py = yg[sub] + 0.5
        w1 = ((px - X0) * (Y2 - Y0) - (X2 - X0) * (py - Y0)) / den
        w2 = ((X1 - X0) * (py - Y0) - (px - X0) * (Y1 - Y0)) / den
        w0 = 1.0 - w1 - w2
        ins = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        if not ins.any():
            continue
        z = w0 * Z0 + w1 * Z1 + w2 * Z2
        ux, uy, uz = X1 - X0, Y1 - Y0, Z1 - Z0
        vx, vy, vz = X2 - X0, Y2 - Y0, Z2 - Z0
        nx = uy * vz - uz * vy; ny = uz * vx - ux * vz; nz = ux * vy - uy * vx
        ln = (nx * nx + ny * ny + nz * nz) ** 0.5 or 1.0
        lam = 0.6 + 0.4 * abs(-0.35 * nx / ln - 0.45 * ny / ln - 0.82 * nz / ln)
        fcs = tuple(min(255, int(c * lam)) for c in fc)
        cur = zb[sub]; m = ins & (z < cur); cur[m] = z[m]; zb[sub] = cur
        c = col[miny:maxy + 1, minx:maxx + 1]; c[m] = fcs
        col[miny:maxy + 1, minx:maxx + 1] = c
    return Image.fromarray(col, 'RGB')
