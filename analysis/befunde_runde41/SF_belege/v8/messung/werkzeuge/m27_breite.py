import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
from m11_kontur import lauf
Q=json.load(open(S8+'/m8_quer.json')); T=Q['T']
dE=np.load(S8+'/m4_dET.npy'); dEs=cv2.GaussianBlur(dE.astype(np.float32),(0,0),1.0)
Z=np.load(S8+'/m10_pv_v7.npz'); bone=Z['bone'][:480]
R=lauf(T,dEs,bone,thr=5,gap=4)
L=json.load(open(S8+'/m2_linien.json'))
offs=[r['vorlage_aussen'] for r in R if r['vorlage_aussen'] is not None and r['vorlage_aussen']<40]
med=float(np.median(offs)); print('Median Abstand Nutzerlinie T -> Vorlage-Kante: %.1f px (n=%d)'%(med,len(offs)))
zeilen=[]
for r in R:
    va=r['vorlage_aussen'] if (r['vorlage_aussen'] is not None and r['vorlage_aussen']<40) else None
    ziel=va if va is not None else med
    ma=r['modell_aussen']
    r['ziel_aussen']=ziel; r['ziel_quelle']='Kante' if va is not None else 'T+Median'
    r['d_kante']=(ma-ziel) if ma is not None else None
    r['d_T']=ma
    vb=r['vorlage_breite'] if va is not None else None
    if r['s']<90: vb=None   # Ruecken: Schwanz nicht vom Ruecken trennbar
    r['vorlage_breite_s']=vb
    zeilen.append(r)
    print('s %3d  ziel %5.1f (%s)  V7 aussen %4s  d %5s | Breite Vorlage %4s  V7-Schwanz %3s %s'%(r['s'],ziel,r['ziel_quelle'],ma,r['d_kante'],vb,r['schwanz_breite'],r['schwanz_knochen']))
d=[abs(r['d_kante']) for r in zeilen if r['d_kante'] is not None]
dt=[abs(r['d_T']) for r in zeilen if r['d_T'] is not None]
print('S1 V7: Abstand zur Vorlage-Kante Mittel %.1f Max %d px; zur Nutzerlinie T Mittel %.1f Max %d px (n=%d)'%(np.mean(d),max(d),np.mean(dt),max(dt),len(d)))
# Mittellinie Vorlage (s>=90): T + n*(aussen+breite/2)
mitte=[]
for r in zeilen:
    if r['vorlage_breite_s']:
        o=r['vorlage_aussen']+r['vorlage_breite_s']/2; mitte.append([round(r['p'][0]+o*r['n'][0],1),round(r['p'][1]+o*r['n'][1],1),r['s']])
print('Vorlage-Mittellinie',mitte)
json.dump(dict(median_T_kante=med,stationen=zeilen,s1_v7=dict(mittel=round(float(np.mean(d)),1),max=int(max(d)),mittel_T=round(float(np.mean(dt)),1),max_T=int(max(dt))),mitte=mitte),open(S8+'/m27_breite.json','w'),indent=0)
