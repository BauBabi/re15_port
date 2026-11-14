import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, b2, birkin_buehne as BB
mo=b2.modell(EMD_V7)
PV=dict(clip=17,bild=47,gier=587,x=-8246,z=-21266)
out={}
for n,(x,y) in {'schwanz_boden_vorlage (Ende rote Linie, Kante +8 px)':(278,333),'fuss_bild_links':(325,430),'fuss_bild_rechts':(435,428),'sohle_mitte':(380,436)}.items():
    w=BB.boden_aus_video('50A0',12,x,y); out[n]=w; print(n,(x,y),'-> Welt',w, 'Abstand zur Lage PV',round(float(np.hypot(w[0]-PV['x'],w[2]-PV['z']))))
# Massstab px/E. an der Lage (senkrecht, Hoehe 1500)
p0=b2.projiziere(PV,[[0,-1500,0]])[0]; p1=b2.projiziere(PV,[[0,-2500,0]])[0]
print('px je 1000 E. senkrecht bei y -1500..-2500:',round(float(np.hypot(*(p1-p0))),1))
out['px_je_1000']=round(float(np.hypot(*(p1-p0))),1)
# Schwanzspitze-Bodenpunkt im Modellraum der PV (Gier zurueckdrehen)
import emd_ansichtsblatt as EA
g=PV['gier']; s,c=EA.sin_q12(g)/4096.0,EA.cos_q12(g)/4096.0
Y=np.array([[c,0,s],[0,1,0],[-s,0,c]])
for n in list(out):
    if isinstance(out[n],list):
        w=np.array(out[n],float)-[PV['x'],0,PV['z']]
        m=Y.T@w; print(n,'Modellraum PV',np.round(m).astype(int).tolist())
        out[n+'_modell']=np.round(m).astype(int).tolist()
json.dump(out,open(S8+'/m20_raum.json','w'),indent=1)
