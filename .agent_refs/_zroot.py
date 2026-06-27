import struct
f=open('extracted/PSX/EMD/CDEMD0/em10.emd','rb').read()
begin=struct.unpack('<I',f[0:4])[0]
def dirent(i): return struct.unpack('<I',f[begin+i*4:begin+i*4+4])[0]
edd=dirent(3); emr=dirent(4)
kf_off=emr+struct.unpack('<H',f[emr+2:emr+4])[0]
kf_size=struct.unpack('<H',f[emr+6:emr+8])[0]
def kf(idx):
    o=kf_off+idx*kf_size
    rx,ry,rz=struct.unpack('<3h',f[o:o+6])
    sx,sy,sz=struct.unpack('<3h',f[o+6:o+12])
    return rx,ry,rz,sx,sy,sz
# EDD: count0@+0 off0@+2 ; frame table @ off0, u32 low12=kf idx
cnt0=struct.unpack('<H',f[edd:edd+2])[0]
off0=struct.unpack('<H',f[edd+2:edd+4])[0]
# clip table: pairs (count,off) ; iterate
clips=[0x01,0x02,0x09,0x0c,0x16,0x1a,0x27,0x29,0x2a]
for ci in clips:
    c=struct.unpack('<H',f[edd+ci*4:edd+ci*4+2])[0]
    o=struct.unpack('<H',f[edd+ci*4+2:edd+ci*4+4])[0]
    rxs=[];rzs=[];rys=[]
    for fr in range(c):
        v=struct.unpack('<I',f[edd+o+fr*4:edd+o+fr*4+4])[0]
        ki=v&0xfff
        rx,ry,rz,sx,sy,sz=kf(ki)
        rxs.append(rx);rys.append(ry);rzs.append(rz)
    if not rxs: 
        print(f"clip 0x{ci:02x}: nf=0"); continue
    print(f"clip 0x{ci:02x}: nf={c:2d}  rootX[{min(rxs):6d}..{max(rxs):6d}] span={max(rxs)-min(rxs):5d}  rootZ[{min(rzs):6d}..{max(rzs):6d}] span={max(rzs)-min(rzs):5d}  rootY[{min(rys):6d}..{max(rys):6d}]")
