#!/usr/bin/env python3
"""md1_view.py - MD1-Mesh parsen, als OBJ schreiben und texturiert rendern.

Format 1:1 nach re15_port/include/re15_md1.h (dort gegen
de.re15.extractors.md1.MD1File belegt):
    [0x00] u32 length   [0x04] u32 unknown   [0x08] u32 object_count
    mesh_count = object_count / 2
    Mesh-Kopf i @ 12 + i*56, 14 u32:
      t_vtx_off t_vtx_cnt t_nrm_off t_nrm_cnt t_face_off t_face_cnt t_uv_off
      q_vtx_off q_vtx_cnt q_nrm_off q_nrm_cnt q_face_off q_face_cnt q_uv_off
    Alle Blockoffsets sind absolut ab (Dateianfang + 12).
    Vertex/Normal 8 B: s16 x,y,z,pad
    Tri 12 B:  u16 n0,v0,n1,v1,n2,v2       TriUV 12 B:  u8 u0,v0; u16 clut; u8 u1,v1; u16 page; u8 u2,v2; u16 pad
    Quad 16 B: u16 n0,v0,..,n3,v3          QuadUV 16 B: wie TriUV + u8 u3,v3; u16 pad

Aufruf:
    md1_view.py info <a.md1>
    md1_view.py obj  <a.md1> <out.obj>
    md1_view.py png  <a.md1> <a.tim> <out.png> [size]
"""
import struct, sys, os, math
from PIL import Image

V = struct.Struct("<hhhh")


def parse(d):
    length, unknown, nobj = struct.unpack_from("<III", d, 0)
    base = 12
    meshes = []
    for i in range(max(0, nobj // 2)):
        f = struct.unpack_from("<14I", d, 12 + i * 56)
        m = {}
        for part, o in (("t", 0), ("q", 7)):
            m[part] = dict(vtx_off=f[o] + base, vtx_cnt=f[o + 1],
                           nrm_off=f[o + 2] + base, nrm_cnt=f[o + 3],
                           face_off=f[o + 4] + base, face_cnt=f[o + 5],
                           uv_off=f[o + 6] + base)
        meshes.append(m)
    return dict(length=length, unknown=unknown, nobj=nobj, meshes=meshes)


def verts(d, off, cnt):
    return [V.unpack_from(d, off + k * 8)[:3] for k in range(cnt)]


def faces(d, off, cnt, n):
    st = 12 if n == 3 else 16
    out = []
    for k in range(cnt):
        w = struct.unpack_from("<%dH" % (2 * n), d, off + k * st)
        out.append([w[2 * j + 1] for j in range(n)])          # nur die v-Indizes
    return out


def uvs(d, off, cnt, n):
    st = 12 if n == 3 else 16
    out = []
    for k in range(cnt):
        b = d[off + k * st: off + k * st + st]
        pts = [(b[0], b[1]), (b[4], b[5]), (b[8], b[9])]
        if n == 4:
            pts.append((b[12], b[13]))
        clut = struct.unpack_from("<H", b, 2)[0]
        page = struct.unpack_from("<H", b, 6)[0]
        out.append(dict(pts=pts, clut=clut, page=page))
    return out


def geometry(d):
    h = parse(d)
    tris, quads = [], []
    for m in h["meshes"]:
        tv = verts(d, m["t"]["vtx_off"], m["t"]["vtx_cnt"])
        qv = verts(d, m["q"]["vtx_off"], m["q"]["vtx_cnt"])
        tf = faces(d, m["t"]["face_off"], m["t"]["face_cnt"], 3)
        qf = faces(d, m["q"]["face_off"], m["q"]["face_cnt"], 4)
        tu = uvs(d, m["t"]["uv_off"], m["t"]["face_cnt"], 3)
        qu = uvs(d, m["q"]["uv_off"], m["q"]["face_cnt"], 4)
        for f, u in zip(tf, tu):
            tris.append(([tv[i] for i in f], u))
        for f, u in zip(qf, qu):
            quads.append(([qv[i] for i in f], u))
    return h, tris, quads


def tim_image(path):
    """TIM -> (PIL-RGB-Bild, bpp). 8bpp-CLUT und 4bpp-CLUT."""
    d = open(path, "rb").read()
    assert struct.unpack_from("<I", d, 0)[0] == 0x10, "kein TIM"
    flags = struct.unpack_from("<I", d, 4)[0]
    bpp = {0: 4, 1: 8, 2: 16, 3: 24}[flags & 3]
    p = 8
    pal = None
    if flags & 8:
        csz, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, p)
        pal = d[p + 12: p + csz]
        p += csz
    isz, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
    body = d[p + 12: p + isz]
    if bpp == 8:
        w = iw * 2
    elif bpp == 4:
        w = iw * 4
    else:
        w = iw
    im = Image.new("RGB", (w, ih), (0, 0, 0))
    px = im.load()

    def col(i):
        v = struct.unpack_from("<H", pal, (i % (len(pal) // 2)) * 2)[0]
        return ((v & 0x1F) << 3, ((v >> 5) & 0x1F) << 3, ((v >> 10) & 0x1F) << 3)
    for y in range(ih):
        for x in range(w):
            if bpp == 8:
                px[x, y] = col(body[y * w + x])
            elif bpp == 4:
                b = body[(y * w + x) // 2]
                px[x, y] = col(b & 0xF if x % 2 == 0 else b >> 4)
            else:
                v = struct.unpack_from("<H", body, (y * w + x) * 2)[0]
                px[x, y] = ((v & 0x1F) << 3, ((v >> 5) & 0x1F) << 3, ((v >> 10) & 0x1F) << 3)
    return im, bpp, (w, ih)


def render(d, timpath, out, size=384, yaw=0.6, pitch=0.5):
    h, tris, quads = geometry(d)
    tex, bpp, (tw, th) = tim_image(timpath) if timpath else (None, 0, (1, 1))
    allv = [v for f, _ in tris + quads for v in f]
    if not allv:
        print("leeres Mesh"); return
    xs = [v[0] for v in allv]; ys = [v[1] for v in allv]; zs = [v[2] for v in allv]
    cx, cy, cz = (min(xs) + max(xs)) / 2, (min(ys) + max(ys)) / 2, (min(zs) + max(zs)) / 2
    ext = max(max(xs) - min(xs), max(ys) - min(ys), max(zs) - min(zs)) or 1
    sc = size * 0.72 / ext
    cyaw, syaw, cp, sp = math.cos(yaw), math.sin(yaw), math.cos(pitch), math.sin(pitch)

    def proj(v):
        x, y, z = v[0] - cx, v[1] - cy, v[2] - cz
        x, z = x * cyaw + z * syaw, -x * syaw + z * cyaw
        y, z = y * cp - z * sp, y * sp + z * cp
        return (size / 2 + x * sc, size / 2 + y * sc, z)

    img = Image.new("RGB", (size, size), (18, 18, 24))
    zbuf = [[1e9] * size for _ in range(size)]
    px = img.load()

    def tri(p, uv, shade):
        (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = p
        minx, maxx = int(max(0, min(x0, x1, x2))), int(min(size - 1, max(x0, x1, x2)))
        miny, maxy = int(max(0, min(y0, y1, y2))), int(min(size - 1, max(y0, y1, y2)))
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if abs(den) < 1e-9:
            return
        for yy in range(miny, maxy + 1):
            for xx in range(minx, maxx + 1):
                a = ((y1 - y2) * (xx - x2) + (x2 - x1) * (yy - y2)) / den
                b = ((y2 - y0) * (xx - x2) + (x0 - x2) * (yy - y2)) / den
                c = 1 - a - b
                if a < -0.002 or b < -0.002 or c < -0.002:
                    continue
                z = a * z0 + b * z1 + c * z2
                if z >= zbuf[yy][xx]:
                    continue
                zbuf[yy][xx] = z
                if tex is not None and uv:
                    u = a * uv[0][0] + b * uv[1][0] + c * uv[2][0]
                    v = a * uv[0][1] + b * uv[1][1] + c * uv[2][1]
                    col = tex.getpixel((min(tw - 1, max(0, int(u))), min(th - 1, max(0, int(v)))))
                else:
                    col = (180, 180, 180)
                px[xx, yy] = tuple(min(255, int(ch * shade)) for ch in col)

    def shade_of(p):
        (x0, y0, _), (x1, y1, _), (x2, y2, _) = p[0], p[1], p[2]
        ar = abs((x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0))
        return 1.0 if ar > 0 else 1.0

    for f, u in tris:
        p = [proj(v) for v in f]
        tri(p, u["pts"][:3], 1.0)
    for f, u in quads:
        p = [proj(v) for v in f]
        tri([p[0], p[1], p[2]], [u["pts"][0], u["pts"][1], u["pts"][2]], 1.0)
        tri([p[1], p[3], p[2]], [u["pts"][1], u["pts"][3], u["pts"][2]], 1.0)
    img.save(out)
    print("geschrieben: %s  (%d Tris, %d Quads, Textur %dx%d %dbpp)" % (
        out, len(tris), len(quads), tw, th, bpp))


if __name__ == "__main__":
    mode = sys.argv[1]
    d = open(sys.argv[2], "rb").read()
    if mode == "info":
        h, tris, quads = geometry(d)
        print("length=%d unknown=%d object_count=%d -> %d Mesh(es)" % (
            h["length"], h["unknown"], h["nobj"], len(h["meshes"])))
        for i, m in enumerate(h["meshes"]):
            print("  Mesh %d: tri v=%d f=%d | quad v=%d f=%d" % (
                i, m["t"]["vtx_cnt"], m["t"]["face_cnt"],
                m["q"]["vtx_cnt"], m["q"]["face_cnt"]))
        allv = [v for f, _ in tris + quads for v in f]
        if allv:
            print("  Bounding-Box x %d..%d  y %d..%d  z %d..%d" % (
                min(v[0] for v in allv), max(v[0] for v in allv),
                min(v[1] for v in allv), max(v[1] for v in allv),
                min(v[2] for v in allv), max(v[2] for v in allv)))
        pgs = sorted(set(u["page"] for _, u in tris + quads))
        cls = sorted(set(u["clut"] for _, u in tris + quads))
        print("  TPAGE-Werte %s  CLUT-Werte %s" % (pgs, cls))
    elif mode == "obj":
        h, tris, quads = geometry(d)
        with open(sys.argv[3], "w") as f:
            f.write("# MD1 -> OBJ (tools/re2_sicherung/md1_view.py)\n")
            n = 1
            for fa, _ in tris:
                for v in fa:
                    f.write("v %d %d %d\n" % v)
                f.write("f %d %d %d\n" % (n, n + 1, n + 2)); n += 3
            for fa, _ in quads:
                for v in fa:
                    f.write("v %d %d %d\n" % v)
                f.write("f %d %d %d %d\n" % (n, n + 1, n + 3, n + 2)); n += 4
        print("geschrieben:", sys.argv[3])
    elif mode == "png":
        render(d, sys.argv[3], sys.argv[4],
               int(sys.argv[5]) if len(sys.argv) > 5 else 384)
