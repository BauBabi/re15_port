import struct,sys
R=r"C:\workspace\git\reAi_v2"
def scan(path, base, hdr, name):
    d=open(path,'rb').read()
    for off in range(hdr,len(d)-3,4):
        w=struct.unpack_from('<I',d,off)[0]
        op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; imm=w&0xffff
        if op==9 and imm==172 and rs==rt:  # addiu rX,rX,172
            print(name, hex(base+off-hdr), "addiu r%d,r%d,172"%(rt,rs))
exe=R+r"\info\Re1.5\PSX.EXE"; d=open(exe,'rb').read(); taddr=struct.unpack_from('<I',d,0x18)[0]
scan(exe,taddr,0x800,"EXE")
scan(R+r"\info\Re1.5\PSX\BIN\STAGE1.BIN",0x80100000,0,"S1")
