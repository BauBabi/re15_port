"""Pose/Gier/Lage von V7 fuer die Vorlage n1439 (Darstellung_Vorlage.png), Koerper OHNE Schwanz (Knochen 16-22 aus,
Bildbereich links der Nutzerlinie J = unbekannt). Verfahren wie foto/f_pose.py (Gesamt = 0,5 IoU + 0,5 Mittel(Anker))."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, time, cv2
from multiprocessing import Pool
import v4_gemein as G, b2, birkin_buehne as BB
SCHW=G.SCHWANZ
KOPF=(422,193,468,254)   # Kopfbox Vorlage n1439 (Gitterbild g_vorlage_x4, weisse Nutzerlinie K x 454-463 = Kopf-Vorderkante)
SOHLE=436                # Messblatt V4 Abschnitt 1: Sohle 435-437 aus n1439..1445
def trimap_koerper():
    T=np.load(S8+'/m7_trimap.npy').copy()
    L=json.load(open(S8+'/m2_linien.json'))
    j1=L['blau_J']['polylinien'][1]; j0=L['blau_J']['polylinien'][0]
    # J von oben nach unten: (384,157)..(381,180) + (379,189)..(357,241), dann senkrecht bis y 480
    J=np.array(j1+j0[::-1]+[[357,480]],float)
    yy,xx=np.mgrid[0:480,0:640]
    xj=np.interp(yy,J[:,1],J[:,0])
    links=(xx<xj)
    T[links&(T==1)]=2
    T[links&(T==0)&(yy>150)&(yy<452)&(xx>240)]=2
    return T
_mo=_T=None
def _init():
    global _mo,_T
    _mo=b2.modell(EMD_V7); _T=trimap_koerper()
def umriss(mo,p):
    r,kam,M,li=b2.raum()
    kf=BB.kf_von_clip(mo,p['clip'],p['bild'])
    tris=b2.dreiecke_ids(mo,kf,(p['x'],p.get('y',0),p['z']),int(p['gier']),kam,None)
    ids=np.full((480+G.UNTEN,640),-1,np.int16)
    for _t,pp,_uv,_z,_f,bn in sorted(tris,key=lambda t:-t[0]):
        x,y=BB.abbilden(np.array([q[0]+BB.PHASE for q in pp],float),np.array([q[1]+BB.PHASE for q in pp],float),M)
        cv2.fillConvexPoly(ids,np.round(np.stack([x,y],1)*4).astype(np.int32),int(bn),lineType=cv2.LINE_8,shift=2)
    return ids
def bewerte(ids_voll,T):
    ids=ids_voll[:480]
    m=(ids>=0)&~np.isin(ids,SCHW)
    F=T==1; K=T!=2
    i=(m&F).sum(); u=((m|F)&K).sum(); iou=i/u if u else 0
    kx0,ky0,kx1,ky1=KOPF
    ys,xs=np.nonzero(ids==9)
    d=np.hypot(xs.mean()-(kx0+kx1)/2,ys.mean()-(ky0+ky1)/2) if len(xs) else 999
    kopf=max(0,1-d/40)
    rows=np.nonzero(((ids_voll>=0)&~np.isin(ids_voll,SCHW)).any(1))[0]
    so=max(0,1-abs(rows.max()-SOHLE)/120) if len(rows) else 0
    return iou,kopf,so,0.5*iou+0.25*(kopf+so)
def grob(a):
    c,i,g,x,z=a
    p=dict(clip=c,bild=i,gier=g,x=x,z=z)
    return bewerte(umriss(_mo,p),_T)+(c,i,g)
def fein(p0):
    f=lambda q: bewerte(umriss(_mo,q),_T)[3]
    p=dict(p0); s=f(p)
    for st in (64,32,16,8,4,2,1):
        besser=True
        while besser:
            besser=False
            zs=[('gier',st),('gier',-st),('x',8*st),('x',-8*st),('z',8*st),('z',-8*st)]
            if st>=16: zs+=[('bild',1),('bild',-1)]
            for k,d in zs:
                q=dict(p); q[k]+=d
                if k=='bild': q['bild']%=_mo.clips[q['clip']][1]
                if k=='gier': q['gier']%=4096
                t=f(q)
                if t>s+1e-5: s,p,besser=t,q,True
    return s,p
if __name__=='__main__':
    _init(); mo=_mo
    P2=dict(clip=12,bild=48,gier=861,x=-8182,z=-21330)
    print('P2',[round(v,4) for v in bewerte(umriss(mo,P2),_T)],flush=True)
    seen={}
    for c,(f0,n) in enumerate(mo.clips):
        for i in range(n): seen.setdefault(BB.kf_von_clip(mo,c,i),(c,i))
    kfs=sorted(seen.values())
    giere=[int(round(g*4096/360))%4096 for g in range(0,360,5)]
    args=[(c,i,g,P2['x'],P2['z']) for c,i in kfs for g in giere]
    print('kandidaten',len(args),flush=True)
    t0=time.time()
    with Pool(14,_init) as pool: R=pool.map(grob,args,chunksize=64)
    R.sort(key=lambda r:-r[3]); print('grob',round(time.time()-t0),'s',R[:5],flush=True)
    starts=[dict(clip=int(r[4]),bild=int(r[5]),gier=int(r[6]),x=P2['x'],z=P2['z']) for r in R[:24]]+[P2]
    with Pool(14,_init) as pool: F=pool.map(fein,starts)
    F.sort(key=lambda t:-t[0])
    out=dict(vorlage='n1439 (f1440)',kopfbox=KOPF,sohle=SOHLE,P2=P2,P2_wert=[round(float(v),4) for v in bewerte(umriss(mo,P2),_T)],
             grob=[[round(float(v),4) for v in r[:4]]+[int(r[4]),int(r[5]),int(r[6])] for r in R[:40]],
             fein=[dict(wert=round(float(s),4),teile=[round(float(v),4) for v in bewerte(umriss(mo,p),_T)[:3]],**p,grad=round(p['gier']*360/4096,1)) for s,p in F])
    json.dump(out,open(S8+'/m9_pose.json','w'),indent=1)
    for f in out['fein'][:8]: print(f)
