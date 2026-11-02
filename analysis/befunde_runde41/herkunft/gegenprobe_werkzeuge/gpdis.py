import sys, capstone
R='C:/workspace/git/reAi_v2/'
# usage: dis.py disc|mod exe|STAGEn.BIN addr count [hdr]
w,f,a,n=sys.argv[1],sys.argv[2],int(sys.argv[3],16),int(sys.argv[4])
base={'disc':R+'Version_40%/','mod':R+'info/Re1.5/'}[w]
if f=='exe':
    d=open(base+'PSX.EXE','rb').read(); off=a-0x80010000+0x800
else:
    d=open(base+'PSX/BIN/'+f,'rb').read(); hdr=int(sys.argv[5],16) if len(sys.argv)>5 else 0
    off=a-0x80100000+hdr
md=capstone.Cs(capstone.CS_ARCH_MIPS,capstone.CS_MODE_MIPS32+capstone.CS_MODE_LITTLE_ENDIAN)
for i in md.disasm(d[off:off+4*n],a):
    print(f'{i.address:08x}: {i.bytes.hex(" ")}  {i.mnemonic} {i.op_str}')
