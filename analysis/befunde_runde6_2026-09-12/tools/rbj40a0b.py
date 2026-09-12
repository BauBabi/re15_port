import struct
d=open("c:/workspace/git/reAi_v2/info/re2leon/PL0/RDT/ROOM40A0.RDT","rb").read()
base=struct.unpack_from("<I",d,8+4*22)[0]
head=struct.unpack_from("<I",d,base)[0]; cnt=struct.unpack_from("<I",d,base+4)[0]
lst=base+head
for k in range(cnt):
    a,b=struct.unpack_from("<II",d,lst+8*k)
    mask=struct.unpack_from("<I",d,base+a)[0]
    emr=base+a+4
    bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,emr)
    edd=base+b
    c0,o0=struct.unpack_from("<HH",d,edd); n=o0//4
    clips=[(0,c0)]; cur=4
    for i in range(1,n):
        c,o=struct.unpack_from("<HH",d,edd+cur); clips.append((( (o-o0)//4),c)); cur+=4
    print("rec%d mask=0x%X EMR@0x%X: bones=%d kfsize=%d kfo=0x%X  EDD@0x%X: %d clips"%(k,mask,emr,bc,kfs,kfo,edd,n))
    tot=0
    for ci,(first,c) in enumerate(clips):
        fw=[struct.unpack_from("<I",d,edd+o0+4*(first+j))[0] for j in range(c)]
        kfi=[w&0xFFF for w in fw]
        fl=[(j,w) for j,w in enumerate(fw) if w>>12]
        print("  clip %2d: %3dF kf %d..%d %s"%(ci,c,kfi[0] if kfi else -1,kfi[-1] if kfi else -1,("flags:"+",".join("f%d=0x%08X"%(j,w) for j,w in fl)) if fl else ""))
    # kf count
    sz=(base+ (lst if k==0 else 0)) # not needed
