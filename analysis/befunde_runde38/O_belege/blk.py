import struct, sys
p = sys.argv[1]
d = open(p, 'rb').read()
blk = struct.unpack_from('<I', d, 0x38)[0]
n = struct.unpack_from('<I', d, blk)[0] & 0xffff
print('BLK @0x%X n=%d' % (blk, n))
for i in range(n):
    o = blk + 4 + i * 12
    x1, z1, x2, z2, r, links = struct.unpack_from('<hhhhHH', d, o)
    nb = [j for j in range(16) if links & (1 << j)]
    print('  zone %2d @0x%05X x %6d..%6d z %6d..%6d resv %04x links %s' % (i, o, x1, x2, z1, z2, r, nb))
