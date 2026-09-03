import struct
P = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\PLD\PL00.MD1"
d=open(P,'rb').read()
def u32(o): return struct.unpack_from("<I",d,o)[0]
def s16(o): return struct.unpack_from("<h",d,o)[0]
length,unk,objcnt = u32(0),u32(4),u32(8)
mc = objcnt//2
print("PL00.MD1 size=%d length=%d objcnt=%d meshes=%d"%(len(d),length,objcnt,mc))
base=12
allv=[]
for i in range(mc):
    h=12+i*56
    f=struct.unpack_from("<14I",d,h)
    tv_off,tv_cnt = f[0],f[1]
    qv_off,qv_cnt = f[7],f[8]
    for voff,vcnt in ((tv_off,tv_cnt),(qv_off,qv_cnt)):
        for k in range(vcnt):
            o=base+voff+k*8
            if o+6<=len(d): allv.append((s16(o),s16(o+2),s16(o+4)))
ys=[v[1] for v in allv]
print("PL00 mesh-local Y range over all bones: %d .. %d  (n=%d)"%(min(ys),max(ys),len(allv)))
# EMR skeleton root->head offsets give the true stature; read EMR
E=r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\PLD\PL00.EMR"
e=open(E,'rb').read()
print("PL00.EMR size=%d  first 32: %s"%(len(e)," ".join("%02x"%b for b in e[:32])))
