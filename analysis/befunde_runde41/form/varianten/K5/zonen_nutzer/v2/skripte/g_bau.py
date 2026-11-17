"""Kopie von zonen_nutzer/skripte/zonen_bau.py (v1), nur Pfade/Kodierung angepasst; Bauarten unveraendert."""
"""Nutzer-Zonen aus den beschrifteten Bildern: Masken im Bildraum des beschrifteten Bildes, dann Grundlage, dann Vorlage."""
import numpy as np, cv2, json, os
from scipy import ndimage as ndi
from skimage.morphology import skeletonize
D='C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
def lies(p): return cv2.imdecode(np.fromfile(p,np.uint8),cv2.IMREAD_UNCHANGED)
from g_basis import VS, AB, ARBEIT
def key(lf): return lf.replace('.png','').encode('ascii','replace').decode().replace('?','_')
_cache={}
def lade(lf):
    if lf not in _cache:
        M={n:m for n,m in dict(np.load(ARBEIT+'/anno_%s.npz'%key(lf))).items() if m.sum()>=50}
        L=lies(D+lf); v=VS[lf]; B=lies(D+v['grund']); H,W=L.shape[:2]; h,w=B.shape[:2]
        fig=np.zeros((H,W),bool); fig[v['dy']:v['dy']+h, v['dx']:v['dx']+w]=B[...,3]>128
        _cache[lf]=(M,fig,(H,W))
    return _cache[lf]
def linie(m,p,q,d=3): cv2.line(m,tuple(map(int,p)),tuple(map(int,q)),1,d)
def endpunkt(lf,farbe,ungefaehr,r=15):
    M,fig,_=lade(lf); s=M[farbe]
    sk=skeletonize(cv2.dilate(s.astype(np.uint8),np.ones((3,3),np.uint8))>0)
    nb=ndi.convolve(sk.astype(int),np.ones((3,3),int),mode='constant')-sk
    e=np.argwhere(sk&(nb==1))[:,::-1]
    d=np.hypot(*(e-np.array(ungefaehr)).T); i=d.argmin()
    if d[i]>r: raise SystemExit('kein Linienende nahe %s %s %s (naechstes %s, %.1f)'%(lf,farbe,ungefaehr,e[i],d[i]))
    return [int(e[i][0]),int(e[i][1])]
def flut(lf,waende,saaten,sehnen=(),box=None,dil=5,nur_figur=True):
    M,fig,(H,W)=lade(lf)
    w=np.zeros((H,W),np.uint8)
    for f in waende: w|=M[f].astype(np.uint8)
    w=cv2.dilate(w,np.ones((dil,dil),np.uint8))
    for p,q in sehnen: linie(w,p,q,3)
    frei=w==0
    if box is not None:
        bb=np.zeros_like(frei); x0,y0,x1,y1=box; bb[y0:y1+1,x0:x1+1]=True; frei&=bb
    lab,_=ndi.label(frei)
    r=np.zeros((H,W),bool)
    for s in saaten:
        l=lab[s[1],s[0]]
        if l==0:
            fr=np.argwhere(lab[max(s[1]-8,0):s[1]+9,max(s[0]-8,0):s[0]+9]>0)
            if len(fr)==0: raise SystemExit('Saat auf Wand %s %s'%(lf,s))
            d=np.hypot(fr[:,0]-min(8,s[1]),fr[:,1]-min(8,s[0])); y,x=fr[d.argmin()]
            l=lab[max(s[1]-8,0)+y,max(s[0]-8,0)+x]
        r|=lab==l
    rand=r[0].any()|r[-1].any()|r[:,0].any()|r[:,-1].any()
    if box is not None:
        x0,y0,x1,y1=box; boxr=bool(r[y0,x0:x1+1].any() or r[y1,x0:x1+1].any() or r[y0:y1+1,x0].any() or r[y0:y1+1,x1].any())
    else: boxr=None
    # Linienbreite zur Haelfte zurueckgeben (Umriss liegt auf der Grenze)
    r=cv2.dilate(r.astype(np.uint8),np.ones((dil+2,dil+2),np.uint8))>0
    if nur_figur: r&=fig
    return r, dict(rand_beruehrt=bool(rand),box_beruehrt=boxr,dil=dil)
def huelle(lf,farbe,box,nur_figur=False):
    M,fig,(H,W)=lade(lf); x0,y0,x1,y1=box
    s=np.zeros((H,W),bool); s[y0:y1+1,x0:x1+1]=M[farbe][y0:y1+1,x0:x1+1]
    pts=np.argwhere(s)[:,::-1].astype(np.int32)
    r=np.zeros((H,W),np.uint8); cv2.fillConvexPoly(r,cv2.convexHull(pts),1)
    r=r>0
    if nur_figur: r&=fig
    return r, {}
def loecher(lf,farbe,box,min_flaeche=15):
    """geschlossene Umrisse einer Farbe in box: Loecher fuellen (Strich + Inneres)."""
    M,fig,(H,W)=lade(lf); x0,y0,x1,y1=box
    s=np.zeros((H,W),bool); s[y0:y1+1,x0:x1+1]=M[farbe][y0:y1+1,x0:x1+1]
    s=cv2.dilate(s.astype(np.uint8),np.ones((2,2),np.uint8))>0
    f=ndi.binary_fill_holes(s); inn=f&~s
    lab,n=ndi.label(inn); keep=np.zeros_like(f)
    teile=[]
    for i in range(1,n+1):
        q=lab==i
        if q.sum()>=min_flaeche:
            keep|=q; ys,xs=np.nonzero(q); teile.append(dict(innen_px=int(q.sum()),bbox=[int(xs.min()),int(ys.min()),int(xs.max()),int(ys.max())]))
    keep=cv2.dilate(keep.astype(np.uint8),np.ones((3,3),np.uint8))>0
    return keep, dict(teile=teile)
def band(lf,farbe,box,r=3,nur_figur=True):
    M,fig,(H,W)=lade(lf); x0,y0,x1,y1=box
    s=np.zeros((H,W),np.uint8); s[y0:y1+1,x0:x1+1]=M[farbe][y0:y1+1,x0:x1+1]
    q=cv2.dilate(s,cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(2*r+1,2*r+1)))>0
    if nur_figur: q&=fig
    return q,{}
def punkte(lf,pts,r=6):
    M,fig,(H,W)=lade(lf); q=np.zeros((H,W),np.uint8)
    for p in pts: cv2.circle(q,tuple(p),r,1,-1)
    return (q>0)&fig, {}
