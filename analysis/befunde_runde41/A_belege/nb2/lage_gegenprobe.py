# Unabhaengige Python-Nachbildung von FUN_80045a64 (Disasm-Lesung N2.2) fuer einen Schritt aus npc.log: Kamera = RID-Satz
# (RDT +0x24) des Cuts, SquareRoot0 hier als floor(sqrt) (die BIOS-Wurzel unterschaetzt -> Abstand kann 1 kleiner sein).
import struct, sys, math, re
rdt = open(sys.argv[1], 'rb').read()
cut = int(sys.argv[2]); px, py, pz = (int(v) for v in sys.argv[3].split(','))
exe = open(sys.argv[4], 'rb').read(); t = struct.unpack_from('<I', exe, 0x18)[0]
tab = exe[0x80074728 - t + 0x800: 0x80074728 - t + 0x800 + 256]
rid = struct.unpack_from('<I', rdt, 0x24)[0]
c = struct.unpack_from('<HHiiiiii', rdt, rid + cut * 32)
cx, cy, cz, tx, ty, tz = c[2], c[3], c[4], c[5], c[6], c[7]
W = [511, 302, 159, 81, 41, 20, 10, 5, 3, 1, 0, 0]
def catan(a):
    x, y, z = 0x1000, a, 0
    for i in range(12):
        xs, ys = x >> i, y >> i
        if y >= 0: x, y, z = x + ys, y - xs, z + W[i]
        else: x, y, z = x - ys, y + xs, z - W[i]
    return z
def az(x1, z1, x2, z2):
    dx, dz = x2 - x1, z2 - z1
    if dx == 0: return 0xc00 if dz > 0 else 0x400
    n_ = dz << 12; q = abs(n_) // abs(dx); q = q if (n_ >= 0) == (dx > 0) else -q   # C-Division: Richtung 0
    return ((0x1000 if dx >= 0 else 0x800) - catan(q)) & 0xfff
o_sq = 0x8007d984 - t + 0x800   # SquareRoot0-Tabelle @0x8007d984 (192 x u16) direkt aus PSX.EXE
SQ = list(struct.unpack_from('<192H', exe, o_sq))
assert len(SQ) == 192
def lzcr(x):
    y = (~x) & 0xffffffff if x & 0x80000000 else x
    if y == 0: return 32
    n = 0
    while not (y & 0x80000000): y = (y << 1) & 0xffffffff; n += 1
    return n
def sr0(x):
    x &= 0xffffffff
    if x == 0: return 0
    v0 = lzcr(x)
    if v0 == 32: return 0
    t2 = v0 & ~1; t1 = (31 - t2) >> 1
    t4 = ((x << (t2 - 24)) & 0xffffffff) if t2 >= 24 else (x >> (24 - t2))
    return ((SQ[t4 - 64] << t1) & 0xffffffff) >> 12
dx, dz, dy = abs(cx - px), abs(cz - pz), abs(cy - py)
r1 = sr0(dx*dx + dz*dz); dyt = cy - dy
r = sr0(dyt*dyt + r1*r1); di = min(r // 250, 127)
h = az(cx, cz, px, pz) - az(0, 0, cx, cz); tt = az(cx, cz, tx, tz) - az(0, 0, cx, cz)
def s16(v): v &= 0xffff; return v - 0x10000 if v & 0x8000 else v
rel = (h + 0x1000 - tt) if s16(h) < s16(tt) else (h - tt)
rel &= 0xffffffff
if s16(rel) in (0, 0x1000, 0x800) or (rel & 0xffff) == 0x1000:
    voll = volr = 0x89
else:
    bit = 1 if rel & 0x800 else 0
    sp = bit
    if ((rel - 0x401) & 0xffff) < 0x7ff: sp = 1 - bit
    i = (rel & 0x3ff) >> 3
    if sp: i = 0x89 - i
    pan = tab[i]
    voll, volr = (0x89, pan) if bit else (pan, 0x89)
att = tab[128 + di]
print('Kamera', (cx, cy, cz), '->', (tx, ty, tz), 'Abstand', di, 'lage', (voll - att) & 0x7f, '/', (volr - att) & 0x7f)
