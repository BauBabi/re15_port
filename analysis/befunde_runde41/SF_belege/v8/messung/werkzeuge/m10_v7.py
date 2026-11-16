import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
import v4_gemein as G, b2, f_rast as FR
mo=b2.modell(EMD_V7)
d=json.load(open(S8+'/m9_pose.json'))
PV={k:d['fein'][0][k] for k in ('clip','bild','gier','x','z')}
A=FR.mit_bg(mo,PV)
np.savez_compressed(S8+'/m10_pv_v7.npz',img=A['img'],bone=A['bone'],zeile=A['zeile'],u=A['u'],v=A['v'],idx=A['idx'],lok=A['lok'],nl=A['nl'],flaeche=A['flaeche'].astype(str))
b=A['bone'][:480]
print('PV',PV)
for k in range(23):
    n=(b==k).sum()
    if n: 
        lx=A['lok'][:480][b==k][:,0]
        print('bone',k,'px',n,'lok x median %.0f'%np.nanmedian(lx),'bbox x',np.nonzero(b==k)[1].min(),np.nonzero(b==k)[1].max(),'y',np.nonzero(b==k)[0].min(),np.nonzero(b==k)[0].max())
# Kamera-Richtung: Kette der (W)-rechts/(W)-links-Projektion: Knochen 13 vs 10 Gelenke
import birkin_buehne as BB
kf=BB.kf_von_clip(mo,PV['clip'],PV['bild']); R,T=mo.pose(kf)
r,kam,M,li=b2.raum()
for bn in (8,9,10,13,16,17,18,19,20,21,22):
    print('gelenk',bn,'bild',np.round(b2.projiziere(PV,T[bn].astype(float)[None])[0],1))
