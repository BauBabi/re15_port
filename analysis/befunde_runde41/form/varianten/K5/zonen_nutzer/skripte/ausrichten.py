import numpy as np, cv2, json
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
F='C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
def grau(im): return cv2.cvtColor(im[...,:3],cv2.COLOR_BGR2GRAY).astype(np.float32)
def lnorm(g,s=7):
    m=cv2.GaussianBlur(g,(0,0),s); v=cv2.GaussianBlur((g-m)**2,(0,0),s)
    return (g-m)/np.sqrt(v+25)
out={}
for n,gf in ((1,'Rückseite.png'),(2,'birkin_form3_02.png')):
    B=lies(D+gf); V=cv2.imread(F+'bild%d.png'%n)
    for vname in ['bild%d.png'%n, ['vlcsnap-2026-10-31-00h28m45s774.png','vlcsnap-2026-10-31-00h29m02s830.png'][n-1]]:
        pass
    a=(B[...,3]>128)
    gB=grau(B); gV=grau(V)
    best=None
    # Vorlage auf Massstab s hochskalieren und Grundlage darin suchen (Grundlage = Vorlage*s + t)
    for s in np.arange(1.70,2.60,0.01):
        Vs=cv2.resize(gV,None,fx=s,fy=s,interpolation=cv2.INTER_LINEAR)
        if Vs.shape[0]<gB.shape[0] or Vs.shape[1]<gB.shape[1]: continue
        r=cv2.matchTemplate(lnorm(Vs),lnorm(gB),cv2.TM_CCORR_NORMED,mask=a.astype(np.float32))
        _,mx,_,loc=cv2.minMaxLoc(r)
        if best is None or mx>best[0]: best=(mx,s,loc)
    print(n,'grob',best)
    mx,s,loc=best
    # Abbildung Vorlage->Grundlage: g = s*v - loc  => v = (g+loc)/s
    out[n]=dict(s=float(s),loc=[int(loc[0]),int(loc[1])],ncc=float(mx))
json.dump(out,open('grob.json','w'),indent=1)
