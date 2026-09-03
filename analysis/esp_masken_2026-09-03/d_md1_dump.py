import struct
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d = open(P,'rb').read()
MD1 = 0xE84
def u32(o): return struct.unpack_from("<I", d, o)[0]
def s16(o): return struct.unpack_from("<h", d, o)[0]
length, unk, objcnt = u32(MD1), u32(MD1+4), u32(MD1+8)
print("MD1 @0x%X: length=0x%X (%d)  unknown=%d  object_count=%d -> mesh_count=%d"%(MD1,length,length,unk,objcnt,objcnt//2))
print("  MD1 spans 0x%X .. 0x%X"%(MD1, MD1+length))
base = MD1 + 12
mesh_count = objcnt//2
allv=[]
for i in range(mesh_count):
    h = MD1+12+i*56
    f = struct.unpack_from("<14I", d, h)
    tv_off,tv_cnt,tn_off,tn_cnt,tf_off,tf_cnt,tu_off, qv_off,qv_cnt,qn_off,qn_cnt,qf_off,qf_cnt,qu_off = f
    print("  mesh %d hdr@0x%X"%(i,h))
    print("    tri : v_off=0x%X n=%d  n_off=0x%X n=%d  f_off=0x%X n=%d  uv_off=0x%X"%(tv_off,tv_cnt,tn_off,tn_cnt,tf_off,tf_cnt,tu_off))
    print("    quad: v_off=0x%X n=%d  n_off=0x%X n=%d  f_off=0x%X n=%d  uv_off=0x%X"%(qv_off,qv_cnt,qn_off,qn_cnt,qf_off,qf_cnt,qu_off))
    for (voff,vcnt,tag) in ((tv_off,tv_cnt,'tri'),(qv_off,qv_cnt,'quad')):
        if vcnt==0: continue
        vs=[]
        for k in range(vcnt):
            o = base+voff+k*8
            vs.append((s16(o),s16(o+2),s16(o+4)))
        allv += vs
        xs=[v[0] for v in vs]; ys=[v[1] for v in vs]; zs=[v[2] for v in vs]
        print("    %s verts n=%d  X[%d..%d] Y[%d..%d] Z[%d..%d]"%(tag,vcnt,min(xs),max(xs),min(ys),max(ys),min(zs),max(zs)))
        print("      first 8:", vs[:8])
if allv:
    xs=[v[0] for v in allv]; ys=[v[1] for v in allv]; zs=[v[2] for v in allv]
    print("\nTOTAL BBOX  X[%d..%d] (w=%d)  Y[%d..%d] (h=%d)  Z[%d..%d] (d=%d)  nverts=%d"%(
        min(xs),max(xs),max(xs)-min(xs), min(ys),max(ys),max(ys)-min(ys), min(zs),max(zs),max(zs)-min(zs), len(allv)))
print("\nraw MD1 first 0x80 bytes:")
for r in range(0,0x80,16):
    print("  0x%X: %s"%(MD1+r," ".join("%02x"%b for b in d[MD1+r:MD1+r+16])))
