import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
from m13_oben import *
import json
T=np.load(S8+'/m7_trimap.npy'); F=T==1
Z=np.load(S8+'/m10_pv_v7.npz'); bone=Z['bone'][:480]
ov={x:int(np.nonzero(F[:,x])[0].min()) for x in range(280,471) if F[:,x].any()}
om=oben_modell(bone,280,470)
print('x   V   V7  d  Knochen')
for x in range(285,466,5): print(x,ov.get(x),om.get(x),(om[x]-ov[x]) if x in ov and x in om else None, int(bone[om[x],x]) if x in om else None)
hv=huckel({k:v for k,v in ov.items() if 340<=k<=465},w=20); hm=huckel({k:v for k,v in om.items() if 340<=k<=465},w=20)
print('Huckel V',hv); print('Huckel V7',hm)
# Abstand der Konturen in den Abschnitten J (x 345-385), Ruecken (385-420), H (420-447), K/Kopf (447-462)
for n,(a,b) in dict(J=(345,385),Ruecken=(385,420),H=(420,447),Kopf=(447,462)).items():
    d=[om[x]-ov[x] for x in range(a,b+1) if x in om and x in ov]
    print(n,'V7-V mittel %.1f max |d| %d'%(np.mean(d),np.max(np.abs(d))))
json.dump(dict(oben_vorlage_maske=ov,oben_v7=om,huckel_vorlage=hv,huckel_v7=hm),open(S8+'/m13b_oben.json','w'))
