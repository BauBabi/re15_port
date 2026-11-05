# NB6: RE2-Primitivpakete der Lampenobjekte aus dem RAM (FUN_80027bec: Dreiecke *(obj+0x18) + Puffer*40, Schritt 0x50 @0x80027d94,
# RGB +4/+0x10/+0x1C @0x80027d9c-a4, XY +8/+0x14/+0x20 @0x80027ce8-f0; Vierecke *(obj+0x20) + Puffer*52, FUN_80027dbc).
import sys, struct
sys.path.insert(0, ".")
from re2ram4 import Ram, SN
n = int(sys.argv[1]); obj = int(sys.argv[2])
r = Ram(SN % n)
o = 0x800d0324 + obj * 0x1f8
print("s%02d Objekt %d: +0x38 %d +0x68 %d  +0x14 %08x +0x18 %08x +0x1c %08x +0x20 %08x  Puffer-Index %d" % (n, obj, r.s32(o+0x38), r.s32(o+0x68),
      r.u32(o+0x14), r.u32(o+0x18), r.u32(o+0x1c), r.u32(o+0x20), r.u8(0x800ce5e0)))
def xy(w): x = w & 0xffff; y = w >> 16; return (x - 65536 if x > 32767 else x, y - 65536 if y > 32767 else y)
def rgb(w): return (w & 0xff, (w >> 8) & 0xff, (w >> 16) & 0xff, w >> 24)
for buf in (0, 1):
    p = r.u32(o + 0x18) + buf * 40
    print(" Dreiecke Puffer %d:" % buf)
    for i in range(18):
        q = p + i * 0x50
        c = [rgb(r.u32(q + k)) for k in (4, 0x10, 0x1c)]; v = [xy(r.u32(q + k)) for k in (8, 0x14, 0x20)]
        print("  T%02d code %02x rgb %s xy %s" % (i, c[0][3], [x[:3] for x in c], v))
    p = r.u32(o + 0x20) + buf * 52
    print(" Vierecke Puffer %d:" % buf)
    for i in range(19):
        q = p + i * 104
        c = [rgb(r.u32(q + k)) for k in (4, 0x10, 0x1c, 0x28)]; v = [xy(r.u32(q + k)) for k in (8, 0x14, 0x20, 0x2c)]
        print("  Q%02d code %02x rgb %s xy %s" % (i, c[0][3], [x[:3] for x in c], v))
