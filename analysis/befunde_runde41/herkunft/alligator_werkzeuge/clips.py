# Linear-Verfolgung: Konstanten in Registern (ori/addiu rt,zero,imm), Stores sb/sh rt,OFF(base) fuer OFF in {0x94 Clip, 0x8f Blend, 0x9a HP}
import struct,sys,collections
path,lo,hi,base=sys.argv[1],int(sys.argv[2],16),int(sys.argv[3],16),int(sys.argv[4],16) if len(sys.argv)>4 else 0x80100000
d=open(path,'rb').read()
reg={}
out=collections.defaultdict(list)
for a in range(lo,hi,4):
    x=struct.unpack_from('<I',d,a-base)[0]
    op=x>>26; rs=(x>>21)&31; rt=(x>>16)&31; imm=x&0xffff; s=imm-0x10000 if imm&0x8000 else imm
    if op in (0xd,) and rs==0: reg[rt]=imm
    elif op==9 and rs==0: reg[rt]=s
    elif op in (0x28,0x29) and imm in (0x94,0x8f,0x95):   # sb/sh
        v=0 if rt==0 else reg.get(rt)
        out[imm].append((hex(a),v))
    elif op==0 and (x&0x3f)==0x21 and ((x>>16)&31)==0:  # addu rd,rs,zero (move)
        rd=(x>>11)&31; reg[rd]=reg.get(rs)
    else:
        # Zielregister ueberschrieben -> vergessen
        if op in (0xf,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0x20,0x21,0x23,0x24,0x25): reg.pop(rt,None)
        if op==0: reg.pop((x>>11)&31,None)
for k in sorted(out):
    vals=collections.Counter(v for _,v in out[k])
    print(hex(k),dict(vals))
    print('   ',out[k])
