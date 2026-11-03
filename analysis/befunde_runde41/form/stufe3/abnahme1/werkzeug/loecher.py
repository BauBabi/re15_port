import os,sys,json
import numpy as np
from scipy import ndimage
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools'
sys.path.insert(0,T)
import form_buehne as B, form_emd as FE
A=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/shared_assets/RE2/'
emd=open(A+'BAHNSTEIG_FORM.EMD','rb').read(); tim=open(A+'BAHNSTEIG_FORM.TIM','rb').read()
raum,schnitt,lage=sys.argv[1],int(sys.argv[2]),tuple(int(v) for v in sys.argv[3].split(','))
R=B.Raum(raum); s=R.schnitt(schnitt); kam=B.Kamera.aus_rid(s['pos'],s['ziel'],s['fov'],name='%s/%d'%(raum,schnitt)); licht=R.licht(schnitt)
mo=FE.modell(emd,tim)
clips=[int(c) for c in sys.argv[4].split(',')]
erg=[]
for c in clips:
    n=mo.clips[c][1]
    for b in range(0,n,max(1,n//12)):
        for g in range(0,4096,512):
            r,m=B.buehne(emd,tim,c,b,lage,g,kam,licht,None)
            h=ndimage.binary_fill_holes(m)&~m
            lab,k=ndimage.label(h)
            if k==0: continue
            gr=ndimage.sum(h,lab,range(1,k+1))
            big=[(int(v),i+1) for i,v in enumerate(gr) if v>=12]
            if big:
                big.sort(reverse=True)
                ys,xs=np.nonzero(lab==big[0][1])
                erg.append(dict(clip=c,bild=b,gier=g,n=len(big),groesst=big[0][0],summe=sum(v for v,_ in big),cx=int(xs.mean()),cy=int(ys.mean()),figur=int(m.sum())))
erg.sort(key=lambda e:-e['groesst'])
json.dump(erg,open(sys.argv[5],'w'),indent=0)
print(len(erg),'Ansichten mit Loch >=12 px')
for e in erg[:25]: print(e)
