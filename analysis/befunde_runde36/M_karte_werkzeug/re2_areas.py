import struct, sys, collections
exe = open(sys.argv[1], 'rb').read()
t = struct.unpack_from('<I', exe, 0x18)[0]
def fo(a): return a - t + 0x800
base = [0, 0x1e, 0x3a, 0x48, 0x59, 0x63, 0x7b]
def room_of(bit):
    st = max(i for i in range(7) if base[i] <= bit)
    return "ROOM%X%02X" % (st + 1, bit - base[st])
occ = collections.defaultdict(list)
for area in range(20):
    ptr, cnt, mib, cd = struct.unpack_from('<IBBH', exe, fo(0x800aaa38 + 8 * area))
    rows = []
    for i in range(cnt):
        e = exe[fo(ptr + 16 * i): fo(ptr + 16 * i) + 16]
        u, v, w, h = e[0], e[1], e[2], e[3]
        x, y, mx, my = struct.unpack_from('<hhhh', e, 4)
        vb, ab, hl, hc = e[12], e[13], e[14], e[15]
        occ[vb].append((area, i))
        rows.append("[%d] uv(%d,%d) wh(%d,%d) xy(%d,%d) mk(%d,%d) vis %d %s alt %d hide %d/%d" % (i, u, v, w, h, x, y, mx, my, vb, room_of(vb), ab, hl, hc))
    print("Area %d @%08x n=%d mapItemBit=%d cd=%d" % (area, ptr, cnt, mib, cd))
    if len(sys.argv) > 2:
        for r in rows: print("   ", r)
print("--- visBits in MEHREREN Eintraegen:")
for vb, l in sorted(occ.items()):
    if len(l) > 1:
        print("  vis %d (%s): %s" % (vb, room_of(vb), l))
