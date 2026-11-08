# Leser A V4 / L2,L4,L5 + H: Endpunkte, Farben, Kopf, H. Alle Zahlen in 640x480-Bildpixeln.
import sys, json, numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab
O=sys.argv[1]; V='C:/workspace/git/reAi_v2/Version_80%/'
SW=17.0
fr=np.load(O+'/frames1300.npy')
I1=np.array(Image.open(V+'Beschreibung1_Vorlage.png').convert('RGB')).astype(float)   # = n1484
I2=np.array(Image.open(V+'Beschreibung2_Vorlage.png').convert('RGB')).astype(float)   # = n1434
assert np.abs(I1-fr[184]).max()==0 and np.abs(I2-fr[134]).max()==0
L1=json.load(open(O+'/a_l1.json',encoding='utf8'))
out={}
# ---------- Abblendung n1484 gegen Ruhestand (Median n1440..1470), statische Wand/Boden ----------
ref=np.median(fr[140:171].astype(float),axis=0)
stat=np.zeros((480,640),bool); stat[100:330,20:250]=True; stat[380:440,30:200]=True; stat[230:330,560:630]=True
k=[]
for c in range(3):
    a=I1[...,c][stat]-SW; b=ref[...,c][stat]-SW; k.append(float((a*b).sum()/(b*b).sum()))
res_fit=[float(np.sqrt((((I1[...,c][stat]-SW)-k[c]*(ref[...,c][stat]-SW))**2).mean())) for c in range(3)]
out['abblendung_1484']=dict(k_rgb=[round(v,4) for v in k],rmse=[round(v,2) for v in res_fit],ref='Median n1440..1470',felder='Wand links x20-249 y100-329, Boden x30-199 y380-439, Wand rechts x560-629 y230-329',
    formel='(v1484-17) = k*(v_ruhe-17)')
kk=np.array(k)
schwarz=dict(balken_unten=fr[184][460:478].reshape(-1,3).mean(0).round(1).tolist(),balken_oben=fr[184][0:40].reshape(-1,3).mean(0).round(1).tolist())
out['schwarzwert']=dict(angesetzt=SW,gemessen_n1484=schwarz)
def lab(rgb): return rgb2lab(np.clip(np.array(rgb,float)/255.0,0,1).reshape(1,1,3)).reshape(3)
def stats(img,mask,fade=False):
    px=img[mask]; n=len(px)
    if n==0: return None
    raw=px.mean(0); sd=px.std(0)
    sub=np.clip((px-SW)*255.0/(255.0-SW),0,255)
    labs=rgb2lab(np.clip(px/255.0,0,1).reshape(-1,1,3)).reshape(-1,3)
    labs_sub=rgb2lab(np.clip(sub/255.0,0,1).reshape(-1,1,3)).reshape(-1,3)
    d=dict(n=int(n),rgb=raw.round(1).tolist(),rgb_std=sd.round(1).tolist(),lab=lab(raw).round(1).tolist(),lab_std=labs.std(0).round(1).tolist(),
           rgb_sw=sub.mean(0).round(1).tolist(),lab_sw=lab(sub.mean(0)).round(1).tolist(),lab_sw_std=labs_sub.std(0).round(1).tolist())
    if fade:
        fc=SW+(px-SW)/kk; fcs=np.clip((fc-SW)*255.0/(255.0-SW),0,255)
        d.update(rgb_entblendet=fc.mean(0).round(1).tolist(),lab_entblendet=lab(fc.mean(0)).round(1).tolist(),
                 rgb_entblendet_sw=fcs.mean(0).round(1).tolist(),lab_entblendet_sw=lab(fcs.mean(0)).round(1).tolist())
    return d
def boxmask(b,ero=2,shape=(480,640)):
    m=np.zeros(shape,bool); x0,y0,x1,y1=b; m[y0+ero:y1-ero,x0+ero:x1-ero]=True; return m
def erode(m,e=2): return cv2.erode(m.astype(np.uint8),np.ones((2*e+1,2*e+1),np.uint8))>0
# ---------- H ----------
def oberkante(img,refimg,xs,y0,y1,thr):
    tops=[]
    for x in xs:
        d=np.abs(img[y0:y1,x]-refimg[y0:y1,x]).sum(1); w=np.where(d>thr)[0]
        tops.append((int(y0+w[0]) if len(w) else None,x))
    return tops
# Bild 1: Hintergrund = Median n1350..1473 abgeblendet mit k (oberhalb y150 frei von Birkin? -> Spaltenprofil gedruckt)
bg=np.array(Image.open(O+'/bg_median_1350_1473.png')).astype(float)
bg1=SW+(bg-SW)*kk
t1=oberkante(I1,bg1,range(360,431,2),120,175,45)
out['H1_oberkante_spalten']=t1
# Bild 2: Hintergrund in Kopfhoehe aus n1360 (Birkin dort links unten, s. Blatt) -> Pruefung per Profil
t2=oberkante(I2,fr[60].astype(float),range(340,461,4),160,215,60)
out['H2_oberkante_spalten']=t2
json.dump(out,open(O+'/a_messung_teil1.json','w'),indent=1)
print(json.dumps(out,indent=0)[:3000])
