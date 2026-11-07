# Abnahme 5: RE2-Lampenobjekte 3/4 (0x800d0324 + n*0x1F8) je Savestate gegen die Lampen-Messzeile der eigenen exe-Laeufe
import re2ram5 as R, re, sys
# Szenenbild je Savestate (Zuordnung Abnahme 4 2.2, dort ueber den G5-Zustand bestimmt)
BILD = {3: 31, 4: 80, 5: 129, 6: 165, 7: 215, 8: 264, 9: 313, 10: 363, 11: 412, 12: 441}
def obj(r, n):
    o = 0x800d0324 + n * 0x1F8
    return (R.s32(r, o + 0x38), R.s32(r, o + 0x3C), R.s32(r, o + 0x40), R.s16(r, o + 0x74) & 0xfff, R.s16(r, o + 0x76) & 0xfff, R.s16(r, o + 0x78) & 0xfff, R.u32(r, o) )
def port(lauf):
    d = {}
    for z in open(lauf + '/debug.log', encoding='latin-1'):
        m = re.match(r'\[g5lampe\] F(\d+) Zustand Vorlauf (-?\d+) Bild (-?\d+): Obj 3 (an|park) x (-?\d+), Obj 4 (an|park) x (-?\d+)', z)
        if m: d[int(m.group(1))] = (int(m.group(2)), int(m.group(3)), int(m.group(5)), int(m.group(7)))
    return d
def g5_t(r):
    g5 = R.u32(r, 0x800cfe1c)
    return R.u8(r, g5 + 5), R.u8(r, g5 + 6), R.s16(r, g5 + 0x158)
L = sys.argv[1:]
P = [port(l) for l in L]
arm = [min(f for f, v in p.items() if v[1] == 0) for p in P]
print('Armierung (Bild 0) im Lauf:', ['F%d' % a for a in arm])
ok = n = 0
for s in range(1, 14):
    r = R.ram(s)
    o3, o4 = obj(r, 3), obj(r, 4)
    sub, ph, t = g5_t(r)
    zeile = 's%02d G5 sub %d ph %d t %d | Obj3 x %d y %d z %d rot %03x %03x %03x w0 %08x | Obj4 x %d' % (s, sub, ph, t, o3[0], o3[1], o3[2], o3[3], o3[4], o3[5], o3[6], o4[0])
    if s in BILD:
        b = BILD[s]
        for i, p in enumerate(P):
            v = p.get(arm[i] + b)
            if v:
                x3 = v[2] if v[2] != -32000 else -32000; x4 = v[3]
                gl = (x3 == o3[0] and x4 == o4[0]); n += 1; ok += gl
                zeile += ' || %s Bild %d: Port Obj3 %d Obj4 %d %s' % (L[i].split('/')[-1], b, x3, x4, 'gleich' if gl else 'ABWEICHUNG')
    print(zeile)
print('gleich %d von %d' % (ok, n))
