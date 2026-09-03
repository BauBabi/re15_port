import struct
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
d = open(P,'rb').read()
RID=0x60; N=10
print("CAMERA CUT TABLE (RID) @0x%X, %d x 32 B  [u16 flag, u16 fov, s32 pos xyz, s32 tgt xyz, u32 pri_off]"%(RID,N))
for i in range(N):
    o=RID+i*32
    flag,fov,px,py,pz,tx,ty,tz,pri = struct.unpack_from("<HHiiiiiiI", d, o)
    prib = d[o+0x1c:o+0x20]
    tgt = "n/a"
    if pri < len(d): tgt = " ".join("%02x"%b for b in d[pri:pri+16])
    print("  cut%d @0x%04X  flag=%d fov=%d pos=(%6d,%7d,%6d) tgt=(%6d,%7d,%6d)  pri_off=0x%08X (bytes %s) -> %s"%(
        i,o,flag,fov,px,py,pz,tx,ty,tz,pri," ".join("%02x"%b for b in prib), tgt))
