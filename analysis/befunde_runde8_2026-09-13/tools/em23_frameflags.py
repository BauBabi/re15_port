#!/usr/bin/env python3
"""EM23: vollstaendiger Frame-Wort-Zensus aller Clips (Paar 1 + Paar 3)."""
import struct, hashlib
REPO="c:/workspace/git/reAi_v2"
EXE=open(REPO+"/info/re2leon/PSX.EXE","rb").read()
taddr=struct.unpack_from("<I",EXE,0x18)[0]
def exe_off(a): return 0x800+(a-taddr)
def toc(kind,rec):
    i=exe_off(0x8009adf4)+((kind-0x10)*4+rec)*8
    sec,size=struct.unpack_from("<II",EXE,i); return sec*0x800,size
EMS=open(REPO+"/re15_port/shared_assets/RE2/CDEMD0.EMS","rb").read()
off,size=toc(0x23,3); emd=EMS[off:off+size]
diroff,dircnt=struct.unpack_from("<II",emd,0)
dirs=[struct.unpack_from("<I",emd,diroff+4*i)[0] for i in range(dircnt)]
order=sorted(range(dircnt),key=lambda i:dirs[i]); ends={}
for k,i in enumerate(order):
    ends[i]=dirs[order[k+1]] if k+1<len(order) else diroff
def parse_edd(base,size):
    d=emd[base:base+size]
    cnt0,off0=struct.unpack_from("<HH",d,0); n=off0//4
    clips=[(0,cnt0)]; cur=4
    for i in range(1,n):
        c,o=struct.unpack_from("<HH",d,cur); clips.append(((o-off0)//4,c)); cur+=4
    return [[struct.unpack_from("<I",d,off0+4*(f+j))[0] for j in range(c)] for (f,c) in clips]
for nm,ei in (("Paar1 (Gator)",1),("Paar3 (Opfer)",5)):
    edd=parse_edd(dirs[ei],ends[ei]-dirs[ei])
    print("=== %s : EDD dir[%d] @0x%X, %d Clips ==="%(nm,ei,dirs[ei],len(edd)))
    seen={}
    for ci,fw in enumerate(edd):
        ev=[]
        for j,w in enumerate(fw):
            hi=w>>27            # Top-5-Bits
            if hi:
                tag="f%d:w=0x%08X top5=0x%02X"%(j,w,hi<<3)
                if w & 0x08000000: tag+=" SE%d"%(w>>28); seen.setdefault(w>>28,[]).append((ci,j))
                ev.append(tag)
        print("  clip %2d (%3d F): %s"%(ci,len(fw), "; ".join(ev) if ev else "-"))
    print("  SE-Ids in Frame-Flags:", {k:v for k,v in sorted(seen.items())})
    print()
