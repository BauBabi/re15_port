import struct, sys
p = sys.argv[1]
xr = (int(sys.argv[2]), int(sys.argv[3])) if len(sys.argv) > 3 else None
zr = (int(sys.argv[4]), int(sys.argv[5])) if len(sys.argv) > 5 else None
d = open(p, 'rb').read()
sca = struct.unpack_from('<I', d, 0x20)[0]
cnt = [struct.unpack_from('<I', d, sca + 4 + 4 * g)[0] for g in range(5)]
tot = sum(cnt)
print('SCA @0x%X counts %s total %d' % (sca, cnt, tot))
def hit(x1, x2, z1, z2):
    if xr and (x2 < xr[0] or x1 > xr[1]): return False
    if zr and (z2 < zr[0] or z1 > zr[1]): return False
    return True
for i in range(tot):
    o = sca + 24 + 12 * i
    w, dn, x, z, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, o)
    if hit(x, x + w, z, z + dn):
        print('  sca %3d @0x%05X x %6d..%6d z %6d..%6d type %d u0 %02x u1 %02x floor %d' % (i, o, x, x + w, z, z + dn, t, u0, u1, fl))
rvd = struct.unpack_from('<I', d, 0x28)[0]
o = rvd; k = 0
print('RVD @0x%X' % rvd)
while o + 20 <= len(d):
    mk, cf, ct = struct.unpack_from('<HBB', d, o)
    if mk == 0xffff and cf == 0xff and ct == 0xff: break
    xs = [struct.unpack_from('<h', d, o + 4 + 4 * i)[0] for i in range(4)]
    zs = [struct.unpack_from('<h', d, o + 6 + 4 * i)[0] for i in range(4)]
    if hit(min(xs), max(xs), min(zs), max(zs)):
        print('  rvd %2d @0x%05X mk %04x cam %d -> %d  x %s z %s' % (k, o, mk, cf, ct, xs, zs))
    o += 20; k += 1
