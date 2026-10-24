# Zensus 2: wie npc_ton_retail.py, aber "hoerbar"/"spielt" = Satz mit Ton, dessen VAG != 0 (SsUtKeyOnV `lhu a0,22(v0)` @0x80059f2c /
# `bne a0,zero` @0x80059f8c) und <= Zahl der VAGs (VH +22) - wie re15_footstep_vag. Stempel-Zeilen getrennt gezaehlt.
import struct, sys, os
ASSETS = sys.argv[1]; rooms = sys.argv[2].split(',')
def u32(b, o): return struct.unpack_from('<I', b, o)[0]
tot = {}; stumm_trocken = []
for r in rooms:
    b = open(os.path.join(ASSETS, 'STAGE%s' % r[4], r + '.RDT'), 'rb').read()
    edt = u32(b, 0x08); vh = u32(b, 0x0C); vb = u32(b, 0x10); flr = u32(b, 0x34)
    nedt = (vh - edt) // 4
    VH = b[vh:vb]
    def spielt(i):
        if i < 0 or i >= nedt: return False
        e = b[edt + i*4: edt + i*4 + 4]
        if e[2] == 0 and e[3] == 0: return False
        prog, ton = e[1] & 0x7f, e[2] >> 4
        progs = min(struct.unpack_from('<H', VH, 18)[0], 128, (len(VH) - 0x820) // 0x200)
        if prog >= progs: return False
        off = 0x820 + prog*0x200 + ton*0x20 + 0x16
        vag = struct.unpack_from('<H', VH, off)[0]
        return vag != 0 and vag <= struct.unpack_from('<H', VH, 22)[0]
    def stimme_ok(i, fuss):
        if i < 0 or i >= nedt: return False
        v = (b[edt + i*4 + 3] & 0x1f) - 16 + (1 if fuss != 4 else 0)
        return 0 <= v <= 7
    n = struct.unpack_from('<H', b, flr)[0]
    typen = []
    for k in range(n):
        t = b[flr + 2 + k*12 + 8]
        if t not in typen: typen.append(t)
    for t in typen:
        for st in (0, 1):
            if st and (t & 0x80): continue
            boden = (t & 0x7f) - (3 if st else 0)
            for a0 in (0, 1):
                for fuss in (7, 4):
                    io_, ir = boden + a0 - 8, boden + a0
                    if stimme_ok(io_, fuss) and spielt(io_): w = 'orig'
                    elif spielt(ir): w = 'retail'
                    else: w = 'stumm'
                    key = ('stempel' if st else 'trocken/boden', w)
                    tot[key] = tot.get(key, 0) + 1
                    if w == 'stumm' and not st: stumm_trocken.append('%s 0x%02x a0=%d fuss=%d npc[%d] pl[%d]' % (r, t, a0, fuss, io_, ir))
for k in sorted(tot): print(k, tot[k])
print('stumm ohne Stempel:', len(stumm_trocken))
for z in stumm_trocken: print('  ', z)
