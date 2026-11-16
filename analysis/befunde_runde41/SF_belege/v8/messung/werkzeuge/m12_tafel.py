import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
L=json.load(open(S8+'/m2_linien.json'))
FARBE={'rot_T1':(237,28,36),'gruen_T2':(34,177,76),'blau_J':(0,162,232),'hellblau_H':(153,217,234),'weiss_K':(220,220,220)}
def linien(img,k=1,ox=0,oy=0,dicke=1):
    for n,c in FARBE.items():
        for p in L[n]['polylinien']:
            P=np.array([[(x-ox+0.5)*k,(y-oy+0.5)*k] for x,y in p],np.int32)
            cv2.polylines(img,[P],False,c,dicke)
def kontur(img,mask,col,k=1,ox=0,oy=0):
    cs,_=cv2.findContours(mask.astype(np.uint8),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
    for c in cs:
        c=((c.astype(float)-[ox,oy]+0.5)*k).astype(np.int32)
        cv2.polylines(img,[c],True,col,1)
if __name__=='__main__':
    Z=np.load(S8+'/m10_pv_v7.npz'); img=Z['img'][:480].copy(); bone=Z['bone'][:480]
    V=vorlage().astype(np.uint8)
    box=(240,160,490,450); k=3
    x0,y0,x1,y1=box
    def up(a): return cv2.resize(np.ascontiguousarray(a[y0:y1,x0:x1]),None,fx=k,fy=k,interpolation=cv2.INTER_NEAREST)
    t1=up(V); t2=up(np.asarray(Image.open(V80+'Darstellung.png').convert('RGB')))
    t3=up(img); 
    kontur(t3,bone>=0,(255,255,255),k,x0,y0); kontur(t3,np.isin(bone,range(16,23)),(255,0,255),k,x0,y0)
    linien(t3,k,x0,y0,2)
    T=np.load(S8+'/m7_trimap.npy')
    kontur(t3,T==1,(255,255,0),k,x0,y0)
    t4=up((0.5*V+0.5*img).astype(np.uint8)); linien(t4,k,x0,y0,2); kontur(t4,np.isin(bone,range(16,23)),(255,0,255),k,x0,y0)
    import b2
    for t,s in ((t1,'Vorlage n1439 (roh)'),(t2,'Darstellung.png (Nutzer)'),(t3,'V7 PV + Nutzerlinien; weiss V7-Umriss, magenta Schwanz 16-22, gelb Vorlage-Figur dE>9'),(t4,'50% Vorlage/V7')):
        b2.schrift(t,s,6,20,(255,255,0),0.55)
    W=np.vstack([np.hstack([t1,t2]),np.hstack([t3,t4])])
    os.makedirs(S8+'/tafeln',exist_ok=True)
    Image.fromarray(W).save(S8+'/tafeln/01_vorlage_linien_v7.png')
