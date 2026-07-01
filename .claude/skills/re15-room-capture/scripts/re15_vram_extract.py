#!/usr/bin/env python3
"""re15_vram_extract.py - extract a PSX texture from a DuckStation ShowVRAM screenshot.

WHY: re15_ss.py's SAVESTATE VRAM decode is WRONG (the DuckStation savestate stores VRAM
in a layout re15_ss reads non-linearly -- the framebuffer lands at x=448 instead of x=0,
and CLUT rows read empty/shifted). The RELIABLE ground truth is DuckStation's own VRAM
VIEW: set [Debug] ShowVRAM=true, bind Screenshot to a controller button (SDL-0/LeftShoulder;
'&' is a CHORD not OR, so SINGLE binding), quick-load a savestate with the effect active,
press the button -> a 1024x512 PNG that is VRAM 1:1 (RGB from RGB555). This tool reconstructs
the raw 15-bit VRAM from that PNG and decodes a 4-bit/8-bit/16-bit texture at a given
tpage+clut. Verified 2026-07-01: effect-0 (tpage 0x001f -> VRAM(960,256) 4-bit, clut 0x7951
-> VRAM(272,485) = blood-red palette) extracted the byte-true hit/blood sprite sheet.

Usage:
  python re15_vram_extract.py <vram_view.png> --tpage 0x001f --clut 0x7951 --out effect0.png
  # tpage low nibble*64 = x, bit4*256 = y, bits7-8 = depth(0=4bit,1=8bit,2=16bit)
  # clut  (v&0x3f)*16 = x, v>>6 = y   (16 entries for 4-bit, 256 for 8-bit)
"""
import argparse
from PIL import Image

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("png")
    ap.add_argument("--tpage", type=lambda s: int(s, 0), required=True)
    ap.add_argument("--clut",  type=lambda s: int(s, 0), default=0)
    ap.add_argument("--w", type=int, default=256)   # texels wide
    ap.add_argument("--h", type=int, default=256)
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    im = Image.open(args.png).convert("RGB"); px = im.load()
    VW, VH = im.size
    def v15(x, y):
        if 0 <= x < VW and 0 <= y < VH:
            r, g, b = px[x, y]; return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10)
        return 0

    tp = args.tpage
    tx = (tp & 0xf) * 64
    ty = ((tp >> 4) & 1) * 256
    depth = (tp >> 7) & 3        # 0=4bit 1=8bit 2/3=15bit
    cl = args.clut
    clx, cly = (cl & 0x3f) * 16, cl >> 6
    ncol = 16 if depth == 0 else 256
    clut = [v15(clx + i, cly) for i in range(ncol)] if depth < 2 else None

    out = Image.new("RGBA", (args.w, args.h)); op = out.load()
    for y in range(args.h):
        for x in range(args.w):
            if depth == 0:      # 4-bit: 4 texels per halfword
                idx = (v15(tx + x // 4, ty + y) >> ((x % 4) * 4)) & 0xf
                pv = clut[idx]; transparent = (idx == 0)
            elif depth == 1:    # 8-bit: 2 texels per halfword
                idx = (v15(tx + x // 2, ty + y) >> ((x % 2) * 8)) & 0xff
                pv = clut[idx]; transparent = (idx == 0)
            else:               # 15/16-bit direct
                pv = v15(tx + x, ty + y); transparent = ((pv & 0x7fff) == 0)
            op[x, y] = (0, 0, 0, 0) if transparent else \
                ((pv & 0x1f) << 3, ((pv >> 5) & 0x1f) << 3, ((pv >> 10) & 0x1f) << 3, 255)
    out.save(args.out)
    op2 = out.load()
    opaque = sum(1 for y in range(args.h) for x in range(args.w) if op2[x, y][3])
    print("wrote %s (%dx%d, depth=%s, tpage->(%d,%d), clut->(%d,%d), %d opaque)"
          % (args.out, args.w, args.h, ["4bit","8bit","15bit","15bit"][depth], tx, ty, clx, cly, opaque))

if __name__ == "__main__":
    main()
