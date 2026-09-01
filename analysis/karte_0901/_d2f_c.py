# -*- coding: utf-8 -*-
import sys, os, math, collections, itertools
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from d2f import *
s16 = G['s16']; u16 = G['u16']; BASES = G['BASES']

print("=== MASSSTABSZEILEN @0x800768b0 (8 B/Zeile, idx = Stage-Basis + Raumnummer) ===")
print("  STAGE1-Basis = %d" % BASES[0])
for r in range(0x1000, 0x1270, 0x10):
    idx = BASES[0] + ((r>>4)&0xFF)
    a = 0x800768b0 + idx*8
    ox,oy,sx,sy = s16(a), s16(a+2), u16(a+4), u16(a+6)
    pg = page_of(r)
    stub = (sx<=1 or sy<=1)
    print("  ROOM%04X idx=%2d @0x%08X  ox=%5d oy=%5d sx=%5d sy=%5d  Seite=%s %s" % (
        r, idx, a, ox,oy,sx,sy, pg, "  <-- STUB" if stub else ""))
