"""Ursache Schwanzform: Skelett (relpos, Eltern) 16-22, Ring-Radien je Glied (Mesh 16-22, Knochenraum), Gelenkwinkel in PV und
ueber alle 912 Keyframes, Lage der Wurzel gegen den Ruecken (Mesh 8)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, b2, birkin_buehne as BB
mo=b2.modell(EMD_V7)
sk=mo.skel
print('parents',[ (i,sk.parent[i] if hasattr(sk,'parent') else None) for i in range(8,23)] if hasattr(sk,'parent') else dir(sk))
out={}
out['relpos']={b:[int(v) for v in sk.rel[b]] for b in range(16,23)}
out['laenge']={b:round(float(np.linalg.norm(sk.rel[b])),1) for b in range(17,23)}
print('relpos',out['relpos']); print('Gliedlaengen (relpos 17..22)',out['laenge'])
# Mesh-Radien: je Mesh Ecken im Knochenraum; Achse = Richtung zum naechsten Gelenk (relpos des Kindes); Radius = Abstand zur Achse
rad={}
for b in range(16,23):
    m=mo.meshes[b]; P=np.array([v[:3] for v in m.tv]+[v[:3] for v in m.qv],float); P=np.unique(P,axis=0)
    ax=np.array(sk.rel[b+1],float) if b<22 else np.array([0,1,0.])
    L=np.linalg.norm(ax); ax=ax/L
    t=P@ax; r=np.linalg.norm(P-np.outer(t,ax),axis=1)
    # Radius-Profil in 5 Abschnitten entlang t
    q=np.linspace(t.min(),t.max(),6)
    prof=[round(float(r[(t>=q[i])&(t<=q[i+1])].max()),0) if ((t>=q[i])&(t<=q[i+1])).any() else None for i in range(5)]
    rad[b]=dict(ecken=len(P),t_min=round(float(t.min())),t_max=round(float(t.max())),glied=round(float(L)) if b<22 else None,r_max=round(float(r.max())),profil=prof)
    print('mesh',b,rad[b])
out['radien']=rad
# Rumpf Mesh 8: Ruecken-Oberseite (Knochenraum, y klein = oben); Gelenk 16 im Raum von 8 = relpos 16
P8=np.array([v[:3] for v in mo.meshes[8].tv]+[v[:3] for v in mo.meshes[8].qv],float)
j16=np.array(sk.rel[16],float)
d=np.linalg.norm(P8-j16,axis=1)
print('Gelenk16 in 8:',j16,'naechste Rumpfecke',round(float(d.min())),'Rumpf y-min (oben)',P8[:,1].min(),'z-min (hinten)',P8[:,2].min(),'x-min',P8[:,0].min())
# Oberste Rumpfecken hinten (W)-rechts
top=P8[np.argsort(P8[:,1])[:6]]; print('oberste Rumpfecken',top.astype(int).tolist())
out['gelenk16_in_8']=j16.tolist(); out['rumpf_oben']=top.astype(int).tolist()
# Gelenkwinkel: in PV und ueber alle Keyframes
def winkel(kf):
    R,T=mo.pose(kf)
    P=[T[b].astype(float) for b in range(16,23)]
    L=float(np.linalg.norm(np.array(sk.rel[22],float))); P.append(T[22].astype(float)+R[22].astype(float)[:,1]/4096.0*L)
    P=np.array(P); w=[]
    for j in range(1,len(P)-1):
        a=P[j]-P[j-1]; c=P[j+1]-P[j]
        w.append(float(np.degrees(np.arccos(np.clip(a@c/np.linalg.norm(a)/np.linalg.norm(c),-1,1)))))
    # Winkel des ersten Glieds gegen die Senkrechte
    a0=P[1]-P[0]; v0=float(np.degrees(np.arccos(np.clip(a0[1]/np.linalg.norm(a0),-1,1))))
    return w,v0,P,R,T
PV=dict(clip=17,bild=47); kf=BB.kf_von_clip(mo,17,47)
w,v0,P,R,T=winkel(kf)
print('PV Gelenkwinkel 17..22 (Grad)',[round(x,1) for x in w],'Glied16 gegen +y',round(v0,1))
print('PV Gelenke Modellraum',P.astype(int).tolist())
# tiefste Koerperecke (ohne Schwanz) in PV vs Spitze
ys=[]
for b in range(16):
    m=mo.meshes[b]; Q=np.array([v[:3] for v in m.tv]+[v[:3] for v in m.qv],float)
    ys.append(((R[b].astype(float)/4096.0)@Q.T).T[:,1].max()+T[b][1])
print('PV Sohle (max y Koerper)',round(max(ys)),'Spitze y',round(P[-1][1]),'Wurzel y',round(P[0][1]),'Ruecken oben y',round(min(((R[8].astype(float)/4096)@P8.T).T[:,1]+T[8][1])))
seen={}
for c,(f0,n) in enumerate(mo.clips):
    for i in range(n): seen.setdefault(BB.kf_von_clip(mo,c,i),(c,i))
W=[];V0=[]
for kf in seen: w,v0,_,_,_=winkel(kf); W.append(w); V0.append(v0)
W=np.array(W)
out['winkel_alle']=dict(n=len(W),median=[round(float(x),1) for x in np.median(W,0)],p10=[round(float(x),1) for x in np.percentile(W,10,0)],p90=[round(float(x),1) for x in np.percentile(W,90,0)],
                        glied16_gegen_senkrechte_median=round(float(np.median(V0)),1))
print('alle Keyframes',out['winkel_alle'])
out['pv']=dict(winkel=[round(x,1) for x in winkel(kf)[0]])
json.dump(out,open(S8+'/m19_kette.json','w'),indent=1)
