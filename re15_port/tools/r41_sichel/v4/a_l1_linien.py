# Leser A V4 / L1: Linienzuordnung Text -> Endpunkt per Pixelbelegung (nicht Augenmass)
# Maske: Kernrot = |RGB - (237,28,36)| <= 40 (Paint-Rot, gemessen als haeufigste Nichtweiss-Farbe im Randfeld)
import sys, json, numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont
from skimage.morphology import skeletonize
V='C:/workspace/git/reAi_v2/Version_80%/'
OUT=sys.argv[1]; BELEG=sys.argv[2]
RED=np.array([237,28,36],float); TOL=40
TEXTE={'Beschreibung1':None,'Beschreibung2':None}
res={}
def skel_ends(m):
    sk=skeletonize(m>0)
    k=np.ones((3,3),int); k[1,1]=0
    nb=cv2.filter2D(sk.astype(np.uint8),-1,k.astype(np.float32),borderType=cv2.BORDER_CONSTANT)
    ys,xs=np.where(sk&(nb==1)); return sk,list(zip(xs.tolist(),ys.tolist()))
def belegung(core_d,p,q,step=0.5):
    p=np.array(p,float); q=np.array(q,float); L=np.linalg.norm(q-p); n=max(2,int(L/step))
    t=np.linspace(0,1,n); pts=p[None]+t[:,None]*(q-p)[None]
    xi=np.clip(np.round(pts[:,0]).astype(int),0,core_d.shape[1]-1); yi=np.clip(np.round(pts[:,1]).astype(int),0,core_d.shape[0]-1)
    return float(core_d[yi,xi].mean()), L
def seg_cross(a,b,c,d):
    def o(p,q,r): return np.sign((q[0]-p[0])*(r[1]-p[1])-(q[1]-p[1])*(r[0]-p[0]))
    return o(a,b,c)*o(a,b,d)<0 and o(c,d,a)*o(c,d,b)<0
for f in ['Beschreibung1','Beschreibung2']:
    B=np.array(Image.open(V+f+'.png').convert('RGB')).astype(float)
    Vl=np.array(Image.open(V+f+'_Vorlage.png').convert('RGB')).astype(float)
    core=(np.linalg.norm(B-RED,axis=2)<=TOL)
    diff=np.zeros(core.shape,bool); diff[:,:640]=np.abs(B[:,:640]-Vl).sum(2)>40
    n,lab,st,cen=cv2.connectedComponentsWithStats(core.astype(np.uint8),connectivity=8)
    glyph=np.zeros_like(core); linie=np.zeros_like(core)
    for i in range(1,n):
        x,y,w,h,a=st[i]
        if w>40: linie|=(lab==i)
        elif x>=640: glyph|=(lab==i)
    # Textbloecke: Glyphen horizontal 12 / vertikal 9 px schliessen
    gd=cv2.dilate(glyph.astype(np.uint8),np.ones((9,13),np.uint8))
    m,glab,gst,_=cv2.connectedComponentsWithStats(gd,connectivity=8)
    bloecke=[]
    for i in range(1,m):
        x,y,w,h,a=gst[i]
        g=glyph&(glab==i); ys,xs=np.where(g)
        if len(xs)<30: continue
        bloecke.append(dict(x0=int(xs.min()),x1=int(xs.max()),y0=int(ys.min()),y1=int(ys.max()),px=int(len(xs))))
    bloecke.sort(key=lambda b:b['y0'])
    # Skelett-Enden der Linien
    sk,ends=skel_ends(linie)
    # Enden je Seite: Bildteil (x<640) vs Randfeld
    links=[e for e in ends if e[0]<640]; rechts=[e for e in ends if e[0]>=640]
    core_d=cv2.dilate(core.astype(np.uint8),np.ones((3,3),np.uint8))>0
    res[f]=dict(maske='|RGB-(237,28,36)|<=40, Kernpixel %d, Linienpixel %d (Bildteil %d), Diff-zur-Vorlage-Pixel %d'%(core.sum(),linie.sum(),linie[:,:640].sum(),diff.sum()),
                bloecke=bloecke,skel_enden_bild=links,skel_enden_rand=rechts)
    np.save(OUT+'/'+f+'_linie.npy',linie); np.save(OUT+'/'+f+'_core.npy',core)
    print(f,res[f]['maske']); print(' Bloecke',bloecke); print(' Enden Bild',links); print(' Enden Rand',rechts)
json.dump(res,open(OUT+'/a_l1_roh.json','w'),indent=1)

# ---- Teil 2: Zuordnung ----
from collections import deque
from itertools import permutations
def merge_bloecke(bl):
    out=[]
    for b in bl:
        if out and b['y0']-out[-1]['y1']<=10 and abs(b['x0']-out[-1]['x0'])<20:
            o=out[-1]; o.update(x0=min(o['x0'],b['x0']),x1=max(o['x1'],b['x1']),y1=b['y1'],px=o['px']+b['px'])
        else: out.append(dict(b))
    return out
NAMEN={'Beschreibung1':['Auge','Haare','Nase'],
       'Beschreibung2':['Auge von der Seite etwa hier','Haare von der Seite','Schwanzspitze Ende','Schwanz Beginn in den Ruecken ueberfuehrend.','Schwanz Knick, von da an wird es breiter nach oben']}
def geodesic(sk,a,b):
    H,W=sk.shape; prev={a:None}; dq=deque([a])
    while dq:
        p=dq.popleft()
        if p==b: break
        for dx in (-1,0,1):
            for dy in (-1,0,1):
                q=(p[0]+dx,p[1]+dy)
                if q!=p and 0<=q[0]<W and 0<=q[1]<H and sk[q[1],q[0]] and q not in prev:
                    prev[q]=p; dq.append(q)
    if b not in prev: return None
    path=[b]
    while prev[path[-1]] is not None: path.append(prev[path[-1]])
    return path[::-1]
def spitze(core,e,richt):
    # aeusserster Kernpixel in Linienrichtung (vom Text weg) innerhalb 5 px um das Skelett-Ende,
    # nur Pixel mit Querabstand <= 2 px zur Linienachse durch e (keine Nachbarlinie)
    ys,xs=np.where(core[max(0,e[1]-5):e[1]+6,max(0,e[0]-5):e[0]+6]); xs=xs+max(0,e[0]-5); ys=ys+max(0,e[1]-5)
    pr=(xs-e[0])*richt[0]+(ys-e[1])*richt[1]; q=np.abs(-(xs-e[0])*richt[1]+(ys-e[1])*richt[0])
    ok=q<=2.0; xs,ys,pr=xs[ok],ys[ok],pr[ok]
    i=np.argmax(pr); return int(xs[i]),int(ys[i]),float(pr[i])
erg={}
for f in ['Beschreibung1','Beschreibung2']:
    linie=np.load(OUT+'/'+f+'_linie.npy'); core=np.load(OUT+'/'+f+'_core.npy')
    core_d=cv2.dilate(core.astype(np.uint8),np.ones((3,3),np.uint8))>0
    sk=skeletonize(linie)
    bl=merge_bloecke(res[f]['bloecke']); names=NAMEN[f]; assert len(bl)==len(names),(f,bl)
    R=res[f]['skel_enden_rand']; E=res[f]['skel_enden_bild']
    # Textanker -> naechstes Rand-Ende (Abstand zum linken Mittelpunkt der ersten Zeile)
    anker=[]
    for b,nm in zip(bl,names):
        tl=(b['x0'],b['y0']+5)
        d=[np.hypot(r[0]-tl[0],r[1]-tl[1]) for r in R]; j=int(np.argmin(d))
        anker.append(dict(text=nm,box=[b['x0'],b['y0'],b['x1'],b['y1']],anker_text=list(tl),linien_start=list(R[j]),abstand_px=round(float(d[j]),1),zweitnaechster_px=round(float(sorted(d)[1]),1)))
    M=np.zeros((len(anker),len(E))); L=np.zeros_like(M)
    for i,a in enumerate(anker):
        for k,e in enumerate(E):
            M[i,k],L[i,k]=belegung(core_d,a['linien_start'],e)
    # beste Gesamtzuordnung (alle Permutationen), Summe der Belegung
    best=None
    for perm in permutations(range(len(E)),len(anker)):
        s=sum(M[i,perm[i]] for i in range(len(anker)))
        if best is None or s>best[0]: best=(s,perm)
    perm=best[1]
    nl,llab=cv2.connectedComponents(linie.astype(np.uint8),connectivity=8)
    def komp(p): return int(llab[p[1],p[0]])
    zeilen=[]
    for i,a in enumerate(anker):
        k=perm[i]; e=E[k]
        alt=[(M[i,kk],kk) for kk in range(len(E)) if kk!=k]; alt.sort(reverse=True)
        st=np.array(a['linien_start'],float); rv=np.array(e,float)-st; rv/=np.linalg.norm(rv)
        g=geodesic(sk,tuple(a['linien_start']),tuple(e))
        geo=None
        if g is not None:
            pts=np.array(g); ch=np.array(e)-st
            dist=np.abs(ch[0]*(pts[:,1]-st[1])-ch[1]*(pts[:,0]-st[0]))/np.linalg.norm(ch)
            ki=int(np.argmax(dist)); knick=pts[ki].tolist()
            ap=cv2.approxPolyDP(pts.reshape(-1,1,2).astype(np.int32),2.0,False).reshape(-1,2)
            if dist[ki]>4 and len(ap)>=3: knick=ap[1].tolist()
            geo=dict(polylinie_dp2=ap.tolist(),skelett_zusammenhaengend=True,pfad_px=len(g),max_abw_von_gerade_px=round(float(dist[ki]),1),knick=knick)
            if dist[ki]>4:
                b1,_=belegung(core_d,a['linien_start'],knick); b2,_=belegung(core_d,knick,e)
                l1=np.hypot(*(np.array(knick)-st)); l2=np.hypot(*(np.array(e)-np.array(knick)))
                geo['belegung_polylinie']=round((b1*l1+b2*l2)/(l1+l2),3)
        if geo and 'belegung_polylinie' in geo:
            rv=np.array(e,float)-np.array(geo['knick'],float); rv/=np.linalg.norm(rv)
        tip=spitze(core,e,rv)
        zeilen.append(dict(**a,endpunkt_skelett=list(e),endpunkt_spitze=[tip[0],tip[1]],belegung=round(float(M[i,k]),3),strecke_px=round(float(L[i,k]),1),
            beste_alternative=list(E[alt[0][1]]),belegung_alternative=round(float(alt[0][0]),3),geodaetisch=geo,
            komponente_start=komp(a['linien_start']),komponenten_enden={str(list(ee)):komp(ee) for ee in E}))
        if geo and 'belegung_polylinie' in geo:
            kn=geo['knick']; altp=[]
            for kk in range(len(E)):
                if kk==k: continue
                b1,_=belegung(core_d,a['linien_start'],kn); b2,_=belegung(core_d,kn,E[kk])
                l1=np.hypot(kn[0]-st[0],kn[1]-st[1]); l2=np.hypot(E[kk][0]-kn[0],E[kk][1]-kn[1])
                altp.append(((b1*l1+b2*l2)/(l1+l2),kk))
            altp.sort(reverse=True)
            zeilen[-1]['belegung_poly']=geo['belegung_polylinie']; zeilen[-1]['beste_alternative_poly']=list(E[altp[0][1]]); zeilen[-1]['belegung_alternative_poly']=round(float(altp[0][0]),3)
    # Kreuzungen der gewaehlten Strecken
    kr=[]
    for i in range(len(zeilen)):
        for j in range(i+1,len(zeilen)):
            def segs(z):
                g=z['geodaetisch']; pts=[z['linien_start']]+([g['knick']] if g and 'belegung_polylinie' in g else [])+[z['endpunkt_skelett']]
                return list(zip(pts[:-1],pts[1:]))
            if any(seg_cross(a1,b1,a2,b2) for a1,b1 in segs(zeilen[i]) for a2,b2 in segs(zeilen[j])):
                kr.append([zeilen[i]['text'],zeilen[j]['text']])
    erg[f]=dict(maske=res[f]['maske'],matrix=dict(zeilen=[a['text'] for a in anker],spalten=[list(e) for e in E],belegung=np.round(M,3).tolist(),laenge=np.round(L,1).tolist()),
                zuordnung=zeilen,kreuzungen=kr,summe_beste=round(best[0],3))
    print('==',f); print(' Spalten',E)
    for a,row in zip(anker,M): print(' %-50s'%a['text'],np.round(row,3))
    for z in zeilen: print(' ->',z['text'],z['linien_start'],z['endpunkt_skelett'],z['endpunkt_spitze'],z['belegung'],'alt',z['beste_alternative'],z['belegung_alternative'],z['geodaetisch'],'komp',z['komponente_start'],z['komponenten_enden'],z.get('belegung_poly'),z.get('beste_alternative_poly'),z.get('belegung_alternative_poly'))
    print(' Kreuzungen',kr)
json.dump(erg,open(OUT+'/a_l1.json','w'),indent=1,ensure_ascii=False)
