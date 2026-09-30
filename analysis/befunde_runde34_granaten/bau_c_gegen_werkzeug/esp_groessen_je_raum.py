import os; exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'esp_zeilen_zensus.py')).read().split("IMPL =")[0])
import collections
rooms=collections.defaultdict(set)
def scan2(name, raw, effs):
    for e in effs:
        for sub,s,r,row in rows_of(raw,e):
            A=u16(row,0); w=u16(row,4); h=u16(row,6)
            if A in (41,42,24,25,13,19) or (A<=60 and (w!=0x1000 or h!=0x1000)):
                rooms[(A,w,h)].add("%s:id%02x/s%d"%(name.replace('.RDT','').replace('ROOM',''),e['id'],sub))
core=open(os.path.join(ROOT,"DATA/CORE00.ESP"),"rb").read()
scan2("CORE00", core, parse(core,0,((len(core)+3)&~3)-4))
for f in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    raw=open(f,"rb").read()
    if len(raw)<0x5c: continue
    idh,pe,tb,te=u32(raw,0x4c),u32(raw,0x50),u32(raw,0x54),u32(raw,0x58)
    if idh==0: continue
    try: effs=parse(raw,idh,pe,tb,te)
    except Exception: continue
    scan2(os.path.basename(f), raw, effs)
for k in sorted(rooms):
    v=sorted(rooms[k]); print("A=%2d w=%04x h=%04x (%d):"%(k[0],k[1],k[2],len(v)), " ".join(v[:14]), "..." if len(v)>14 else "")
