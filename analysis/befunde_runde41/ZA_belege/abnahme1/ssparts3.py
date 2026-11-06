import sys, os
from ssram import R
EB=0x800acc2c; ST=0x1f4
for f in sys.argv[1:]:
    r=R(f)
    print("== %s exe=%s stage=%d room=%02x rng=%08x"%(os.path.basename(f),r.exe,r.u16(0x800b0fe0)+1,r.u16(0x800b0fe2),r.u32(0x800ac774)))
    for i in range(16):
        a0=EB+i*ST; w0=r.u32(a0); typ=r.u8(a0+8)
        if not (0x10<=typ<=0x1f): continue
        p=r.u32(a0+0x188); n=r.u8(a0+131)
        x=r.s16(a0+0x38) ; 
        words=[r.u32(p+k*0xac) for k in range(15)]
        var=[]
        for k in range(15):
            pa=p+k*0xac; sc=(r.u16(pa+0x88),r.u16(pa+0x8a),r.u16(pa+0x8c)); t=(r.u32(pa+0x94),r.u32(pa+0x98))
            if sc[1]==0x1000 and sc[0]==sc[2] and sc[0] in (0x5f4,0x7e8) and t==(0x80,0x8080e0): var.append("T%d sk=%x"%(k,sc[0]))
        print("  slot%-2d w0=%08x typ=%02x nparts(+0x83)=%d parts@%08x words=%s variante=%s"%(i,w0,typ,n,p,
              ("alle %08x"%words[0]) if len(set(words))==1 else ",".join("%x"%w for w in words), var))
