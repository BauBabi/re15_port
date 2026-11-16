import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import b2, v4_gemein as G
mo=b2.modell(EMD_V7)
ot=np.load(S8+'/m15_orig_tex.npy'); ol=np.load(S8+'/m15_orig_lut.npy')
vt=mo.tex; vl=mo.lut
print('V7 tex',vt.shape,'lut',vl.shape)
gleich=(vt==ot)
for n,(u0,v0,u1,v1) in {'Arm-Adern p0 u0-75 v0-150':(0,0,76,151),'Augapfel p0 u78-127 v0-100':(78,0,128,101),'Brust p1 u128-255 v0-90':(128,0,256,91),'Kopf p0 u56-127 v136-255':(56,136,128,256),'p2 u256-383':(256,0,384,256),'Bauch p1 u128-255 v90-125':(128,90,256,126)}.items():
    g=gleich[v0:v1,u0:u1]; print(n,'Texel gleich Original %.1f %%'%(100*g.mean()))
def lab_lut(l):
    return G.lab(l[...,:3].astype(float))
OL=lab_lut(ol); VL=lab_lut(vl)
for z in range(ol.shape[0]):
    print('Original Zeile',480+z,'Eintraege a*>=15:',int((OL[z][:,1]>=15).sum()),'a*>=10:',int((OL[z][:,1]>=10).sum()))
for z in range(vl.shape[0]):
    eq=[zo+480 for zo in range(ol.shape[0]) if np.array_equal(vl[z],ol[zo])]
    print('V7 Zeile',480+z,'a*>=15:',int((VL[z][:,1]>=15).sum()),'a*>=10:',int((VL[z][:,1]>=10).sum()),'= Original-Zeile' if eq else '',eq)
