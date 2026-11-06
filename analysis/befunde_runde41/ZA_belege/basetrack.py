import struct
R=r"C:\workspace\git\reAi_v2"
partoffs={k*0xac:k for k in range(20)}
def scan(path,base,hdr,name):
    d=open(path,'rb').read()
    n=(len(d)-hdr)//4
    ins=[struct.unpack_from('<I',d,hdr+4*i)[0] for i in range(n)]
    for i,w in enumerate(ins):
        op=w>>26
        if op not in (0x2b,0x29,0x28): continue  # sw sh sb
        rb=(w>>21)&31; rt=(w>>16)&31; off=w&0xffff
        if off not in partoffs: continue
        # find most recent writer of rb within 16 instr
        src=None
        for j in range(i-1,max(i-24,-1),-1):
            w2=ins[j]; op2=w2>>26
            dest=None
            if op2 in (0x23,0x21,0x25,0x20,0x24,0x0f,0x09,0x0d): dest=(w2>>16)&31
            elif op2==0: dest=(w2>>11)&31
            if dest==rb:
                if op2==0x23 and (w2&0xffff)==392: src="parts"
                break
        if src=="parts":
            print(name, hex(base+4*i), ["sb","sh","?","sw"][[0x28,0x29,0,0x2b].index(op)], "k=%d"%partoffs[off], "rt=r%d"%rt)
exe=R+r"\info\Re1.5\PSX.EXE"; d=open(exe,'rb').read(); taddr=struct.unpack_from('<I',d,0x18)[0]
scan(exe,taddr,0x800,"EXE")
scan(R+r"\info\Re1.5\PSX\BIN\STAGE1.BIN",0x80100000,0,"S1")
