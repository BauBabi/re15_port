import struct, sys
exe = open(sys.argv[1], 'rb').read()
t = struct.unpack_from('<I', exe, 0x18)[0]
fo = lambda a: a - t + 0x800
for pg in map(int, sys.argv[2:]):
    cnt = struct.unpack_from('<H', exe, fo(0x80076840 + 8 * pg))[0]
    ptr = struct.unpack_from('<I', exe, fo(0x80076840 + 8 * pg + 4))[0]
    print("Blatt %d: %d Rechtecke @%08x" % (pg, cnt, ptr))
    for i in range(cnt):
        x, y, w, h = struct.unpack_from('<hhhh', exe, fo(ptr + 12 * i))
        u, _, v, _ = struct.unpack_from('<BBBB', exe, fo(ptr + 12 * i + 8))
        print("  r%d (%d,%d,%d,%d) uv(%d,%d)" % (i, x, y, w, h, u, v))
