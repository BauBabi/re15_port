# -*- coding: utf-8 -*-
import sys, os, statistics, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as L
G = L.G
s16 = G['s16']; u16 = G['u16']; BASES = G['BASES']; page_of = G['page_of']
rects = G['rects']

# alle Raum-IDs aus der Port-Raumliste
room_ids = []
for line in open('re15_port/include/re15_room_list.h'):
    for tok in line.replace(',', ' ').split():
        if tok.startswith('0x') and len(tok) == 6:
            try: room_ids.append(int(tok, 16))
            except ValueError: pass
bases = sorted(set(r & 0xFFF0 for r in room_ids))
print('Basis-Raeume gesamt:', len(bases))

def zeile(rid):
    idx = BASES[(rid >> 12) - 1] + ((rid >> 4) & 0xff)
    a = 0x800768b0 + idx*8
    return idx, a, (s16(a), s16(a+2), u16(a+4), u16(a+6))

karten = [b for b in bases if page_of(b) not in (None, 0xd)]
print('Raeume mit Kartenseite (page_of != None, != 0xd):', len(karten))
karten_all = [b for b in bases if page_of(b) is not None]
print('Raeume mit page_of != None (inkl 0xd):', len(karten_all))

echt = []; stub = []
for b in karten:
    idx, a, (ox,oy,sx,sy) = zeile(b)
    if sx > 1 and sy > 1: echt.append((b, idx, a, ox,oy,sx,sy))
    else: stub.append((b, idx, a, ox,oy,sx,sy))
print('echt:', len(echt), ' stub:', len(stub))
print()
print('%-6s %-4s %-12s %6s %6s %7s %7s %10s %10s %5s %s' % ('Raum','idx','addr','ox','oy','sx','sy','E/px_x','E/px_y','Seite','sy/sx'))
for (b, idx, a, ox,oy,sx,sy) in echt:
    print('%04X %4d @0x%08X %6d %6d %7d %7d %10.2f %10.2f %5s %6.4f' % (
        b, idx, a, ox,oy,sx,sy, (1<<20)/sx, (1<<20)/sy, page_of(b), sy/sx))
print()
print('--- Stub-Raeume ---')
for (b, idx, a, ox,oy,sx,sy) in stub:
    print('%04X idx=%3d @0x%08X {%d,%d,%d,%d} seite=%s' % (b,idx,a,ox,oy,sx,sy,page_of(b)))
