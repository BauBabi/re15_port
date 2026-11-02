import struct
from xref import bins
INIT={"STAGE1.BIN":(0x80100688,0x801008ec,0x80100fe0,0x80101000),"STAGE2.BIN":(None,0x80100780,0x80100e74,0x80100e94),
      "STAGE3.BIN":(None,0x801009d8,0x801010cc,0x801010ec),"STAGE4.BIN":(None,0x801008a0,0x80100f94,0x80100fb4),"STAGE5.BIN":(None,0x80100a20,0x80101114,0x80101134)}
for which in ("disc","mod"):
  for name,d,base in bins(which):
    if name not in INIT: continue
    st,jalr,ee,jr=INIT[name]
    W=lambda a: struct.unpack_from("<I",d,a-base)[0]
    # function start: search back from jalr to previous jr ra
    x=jalr
    while W(x)!=0x03e00008: x-=4
    fs=x+8
    bad=[]; jrs=[]
    for a in range(fs,jr+4,4):
        w=W(a); op=w>>26
        if w==0x03e00008 and a!=jr: jrs.append(a)
        if op in (1,4,5,6,7):
            imm=w&0xffff; imm=imm-0x10000 if imm&0x8000 else imm; t=a+4+imm*4
            if t>ee or t<fs: bad.append("%08x->%08x"%(a,t))
        if op in (2,3):
            t=((a+4)&0xf0000000)|((w&0x3ffffff)<<2)
            if op==2: bad.append("j %08x->%08x"%(a,t))
        if op==0 and (w&63)==8 and w!=0x03e00008: bad.append("jr reg @%08x"%a)
    print(which,name,"INIT %08x..%08x"%(fs,jr),"weitere jr ra:",["%08x"%j for j in jrs],"Spruenge hinter/vor den Reset:",bad)
