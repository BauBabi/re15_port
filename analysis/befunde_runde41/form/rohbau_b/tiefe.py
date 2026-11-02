import numpy as np, figur, schau, json
f=figur.bauen(); fig=figur.fertig(f)
g=figur.lade_gerippe()
for nm,p in fig['posen'].items():
    R,T=schau.fk(f.eltern,f.versatz,p['wurzel'],p['winkel'])
    z={}
    for b,V,_ in fig['netze']:
        y=float((V@R[b].T+T[b])[:,1].max())
        z[f.namen[b]]=max(z.get(f.namen[b],-1e9),y)
    q=sorted(z.items(),key=lambda x:-x[1])[:12]
    print(nm,'wurzel',p['wurzel'][1],[(k,int(v)) for k,v in q])
