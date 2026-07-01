#!/usr/bin/env python3
"""vram_png_to_tim.py — build a byte-true PSX TIM from a DuckStation ShowVRAM PNG.

The reliable ground truth for a PSX texture that only lives in VRAM (not in any RDT/DATA
file — e.g. the GLOBAL effect bank's sprite sheets) is DuckStation's own VRAM VIEW:
[Debug] ShowVRAM=true + a bound Screenshot button -> a 1024x512 PNG that is VRAM 1:1
(RGB from RGB555). See .claude/skills/re15-room-capture/scripts/re15_vram_extract.py for
the extraction of a *decoded* PNG; this tool instead emits a real PSX **TIM** (4/8/16-bit
+ CLUT) that reconstructs the exact VRAM halfwords, so the port loads it through its normal
re15_tim_parse path (no runtime PNG decoder, no new dependency) into a render slot.

Pure Python (stdlib zlib only — no PIL), so it runs in CI/headless too.

effect-0 (universal hit/blood, CORE00.ESP id 0) — byte-true 2026-07-01:
  tpage 0x001f -> VRAM(960,256) 4-bit ; clut 0x7951 -> VRAM(272,485) 16 entries
  python vram_png_to_tim.py vram_view_ground_truth.png --tpage 0x001f --clut 0x7951 \
         --w 256 --h 256 --out ../shared_assets/extracted_fx/effect0_blood.tim
"""
import argparse, struct, zlib, sys


def read_png_rgba(path):
    d = open(path, "rb").read()
    if d[:8] != b"\x89PNG\r\n\x1a\n":
        raise SystemExit("not a PNG: %s" % path)
    off = 8
    w = h = bd = ct = None
    idat = b""
    while off < len(d):
        ln = struct.unpack(">I", d[off:off + 4])[0]
        typ = d[off + 4:off + 8]
        body = d[off + 8:off + 8 + ln]
        if typ == b"IHDR":
            w, h, bd, ct, comp, filt, inter = struct.unpack(">IIBBBBB", body[:13])
            if bd != 8 or ct != 6 or inter != 0:
                raise SystemExit("need 8-bit RGBA non-interlaced PNG (got bd=%d ct=%d inter=%d)" % (bd, ct, inter))
        elif typ == b"IDAT":
            idat += body
        elif typ == b"IEND":
            break
        off += 12 + ln
    raw = zlib.decompress(idat)
    bpp = 4  # RGBA
    stride = w * bpp
    out = bytearray(w * h * bpp)
    prev = bytearray(stride)
    pos = 0
    for y in range(h):
        ft = raw[pos]; pos += 1
        line = bytearray(raw[pos:pos + stride]); pos += stride
        if ft == 0:
            pass
        elif ft == 1:  # Sub
            for i in range(bpp, stride):
                line[i] = (line[i] + line[i - bpp]) & 0xff
        elif ft == 2:  # Up
            for i in range(stride):
                line[i] = (line[i] + prev[i]) & 0xff
        elif ft == 3:  # Average
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                line[i] = (line[i] + ((a + prev[i]) >> 1)) & 0xff
        elif ft == 4:  # Paeth
            for i in range(stride):
                a = line[i - bpp] if i >= bpp else 0
                b = prev[i]
                c = prev[i - bpp] if i >= bpp else 0
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                pr = a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)
                line[i] = (line[i] + pr) & 0xff
        else:
            raise SystemExit("bad PNG filter %d" % ft)
        out[y * stride:(y + 1) * stride] = line
        prev = line
    return w, h, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("png")
    ap.add_argument("--tpage", type=lambda s: int(s, 0), required=True)
    ap.add_argument("--clut", type=lambda s: int(s, 0), default=0)
    ap.add_argument("--w", type=int, default=256, help="texture width in texels")
    ap.add_argument("--h", type=int, default=256, help="texture height in texels")
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    VW, VH, rgba = read_png_rgba(args.png)

    def v15(x, y):
        if 0 <= x < VW and 0 <= y < VH:
            o = (y * VW + x) * 4
            r, g, b = rgba[o], rgba[o + 1], rgba[o + 2]
            return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)
        return 0

    tp = args.tpage
    tx = (tp & 0xf) * 64
    ty = ((tp >> 4) & 1) * 256
    depth = (tp >> 7) & 3            # 0=4bit 1=8bit 2/3=16bit
    cl = args.clut
    clx, cly = (cl & 0x3f) * 16, cl >> 6
    ncol = 16 if depth == 0 else 256

    # --- pixel halfwords: one VRAM row per texel row; row width in halfwords ---
    if depth == 0:
        hw_per_row = args.w // 4
    elif depth == 1:
        hw_per_row = args.w // 2
    else:
        hw_per_row = args.w
    pix = bytearray()
    for y in range(args.h):
        for hx in range(hw_per_row):
            pix += struct.pack("<H", v15(tx + hx, ty + y))

    # --- CLUT block (VRAM row cly, ncol entries at clx) ---
    tim = bytearray()
    tim += struct.pack("<I", 0x00000010)                     # magic
    bpp_bits = {0: 0, 1: 1, 2: 2, 3: 2}[depth]
    flag = bpp_bits | (0x08 if depth < 2 else 0)             # bit3 = has CLUT (4/8-bit only)
    tim += struct.pack("<I", flag)
    if depth < 2:
        clut = bytearray()
        for i in range(ncol):
            clut += struct.pack("<H", v15(clx + i, cly))
        bnum = 12 + len(clut)
        tim += struct.pack("<IHHHH", bnum, clx, cly, ncol, 1)
        tim += clut

    bnum = 12 + len(pix)
    tim += struct.pack("<IHHHH", bnum, tx, ty, hw_per_row, args.h)
    tim += pix

    open(args.out, "wb").write(tim)
    opaque = sum(1 for i in range(0, len(pix), 2)
                 for nib in (pix[i] & 0xf, pix[i] >> 4, pix[i + 1] & 0xf, pix[i + 1] >> 4)
                 if nib != 0) if depth == 0 else -1
    print("wrote %s (%d bytes) %dbit %dx%d tpage->(%d,%d) clut->(%d,%d) %d opaque texels"
          % (args.out, len(tim), [4, 8, 16, 16][depth], args.w, args.h, tx, ty, clx, cly, opaque))


if __name__ == "__main__":
    main()
