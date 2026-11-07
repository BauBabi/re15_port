# Eigener Leser (Abnahme 3): RDT-Kopf +0x24 (RID) / +0x28 (RVD), RE15_KNOWLEDGE.md 1.1; Eintraege roh als Hex mit ausgeben.
import sys, struct
for p in sys.argv[1:]:
    d = open(p, 'rb').read()
    ncut = d[1]
    rid = struct.unpack_from('<I', d, 0x24)[0]
    rvd = struct.unpack_from('<I', d, 0x28)[0]
    print('%s  Laenge %d  nCut(+1)=%d  RID(+0x24)=0x%x  RVD(+0x28)=0x%x' % (p.split('/')[-1], len(d), ncut, rid, rvd))
    for i in range(11, ncut):
        o = rid + 32 * i
        fl, fov = struct.unpack_from('<HH', d, o)
        px, py, pz, tx, ty, tz = struct.unpack_from('<6i', d, o + 4)
        print('  Cut %2d @0x%03x  %s  flag=%d fov=%d H=fov>>7=%d  Auge=(%d,%d,%d) Ziel=(%d,%d,%d)' % (
            i, o, d[o:o+28].hex(' '), fl, fov, fov >> 7, px, py, pz, tx, ty, tz))
    o = rvd
    while struct.unpack_from('<I', d, o)[0] != 0xFFFFFFFF:
        be, fl, fc, tc = d[o], d[o+1], d[o+2], d[o+3]
        xz = struct.unpack_from('<8h', d, o + 4)
        if fc >= 11:
            print('  RVD @0x%03x  %s  %2d->%2d  x %s z %s' % (o, d[o:o+20].hex(' '), fc, tc, xz[0::2], xz[1::2]))
        o += 20
    print('  RVD-Ende @0x%x' % o)
