import struct
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d = open(P,'rb').read()
S=0x570
cx,cz = struct.unpack_from("<HH", d, S)
cnt = struct.unpack_from("<5I", d, S+4)
print("SCA @0x%X  ceilingX=%d ceilingZ=%d  group counts=%s  total=%d"%(S,cx,cz,cnt,sum(cnt)))
print("header bytes:", " ".join("%02x"%b for b in d[S:S+24]))
off=S+24
tot=sum(cnt)
xs=[];zs=[]
for i in range(tot):
    o=off+i*12
    w,dn,x,z,ty,u0,u1,fl = struct.unpack_from("<HHhhBBBB", d, o)
    xs += [x, x+w]; zs += [z, z+dn]
    print("  [%3d] x=%7d z=%7d w=%6d d=%6d type=%2d u0=%02x u1=%02x floor=%d"%(i,x,z,w,dn,ty,u0,u1,fl))
print("\nSCA extents: X[%d..%d] Z[%d..%d]"%(min(xs),max(xs),min(zs),max(zs)))
