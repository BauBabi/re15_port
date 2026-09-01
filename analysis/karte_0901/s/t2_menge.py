# -*- coding: utf-8 -*-
import sys, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as L
G = L.G
s16=G['s16']; u16=G['u16']; BASES=G['BASES']; page_of=G['page_of']; read_rdt=G['read_rdt']; zones_of=G['zones_of']

room_ids=[]
for line in open('re15_port/include/re15_room_list.h'):
    for tok in line.replace(',',' ').split():
        if tok.startswith('0x') and len(tok)==6:
            try: room_ids.append(int(tok,16))
            except ValueError: pass
bases=sorted(set(r&0xFFF0 for r in room_ids))
zinfo={}; sca_all={}; doors_all={}; stairs_all={}
for b in bases:
    got=read_rdt(b) or read_rdt(b+1)
    if not got: continue
    sca,doors,stairs=got
    sca_all[b]=sca; doors_all[b]=doors; stairs_all[b]=stairs
    zs=zones_of(sca)
    if zs: zinfo[b]=zs
print('mit RDT:',len(sca_all),' mit Zonen:',len(zinfo))
def zeile(rid):
    idx=BASES[(rid>>12)-1]+((rid>>4)&0xff); a=0x800768b0+idx*8
    return idx,(s16(a),s16(a+2),u16(a+4),u16(a+6))
kart=[b for b in sorted(zinfo) if page_of(b) not in (None,0xd)]
print('Karten-Raeume (Zonen + Seite):',len(kart))
ec=[b for b in kart if zeile(b)[1][2]>1 and zeile(b)[1][3]>1]
print('davon echt:',len(ec),' stub:',len(kart)-len(ec))
per=collections.Counter(); pere=collections.Counter()
for b in kart:
    st=b>>12; per[st]+=1
    if b in ec: pere[st]+=1
for st in sorted(per): print(' Stage',st,':',pere[st],'/',per[st])
import json
json.dump({'kart':kart,'echt':ec},open(r'analysis\karte_0901\s\menge.json','w'))
