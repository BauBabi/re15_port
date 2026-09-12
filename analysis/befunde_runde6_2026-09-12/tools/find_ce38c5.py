import struct
REPO="c:/workspace/git/reAi_v2"
EXE=open(REPO+"/info/re2leon/PSX.EXE","rb").read()
taddr=struct.unpack_from("<I",EXE,0x18)[0]
EMS=open(REPO+"/re15_port/shared_assets/RE2/CDEMD0.EMS","rb").read()
TOC=0x8009adf4
def exe_off(a): return 0x800+(a-taddr)
found={}
for kind in range(0x10,0x5B):
    for rec in (0,1):
        i=exe_off(TOC)+((kind-0x10)*4+rec)*8
        sec,size=struct.unpack_from("<II",EXE,i)
        if size==0: continue
        off=sec*0x800
        d=EMS[off:off+size]
        base=0x8010D000 if rec==0 else 0x80100000
        for j in range(0,len(d)-3,4):
            w=struct.unpack_from("<I",d,j)[0]
            op=w>>26; imm=w&0xFFFF
            if imm==0xE38C and op in (0x23,0x2B,0x09):
                print("kind 0x%02X rec %d: @0x%08X op=%s w=0x%08X"%(kind,rec,base+j,{0x23:"lw",0x2B:"sw",9:"addiu"}[op],w))
