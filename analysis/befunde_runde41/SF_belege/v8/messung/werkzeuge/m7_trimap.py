import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import cv2, json
from scipy import ndimage as nd
dE=np.load(S8+'/m4_dET.npy')
dEs=cv2.GaussianBlur(dE.astype(np.float32),(0,0),1.0)
box=np.zeros_like(dE,bool); box[150:452,250:505]=True
F=(dEs>9)&box
F=nd.binary_closing(F,np.ones((5,5)),iterations=2)
F=nd.binary_fill_holes(F)
lab,n=nd.label(F); sz=nd.sum(F,lab,range(1,n+1)); F=lab==(1+int(np.argmax(sz)))
B=(dEs<3.5)&~nd.binary_dilation(F,np.ones((7,7)))
T=np.full(dE.shape,2,np.int8); T[F]=1; T[B]=0
# Bodenschatten/Spiegelung: Boden y>=340 ausser Beinen nur unbekannt lassen (Schatten aendert den Boden)
T[(T==1)&(np.arange(480)[:,None]>=345)&~((np.arange(640)[None,:]>330)&(np.arange(640)[None,:]<460))]=2
L=json.load(open(S8+'/m2_linien.json'))
# Schwanzzone: Polygon aus T (rot+gruen, von unten nach oben) und J (blau unten->oben), geschlossen
Tl=L['rot_T1']['polylinien'][0][::-1]  # unten->oben? rot: von [270,333] bis [268,296]
Tl=L['rot_T1']['polylinien'][0]
g=L['gruen_T2']['polylinien'][0][::-1]   # von [272,288] bis [372,180]
j=L['blau_J']['polylinien'][0]            # [357,241] -> [379,189]
pol=np.array(Tl+g+[[381,182]]+j[::-1]+[[340,300],[300,340]],np.int32)
Z=np.zeros(dE.shape,np.uint8); cv2.fillPoly(Z,[pol],1)
np.save(S8+'/m7_trimap.npy',T); np.save(S8+'/m7_schwanzzone.npy',Z>0)
print('figur',(T==1).sum(),'bg',(T==0).sum(),'unbekannt',(T==2).sum(),'schwanzzone',Z.sum())
V=vorlage().astype(np.uint8).copy()
c=V.copy(); c[T==1]=(0.5*c[T==1]+[0,120,0]).astype(np.uint8); c[T==2]=(0.5*c[T==2]+[120,0,0]).astype(np.uint8)
cv2.polylines(c,[pol],True,(255,255,0),1)
Image.fromarray(c).crop((220,130,540,470)).resize((640,680),Image.NEAREST).save(S8+'/m7_trimap.png')
