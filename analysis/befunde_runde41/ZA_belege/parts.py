import sys, os
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram
ENEMY_BASE=0x800acc2c; STRIDE=0x1f4
def dump(path, verbose=False):
    try: r = Ram(path)
    except Exception as e: print(os.path.basename(path), "ERR", e); return
    # stage/room
    room = r.u16(0x800aca30) if False else None
    out=[]
    for i in range(16):
        a0 = ENEMY_BASE + i*STRIDE
        if (r.u32(a0)&1)==0: continue
        typ=r.u8(a0+8)
        if not (0x10<=typ<=0x1f): continue
        p = r.u32(a0+0x188)
        if not (0x80000000<=p<0x80200000): out.append("slot%d t%02x parts=?%08x"%(i,typ,p)); continue
        mod=[]
        for k in range(20):
            pa=p+k*0xac
            w=r.u32(pa)
            sx,sy,sz=r.s16(pa+0x88),r.s16(pa+0x8a),r.s16(pa+0x8c)
            t1=r.u32(pa+0x94); t2=r.u32(pa+0x98)
            if (w & 0x480) or verbose:
                mod.append("k%d w=%04x s=(%x,%x,%x) t=%x/%x"%(k,w,sx,sy,sz,t1,t2))
        out.append("slot%d t%02x +9=%02x st=%d/%d parts@%08x %s"%(i,typ,r.u8(a0+9),r.u8(a0+4),r.u8(a0+5),p, " | ".join(mod) if mod else "keine 0x400/0x80"))
    print(os.path.basename(path)); [print("   ",o) for o in out]
for f in sys.argv[1:]: dump(f, verbose=os.environ.get("V")=="1")
