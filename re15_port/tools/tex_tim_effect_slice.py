#!/usr/bin/env python3
"""tex_tim_effect_slice.py — byte-true slice of a GLOBAL effect sprite page out of DATA/TEX.TIM.

The global effect bank `DATA/CORE00.ESP` (ids {3,8,0,2,4}) stores only anim/coord records plus a
`word1 = (tpage<<16)|clut` per effect — its SPRITE PAGES are not in the ESP. They are uploaded to
VRAM at boot. Previously (2026-07) they were recovered from a DuckStation ShowVRAM screenshot
(`vram_png_to_tim.py`). That route is LOSSY: the ShowVRAM PNG has no alpha channel, so VRAM
halfword bit 15 is dropped — and for a 4bpp page bit 15 is the high bit of every 4th texel index,
so indices 8..15 silently collapse to 0..7 there.

This tool sources the same pixels from the FILE instead, byte-true including bit 15.

  Proof that the file route is exact (2026-08-21, re-runnable via --verify-vram):
    DATA/TEX.TIM image block: @file 0x620, 320 halfwords x 256 rows, 4bpp, uploads to VRAM(0,0).
    Its halfword columns 192..319 rows 0..255 are byte-identical (all 32768 halfwords, full 16-bit
    minus the alpha-less bit 15) to VRAM(896..1023, 256..511) in `extracted_fx/
    vram_view_ground_truth.png` -- i.e. the boot installer blits that 128x256 halfword rect
    straight to (896,256). (The extracted_fx README's claim that the upload is "repacked" and
    cannot be sliced row-for-row is hereby REFUTED: 0/32768 mismatches at srcx=192.)
    TEX.TIM CLUT block: @file 0x14, 32 halfwords x 24 rows, uploads to VRAM(256,480).

  Effect ids and their CORE00.ESP word1 (u32 LE at the head of each effect block):
    id 3 @ESP 0x00c = 0x001e7811  tpage 0x001e -> VRAM(896,256)  clut 0x7811 -> VRAM(272,480) smoke
    id 8 @ESP 0x62c = 0x001e7911  tpage 0x001e -> VRAM(896,256)  clut 0x7911 -> VRAM(272,484) FIRE
    id 0 @ESP 0x828 = 0x001f7951  tpage 0x001f -> VRAM(960,256)  clut 0x7951 -> VRAM(272,485) blood
    id 2 @ESP 0xf04 = 0x001f7a51  tpage 0x001f -> VRAM(960,256)  clut 0x7a51 -> VRAM(272,489)
    id 4 @ESP 0x172c = 0x001f7ad1 tpage 0x001f -> VRAM(960,256)  clut 0x7ad1 -> VRAM(272,491)

  Usage (effect-8 fire, the ROOM1090 burning-debris sheet):
    python tex_tim_effect_slice.py --tex ../shared_assets/PSX/DATA/TEX.TIM \
        --word1 0x001e7911 --out ../shared_assets/extracted_fx/effect8_fire.tim

Pure stdlib.
"""
import argparse, struct, sys

# --- TEX.TIM layout constants (parsed, not hardcoded -- these are the expected values) ---
TEX_VRAM_X = 0            # image block DX
TEX_VRAM_Y = 0            # image block DY


def parse_tim(path):
    d = open(path, "rb").read()
    magic, flag = struct.unpack_from("<II", d, 0)
    if magic != 0x10:
        raise SystemExit("%s: not a TIM (magic %08x)" % (path, magic))
    off = 8
    clut = None
    if flag & 8:
        bnum, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, off)
        clut = dict(off=off + 12, x=cx, y=cy, w=cw, h=ch)
        off += bnum
    bnum, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, off)
    img = dict(off=off + 12, x=ix, y=iy, w=iw, h=ih)
    return d, flag & 3, clut, img


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tex", required=True, help="path to DATA/TEX.TIM")
    ap.add_argument("--word1", type=lambda s: int(s, 0), required=True,
                    help="CORE00.ESP effect word1 = (tpage<<16)|clut")
    ap.add_argument("--w", type=int, default=256, help="page width in texels (default 256)")
    ap.add_argument("--h", type=int, default=256, help="page height in texels (default 256)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--verify-vram", default=None,
                    help="optional ShowVRAM 1024x512 PNG: assert the sliced halfwords match it (low 15 bits)")
    args = ap.parse_args()

    tpage = (args.word1 >> 16) & 0xFFFF
    cl = args.word1 & 0xFFFF
    tx, ty = (tpage & 0xF) * 64, ((tpage >> 4) & 1) * 256
    depth = (tpage >> 7) & 3                      # 0=4bit 1=8bit 2/3=16bit
    if depth != 0:
        raise SystemExit("only 4bpp effect pages implemented (tpage %04x depth %d)" % (tpage, depth))
    clx, cly = (cl & 0x3F) * 16, cl >> 6
    ncol = 16

    d, texdepth, tclut, timg = parse_tim(args.tex)
    if texdepth != 0:
        raise SystemExit("TEX.TIM is not 4bpp (depth %d)" % texdepth)
    if (timg["x"], timg["y"]) != (TEX_VRAM_X, TEX_VRAM_Y):
        raise SystemExit("TEX.TIM image block uploads to (%d,%d), expected (%d,%d)"
                         % (timg["x"], timg["y"], TEX_VRAM_X, TEX_VRAM_Y))

    # VRAM(x,y) -> TEX.TIM halfword. The boot installer blits TEX halfword cols 192..319 rows
    # 0..255 to VRAM(896,256); i.e. VRAM x -= 704, VRAM y -= 256 within that rect.
    BLIT_DX, BLIT_DY = 896 - 192, 256 - 0         # = (704, 256)

    def tex_hw(vx, vy):
        hx, hy = vx - BLIT_DX, vy - BLIT_DY
        if not (0 <= hx < timg["w"] and 0 <= hy < timg["h"]):
            raise SystemExit("VRAM(%d,%d) outside the TEX.TIM blit rect" % (vx, vy))
        return struct.unpack_from("<H", d, timg["off"] + (hy * timg["w"] + hx) * 2)[0]

    def tex_clut(vx, vy):
        cxr, cyr = vx - tclut["x"], vy - tclut["y"]
        if not (0 <= cxr < tclut["w"] and 0 <= cyr < tclut["h"]):
            raise SystemExit("CLUT VRAM(%d,%d) outside TEX.TIM CLUT block" % (vx, vy))
        return struct.unpack_from("<H", d, tclut["off"] + (cyr * tclut["w"] + cxr) * 2)[0]

    hw_per_row = args.w // 4
    pix = bytearray()
    first_off = timg["off"] + ((ty - BLIT_DY) * timg["w"] + (tx - BLIT_DX)) * 2
    for y in range(args.h):
        for hx in range(hw_per_row):
            pix += struct.pack("<H", tex_hw(tx + hx, ty + y))
    clut = bytearray()
    clut_first_off = tclut["off"] + ((cly - tclut["y"]) * tclut["w"] + (clx - tclut["x"])) * 2
    for i in range(ncol):
        clut += struct.pack("<H", tex_clut(clx + i, cly))

    if args.verify_vram:
        sys.path.insert(0, __file__.rsplit("\\", 1)[0].rsplit("/", 1)[0])
        from vram_png_to_tim import read_png_rgba
        VW, VH, rgba = read_png_rgba(args.verify_vram)

        def v15(x, y):
            o = (y * VW + x) * 4
            return (rgba[o] >> 3) | ((rgba[o + 1] >> 3) << 5) | ((rgba[o + 2] >> 3) << 10)
        bad = 0
        for y in range(args.h):
            for hx in range(hw_per_row):
                a = struct.unpack_from("<H", pix, (y * hw_per_row + hx) * 2)[0]
                if (a & 0x7FFF) != (v15(tx + hx, ty + y) & 0x7FFF):
                    bad += 1
        badc = sum(1 for i in range(ncol)
                   if (struct.unpack_from("<H", clut, i * 2)[0] & 0x7FFF) != (v15(clx + i, cly) & 0x7FFF))
        print("verify-vram: pixel mismatches %d/%d, clut mismatches %d/%d"
              % (bad, args.h * hw_per_row, badc, ncol))
        if bad or badc:
            raise SystemExit("VERIFY FAILED -- the file slice does not match VRAM")

    tim = bytearray()
    tim += struct.pack("<I", 0x00000010)
    tim += struct.pack("<I", 0x00000008)          # 4bpp + CLUT
    tim += struct.pack("<IHHHH", 12 + len(clut), clx, cly, ncol, 1) + clut
    tim += struct.pack("<IHHHH", 12 + len(pix), tx, ty, hw_per_row, args.h) + pix
    open(args.out, "wb").write(tim)

    stp = sum(1 for i in range(0, len(pix), 2)
              if struct.unpack_from("<H", pix, i)[0] & 0x8000)
    opaque = sum(1 for i in range(0, len(pix), 2)
                 for nib in (pix[i] & 0xF, pix[i] >> 4, pix[i + 1] & 0xF, pix[i + 1] >> 4)
                 if nib != 0)
    print("wrote %s (%d bytes) 4bpp %dx%d" % (args.out, len(tim), args.w, args.h))
    print("  word1 0x%08x -> tpage 0x%04x VRAM(%d,%d) | clut 0x%04x VRAM(%d,%d)"
          % (args.word1, tpage, tx, ty, cl, clx, cly))
    print("  source TEX.TIM: pixels @file 0x%x (stride %d B), clut @file 0x%x (%d B)"
          % (first_off, timg["w"] * 2, clut_first_off, ncol * 2))
    print("  %d/%d opaque texels, %d halfwords carry bit15(STP)" % (opaque, args.w * args.h, stp))


if __name__ == "__main__":
    main()
