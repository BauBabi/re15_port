import numpy as np, cv2, json, os
from skimage.morphology import skeletonize
from scipy import ndimage as ndi
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
PAL={'rot':(237,28,36),'gruen':(34,177,76),'grau':(127,127,127),'blau':(63,72,204),'lila':(163,73,164),'orange':(255,127,39),
     'dunkelrot':(136,0,21),'hellblau':(0,162,232),'braun':(185,122,87)}
vs=json.load(open('versatz.json'))
os.makedirs('dbg2',exist_ok=True)
alle={}
for lf,v in vs.items():
    key=lf.replace('.png','').encode('ascii','replace').decode().replace('?','_')
    M={n:m for n,m in dict(np.load('anno_%s.npz'%key)).items() if m.sum()>=50}
    L=lies(D+lf); B=lies(D+v['grund']); H,W=L.shape[:2]; h,w=B.shape[:2]
    fig=np.zeros((H,W),bool); fig[v['dy']:v['dy']+h, v['dx']:v['dx']+w]=B[...,3]>128
    a=L[...,3:4]/255.0; base=(L[...,:3]*a+255*(1-a)).astype(np.uint8)
    vis=(base*0.4+255*0.6).astype(np.uint8)
    big=cv2.resize(vis,None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)
    erg={}
    for n,m in M.items():
        col=np.array(PAL[n][::-1])
        wall=cv2.dilate(m.astype(np.uint8),np.ones((7,7),np.uint8))>0
        filled=ndi.binary_fill_holes(wall)
        inner=filled&~wall
        lab,nl=ndi.label(cv2.morphologyEx(filled.astype(np.uint8),cv2.MORPH_OPEN,np.ones((9,9),np.uint8))>0)
        regs=[]
        for i in range(1,nl+1):
            r=lab==i
            if (r&inner).sum()<30: continue
            fa=(r&fig).sum()/r.sum()
            if fa<0.5: continue
            ys,xs=np.nonzero(r)
            regs.append(dict(id=len(regs),flaeche=int(r.sum()),anteil_figur=round(float(fa),3),bbox=[int(xs.min()),int(ys.min()),int(xs.max()),int(ys.max())]))
            rb=cv2.resize(r.astype(np.uint8),None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)>0
            big[rb]=(big[rb]*0.6+col*0.4).astype(np.uint8)
            cv2.putText(big,'R%d'%regs[-1]['id'],(int(xs.mean()*2),int(ys.mean()*2)),cv2.FONT_HERSHEY_SIMPLEX,0.6,(0,0,0),2)
        # Strichkomponenten
        sl,ns=ndi.label(cv2.dilate(m.astype(np.uint8),np.ones((3,3),np.uint8))>0,structure=np.ones((3,3)))
        striche=[]
        for i in range(1,ns+1):
            s=sl==i
            if s.sum()<15: continue
            ys,xs=np.nonzero(s)
            sk=skeletonize(s); nb=ndi.convolve(sk.astype(int),np.ones((3,3),int),mode='constant')-sk
            ends=np.argwhere(sk&(nb==1))
            fa=(s&fig).sum()/s.sum()
            striche.append(dict(id=len(striche),pixel=int(s.sum()),anteil_figur=round(float(fa),3),bbox=[int(xs.min()),int(ys.min()),int(xs.max()),int(ys.max())],enden=[[int(x),int(y)] for y,x in ends]))
        mb=cv2.resize(m.astype(np.uint8),None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)>0
        big[mb]=col
        for st in striche:
            if st['bbox'][2]-st['bbox'][0]<4 and st['bbox'][3]-st['bbox'][1]<4: continue
            x0,y0,x1,y1=st['bbox']
            if st['anteil_figur']>0.02 or (x1-x0)>60 or (y1-y0)>30:
                cv2.putText(big,'%s%d'%(n[:2],st['id']),(x0*2,y0*2-2),cv2.FONT_HERSHEY_SIMPLEX,0.5,tuple(int(c) for c in col),1)
                for (x,y) in st['enden']: cv2.circle(big,(x*2,y*2),5,(0,0,0),1)
        erg[n]=dict(regionen=regs,striche=striche)
    alle[lf]=erg
    cv2.imwrite('dbg2/%s.png'%key,big)
    print(lf)
    for n,e in erg.items():
        print('  ',n,'R',[(r['id'],r['flaeche'],r['bbox']) for r in e['regionen']])
        print('     S',[(s['id'],s['pixel'],s['bbox'],s['anteil_figur'],len(s['enden'])) for s in e['striche'] if s['pixel']>=40 and (s['anteil_figur']>0.02 or s['bbox'][2]-s['bbox'][0]>60 or s['bbox'][3]-s['bbox'][1]>30)])
json.dump(alle,open('komp2.json','w'),ensure_ascii=False,indent=1)
