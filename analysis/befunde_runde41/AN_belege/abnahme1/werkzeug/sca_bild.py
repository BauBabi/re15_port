"""SCA-Zellen + Tuer-Rechtecke + Spielerweg (state.log) eines Raums als Bild. Nur lesen."""
import struct, sys, re
from PIL import Image, ImageDraw

rdt_pfad, ziel = sys.argv[1], sys.argv[2]
rects = []      # "x,z,w,d,name"
logs = []
for a in sys.argv[3:]:
    if a.startswith('rect='):
        rects.append(a[5:].split(','))
    elif a.startswith('log='):
        logs.append(a[4:])
rdt = open(rdt_pfad, 'rb').read()
s = struct.unpack_from('<I', rdt, 0x20)[0]
counts = struct.unpack_from('<5I', rdt, s + 4)
n = sum(counts)
zellen = []
for i in range(n):
    w, dep, x, z = struct.unpack_from('<HHhh', rdt, s + 24 + 12 * i)
    typ = rdt[s + 24 + 12 * i + 8]
    flr = rdt[s + 24 + 12 * i + 11]
    zellen.append((x, z, w, dep, typ, flr >> 4, i))
xs = [c[0] for c in zellen] + [c[0] + c[2] for c in zellen]
zs = [c[1] for c in zellen] + [c[1] + c[3] for c in zellen]
x0, x1, z0, z1 = min(xs), max(xs), min(zs), max(zs)
sk = 1400.0 / max(x1 - x0, z1 - z0)
W, H = int((x1 - x0) * sk) + 40, int((z1 - z0) * sk) + 40
im = Image.new('RGB', (W, H), (255, 255, 255))
d = ImageDraw.Draw(im)
def P(x, z):
    return (20 + (x - x0) * sk, 20 + (z1 - z) * sk)
farben = [(120, 120, 120), (200, 120, 60), (60, 120, 200), (120, 200, 60)]
for (x, z, w, dep, typ, b, i) in zellen:
    a, c = P(x, z + dep), P(x + w, z)
    d.rectangle((a, c), outline=farben[b % 4], width=1)
    d.text((a[0] + 2, a[1] + 2), '%d/t%d/b%d' % (i, typ, b), fill=farben[b % 4])
for r in rects:
    x, z, w, dep = map(int, r[:4])
    a, c = P(x, z + dep), P(x + w, z)
    d.rectangle((a, c), outline=(220, 0, 0), width=3)
    d.text((a[0] + 2, c[1] - 12), r[4] if len(r) > 4 else '', fill=(220, 0, 0))
for lp in logs:
    pts = []
    for zeile in open(lp, errors='replace'):
        m = re.search(r'PL\((-?\d+),(-?\d+),', zeile)
        if m:
            pts.append(P(int(m.group(1)), int(m.group(2))))
    if len(pts) > 1:
        d.line(pts, fill=(0, 160, 0), width=2)
# Raster 1000
for gx in range((x0 // 1000) * 1000, x1 + 1, 2000):
    d.text((P(gx, z1)[0], 2), str(gx), fill=(0, 0, 0))
for gz in range((z0 // 1000) * 1000, z1 + 1, 2000):
    d.text((2, P(x0, gz)[1]), str(gz), fill=(0, 0, 0))
im.save(ziel)
print('Zellen', n, 'x', x0, x1, 'z', z0, z1)
