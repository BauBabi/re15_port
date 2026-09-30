import os; exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'esp_zeilen_zensus.py')).read().split("IMPL =")[0])
import collections
FL0E={3,4,8,10,16,17,38,41}; FL1E={4,16,41}
res=collections.defaultdict(set)
def sc(name,raw,effs):
    for e in effs:
        for sub,s,r,row in rows_of(raw,e):
            A=u16(row,0)
            for rs,off in ((FL0E,0x0e),(FL1E,0x1e)):
                if A in rs:
                    v=row[off]
                    if (v&3)==2: res[('bit1_ohne_bit0',A,off,v)].add("%s:id%02x/s%d/st%d/r%d"%(name,e['id'],sub,s,r))
                    if (v&0x0b)==0x0b: res[('bit3',A,off,v)].add("%s:id%02x/s%d"%(name,e['id'],sub))
core=open(os.path.join(ROOT,"DATA/CORE00.ESP"),"rb").read()
sc("CORE00",core,parse(core,0,((len(core)+3)&~3)-4))
for f in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    raw=open(f,"rb").read()
    if len(raw)<0x5c: continue
    idh,pe,tb,te=u32(raw,0x4c),u32(raw,0x50),u32(raw,0x54),u32(raw,0x58)
    if idh==0: continue
    try: sc(os.path.basename(f).replace('.RDT',''),raw,parse(raw,idh,pe,tb,te))
    except Exception: pass
for k,v in sorted(res.items()): print(k,len(v),sorted(v)[:6])
print("fertig")
