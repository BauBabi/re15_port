import sys, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as L
G=L.G; s16=G['s16']; u16=G['u16']; BASES=G['BASES']; page_of=G['page_of']
rooms=sorted(set(k[0] for k in ZONES))
print('Raeume im generierten Header:',len(rooms),' Zonen:',len(ZONES))
mit=[z for z in ZONES.values() if z['sx']>1]
print('Zonen MIT Zeile im Header:',len(mit),' ohne:',len(ZONES)-len(mit))
rmit=sorted(set(z['room'] for z in mit))
print('Raeume MIT Zeile im Header:',len(rmit))
print('Raeume OHNE:',len(rooms)-len(rmit))
per=collections.Counter(); pere=collections.Counter()
for r in rooms:
    per[r>>12]+=1
    if r in rmit: pere[r>>12]+=1
for st in sorted(per): print(' Stage',st,':',pere[st],'/',per[st])
