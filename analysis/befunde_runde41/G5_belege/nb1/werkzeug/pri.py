import struct, sys
p = sys.argv[1]
d = open(p,'rb').read()
off = int(sys.argv[2],0) if len(sys.argv)>2 else 0
gc, mc = struct.unpack_from('<HH', d, off)
print('groups', gc, 'decl', mc, 'filesize', len(d))
gp = off+4; mp = gp + gc*8; idx = 0
G=[]
for g in range(gc):
    n, base, gx, gy = struct.unpack_from('<HHhh', d, gp+g*8)
    G.append((n,base,gx,gy)); print(' group', g, 'n', n, 'base', hex(base), 'anchor', gx, gy)
for g,(n,base,gx,gy) in enumerate(G):
    for i in range(n):
        sx, sy, dx, dy, depth, feld = struct.unpack_from('<BBBBHH', d, mp); mp += 8
        if feld == 0:
            w, h = struct.unpack_from('<HH', d, mp); mp += 4
        else: w = h = feld
        print('  m%02d g%d src(%3d,%3d) dst(%4d,%4d) %3dx%-3d depth %4d  -> x %d..%d y %d..%d' % (idx, g, sx, sy, dx+gx, dy+gy, w, h, depth, dx+gx, dx+gx+w-1, dy+gy, dy+gy+h-1))
        idx += 1
print('end', hex(mp))
