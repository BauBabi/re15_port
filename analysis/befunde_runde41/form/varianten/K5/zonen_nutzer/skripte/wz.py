import numpy as np, cv2, json
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
F='C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
ab=json.load(open('abbildung.json'))
for n,gf in ((1,'Rückseite.png'),(2,'birkin_form3_02.png')):
    B=lies(D+gf); V=cv2.imread(F+'bild%d.png'%n)
    gB=cv2.cvtColor(B[...,:3],cv2.COLOR_BGR2GRAY).astype(np.float32); gV=cv2.cvtColor(V,cv2.COLOR_BGR2GRAY).astype(np.float32)
    # Wasserzeichen: helle Schrift (>200) im unteren linken Teil der Grundlage
    h,w=gB.shape
    m=np.zeros_like(gB,bool); m[int(h*0.8):, :int(w*0.45)]=True
    txt=(gB>215)&m&(B[...,3]>128)
    ys,xs=np.nonzero(txt); print(n,'Textpixel',txt.sum(),'bbox',xs.min(),xs.max(),ys.min(),ys.max())
    best=None
    for s in np.arange(1.96,2.04,0.0025):
        for tx in np.arange(-3,3.01,0.25):
            for ty in np.arange(-3,3.01,0.25):
                q=ab[str(n)]['aehnlich']; m0=q['massstab']; t0=q['versatz']
                W=np.array([[1/s,0,t0[0]+tx],[0,1/s,t0[1]+ty]],np.float32)
                y0,y1,x0,x1=max(ys.min()-6,0),min(ys.max()+6,h),max(xs.min()-6,0),min(xs.max()+6,w)
                sub=gB[y0:y1,x0:x1]
                Wl=W.copy(); Wl[:,2]+=W[:,:2]@np.array([x0,y0],np.float32)
                wv=cv2.warpAffine(gV,Wl,(x1-x0,y1-y0),flags=cv2.INTER_LINEAR|cv2.WARP_INVERSE_MAP)
                a_=sub-sub.mean(); b_=wv-wv.mean(); c=(a_*b_).sum()/np.sqrt((a_*a_).sum()*(b_*b_).sum())
                if best is None or c>best[0]: best=(c,s,t0[0]+tx,t0[1]+ty)
    print('  Wasserzeichen-Anpassung ncc %.4f Faktor %.4f Versatz (%.2f,%.2f)'%best, ' gesamt: Faktor %.4f Versatz (%.2f,%.2f)'%(1/ab[str(n)]['aehnlich']['massstab'],*ab[str(n)]['aehnlich']['versatz']))
    ab[str(n)]['wasserzeichen']=dict(ncc=float(best[0]),faktor=float(best[1]),versatz=[float(best[2]),float(best[3])],textpixel=int(txt.sum()))
json.dump(ab,open('abbildung.json','w'),indent=1)
