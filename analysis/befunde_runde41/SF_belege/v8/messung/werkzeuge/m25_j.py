"""S2/S3-Masse: (a) Sattel-Tiefe der oberen Kontur unter ihrer oberen konvexen Huelle (x 300-447); (b) Naht entlang J: Helligkeits-
sprung quer zu J (+-4 px) und Knochenwechsel (V7); (c) H: Huegelhoehe = Kontur ueber der Sehne x 405 -> 450; (d) K: Abweichung der
Vorderkante von der Geraden (y 210-244)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2, v4_gemein as G
O=json.load(open(S8+'/m13b_oben.json')); ov={int(k):v for k,v in O['oben_vorlage_maske'].items()}; om={int(k):v for k,v in O['oben_v7'].items()}
def sattel(prof,a,b):
    xs=np.array([x for x in range(a,b+1) if x in prof]); ys=np.array([prof[x] for x in xs],float)
    # obere Huelle (kleinstes y = oben): konvexe Huelle der Punkte (x,-y), obere Kette
    pts=sorted(zip(xs,-ys)); H=[]
    for p in pts:
        while len(H)>=2 and (H[-1][0]-H[-2][0])*(p[1]-H[-2][1])-(H[-1][1]-H[-2][1])*(p[0]-H[-2][0])>=0: H.pop()
        H.append(p)
    hx=np.array([h[0] for h in H]); hy=-np.array([h[1] for h in H])
    hull=np.interp(xs,hx,hy); dep=ys-hull
    i=int(np.argmax(dep)); return round(float(dep.max()),1),int(xs[i])
res={}
for n,(a,b) in dict(J_Ruecken=(300,420),Ruecken_Kopf=(380,447)).items():
    res['sattel_'+n]=dict(vorlage=sattel(ov,a,b),v7=sattel(om,a,b))
# Anstieg Schwanzwurzel -> Ruecken (x 330 -> 405)
res['anstieg_330_405']=dict(vorlage=ov[330]-ov[405],v7=om[330]-om[405])
# H: Huegel ueber der Sehne 405->447 (Vorlage) bzw. V7
def huegel(prof,a,b):
    xs=np.arange(a,b+1); ys=np.array([prof[x] for x in xs],float); ch=np.interp(xs,[a,b],[ys[0],ys[-1]])
    h=ch-ys; i=int(np.argmax(h)); return round(float(h.max()),1),int(xs[i])
res['huegel_H_405_447']=dict(vorlage=huegel(ov,405,447),v7=huegel(om,405,447))
# Naht J
L=json.load(open(S8+'/m2_linien.json')); J=np.array(L['blau_J']['polylinien'][0],float)   # (357,241)->(379,189)
V=vorlage(); LV=G.lab(V)[...,0]
Z=np.load(S8+'/m10_pv_v7.npz'); img=Z['img'][:480]; bone=Z['bone'][:480]; LM=G.lab(G.videoraum(img))[...,0]
sv=[];sm=[];wech=0;n=0
for i in range(2,len(J)-2,2):
    p=J[i]; t=J[i+2]-J[i-2]; t/=np.hypot(*t); nn=np.array([t[1],-t[0]])
    a=p+4*nn; b=p-4*nn
    ai=tuple(np.round(a).astype(int)); bi=tuple(np.round(b).astype(int))
    sv.append(abs(LV[ai[1],ai[0]]-LV[bi[1],bi[0]])); sm.append(abs(LM[ai[1],ai[0]]-LM[bi[1],bi[0]]))
    ba,bb=bone[ai[1],ai[0]],bone[bi[1],bi[0]]; n+=1
    if (ba in range(16,23))!=(bb in range(16,23)): wech+=1
res['naht_J']=dict(dL_vorlage_mittel=round(float(np.mean(sv)),1),dL_v7_mittel=round(float(np.mean(sm)),1),v7_knochenwechsel_schwanz_rumpf=f'{wech}/{n}')
# K: Vorderkante
fv={};fm={}
T=np.load(S8+'/m7_trimap.npy')
Kl=np.array(L['weiss_K']['polylinien'][0],float)
for y in range(212,244):
    xs=np.nonzero(bone[y,420:500]>=0)[0]; fm[y]=420+int(xs.max()) if len(xs) else None
def gerade_abw(prof):
    ys=np.array([y for y in prof if prof[y] is not None]); xs=np.array([prof[y] for y in ys],float)
    c=np.polyfit(ys,xs,1); r=xs-np.polyval(c,ys); return round(float(np.abs(r).max()),1)
res['K_nutzerlinie_abw_gerade']=gerade_abw({int(p[1]):p[0] for p in Kl})
res['K_v7_vorderkante_abw_gerade']=gerade_abw(fm)
res['K_v7_vorderkante']=fm
print(json.dumps({k:v for k,v in res.items() if k!='K_v7_vorderkante'},indent=0))
json.dump(res,open(S8+'/m25_j.json','w'))
