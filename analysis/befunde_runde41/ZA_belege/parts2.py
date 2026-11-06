import sys, os
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram
ENEMY_BASE=0x800acc2c; STRIDE=0x1f4
r = Ram(sys.argv[1])
for i in range(16):
    a0 = ENEMY_BASE + i*STRIDE
    if (r.u32(a0)&1)==0: continue
    typ=r.u8(a0+8)
    if not (0x10<=typ<=0x1f): continue
    p = r.u32(a0+0x188)
    print("slot%d t%02x parts@%08x"%(i,typ,p))
    for k in range(15):
        pa=p+k*0xac
        print("   k%2d w=%08x s=(%04x,%04x,%04x) col68=%08x t94=%08x t98=%08x"%(k,r.u32(pa),r.u16(pa+0x88),r.u16(pa+0x8a),r.u16(pa+0x8c),r.u32(pa+0x68),r.u32(pa+0x94),r.u32(pa+0x98)))
