# Leser A V4 / L3: Schwanzmaske Bild 2 (n1434) + Achse + Breiten
import sys, json, numpy as np, cv2
from PIL import Image
from skimage.morphology import skeletonize
from collections import deque
O=sys.argv[1]
RGT=int(sys.argv[2]) if len(sys.argv)>2 else 5; DT=int(sys.argv[3]) if len(sys.argv)>3 else 40; TAG=sys.argv[4] if len(sys.argv)>4 else ''
V='C:/workspace/git/reAi_v2/Version_80%/'
f=np.array(Image.open(V+'Beschreibung2_Vorlage.png').convert('RGB')).astype(int)
bg=np.array(Image.open(O+'/bg_median_1350_1473.png')).astype(int)
R,G,B=f[...,0],f[...,1],f[...,2]; L=f.mean(2); RG=R-G
diff=np.abs(f-bg).sum(2)
yy,xx=np.mgrid[0:480,0:640]
roi=(xx>=262)&(xx<=358)&(yy>=180)&(yy<=356)
roi&=~((xx>=345)&(yy>=250))          # Arm/Koerper rechts unten ausgeschlossen
roi&=~((yy>=330)&(xx>=306))
roi&=~((xx>=330)&(yy<196))          # dunkle Tuer-Oberkante (Rotstich ohne Platte) ausgeschlossen          # Boden/Vordergrund unten rechts ausgeschlossen
rot=(RG>=RGT)&(L<120)&((xx>=318)|(diff>=15))   # Rotstich nur, wo das Pixel nicht der Hintergrundplatte gleicht
hell=(diff>=DT)&(xx<318)               # Medianplatte nur fuer x<318 frei von Birkin (gemessen, s. Blatt)
M=(rot|hell)&roi
M=cv2.morphologyEx(M.astype(np.uint8),cv2.MORPH_CLOSE,np.ones((3,3),np.uint8))
n,lab,st,_=cv2.connectedComponentsWithStats(M,connectivity=8)
keep=[lab[345,292],lab[307,288],lab[242,355]]
print('Komponenten an Spitze/Knick/Beginn:',keep,[st[k][4] for k in keep])
M=np.isin(lab,[k for k in set(keep) if k>0])
# Loecher fuellen
inv=(~M).astype(np.uint8); nn,ll=cv2.connectedComponents(inv,connectivity=4)
for i in range(1,nn):
    ys,xs=np.where(ll==i)
    if len(xs)<300 and xs.min()>0 and ys.min()>0: M[ll==i]=True
np.save(O+'/schwanz_maske%s.npy'%TAG,M)
print('Maskenpixel',M.sum())
# Skelett + Pfad Beginn -> Spitze
sk=skeletonize(M)
pts=np.argwhere(sk)  # (y,x)
def nearest(p): d=np.hypot(pts[:,1]-p[0],pts[:,0]-p[1]); i=np.argmin(d); return (int(pts[i,1]),int(pts[i,0])),float(d[i])
a,da=nearest((355,242)); b,db=nearest((292,345)); kn,dk=nearest((288,307))
print('Skelett naechst Beginn',a,da,'Spitze',b,db,'Knick',kn,dk)
def bfs(src,dst=None):
    prev={src:None}; dq=deque([src]); last=src
    while dq:
        p=dq.popleft(); last=p
        if p==dst: break
        for dx in (-1,0,1):
            for dy in (-1,0,1):
                q=(p[0]+dx,p[1]+dy)
                if q!=p and 0<=q[0]<640 and 0<=q[1]<480 and sk[q[1],q[0]] and q not in prev: prev[q]=p; dq.append(q)
    end=dst if dst is not None else last
    pa=[end]
    while prev[pa[-1]] is not None: pa.append(prev[pa[-1]])
    return pa[::-1]
nl,cl=cv2.connectedComponents(M.astype(np.uint8),connectivity=8)
if cl[a[1],a[0]]==cl[b[1],b[0]]:
    path=bfs(a,b); luecken=[]
else:
    # oberer Teil: vom Beginn zum geodaetisch fernsten Skelettpunkt (BFS-Ende); unterer Teil: oberster Skelettpunkt -> Spitze
    p1=bfs(a)
    lo=np.argwhere(sk&(cl==cl[b[1],b[0]])); top=lo[np.argmin(lo[:,0])]; t=(int(top[1]),int(top[0]))
    p2=bfs(t,b)
    g0=np.array(p1[-1],float); g1=np.array(p2[0],float); ng=int(np.hypot(*(g1-g0)))
    bridge=[tuple(np.round(g0+(g1-g0)*k/ng).astype(int)) for k in range(1,ng)]
    path=p1+bridge+p2; luecken=[dict(von=list(p1[-1]),bis=list(p2[0]),laenge_px=round(float(np.hypot(*(g1-g0))),1))]
    print('Maske getrennt, Luecke',luecken)
path=np.array(path,float)
seg=np.r_[0,np.cumsum(np.hypot(*np.diff(path,axis=0).T))]
# Achse glaetten (gleitendes Mittel 7) nur fuer Tangenten
def tang(i):
    j0=max(0,i-5); j1=min(len(path)-1,i+5); t=path[j1]-path[j0]; return t/np.linalg.norm(t)
def breite(i):
    p=path[i]; t=tang(i); nrm=np.array([-t[1],t[0]])
    out=[]
    for sgn in (1,-1):
        s=0.0
        while s<60:
            q=p+sgn*(s+0.5)*nrm; x,y=int(round(q[0])),int(round(q[1]))
            if not(0<=x<640 and 0<=y<480) or not M[y,x]: break
            s+=0.5
        out.append(s)
    return out[0]+out[1]+1.0, (p+out[0]*nrm).tolist(), (p-out[1]*nrm).tolist()
ik=int(np.argmin(np.hypot(path[:,0]-288,path[:,1]-307)))
res=dict(luecken=luecken,pfad_len_px=round(float(seg[-1]),1),knick_index=ik,knick_pfad=path[ik].tolist(),s_knick=round(float(seg[ik]),1),
         beginn_pfad=path[0].tolist(),spitze_pfad=path[-1].tolist(),abstand_skelett=dict(beginn=round(da,1),spitze=round(db,1),knick=round(dk,1)))
def prof(i0,i1,name):
    rows=[]
    for fr_ in (0.1,0.3,0.5,0.7,0.9):
        target=seg[i0]+fr_*(seg[i1]-seg[i0]); i=int(np.argmin(np.abs(seg-target)))
        w,p1,p2=breite(i); rows.append(dict(anteil=fr_,punkt=[round(v,1) for v in path[i].tolist()],breite_px=round(w,1),rand1=[round(v,1) for v in p1],rand2=[round(v,1) for v in p2]))
    res[name]=rows
prof(ik,len(path)-1,'knick_zu_ende'); prof(ik,0,'knick_zu_beginn')
# letzte 10 % (Knick->Ende): Breiten
i9=int(np.argmin(np.abs(seg-(seg[ik]+0.9*(seg[-1]-seg[ik])))))
res['letzte10_breiten']=[round(breite(i)[0],1) for i in range(i9,len(path)-1)]
# Polygonzug aus den Linien
P={'beginn':(355,242),'knick':(288,307),'ende':(292,345)}
v1=np.subtract(P['knick'],P['beginn']); v2=np.subtract(P['ende'],P['knick'])
ang=lambda v: float(np.degrees(np.arctan2(-v[1],v[0])))
res['text_polygon']=dict(punkte=P,len_beginn_knick=round(float(np.hypot(*v1)),1),len_knick_ende=round(float(np.hypot(*v2)),1),
    richtung_beginn_knick_grad=round(ang(v1),1),richtung_knick_ende_grad=round(ang(v2),1),
    knickwinkel_richtungsaenderung=round(float(abs((ang(v2)-ang(v1)+180)%360-180)),1))
# Pfad-Richtungen stueckweise (Grad, 0=rechts, 90=oben im Bild)
dirs=[]
for k in range(0,len(path)-10,10):
    v=path[k+10]-path[k]; dirs.append(dict(von=path[k].tolist(),grad=round(ang(v),1)))
res['pfad_richtungen']=dirs
res['pfad']=path[::3].tolist()
# Spitze: Masken-Unterkante
ys,xs=np.where(M); res['maske_bbox']=[int(xs.min()),int(ys.min()),int(xs.max()),int(ys.max())]
res['parameter']=dict(RG_min=RGT,diff_min=DT); json.dump(res,open(O+'/schwanz%s.json'%TAG,'w'),indent=1)
for k in ['pfad_len_px','knick_pfad','s_knick','text_polygon','knick_zu_ende','knick_zu_beginn','letzte10_breiten','maske_bbox']: print(k,res[k])
print('luecken',luecken)
for d in dirs: print(d)
# Belegbild
vis=f.copy().astype(np.uint8); ov=vis.copy(); ov[M]=(0.5*ov[M]+0.5*np.array([255,0,255])).astype(np.uint8)
Image.fromarray(ov).crop((240,170,380,370)).resize((560,800),Image.NEAREST).save(O+'/schwanz_maske_zoom%s.png'%TAG)
