#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""PSX TIM decoder.

Layout per the repo's own reader src/main/java/de/re15/extractors/md1/TimFile.java
and psx-spx (info/Resident_Evil_und_Playstation_Information/psx-spx.github.io-master):
  u32 magic 0x10
  u32 flags   bit3 = has CLUT, bits0-2 = bpp code (0=4bpp 1=8bpp 2=16bpp 3=24bpp)
  if CLUT: u32 blockSize, u16 x, u16 y, u16 w(words), u16 h  then h rows of w u16
  image:   u32 blockSize, u16 x, u16 y, u16 w(words), u16 h  then pixel data
  16-bit colour = 1bit STP + 5bit B + 5bit G + 5bit R (little endian u16)
"""
import struct


def c16(v):
    r = (v & 0x1F) << 3
    g = ((v >> 5) & 0x1F) << 3
    b = ((v >> 10) & 0x1F) << 3
    return (r | r >> 5, g | g >> 5, b | b >> 5)


def decode(d, off=0):
    """-> dict(bpp, w, h, cluts=[[(r,g,b)...]], idx=bytes-or-None, rgb=list)"""
    if len(d) - off < 20:
        return None
    magic, flags = struct.unpack_from('<II', d, off)
    if magic != 0x10:
        return None
    has_clut = bool(flags & 0x08)
    code = flags & 0x07
    bpp = {0: 4, 1: 8, 2: 16, 3: 24}.get(code)
    if bpp is None:
        return None
    p = off + 8
    cluts = []
    cx = cy = 0
    if has_clut:
        bsz, cx, cy, cw, ch = struct.unpack_from('<IHHHH', d, p)
        if bsz < 12 or p + bsz > len(d):
            return None
        q = p + 12
        for row in range(ch):
            pal = []
            for i in range(cw):
                v = struct.unpack_from('<H', d, q + (row * cw + i) * 2)[0]
                pal.append(c16(v))
            cluts.append(pal)
        p += bsz
    if p + 12 > len(d):
        return None
    bsz, ix, iy, wwords, h = struct.unpack_from('<IHHHH', d, p)
    if bsz < 12 or p + bsz > len(d):
        return None
    if bpp == 4:
        w = wwords * 4
    elif bpp == 8:
        w = wwords * 2
    else:
        w = wwords
    px = d[p + 12: p + bsz]
    return {'bpp': bpp, 'w': w, 'h': h, 'cluts': cluts, 'px': px,
            'ix': ix, 'iy': iy, 'cx': cx, 'cy': cy, 'end': p + bsz}


def to_image(t, clut=0):
    from PIL import Image
    w, h, bpp, px = t['w'], t['h'], t['bpp'], t['px']
    img = Image.new('RGB', (max(1, w), max(1, h)), (0, 0, 0))
    pxs = img.load()
    if bpp == 16:
        for y in range(h):
            for x in range(w):
                i = (y * w + x) * 2
                if i + 2 > len(px):
                    break
                pxs[x, y] = c16(struct.unpack_from('<H', px, i)[0])
        return img
    pal = t['cluts'][clut] if t['cluts'] and clut < len(t['cluts']) else [(0, 0, 0)] * 256
    if bpp == 4:
        stride = w // 2
        for y in range(h):
            for x in range(w):
                i = y * stride + (x >> 1)
                if i >= len(px):
                    break
                b = px[i]
                v = (b & 0x0F) if (x & 1) == 0 else (b >> 4)
                pxs[x, y] = pal[v] if v < len(pal) else (255, 0, 255)
    else:
        for y in range(h):
            for x in range(w):
                i = y * w + x
                if i >= len(px):
                    break
                v = px[i]
                pxs[x, y] = pal[v] if v < len(pal) else (255, 0, 255)
    return img
