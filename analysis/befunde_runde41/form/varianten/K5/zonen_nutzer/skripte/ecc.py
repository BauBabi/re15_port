import numpy as np, cv2, json
from scipy.optimize import minimize
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
F='C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
def grau(im): return cv2.cvtColor(im[...,:3],cv2.COLOR_BGR2GRAY).astype(np.float32)
def lnorm(g,s=7):
    m=cv2.GaussianBlur(g,(0,0),s); v=cv2.GaussianBlur((g-m)**2,(0,0),s)
    return ((g-m)/np.sqrt(v+25)).astype(np.float32)
grob=json.load(open('grob.json'))
out={}
for n,gf in ((1,'Rückseite.png'),(2,'birkin_form3_02.png')):
    B=lies(D+gf); V=cv2.imread(F+'bild%d.png'%n)
    a=(B[...,3]>128)
    gB=grau(B); gV=grau(V); h,w=gB.shape
    s=grob[str(n)]['s']; lx,ly=grob[str(n)]['loc']
    Vs=cv2.resize(gV,None,fx=s,fy=s)
    r0=cv2.matchTemplate(lnorm(Vs),lnorm(gB),cv2.TM_CCORR_NORMED,mask=a.astype(np.float32)).max()
    r1=cv2.matchTemplate(lnorm(Vs),lnorm(gB[:,::-1].copy()),cv2.TM_CCORR_NORMED,mask=a[:,::-1].astype(np.float32).copy()).max()
    LB=lnorm(gB,5)[a]; LB=(LB-LB.mean())/LB.std()
    LVf=lnorm(gV,5/s)
    def ncc(p,glatt=True):
        W=np.array([[p[0],p[1],p[2]],[p[3],p[4],p[5]]],np.float32)  # g->v
        wv=cv2.warpAffine(LVf,W,(w,h),flags=cv2.INTER_LINEAR|cv2.WARP_INVERSE_MAP)[a]
        wv=(wv-wv.mean())/(wv.std()+1e-9)
        return float((wv*LB).mean())
    def ziel_aehnl(q):  # Aehnlichkeit: Massstab, Versatz
        return -ncc([q[0],0,q[1],0,q[0],q[2]])
    q0=[1/s,lx/s,ly/s]
    r=minimize(ziel_aehnl,q0,method='Powell',options=dict(xtol=1e-6,ftol=1e-8))
    qa=r.x; p_aeh=[qa[0],0,qa[1],0,qa[0],qa[2]]
    r2=minimize(lambda p:-ncc(p),p_aeh,method='Powell',options=dict(xtol=1e-6,ftol=1e-8))
    p=r2.x
    W=np.array([[p[0],p[1],p[2]],[p[3],p[4],p[5]]])
    A=W[:,:2]; sx=np.hypot(A[0,0],A[1,0]); sy=np.hypot(A[0,1],A[1,1]); rot=np.degrees(np.arctan2(A[1,0],A[0,0]))
    # Unterschied affin vs aehnlich an den Grundlagenecken
    ec=np.array([[0,0],[w,0],[0,h],[w,h],[w/2,h/2]],float)
    va=ec@A.T+W[:,2]; ve=ec*qa[0]+qa[1:]
    print(n,gf,'ncc normal %.3f gespiegelt %.3f | aehnlich: Massstab %.4f (=1/%.4f) t (%.2f,%.2f) ncc %.4f | affin ncc %.4f 1/sx %.4f 1/sy %.4f rot %.3f'%(r0,r1,qa[0],1/qa[0],qa[1],qa[2],-r.fun,-r2.fun,1/sx,1/sy,rot))
    print('  affin - aehnlich an Ecken/Mitte (Vorlage-px):',np.hypot(*(va-ve).T).round(2))
    # Restversatz in Kacheln: Vorlage in Grundlagenraster (affin) gegen Grundlage
    Wf=W.astype(np.float32)
    gw=cv2.warpAffine(gV,Wf,(w,h),flags=cv2.INTER_LINEAR|cv2.WARP_INVERSE_MAP)
    L1=lnorm(gB,4); L2=lnorm(gw,4)
    sh=[]; k=64
    for y in range(0,h-k,24):
        for x in range(0,w-k,24):
            if a[y:y+k,x:x+k].mean()<0.97: continue
            pc=cv2.phaseCorrelate(L1[y:y+k,x:x+k].astype(np.float64),L2[y:y+k,x:x+k].astype(np.float64),cv2.createHanningWindow((k,k),cv2.CV_64F))
            if pc[1]>0.15: sh.append((x,y,pc[0][0],pc[0][1],pc[1]))
    sh=np.array(sh); d=np.hypot(sh[:,2],sh[:,3])*qa[0]   # in Vorlage-px
    print('  Kacheln %d Restversatz (Vorlage-px) median %.2f p90 %.2f max %.2f'%(len(sh),np.median(d),np.percentile(d,90),d.max()))
    out[n]=dict(grundlage=gf,aehnlich=dict(massstab=float(qa[0]),versatz=[float(qa[1]),float(qa[2])],ncc=float(-r.fun),faktor_grundlage_je_vorlagepx=float(1/qa[0])),
        affin=dict(W=W.tolist(),ncc=float(-r2.fun),faktor_x=float(1/sx),faktor_y=float(1/sy),drehung_grad=float(rot)),
        affin_minus_aehnlich_px=np.hypot(*(va-ve).T).tolist(),ncc_normal=float(r0),ncc_gespiegelt=float(r1),
        rest_kacheln=len(sh),rest_median=float(np.median(d)),rest_p90=float(np.percentile(d,90)),rest_max=float(d.max()),
        ecken_in_vorlage=(ec[:4]*qa[0]+qa[1:]).tolist())
    vis=cv2.cvtColor(gB.astype(np.uint8),cv2.COLOR_GRAY2BGR); vis[...,1]=gw.astype(np.uint8)
    cv2.imwrite('ueber%d.png'%n,vis)
json.dump(out,open('abbildung.json','w'),indent=1)
