"""Schwanzkontur/Breite entlang der Nutzerlinie T (gruen+rot): Vorlage (dE gegen Konsens-Hintergrund) gegen Modell (Id-Puffer)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
def lauf(T, dEs, bone, thr=7, gap=2, schw=range(16,23)):
    Tg=np.array(T)
    out=[]
    def at(A,x,y,dflt):
        xi,yi=int(round(x)),int(round(y))
        return A[yi,xi] if 0<=xi<A.shape[1] and 0<=yi<A.shape[0] else dflt
    for st in range(0,len(Tg),10):
        p=Tg[st]; t=Tg[min(st+3,len(Tg)-1)]-Tg[max(st-3,0)]; t/=np.hypot(*t)
        n=np.array([t[1],-t[0]]); n=-n if n[0]<0 else n
        offs=np.arange(-30,71)
        pts=[p+o*n for o in offs]
        dv=np.array([at(dEs,x,y,0) for x,y in pts]); bv=np.array([at(bone,x,y,-1) for x,y in pts])
        r=dict(s=st,p=[round(float(p[0]),1),round(float(p[1]),1)],n=[round(float(n[0]),3),round(float(n[1]),3)])
        # Vorlage aussen: erste Stelle o>=-6 mit 3x dEs>=8
        va=None
        for j in range(24,len(offs)-2):
            if all(dv[j+q]>=8 for q in range(3)): va=int(offs[j]); break
        vb=None
        if va is not None:
            j=list(offs).index(va); k=j; g=0
            while k+1<len(offs):
                if dv[k+1]>=thr: k+=1; g=0
                elif g<gap and k+2<len(offs) and dv[k+2]>=thr: k+=2
                else: break
            vb=int(offs[k])
        r['vorlage_aussen']=va; r['vorlage_innen']=vb; r['vorlage_breite']=(vb-va+1) if va is not None else None
        ma=[int(o) for o,b in zip(offs,bv) if b>=0]; r['modell_aussen']=ma[0] if ma else None
        mt=[int(o) for o,b in zip(offs,bv) if b in schw]
        if mt:
            j0=list(offs).index(mt[0]); k=j0
            while k+1<len(offs) and bv[k+1] in schw: k+=1
            r['schwanz_aussen']=mt[0]; r['schwanz_breite']=int(offs[k]-mt[0]+1); r['schwanz_knochen']=sorted(set(int(b) for b in bv[j0:k+1]))
        else: r['schwanz_aussen']=None; r['schwanz_breite']=0; r['schwanz_knochen']=[]
        r['erster_knochen']=int(bv[list(offs).index(ma[0])]) if ma else None
        out.append(r)
    return out
if __name__=='__main__':
    Q=json.load(open(S8+'/m8_quer.json')); T=Q['T']
    dE=np.load(S8+'/m4_dET.npy'); dEs=cv2.GaussianBlur(dE.astype(np.float32),(0,0),1.0)
    Z=np.load(S8+'/m10_pv_v7.npz'); bone=Z['bone'][:480]
    R=lauf(T,dEs,bone)
    print(' s   p            Vaussen Vinnen Vbreite | Maussen erst  Saussen Sbreite Knochen')
    for r in R: print('%3d (%5.1f,%5.1f) %5s %5s %5s | %5s %4s %5s %4s %s'%(r['s'],r['p'][0],r['p'][1],r['vorlage_aussen'],r['vorlage_innen'],r['vorlage_breite'],r['modell_aussen'],r['erster_knochen'],r['schwanz_aussen'],r['schwanz_breite'],r['schwanz_knochen']))
    json.dump(R,open(S8+'/m11_kontur_v7.json','w'),indent=0)
