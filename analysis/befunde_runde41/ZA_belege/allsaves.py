import sys, os, glob
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram
ENEMY_BASE=0x800acc2c; STRIDE=0x1f4
files=sorted(glob.glob(r"C:\workspace\git\reAi_v2\stage_saves\**\*.sav", recursive=True))
for f in files:
    if "PATCHED" in f: continue
    try: r=Ram(f)
    except Exception as e: print(os.path.basename(f),"ERR",e); continue
    stage=r.u16(0x800b0fe6) ; room=r.u16(0x800b0fe2)
    rows=[]
    for i in range(16):
        a0=ENEMY_BASE+i*STRIDE
        if (r.u32(a0)&1)==0: continue
        typ=r.u8(a0+8)
        if not (0x10<=typ<=0x1f): continue
        p=r.u32(a0+0x188)
        if not (0x80000000<=p<0x80200000): continue
        res=[];bits=[]
        for k in range(15):
            pa=p+k*0xac; w=r.u32(pa)
            sx=r.u16(pa+0x88); t1=r.u32(pa+0x94); t2=r.u32(pa+0x98)
            if sx in (0x5f4,0x7e8) and t1==0x80 and t2==0x8080e0: res.append(k)
            if w & 0x480: bits.append("k%d:%x"%(k,w))
        rows.append("s%d t%02x st%d res%s bits%s"%(i,typ,r.u8(a0+4),res,bits))
    if rows: print("%-40s room=%04x | %s"%(os.path.relpath(f,r"C:\workspace\git\reAi_v2\stage_saves"),room," ; ".join(rows)))
