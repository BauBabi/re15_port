import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
import v4_gemein as G
L=json.load(open(S8+'/m2_linien.json'))
dE=np.load(S8+'/m4_dET.npy'); dEs=cv2.GaussianBlur(dE.astype(np.float32),(0,0),1.0)
V=vorlage(); Lab=G.lab(cv2.GaussianBlur(V.astype(np.float32),(0,0),0.8))
# T = gruen (oben->unten) + rot (oben->unten): Ursprung an der Rueckenoberkante
g=L['gruen_T2']['polylinien'][0]           # [372,180] -> [272,288]
r=L['rot_T1']['polylinien'][0][::-1]       # [268,296] -> [270,333]
T=np.array(g+r,float)
def resample(P,step):
    d=np.r_[0,np.cumsum(np.hypot(*np.diff(P,axis=0).T))]
    s=np.arange(0,d[-1]+1e-6,step)
    return np.stack([np.interp(s,d,P[:,0]),np.interp(s,d,P[:,1])],1),s,d[-1]
# Glaettung der Polylinie vor Normalen
Ts,s,Ltot=resample(T,1.0)
k=7; Tg=np.stack([np.convolve(np.pad(Ts[:,i],k,'edge'),np.ones(2*k+1)/(2*k+1),'valid') for i in (0,1)],1)
print('T Laenge %.1f px'%Ltot)
def samp(A,x,y):
    xi=int(round(x)); yi=int(round(y))
    if 0<=xi<640 and 0<=yi<480: return A[yi,xi]
    return np.nan
rows=[]
for st in range(0,int(Ltot)+1,10):
    i=min(st,len(Tg)-1); p=Tg[i]
    t=Tg[min(i+3,len(Tg)-1)]-Tg[max(i-3,0)]; t/=np.hypot(*t)
    n=np.array([t[1],-t[0]])   # Normale: nach innen? pruefen: Richtung zum Koerper (rechts/unten)
    if n[0]<0: n=-n
    prof=[(o,samp(dEs,*(p+o*n)),samp(Lab[...,0],*(p+o*n)),samp(Lab[...,1],*(p+o*n))) for o in range(-6,80)]
    # aeussere Kante: erste Stelle o>=-6 mit dEs>=8 (3 in Folge)
    au=None
    for j in range(len(prof)-2):
        if all(prof[j+q][1]>=8 for q in range(3)): au=prof[j][0]; break
    rows.append(dict(s=st,p=[round(float(p[0]),1),round(float(p[1]),1)],n=[round(float(n[0]),3),round(float(n[1]),3)],aussen=au,
        prof=[[o,round(float(a),1),round(float(b),1),round(float(c),1)] for o,a,b,c in prof]))
    print('s %3d p (%5.1f,%5.1f) n (%5.2f,%5.2f) aussen %s'%(st,p[0],p[1],n[0],n[1],au))
    print('   dE ',''.join('%3d'%min(99,a) if a==a else '  .' for o,a,b,c in prof[6:66:2]))
    print('   L  ',''.join('%3d'%b if b==b else '  .' for o,a,b,c in prof[6:66:2]))
    print('   a  ',''.join('%3d'%c if c==c else '  .' for o,a,b,c in prof[6:66:2]))
json.dump(dict(T=Tg.tolist(),quer=rows),open(S8+'/m8_quer.json','w'))
