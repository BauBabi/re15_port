#!/usr/bin/env python3
"""Regenerate shadow_blob_data.h byte-true from the runtime VRAM shadow blob.

The blob lives at VRAM texture page (960,256), UV u=1..26 v=225..254 (26x30, 4bpp),
CLUT at (272,503) (= TEX.TIM CLUT block row23). Read via re15_ss.Ram.vpix (byte-true
VRAM base = 'GPU-VRAM' tag). Verified identical across 4 savestates. Emits ABGR8888
(0xFF|B<<16|G<<8|R) = the exact subtractive-amount encoding the PC renderer expects.
"""
import sys, os
sys.path.insert(0, os.path.abspath(".claude/skills/re15-room-capture/scripts"))
from re15_ss import Ram

def build(save):
    r = Ram(save)
    assert r.vpix(272, 503) == 0xad6c, "shadow CLUT not present in this savestate"
    def clut_rgb(i):
        p = r.vpix(272 + i, 503)
        return ((p & 0x1f) << 3, ((p >> 5) & 0x1f) << 3, ((p >> 10) & 0x1f) << 3)
    def texel(u, v):
        return (r.vpix(960 + (u >> 2), 256 + v) >> ((u & 3) * 4)) & 0xf
    vals = []
    for v in range(225, 255):          # 30 rows
        for u in range(1, 27):         # UV u=1..26 (the range the prim samples)
            R, G, B = clut_rgb(texel(u, v))
            vals.append(0xFF000000 | (B << 16) | (G << 8) | R)
    return vals

def emit(vals):
    L = []
    L.append("/* RE1.5 character shadow blob - byte-true from the runtime VRAM atlas slot")
    L.append(" * (texture page VRAM (960,256), UV u=1..26 v=225..254, 4bpp; CLUT (272,503) =")
    L.append(" * TEX.TIM CLUT block row 23). NOT from TEX.TIM's image (that lives at VRAM (0,0));")
    L.append(" * the blob is packed into the VRAM atlas by FUN_8004ee78. RGB = subtractive amount")
    L.append(" * (ABR mode 2, dst=dst-src); centre bright, edge black. Regenerated 2026-07-24 via")
    L.append(" * scratchpad/gen_shadow_blob.py (re15_ss.Ram.vpix, byte-true VRAM base); identical")
    L.append(" * across 4 savestates. Prior version was UV u=0..25 (1 texel too far left). */")
    L.append("#ifndef RE15_SHADOW_BLOB_DATA_H")
    L.append("#define RE15_SHADOW_BLOB_DATA_H")
    L.append("#define RE15_SHADOW_BLOB_W 26")
    L.append("#define RE15_SHADOW_BLOB_H 30")
    L.append("static const unsigned int re15_shadow_blob_rgba[780] = {")
    for i in range(0, len(vals), 8):
        L.append(",".join("0x%08X" % v for v in vals[i:i+8]) + ",")
    L.append("};")
    L.append("#endif")
    return "\n".join(L) + "\n"

if __name__ == "__main__":
    vals = build(sys.argv[1] if len(sys.argv) > 1 else "stage_saves/lamp_near_0.9.sav")
    out = sys.argv[2] if len(sys.argv) > 2 else "/dev/stdout"
    open(out, "w").write(emit(vals))
