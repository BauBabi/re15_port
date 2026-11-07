import sys, glob, os
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts')
from re15_ss import Ram
EN=0x800acc2c; ST=0x1f4
for p in sys.argv[1:]:
    try:
        r=Ram(p)
    except Exception as ex:
        print(p, 'ERR', ex); continue
    pl=r.u32(0x800aca54)
    out=[]
    for i in range(16):
        a=EN+i*ST
        w0=r.u32(a)
        if not (w0&1): continue
        t=r.u8(a+8)
        out.append('s%d t=%02x w0=%08x band=%x hp=%d'%(i,t,w0,w0>>29, r.s16(a+0x9a) if hasattr(r,'s16') else 0))
    print(os.path.basename(p), 'pl.w0=%08x'%pl, '; '.join(o for o in out if True))
