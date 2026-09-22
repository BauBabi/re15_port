"""Liest den R15M-Container + Masken-TIM und baut die TEXELGENAUE Tiefenkarte
eines Cuts - genau wie bg_pc.c/render_pc.c sie zeichnet (Index 0 = durchsichtig).

Format: re15_port/include/re15_pri.h (R15M) + re15_port/engine/src/pri_common.c
(re15_pri_parse_section).  Atlas: MASKS/ROOM####_PRI##.TIM, 8 bpp + CLUT,
Index 0 = transparent (bg_pc.c:145 pri_publish_tim).
"""
import os
import struct
import numpy as np

CD = os.environ.get("RE15_CD", "re15_port/shared_assets/PSX")


def msk_section(room, cut):
    b = open("%s/MASKS/%s.MSK" % (CD, room), "rb").read()
    assert b[:4] == b"R15M", b[:4]
    ver, n = struct.unpack_from("<II", b, 4)
    off = struct.unpack_from("<I", b, 12 + 4 * cut)[0]
    return b, off


def _s16(v):
    return ((v + 0x8000) % 0x10000) - 0x8000


def parse_section(data, off):
    gc, mdecl = struct.unpack_from("<HH", data, off)
    if gc == 0xFFFF or gc == 0 or mdecl == 0:
        return [], 0, 0
    gp = off + 4
    gn, gdx, gdy = [], [], []
    for i in range(gc):
        n, base, dx, dy = struct.unpack_from("<HHhh", data, gp + 8 * i)
        gn.append(n)
        gdx.append(dx)
        gdy.append(dy)
    mp = gp + 8 * gc
    total = sum(gn)
    out = []
    grp, used = 0, 0
    for i in range(total):
        sx, sy, dxl, dyl = data[mp], data[mp + 1], data[mp + 2], data[mp + 3]
        depth = struct.unpack_from("<H", data, mp + 4)[0]
        sizeb = data[mp + 7]
        mp += 8
        if (sizeb & 0xF0) == 0:
            w, h = struct.unpack_from("<HH", data, mp)
            mp += 4
        else:
            w = h = (sizeb >> 4) * 8
        while grp < gc and used >= gn[grp]:
            grp += 1
            used = 0
        ax = gdx[grp] if grp < gc else 0
        ay = gdy[grp] if grp < gc else 0
        used += 1
        out.append(dict(i=i, srcX=sx, srcY=sy, dstX=_s16(dxl + ax),
                        dstY=_s16(dyl + ay), depth=depth, w=w, h=h, grp=grp))
    return out, gc, mdecl


def atlas_idx(room, cut):
    b = open("%s/MASKS/%s_PRI%02d.TIM" % (CD, room, cut), "rb").read()
    magic, flags = struct.unpack_from("<II", b, 0)
    assert flags & 7 == 1, flags          # 8 bpp
    off = 8
    cl = struct.unpack_from("<I", b, off)[0]
    clut = np.frombuffer(b, np.uint16, count=(cl - 12) // 2, offset=off + 12).copy()
    off += cl
    il, ix, iy, iw, ih = struct.unpack_from("<I4H", b, off)
    px = np.frombuffer(b, np.uint8, count=iw * 2 * ih, offset=off + 12).reshape(ih, iw * 2).copy()
    return px, clut


def tiefenkarte(room, cut, W=320, H=240):
    """-> masks, gc, mdecl, draw, tief(HxW, kleinste Tiefe je Punkt), wer(Masken-Index)"""
    data, off = msk_section(room, cut)
    masks, gc, mdecl = parse_section(data, off)
    px, clut = atlas_idx(room, cut)
    draw = min(mdecl & 0xFF, len(masks))
    tief = np.full((H, W), 1 << 30, np.int32)
    wer = np.full((H, W), -1, np.int32)
    for m in masks[:draw]:
        sub = px[m["srcY"]:m["srcY"] + m["h"], m["srcX"]:m["srcX"] + m["w"]]
        if sub.shape != (m["h"], m["w"]):
            pad = np.zeros((m["h"], m["w"]), np.uint8)
            pad[:sub.shape[0], :sub.shape[1]] = sub
            sub = pad
        y0, x0 = m["dstY"], m["dstX"]
        ys0, xs0 = max(0, -y0), max(0, -x0)
        ys1, xs1 = min(m["h"], H - y0), min(m["w"], W - x0)
        if ys1 <= ys0 or xs1 <= xs0:
            continue
        op = sub[ys0:ys1, xs0:xs1] != 0
        ziel = (slice(y0 + ys0, y0 + ys1), slice(x0 + xs0, x0 + xs1))
        besser = op & (m["depth"] < tief[ziel])
        tief[ziel] = np.where(besser, m["depth"], tief[ziel])
        wer[ziel] = np.where(besser, m["i"], wer[ziel])
    return masks, gc, mdecl, draw, tief, wer, px, clut


def bucket(vz):
    """Figur-Bucket (1023*vz)>>16 - re15_pri.h re15_pri_bucket_of_vz."""
    return (1023 * int(vz)) >> 16
