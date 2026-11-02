from bau import *
T=pickle.load(open('teile.pkl','rb'))
kn=knochen_tafel(T)
import form_pruefung as PR
fl=[i for i,t in enumerate(T) if t['name'].startswith('fl_')]
linie=[(int(x),int(338.5-(x-340)*3/24)) for x in range(347,358)]
z=ZIEL[2]
bo=PR.rechteck(345,382,405,396)&(z==0)
best=[]
for a in np.arange(-16,5,3.0):
    for b in np.arange(-20,31,5.0):
        for c in (0,-4,-8,-12,-16,-20):
            r5=dreh('x',float(c))@dreh('z',float(a)); r6=dreh('z',float(b)); r7=(r5@r6).T
            p=Pose(kn,{5:r5,6:r6,7:r7})
            tr=p.trafo(T)
            r=np.zeros((480,640),np.uint8); lo=-1e9
            for i in fl:
                Q=tr(i,T[i]['V']); lo=max(lo,Q[:,1].max())
                raster(Q,T[i]['F'],S2,r)
            dk=np.mean([r[y,x] for x,y in linie])
            h=int((r.astype(bool)&bo).sum())
            s=int((r.astype(bool)&(z==0)).sum())
            if lo<=2:
                best.append((round(dk,2),-h,-s,round(float(lo)),float(a),float(b),c))
best.sort(reverse=True)
print(len(best),best[:14])
