"""Was ist der entpackte SLD-Block?  TIM-Kopf dekodieren."""
import struct, os, json, sys
sys.path.insert(0, 'analysis/esp_masken_2026-09-03')
from sld import sld_decompress

CASES = [
    ('info/Re1.5/PSX/STAGE1/ROOM102.BSS', 0, 31672),   # size 61984
    ('info/Re1.5/PSX/STAGE1/ROOM109.BSS', 1, 25196),   # size 66080
    ('info/Re1.5/PSX/STAGE5/ROOM50A.BSS', 2, 31140),
]
for path, cut, off in CASES:
    d = open(path, 'rb').read()[cut * 0x10000:(cut + 1) * 0x10000]
    sz = struct.unpack_from('<I', d, off)[0]
    out, sp = sld_decompress(d, off + 4, sz)
    magic, flag = struct.unpack_from('<II', out, 0)
    bpp = flag & 7
    p = 8
    fields = []
    if flag & 8:  # CLUT
        clen, cx, cy, cw, chh = struct.unpack_from('<IHHHH', out, p)
        fields.append('CLUT len=%d @VRAM(%d,%d) %dx%d' % (clen, cx, cy, cw, chh))
        p += clen
    ilen, ix, iy, iw, ih = struct.unpack_from('<IHHHH', out, p)
    fields.append('IMG len=%d @VRAM(%d,%d) %dx%d Halbwoerter' % (ilen, ix, iy, iw, ih))
    px = {0: iw * 4, 1: iw * 2, 2: iw, 3: iw}[bpp]
    print("%s cut%-2d off=%-6d packed=%-6d entpackt=%-6d  magic=0x%08x flag=0x%x (bpp-code %d)" %
          (os.path.basename(path), cut, off, sp - (off + 4), sz, magic, flag, bpp))
    for f in fields:
        print("      ", f)
    print("       -> Bildflaeche %d x %d Pixel (%s)" %
          (px, ih, {0: '4bpp', 1: '8bpp', 2: '16bpp', 3: '24bpp'}[bpp]))
    print("       Kopf-Bytes: %s" % out[:32].hex())
