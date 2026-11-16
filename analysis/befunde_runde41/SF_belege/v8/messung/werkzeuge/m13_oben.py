"""Obere Kontur (je Spalte) und Vorderkante Kopf (je Zeile): Vorlage (dE>=8, 3 in Folge) gegen V7 (Id-Puffer).
Huckel = lokales Maximum der Hoehe (kleinstes y) mit Prominenz >= 3 px gegen beide Seiten (innerhalb 25 px)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
def oben_vorlage(dEs,x0=270,x1=470,y0=150,y1=300,thr=8):
    o={}
    for x in range(x0,x1+1):
        c=dEs[y0:y1,x]
        for y in range(len(c)-2):
            if c[y]>=thr and c[y+1]>=thr and c[y+2]>=thr: o[x]=y0+y; break
    return o
def oben_modell(bone,x0=270,x1=470,ausser=()):
    o={}
    for x in range(x0,x1+1):
        ys=np.nonzero((bone[:,x]>=0)&~np.isin(bone[:,x],ausser))[0]
        if len(ys): o[x]=int(ys.min())
    return o
def vorn_vorlage(dEs,y0=195,y1=255,x0=420,x1=500,thr=8):
    o={}
    for y in range(y0,y1+1):
        c=dEs[y,x0:x1][::-1]
        for i in range(len(c)-2):
            if c[i]>=thr and c[i+1]>=thr and c[i+2]>=thr: o[y]=x1-1-i; break
    return o
def vorn_modell(bone,y0=195,y1=255,x0=420,x1=500,nur=None):
    o={}
    for y in range(y0,y1+1):
        m=bone[y,x0:x1]>=0 if nur is None else np.isin(bone[y,x0:x1],nur)
        xs=np.nonzero(m)[0]
        if len(xs): o[y]=x0+int(xs.max())
    return o
def huckel(prof,prom=3,w=25,hoch=True):
    ks=sorted(prof); v=np.array([prof[k] for k in ks],float)
    h=-v if hoch else v
    H=[]
    for i in range(len(ks)):
        l=h[max(0,i-w):i]; r=h[i+1:i+1+w]
        if len(l)==0 or len(r)==0: continue
        if h[i]>=l.max() and h[i]>r.max():
            pl=h[i]-l.min(); pr=h[i]-r.min(); p=min(pl,pr)
            if p>=prom: H.append(dict(k=int(ks[i]),wert=int(v[i]),prom=round(float(p),1)))
    return H
def stufen(prof,schwelle=3):
    ks=sorted(prof); out=[]
    for a,b in zip(ks,ks[1:]):
        if b==a+1 and abs(prof[b]-prof[a])>=schwelle: out.append(dict(k=int(a),von=int(prof[a]),nach=int(prof[b]),d=int(prof[b]-prof[a])))
    return out
if __name__=='__main__':
    dE=np.load(S8+'/m4_dET.npy'); dEs=cv2.GaussianBlur(dE.astype(np.float32),(0,0),1.0)
    Z=np.load(S8+'/m10_pv_v7.npz'); bone=Z['bone'][:480]
    ov=oben_vorlage(dEs); om=oben_modell(bone); oms=oben_modell(bone,ausser=range(16,23))
    print('x    V    M   M_ohneSchwanz  oberster Knochen')
    for x in range(270,471,5):
        kb=int(bone[om[x],x]) if x in om else None
        print(x,ov.get(x),om.get(x),oms.get(x),kb)
    hv=huckel({k:v for k,v in ov.items() if 330<=k<=470}); hm=huckel({k:v for k,v in om.items() if 330<=k<=470})
    print('Huckel Vorlage',hv); print('Huckel V7',hm)
    print('Stufen Vorlage',stufen(ov)); print('Stufen V7',stufen(om))
    fv=vorn_vorlage(dEs); fm=vorn_modell(bone)
    print('Vorderkante y: V / M'); print([(y,fv.get(y),fm.get(y)) for y in range(195,256,3)])
    print('Huckel vorn V',huckel(fv,hoch=False,w=15),'M',huckel(fm,hoch=False,w=15))
    json.dump(dict(oben_vorlage=ov,oben_v7=om,oben_v7_ohne_schwanz=oms,huckel_vorlage=hv,huckel_v7=hm,stufen_vorlage=stufen(ov),stufen_v7=stufen(om),vorn_vorlage=fv,vorn_v7=fm),open(S8+'/m13_oben.json','w'))
