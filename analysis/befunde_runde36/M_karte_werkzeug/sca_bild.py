# SCA-Zellen eines Raums als Bild (Band b), Tueren rot, Spawns gruen, Flutbereich hellblau.
# sca_bild.py <tools> <raum> <band> <out.png> [x,z ...]
import sys, collections
sys.path.insert(0, sys.argv[1])
import gen_map_zones as G
from PIL import Image, ImageDraw
rid = int(sys.argv[2], 16); band = int(sys.argv[3]); out = sys.argv[4]
pts = [tuple(map(int, p.split(','))) for p in sys.argv[5:]]
sca, doors, stairs = G.read_rdt(rid)
cells = [c for c in sca if (c[4] >> 4) == band]
xs = [c[0] for c in cells] + [c[0] + c[2] for c in cells]
zs = [c[1] for c in cells] + [c[1] + c[3] for c in cells]
x0, x1, z0, z1 = min(xs) - 1000, max(xs) + 1000, min(zs) - 1000, max(zs) + 1000
K = 40.0  # Einheiten je Pixel
W, H = int((x1 - x0) / K) + 1, int((z1 - z0) / K) + 1
im = Image.new('RGB', (W, H), (0, 0, 0)); d = ImageDraw.Draw(im)
P = lambda x, z: ((x - x0) / K, (z1 - z) / K)   # z nach oben
for (x, z, w, dp, flr, typ, u0) in cells:
    a = P(x, z + dp); b = P(x + w, z)
    d.rectangle([a, b], fill=(140, 140, 140) if (u0 & 1) else (60, 60, 120), outline=(200, 200, 200))
for dr in doors:
    a = P(dr['rx'], dr['rz'] + dr['rd']); b = P(dr['rx'] + dr['rw'], dr['rz'])
    d.rectangle([a, b], outline=(255, 0, 0))
    d.text(P(dr['rx'], dr['rz']), "%04X" % dr['dest'], fill=(255, 80, 80))
for st in stairs:
    c = P(st['x'], st['z']); d.ellipse([c[0]-3, c[1]-3, c[0]+3, c[1]+3], outline=(255, 255, 0))
for p in pts:
    c = P(*p); d.ellipse([c[0]-4, c[1]-4, c[0]+4, c[1]+4], fill=(0, 255, 0))
im.save(out); print(out, W, H, "x %d..%d z %d..%d (oben = +z)" % (x0, x1, z0, z1))
