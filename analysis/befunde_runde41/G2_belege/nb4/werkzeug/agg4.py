# Zusammenfassung von treffer_abn6.txt (treffer4.py) je Ort und Angriff
import re,sys
from collections import defaultdict
L=open(sys.argv[1]).read().splitlines()
agg=defaultdict(list)
for l in L:
    m=re.search(r'(oben|Wasser)\s+(\S+)\s+Lage.*heute Weg\s+(\d+) Luftlinie\s+(\d+) Dauer\s+(\d+) \| RE2 Sub (\S)',l)
    if not m: continue
    ort,art,weg,luft,dau,sub=m.groups()
    agg[(ort,art)].append((int(weg),int(luft),int(dau),sub))
for k,v in sorted(agg.items()):
    print('%-8s %-10s Treffer %3d  heute: Weg max %5d, Luftlinie max %5d / Mittel %5d, Dauer %d..%d Bilder | RE2: Sub 2 %d x (3735 rueckwaerts, 20 Bilder), Sub 3 %d x (3080 vorwaerts, 54 Bilder), unbestimmt %d'%(
        k[0],k[1],len(v),max(a[0] for a in v),max(a[1] for a in v),sum(a[1] for a in v)//len(v),min(a[2] for a in v),max(a[2] for a in v),
        sum(a[3]=='2' for a in v),sum(a[3]=='3' for a in v),sum(a[3]=='-' for a in v)))
A=defaultdict(int);A90=defaultdict(int)
for l in L:
    if ' oben ' not in l or '| A: -' in l: continue
    a=l.split('| A: ')[1].split(' | A90: ')
    A[re.sub(r' nach \d+ Bildern.*| \(.*','',a[0]).strip()]+=1; A90[re.sub(r' nach \d+ Bildern.*| \(.*','',a[1]).strip()]+=1
print('oben, Richtung A (Gator-Gier):',dict(A)); print('oben, Richtung A90 (Gator-Gier auf 90 Grad gerastet):',dict(A90))
