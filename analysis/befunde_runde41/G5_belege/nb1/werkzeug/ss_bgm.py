import sys, glob, os
sys.path.insert(0, os.path.dirname(__file__))
from re2ss import SS
SN=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps'
files=sorted(glob.glob(SN+'/s*.sav'))
for f in files:
    s=SS(f)
    st=s.s16(0x800d481c); rm=s.s16(0x800d481e); p=s.u32(0x800dfd64)
    ent=s.bytes(p,2) if p else b''
    cur=s.bytes(0x800d7856,2)
    slots=s.bytes(0x800d75c0,0x18)
    vm=s.u32(0x800d75b4); vs=s.u32(0x800d75b8)
    def vab(v):
        if not (0x80000000<=v<0x80200000): return 'n/a'
        h=s.bytes(v,0x20)
        progs=[ (s.u8(v+0x20+i*16+1), s.u8(v+0x20+i*16+4), s.u8(v+0x20+i*16)) for i in range(6)]
        return '%s mvol=%02x pan=%02x ps=%d prog(mvol,mpan,tones)=%s'%(h[:4], h[0x18],h[0x19], int.from_bytes(h[0x12:0x14],'little'), progs)
    xa=s.u32(0x800cfbd8)
    print(os.path.basename(f), 'st',st,'rm',rm,'tab@%08x=%s'%(p,ent.hex()),'cur',cur.hex(),'slots',slots.hex(' '),'xa_flags %08x'%xa, 'optvol', s.u8(0x800eae3c))
    print('   MAIN vab@%08x %s'%(vm,vab(vm)))
    print('   SUB  vab@%08x %s'%(vs,vab(vs)))
