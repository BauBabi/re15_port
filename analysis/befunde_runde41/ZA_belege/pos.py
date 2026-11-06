import sys
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts')
from re15_ss import Ram
for f in ['mzd_stage1_briefing.sav','mzd_stage1_engage_live.sav','mzd_stage1_combat_death.sav']:
    r=Ram('C:/workspace/git/reAi_v2/stage_saves/'+f)
    print(f,'player',r.s32(0x800aca88),r.s32(0x800aca90))
    for i in range(8):
        a=0x800acc2c+i*0x1f4
        if r.u32(a)&1: print('  slot',i,'typ %02x'%r.u8(a+8),'pos',r.s32(a+0x34),r.s32(a+0x3c),'rot',r.s16(a+0x6a))
