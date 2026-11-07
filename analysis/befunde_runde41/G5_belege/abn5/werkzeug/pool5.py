# Abnahme 5: RE2-FX-Pool 0x800D8CF0 (96 x 0x7C) je Savestate - lebende Plaetze (Status +0x18 != 0)
import re2ram5 as R, sys
def pool(i):
    r = R.ram(i); out = []
    for k in range(96):
        a = 0x800D8CF0 + k * 0x7C
        st = R.u16(r, a + 0x18)
        if not st: continue
        out.append(dict(k=k, bank=R.u8(r, a + 0x1C), sub=R.u8(r, a + 0x1E), st=st, opA=R.u8(r, a), opB=R.u8(r, a + 1), s2=R.u8(r, a + 2),
                        vy=R.s16(r, a + 0x0E), ly=R.s16(r, a + 0x26), tp=R.u16(r, a + 0x2A), cl=R.u16(r, a + 0x32), sk=R.u16(r, a + 0x3A),
                        an=R.u8(r, a + 0x21), cd=R.u8(r, a + 0x20), o=(R.s16(r, a + 0x2C), R.s16(r, a + 0x2E), R.s16(r, a + 0x30)),
                        w=(R.s16(r, a + 0x34), R.s16(r, a + 0x36), R.s16(r, a + 0x38))))
    return out
if __name__ == '__main__':
    for i in [int(x) for x in sys.argv[1:]]:
        p = pool(i)
        print('s%02d: %d lebende Plaetze' % (i, len(p)))
        for e in p: print('  ', e)
