import struct, sys
p = sys.argv[1]
d = open(p,'rb').read()
s = struct.unpack_from('<I', d, 0x20)[0]
cx, cz = struct.unpack_from('<HH', d, s)
cnt = struct.unpack_from('<5I', d, s+4)
print("SCA @0x%x ceiling (%d,%d) counts %s" % (s, cx, cz, cnt))
off = s + 24
q = 0; k = 0
for g in range(5):
    for i in range(cnt[g]):
        w, dn, x, z, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, off)
        print("q%d #%d x %6d..%6d z %6d..%6d type %d u0 %02x u1 %02x floor %02x band %d" % (g, k, x, x+w, z, z+dn, t, u0, u1, fl, fl>>4))
        off += 12; k += 1
