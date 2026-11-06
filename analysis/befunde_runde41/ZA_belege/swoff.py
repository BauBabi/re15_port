import struct,sys
R=r"C:\workspace\git\reAi_v2"
offs=set(int(x,0) for x in sys.argv[1].split(','))
def scan(path, base, hdr, name):
    d=open(path,'rb').read()
    for off in range(hdr,len(d)-3,4):
        w=struct.unpack_from('<I',d,off)[0]
        op=w>>26; imm=w&0xffff
        if op==0x2b and imm in offs:
            print(name, hex(base+off-hdr), "sw r%d,%d(r%d)"%((w>>16)&31,imm,(w>>21)&31))
exe=R+r"\info\Re1.5\PSX.EXE"; d=open(exe,'rb').read(); taddr=struct.unpack_from('<I',d,0x18)[0]
scan(exe,taddr,0x800,"EXE")
for s in range(1,7): scan(R+r"\info\Re1.5\PSX\BIN\STAGE%d.BIN"%s,0x80100000,0,"S%d"%s)
