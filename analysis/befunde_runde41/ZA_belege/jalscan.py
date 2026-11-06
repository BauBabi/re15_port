import sys, struct
def scan(path, base, hdr, target):
    d=open(path,'rb').read()
    ins = 0x0C000000 | ((target & 0x0FFFFFFF)>>2)
    res=[]
    for off in range(hdr, len(d)-3, 4):
        if struct.unpack_from('<I', d, off)[0]==ins: res.append(base+off-hdr)
    return res
t=int(sys.argv[1],16)
R=r"C:\workspace\git\reAi_v2"
for tag,root in (("mod",R+r"\info\Re1.5"),("40%",R+r"\Version_40%")):
    exe=root+r"\PSX.EXE"
    d=open(exe,'rb').read(); taddr=struct.unpack_from('<I',d,0x18)[0]
    print(tag,"EXE",[hex(a) for a in scan(exe,taddr,0x800,t)])
    for s in range(1,7):
        p=root+r"\PSX\BIN\STAGE%d.BIN"%s
        print(tag,"STAGE%d"%s,[hex(a) for a in scan(p,0x80100000,0,t)])
