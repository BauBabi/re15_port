import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts")
import re15_disasm as D
from xref import bins
which=sys.argv[1]
RNG=0x0c000000|((0x8001af20>>2)&0x3ffffff); EE68=0x0c000000|((0x8001ee68>>2)&0x3ffffff)
for name,d,base in bins(which):
    if not name.startswith("STAGE"): continue
    W=lambda a: struct.unpack_from("<I",d,a-base)[0]
    for o in range(0,len(d)-0x40,4):
        a=base+o
        # pattern: jal rng ; ... andi v0,v0,0x3 ; bne v0,zero ; ... jal rng ; nop ; andi v0,v0,0x7
        if W(a)==RNG and (W(a+12)&0xffff0000)==0x30420000 and (W(a+12)&0xffff)==3 and W(a+0x18)==RNG and W(a+0x20)==0x30420007:
            # find jalr v0 after
            j=a+0x20
            while W(j)!=0x0040f809: j+=4
            tbl_lui=None
            # table address from lui at / addiu at
            hi=lo=None
            for k in range(a,j,4):
                w=W(k)
                if (w>>16)==0x3c01: hi=(w&0xffff)<<16
                if (w>>16)==0x2421: lo=w&0xffff; lo=lo-0x10000 if lo&0x8000 else lo
            tbl=(hi+lo)&0xffffffff
            entries=[W(tbl+4*i) for i in range(8)] if base<=tbl<base+len(d) else []
            # scan forward to first jr ra, record branches + ee68 call
            k=j; ee=None; bt=[]
            while W(k)!=0x03e00008:
                w=W(k)
                if w==EE68: ee=k
                k+=4
            jr=k
            # who branches/jumps to the 2 instrs before ee68 (tail label)?
            tails=[]
            if ee:
                for x in range(a-0x400,jr,4):
                    w=W(x); op=w>>26
                    if op in (4,5,6,7,1):
                        imm=w&0xffff; imm=imm-0x10000 if imm&0x8000 else imm
                        t=x+4+imm*4
                        if ee-12<=t<=ee: tails.append("%08x->%08x"%(x,t))
            print("%s: variante rng@%08x jalr@%08x tabelle@%08x=%s | jal 8001ee68 @%s | jr ra @%08x | Spruenge zum Schwanz: %s"%(
                name,a,j,tbl," ".join("%08x"%e for e in entries),("%08x"%ee) if ee else "FEHLT",jr,tails))
