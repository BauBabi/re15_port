# geo.py <room> [xmin xmax zmin zmax]: SCA-Boxen (RDT+0x20) und RVD-Zonen (RDT+0x28) im Ausschnitt
import struct, sys
base = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_montage/re15_port/shared_assets/PSX/STAGE1/'
room = sys.argv[1]
box = [int(v) for v in sys.argv[2:6]] if len(sys.argv) >= 6 else None
d = open(base + 'ROOM%s.RDT' % room, 'rb').read()
sca = struct.unpack_from('<I', d, 0x20)[0]
cx, cz = struct.unpack_from('<HH', d, sca)
cnt = struct.unpack_from('<5I', d, sca + 4)
print('SCA @0x%X ceiling (%d,%d) counts %s' % (sca, cx, cz, cnt))
def inb(x0, x1, z0, z1):
    if not box: return True
    return not (x1 < box[0] or x0 > box[1] or z1 < box[2] or z0 > box[3])
o = sca + 24
for i in range(sum(cnt)):
    w, dn, x, z, ty, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, o + i * 12)
    if inb(x, x + w, z, z + dn):
        print('  SCA %2d @0x%05X x %6d..%6d z %6d..%6d type %d u0 %02x u1 %02x floor %d' % (i, o + i * 12, x, x + w, z, z + dn, ty, u0, u1, fl))
zs = struct.unpack_from('<I', d, 0x28)[0]
p = zs; k = 0
while True:
    mk, cf, ct = struct.unpack_from('<HBB', d, p)
    if mk == 0xFFFF and cf == 0xFF and ct == 0xFF: break
    pts = [struct.unpack_from('<hh', d, p + 4 + j * 4) for j in range(4)]
    xs = [q[0] for q in pts]; zz = [q[1] for q in pts]
    if inb(min(xs), max(xs), min(zz), max(zz)):
        print('  RVD %2d @0x%05X mk %04x cam %d->%d  x %6d..%6d z %6d..%6d  %s' % (k, p, mk, cf, ct, min(xs), max(xs), min(zz), max(zz), pts))
    p += 20; k += 1
