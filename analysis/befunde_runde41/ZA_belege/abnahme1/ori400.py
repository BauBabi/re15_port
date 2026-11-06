import sys, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts")
import re15_disasm as D
from xref import bins
which=sys.argv[1]
for name,d,base in bins(which):
    hits=[]
    for o in range(0,len(d)-3,4):
        w=struct.unpack_from("<I",d,o)[0]
        op=w>>26; rs=(w>>21)&31; rt=(w>>16)&31; imm=w&0xffff
        if op==0xd and rs==rt and rs!=0 and imm in (0x400,0x2000,0x480,0x2400):
            hits.append(base+o)
        if op==0xd and rs==0 and imm in (0x5f4,0x7e8): hits.append(base+o)
    if hits: print(name, len(hits), " ".join("%08x"%h for h in hits))
