"""8x8-Schrift aus DATA/TEX.TIM (4 bpp, Bild bei VRAM (704,256) laut Slot 0x1b; tpage GetTPage(0,0,0x300,0x100) @0x800295dc -> TIM-Texel x 256),
Zeichen c: u = ((c-0x20)&0x1f)*8, v = ((c-0x20)&0xe0)>>5*8 (FUN_80029214 @0x8002931c..@0x80029330), CLUT (256,480) Zeile 0 (@0x800292b4..@0x800292dc)."""
import struct, numpy as np
TIM = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/shared_assets/PSX/DATA/TEX.TIM'
def lade():
    d = open(TIM, 'rb').read()
    flags = struct.unpack_from('<I', d, 4)[0]; o = 8
    n, cx, cy, cw, ch = struct.unpack_from('<IHHHH', d, o)
    clut = np.frombuffer(d[o + 12:o + 12 + cw * ch * 2], '<u2').reshape(ch, cw); o += n
    n, ix, iy, iw, ih = struct.unpack_from('<IHHHH', d, o)
    raw = np.frombuffer(d[o + 12:o + 12 + iw * ih * 2], np.uint8).reshape(ih, iw * 2)
    idx = np.zeros((ih, iw * 4), np.uint8); idx[:, 0::2] = raw & 15; idx[:, 1::2] = raw >> 4
    return idx, clut
IDX, CLUT = lade()
def glyphe(c):
    k = ord(c) - 0x20
    u = 256 + (k & 0x1f) * 8; v = ((k & 0xe0) >> 5) * 8
    return IDX[v:v + 8, u:u + 8]
def farbe(i, zeile=0):
    w = int(CLUT[zeile, i]); return ((w & 31) * 255 // 31, ((w >> 5) & 31) * 255 // 31, ((w >> 10) & 31) * 255 // 31, w)
