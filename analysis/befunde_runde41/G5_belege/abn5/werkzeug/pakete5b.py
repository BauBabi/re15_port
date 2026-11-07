# Abnahme 5: RE2-Pakete der Lampe (Objekt 4) im RAM s04 - POLY_GT3 (40 B, Code 0x34) / POLY_GT4 (52 B, Code 0x3C), Doppelpuffer
# verschraenkt (Schritt 2 x 40 / 2 x 52); Paketlisten-Zeiger Objekt +0x18 (Dreiecke) / +0x20 (Vierecke).
import re2ram5 as R, sys, json
s = int(sys.argv[1]); obj = int(sys.argv[2])
r = R.ram(s)
o = 0x800d0324 + obj * 0x1F8
def xy(w): x = w & 0xffff; y = w >> 16; return (x - 65536 if x > 32767 else x, y - 65536 if y > 32767 else y)
def rgb(w): return (w & 0xff, (w >> 8) & 0xff, (w >> 16) & 0xff)
out = {'x': R.s32(r, o + 0x38), 'x68': R.s32(r, o + 0x68), 'T': {}, 'Q': {}}
for buf in (0, 1):
    p = R.u32(r, o + 0x18) + buf * 40
    for i in range(18):
        q = p + i * 0x50
        out['T'].setdefault(buf, []).append(([rgb(R.u32(r, q + k)) for k in (4, 0x10, 0x1c)], [xy(R.u32(r, q + k)) for k in (8, 0x14, 0x20)], R.u8(r, q + 7)))
    p = R.u32(r, o + 0x20) + buf * 52
    for i in range(19):
        q = p + i * 104
        out['Q'].setdefault(buf, []).append(([rgb(R.u32(r, q + k)) for k in (4, 0x10, 0x1c, 0x28)], [xy(R.u32(r, q + k)) for k in (8, 0x14, 0x20, 0x2c)], R.u8(r, q + 7)))
json.dump(out, open(sys.argv[3], 'w'))
print('Objekt', obj, 'x', out['x'], 'x68', out['x68'])
for buf in (0,):
    for i, (c, v, code) in enumerate(out['T'][buf]): print(' T%02d %02x %s %s' % (i, code, c, v))
    for i, (c, v, code) in enumerate(out['Q'][buf]): print(' Q%02d %02x %s %s' % (i, code, c, v))
