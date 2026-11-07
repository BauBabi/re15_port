import sys, struct
p = sys.argv[1]
d = open(p,'rb').read()
ncut = d[1]
addr = struct.unpack_from('<21I', d, 8)
rid, rvd = addr[7], addr[8]
print('%s nCut=%d RID@0x%x RVD@0x%x' % (p, ncut, rid, rvd))
for i in range(ncut):
    o = rid + i*32
    fl, fov, px,py,pz, tx,ty,tz, spr = struct.unpack_from('<HHiiiiiiI', d, o)
    dx, dz = tx-px, tz-pz
    print(' Cut %2d @0x%05x fov=%5d pos=(%6d,%6d,%7d) ziel=(%6d,%6d,%7d) d=(%6d,%7d)' % (i,o,fov,px,py,pz,tx,ty,tz,dx,dz))
o = rvd
while True:
    if struct.unpack_from('<I', d, o)[0] == 0xFFFFFFFF: print(' RVD Ende @0x%x' % o); break
    be, fl, fc, tc = d[o], d[o+1], d[o+2], d[o+3]
    xz = struct.unpack_from('<8h', d, o+4)
    xs = xz[0::2]; zs = xz[1::2]
    print(' RVD @0x%05x be=%02x fl=%02x %2d->%2d x %6d..%6d z %7d..%7d  pts=%s' % (o,be,fl,fc,tc,min(xs),max(xs),min(zs),max(zs),list(zip(xs,zs))))
    o += 20
