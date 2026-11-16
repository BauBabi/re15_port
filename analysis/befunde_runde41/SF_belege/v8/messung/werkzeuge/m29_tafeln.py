import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2, b2
from m12_tafel import linien, kontur
V=vorlage().astype(np.uint8); Z=np.load(S8+'/m10_pv_v7.npz'); img=Z['img'][:480]; bone=Z['bone'][:480]
B=json.load(open(S8+'/m27_breite.json'))
box=(240,160,490,360); k=3; x0,y0=box[:2]
def up(a): return cv2.resize(np.ascontiguousarray(a[box[1]:box[3],box[0]:box[2]]),None,fx=k,fy=k,interpolation=cv2.INTER_NEAREST)
def P(x,y): return (int((x-x0+0.5)*k),int((y-y0+0.5)*k))
# 02 Kontur entlang T
t1=up(V); t2=up(img)
for t in (t1,t2):
    linien(t,k,x0,y0,1)
    for r in B['stationen']:
        p=np.array(r['p']); n=np.array(r['n'])
        cv2.line(t,P(*(p-10*n)),P(*(p+60*n)),(90,90,90),1)
        if r['ziel_aussen'] is not None: cv2.circle(t,P(*(p+r['ziel_aussen']*n)),4,(0,255,0) if r['ziel_quelle']=='Kante' else (0,140,0),-1)
        if r['modell_aussen'] is not None: cv2.circle(t,P(*(p+r['modell_aussen']*n)),3,(255,0,255),-1)
        if r['vorlage_breite_s']: cv2.circle(t,P(*(p+(r['vorlage_aussen']+r['vorlage_breite_s'])*n)),3,(0,255,255),-1)
for m in B['mitte']: cv2.circle(t1,P(m[0],m[1]),3,(255,255,255),1)
kontur(t2,np.isin(bone,range(16,23)),(255,0,255),k,x0,y0)
b2.schrift(t1,'Vorlage n1439: gruen Kante (dunkelgruen T+11), gelb Innenkante, weiss Mitte',6,20,(255,255,0),0.5)
b2.schrift(t2,'V7 PV: magenta Aussenkante V7 je Station, Schwanz 16-22 umrandet',6,20,(255,255,0),0.5)
Image.fromarray(np.hstack([t1,t2])).save(S8+'/tafeln/02_kontur_entlang_T.png')
# 04 Oberkontur
O=json.load(open(S8+'/m13b_oben.json')); ov={int(a):b for a,b in O['oben_vorlage_maske'].items()}; om={int(a):b for a,b in O['oben_v7'].items()}
box2=(320,165,475,260); k2=5
def up2(a): return cv2.resize(np.ascontiguousarray(a[box2[1]:box2[3],box2[0]:box2[2]]),None,fx=k2,fy=k2,interpolation=cv2.INTER_NEAREST)
def P2(x,y): return (int((x-box2[0]+0.5)*k2),int((y-box2[1]+0.5)*k2))
s1=up2(V); s2=up2((0.5*V+0.5*img).astype(np.uint8))
for t in (s1,s2):
    linien(t,k2,box2[0],box2[1],2)
    cv2.polylines(t,[np.array([P2(x,ov[x]) for x in sorted(ov) if 330<=x<=460],np.int32)],False,(255,255,0),1)
    cv2.polylines(t,[np.array([P2(x,om[x]) for x in sorted(om) if 330<=x<=462],np.int32)],False,(255,0,255),1)
b2.schrift(s1,'Vorlage: gelb Oberkontur Vorlage, magenta V7 (PV)',6,20,(255,255,0),0.5)
b2.schrift(s2,'50% Vorlage/V7',6,20,(255,255,0),0.5)
Image.fromarray(np.vstack([s1,s2])).save(S8+'/tafeln/04_oberkontur_J_H_K.png')
# 07 Rotkarten
R=np.load(S8+'/m14_rotkarten.npy'); box3=(250,160,490,452)
r1=cv2.resize(R[0][box3[1]:box3[3],box3[0]:box3[2]],None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)
r2=cv2.resize(R[1][box3[1]:box3[3],box3[0]:box3[2]],None,fx=2,fy=2,interpolation=cv2.INTER_NEAREST)
r1=np.ascontiguousarray(r1); r2=np.ascontiguousarray(r2)
b2.schrift(r1,'Vorlage a*>=10 (rot) / Figur sicher (grau)',6,20,(255,255,0),0.5); b2.schrift(r2,'V7 PV a*>=10 (Videoraum)',6,20,(255,255,0),0.5)
Image.fromarray(np.hstack([r1,r2])).save(S8+'/tafeln/07_rotkarte_vorlage_v7.png')
