import struct, sys
from PIL import Image, ImageDraw
rdt, out = sys.argv[1], sys.argv[2]
band = int(sys.argv[3]) if len(sys.argv) > 3 else 4
d = open(rdt, 'rb').read()
sca = struct.unpack_from('<I', d, 0x20)[0]
cx, cz = struct.unpack_from('<HH', d, sca)
cnt = struct.unpack_from('<5I', d, sca + 4)
cells = []
o = sca + 24
for g in range(5):
    for i in range(cnt[g]):
        w, dd, x, z, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, o); o += 12
        cells.append((g, i, x, z, w, dd, t, u0, u1, fl))
X0, X1, Z0, Z1 = -6000, 28000, -30000, 8000
S = 0.03
W, H = int((X1 - X0) * S), int((Z1 - Z0) * S)
im = Image.new('RGB', (W, H), (255, 255, 255)); dr = ImageDraw.Draw(im)
def P(x, z): return ((x - X0) * S, (z - Z0) * S)
seen = set()
for g, i, x, z, w, dd, t, u0, u1, fl in cells:
    key = (x, z, w, dd, fl)
    b = fl >> 4
    col = (255, 120, 120) if b == band else (215, 215, 215)
    if key in seen and b != band: continue
    seen.add(key)
    dr.rectangle([P(x, z), P(x + w, z + dd)], outline=col if b == band else (190, 190, 190), width=2 if b == band else 1)
    if b == band:
        dr.text(P(x + 50, z + 50), '%d' % i, fill=(160, 0, 0))
# Objekte
dr.rectangle([P(12250 - 900, -12400 - 900), P(12250 + 900, -12400 + 900)], outline=(0, 0, 255), width=3)
dr.text(P(12250 - 600, -12400), 'Kiste', fill=(0, 0, 255))
dr.line([P(7942, -20510), P(14422, -20510)], fill=(0, 160, 0), width=4)
dr.text(P(9000, -20300), 'Rolladen obj1', fill=(0, 120, 0))
dr.rectangle([P(14900, -20400), P(15700, -19600)], outline=(255, 160, 0), width=3); dr.text(P(14900, -19500), 'Schalter', fill=(200, 100, 0))
dr.rectangle([P(16000, -21550), P(17000, -20550)], outline=(160, 0, 160), width=3); dr.text(P(16000, -21900), 'Leiter', fill=(160, 0, 160))
dr.ellipse([P(9648 - 200, -20700 - 200), P(9648 + 200, -20700 + 200)], outline=(0, 0, 0), width=3); dr.text(P(9800, -21300), 'Kam 11', fill=(0, 0, 0))
for gx in range(-6000, 28001, 2000):
    dr.text(P(gx, Z0 + 200), str(gx), fill=(100, 100, 100))
for gz in range(-30000, 8001, 2000):
    dr.text(P(X0 + 50, gz), str(gz), fill=(100, 100, 100))
im.save(out)
print('cells', len(cells), cnt, 'ceiling', cx, cz)
for g, i, x, z, w, dd, t, u0, u1, fl in cells:
    if g == 0 and (fl >> 4) == band:
        print('r%d[%d] x %d..%d z %d..%d typ %d u0 %02x u1 %02x fl %02x' % (g, i, x, x + w, z, z + dd, t, u0, u1, fl))
