# gp_vdve.py - Kurzbild der Acid/Incendiary-Laeufe vd/ve (Ermittler-Saves) mit eigenem Leser gp_ss (Aufruf aus dem Repo-Root).
import sys,glob,os
sys.path.insert(0,'analysis/befunde_runde34_granaten/re_absturz_gegen_werkzeug')
from gp_ss import SS
for d in ('vd','ve'):
    for p in sorted(glob.glob('build/r34g_absturz/%s/*.sav'%d)):
        s=SS(p)
        act=[i for i in range(96) if s.u8(0x800a73b8+i*0x84+0x6c)&1]
        pl=0x800aca54
        print('%-14s aca5d=%02x inv3=%s modus=%s clip=%d bild=%d acaec=%04x ESP-aktiv=%s EPC=%08x'%(os.path.basename(p), s.u8(0x800aca5d), s.mem(0x800b10b8,2).hex(' '), s.mem(pl+4,2).hex(), s.u8(pl+0x94), s.u8(pl+0x95), s.u16(0x800acaec), act, s.c['EPC']))
