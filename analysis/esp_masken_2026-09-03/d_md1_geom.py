import struct
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d = open(P,'rb').read()
MD1=0xE84; base=MD1+12
def s16(o): return struct.unpack_from("<h", d, o)[0]
def u8(o): return d[o]
verts=[(s16(base+0x38+k*8),s16(base+0x38+k*8+2),s16(base+0x38+k*8+4)) for k in range(24)]
print("VERTICES (x,y,z) — PSX space, Y up = negative:")
for i,v in enumerate(verts): print("  v%-2d %s"%(i,v))
print("\nQUADS (16 B each) @0x%X:"%(base+0x170))
qs=[]
for i in range(15):
    o=base+0x170+i*16
    raw=d[o:o+16]
    n0,v0,n1,v1,n2,v2,n3,v3 = struct.unpack_from("<8H", d, o)
    qs.append((v0,v1,v2,v3))
    P4=[verts[v0],verts[v1],verts[v2],verts[v3]]
    xs=[p[0] for p in P4]; ys=[p[1] for p in P4]; zs=[p[2] for p in P4]
    print("  q%-2d n=%s v=%s  X[%5d..%5d] Y[%6d..%6d] Z[%5d..%5d]   raw=%s"%(
        i,(n0,n1,n2,n3),(v0,v1,v2,v3),min(xs),max(xs),min(ys),max(ys),min(zs),max(zs)," ".join("%02x"%b for b in raw)))
print("\nQUAD UVs @0x%X (16 B each):"%(base+0x260))
for i in range(15):
    o=base+0x260+i*16
    print("  q%-2d %s"%(i," ".join("%02x"%b for b in d[o:o+16])))
