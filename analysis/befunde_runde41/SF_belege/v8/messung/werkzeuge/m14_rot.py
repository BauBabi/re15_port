"""Rot-Anteil (a* >= 10, Videoraum) je Koerperteil (Id-Puffer V7 in PV) und je fester Bildregion: Vorlage n1439 gegen V7."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, cv2
import v4_gemein as G
V=vorlage(); LV=G.lab(V)
Z=np.load(S8+'/m10_pv_v7.npz'); img=Z['img'][:480]; bone=Z['bone'][:480]; lok=Z['lok'][:480]; zeile=Z['zeile'][:480]
LM=G.lab(G.videoraum(img))
T=np.load(S8+'/m7_trimap.npy'); F=T==1
teile={'Oberarm (W)-rechts 13':[13],'Unterarm/Sichel (W)-rechts 14+15':[14,15],'Rumpf 8 (W)-rechts lok.x<0':'8r','Rumpf 8 (W)-links lok.x>=0':'8l','Kopf 9':[9],'Schwanz 16-22':list(range(16,23)),'Oberarm (W)-links 10':[10],'Klauenarm 11+12':[11,12],'Becken/Beine 0-7':list(range(0,8))}
out={}
print('Teil: px | Vorlage rot %% (a* Median) | V7 rot %% (a* Median)')
for n,k in teile.items():
    if k=='8r': m=(bone==8)&(lok[...,0]<0)
    elif k=='8l': m=(bone==8)&(lok[...,0]>=0)
    else: m=np.isin(bone,k)
    mv=m&F   # nur wo die Vorlage sicher Figur ist
    rv=(LV[mv][:,1]>=10).mean()*100 if mv.sum() else float('nan')
    rm=(LM[m][:,1]>=10).mean()*100 if m.sum() else float('nan')
    out[n]=dict(px=int(m.sum()),px_vorlage=int(mv.sum()),vorlage_rot=round(rv,1),vorlage_a_med=round(float(np.median(LV[mv][:,1])),1) if mv.sum() else None,
                v7_rot=round(rm,1),v7_a_med=round(float(np.median(LM[m][:,1])),1) if m.sum() else None)
    print(n,out[n])
# Feste Regionen der Vorlage (Bildkoordinaten, Figur sicher): Bild-rechts / Bild-links der Kopf-Ruecken-Linie
reg={'Figur Bild-rechts x>=400':(400,150,480,452),'Figur Bild-links x<400':(250,150,400,452),'Ruecken oben y<215':(330,150,450,215),'Brust/Arm-Mitte 345-430 x 215-300 y':(345,215,430,300)}
for n,(x0,y0,x1,y1) in reg.items():
    m=np.zeros_like(F); m[y0:y1,x0:x1]=True
    mv=m&F; mm=m&(bone>=0)
    out[n]=dict(vorlage_rot=round((LV[mv][:,1]>=10).mean()*100,1),vorlage_px=int(mv.sum()),v7_rot=round((LM[mm][:,1]>=10).mean()*100,1),v7_px=int(mm.sum()))
    print(n,out[n])
# Rotkarten
rk=np.zeros((480,640,3),np.uint8); rk[F]=(80,80,80); rk[F&(LV[...,1]>=10)]=(255,40,40)
rm=np.zeros((480,640,3),np.uint8); rm[bone>=0]=(80,80,80); rm[(bone>=0)&(LM[...,1]>=10)]=(255,40,40)
np.save(S8+'/m14_rotkarten.npy',np.stack([rk,rm]))
# CLUT-Zeilen der roten V7-Pixel je Teil
for n,k in (('13',[13]),('8',[8]),('14',[14])):
    m=np.isin(bone,k)&(LM[...,1]>=10); z,c=np.unique(zeile[m],return_counts=True); print('rot V7 Knochen',n,'Zeilen',dict(zip(z.tolist(),c.tolist())))
    m=np.isin(bone,k); z,c=np.unique(zeile[m],return_counts=True); print('alle V7 Knochen',n,'Zeilen',dict(zip(z.tolist(),c.tolist())))
json.dump(out,open(S8+'/m14_rot.json','w'),indent=1)
