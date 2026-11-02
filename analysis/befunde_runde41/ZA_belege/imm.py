import struct,sys
R=r"C:\workspace\git\reAi_v2"
tgt=int(sys.argv[1],16)
hi=((tgt+0x8000)>>16)&0xffff; lo=tgt&0xffff
def scan(path,base,hdr,name):
    d=open(path,'rb').read()
    for off in range(hdr,len(d)-3,4):
        w=struct.unpack_from('<I',d,off)[0]
        if w==tgt: print(name,"word",hex(base+off-hdr))
        op=w>>26
        if op in (9,13) and (w&0xffff)==lo:
            # look back for lui hi within 8 instr
            for b in range(1,9):
                o2=off-4*b
                if o2<hdr: break
                w2=struct.unpack_from('<I',d,o2)[0]
                if (w2>>26)==15 and (w2&0xffff)==hi: print(name,"lui/addiu",hex(base+off-hdr)); break
exe=R+r"\info\Re1.5\PSX.EXE"; d=open(exe,'rb').read(); taddr=struct.unpack_from('<I',d,0x18)[0]
scan(exe,taddr,0x800,"EXE")
for s in range(1,7): scan(R+r"\info\Re1.5\PSX\BIN\STAGE%d.BIN"%s,0x80100000,0,"S%d"%s)
