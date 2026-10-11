# Baut das RE2-Kartenblatt einer Area aus MAPS.PIX (Block = area cd-file, 256x256 4bpp) und den
# Raum-Eintraegen (@0x800aaa38) zusammen: jede Kachel an (x,y), Graustufe je Index.
import struct, sys
from PIL import Image, ImageDraw
exe = open(sys.argv[1], 'rb').read(); pix = open(sys.argv[2], 'rb').read()
area = int(sys.argv[3]); out = sys.argv[4]; scale = int(sys.argv[5]) if len(sys.argv) > 5 else 3
t = struct.unpack_from('<I', exe, 0x18)[0]
fo = lambda a: a - t + 0x800
ptr, cnt, mib, cd = struct.unpack_from('<IBBH', exe, fo(0x800aaa38 + 8 * area))
blk = pix[cd * 32768:(cd + 1) * 32768]
def idx(u, v):
    b = blk[v * 128 + u // 2]
    return (b >> 4) if (u & 1) else (b & 15)
pal = [(0,0,0),(40,110,40),(70,150,70),(230,200,40),(200,200,200),(150,150,150),(100,100,100),(60,60,60),
       (255,0,0),(0,255,0),(0,0,255),(255,255,0),(255,0,255),(0,255,255),(120,30,30),(30,30,120)]
im = Image.new('RGB', (320, 240), (0, 0, 40))
for i in range(cnt):
    e = exe[fo(ptr + 16 * i): fo(ptr + 16 * i) + 16]
    u, v, w, h = e[0], e[1], e[2], e[3]
    x, y = struct.unpack_from('<hh', e, 4)
    for yy in range(h):
        for xx in range(w):
            k = idx(u + xx, v + yy)
            if k and 0 <= x + xx < 320 and 0 <= y + yy < 240:
                im.putpixel((x + xx, y + yy), pal[k])
im = im.resize((320 * scale, 240 * scale), Image.NEAREST)
d = ImageDraw.Draw(im)
for i in range(cnt):
    e = exe[fo(ptr + 16 * i): fo(ptr + 16 * i) + 16]
    x, y = struct.unpack_from('<hh', e, 4)
    d.text((x * scale + 2, y * scale + 2), "%d/v%d" % (i, e[12]), fill=(255, 128, 0))
im.save(out); print(out, cnt)
