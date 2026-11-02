# Jeder Setzer von Bit 0x400 (ori r,r,0x400) in STAGE1-5: steht im Fenster [-16,+6] ein sh auf Skala (+0x88/8a/8c eines Teils)?
import struct, sys
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts")
import re15_disasm as D
from xref import bins
for name,d,base in bins("disc"):
    W=lambda a: struct.unpack_from("<I",d,a-base)[0]
    for o in range(0,len(d)-3,4):
        w=struct.unpack_from("<I",d,o)[0]
        if (w>>26)==0xd and ((w>>21)&31)==((w>>16)&31)!=0 and (w&0xffff)==0x400:
            a=base+o; sk=[]; st=None
            for x in range(a-64,a+28,4):
                if not (base<=x<base+len(d)): continue
                v=W(x); op=v>>26; imm=v&0xffff; imm=imm-0x10000 if imm&0x8000 else imm
                if op==0x29 and imm%172 in (136,138,140): sk.append("%08x:sh @%d(T%d+%d)"%(x,imm,imm//172,imm%172))
                if op==0x2b and x>a and st is None and imm%172==0: st="%08x:sw @%d(T%d)"%(x,imm,imm//172)
            print(name,"%08x"%a,"Teilwort-Store:",st,"| Skala-Stores:",len(sk),sk[:3])
