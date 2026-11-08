"""Plan-Probe (nur im Speicher, keine Datei geaendert): Ader-Zeilen X1 = V7-Zeile 480 + Original-Ader-Eintraege (Original-Zeile 480,
a* >= 15, L* <= 38), X2 = V7-Zeile 481 + Original-Ader-Eintraege (Original-Zeile 481). Flaechen: Mesh 8 (W)-rechts (Schwerpunkt x < 0)
Zeile 480 -> X1, 481 -> X2; Mesh 13 Zeile 480 -> X1. Messung PV: Rot-Anteil je Teil (a* >= 10, Videoraum)."""
import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import json, b2, f_rast as FR, v4_gemein as G
mo=b2.modell(EMD_V7)
ol=np.load(S8+'/m15_orig_lut.npy')
OL=[G.lab(ol[z][...,:3].astype(float)) for z in range(3)]
X1=mo.lut[0].copy(); X2=mo.lut[1].copy()
a1=(OL[0][:,1]>=15)&(OL[0][:,0]<=38); a2=(OL[1][:,1]>=15)&(OL[1][:,0]<=38)
X1[a1]=ol[0][a1]; X2[a2]=ol[1][a2]
print('Ader-Eintraege Zeile 480:',int(a1.sum()),'481:',int(a2.sum()))
nz=mo.lut.shape[0]
mo.lut=np.concatenate([mo.lut,X1[None],X2[None]],0)
Z1,Z2=480+nz,480+nz+1
def setz(uv,z): uv=list(uv); uv[2]=(uv[2]&~(0x1FF<<6))|(z<<6); return uv
n=0
for b,regel in ((8,'r'),(13,'alle')):
    m=mo.meshes[b]
    P=[v[:3] for v in m.tv]
    for name in ('tuv','quv'):
        fs=m.tf if name=='tuv' else m.qf
        Pv=m.tv if name=='tuv' else m.qv
        uvs=list(getattr(m,name))
        for i,(g,uv) in enumerate(zip(fs,uvs)):
            z=(uv[2]>>6)&0x1FF
            x=np.mean([Pv[k][0] for k in g[1::2]])
            if regel=='r' and x>=0: continue
            if z==480: uvs[i]=setz(uv,Z1); n+=1
            elif z==481 and b==8: uvs[i]=setz(uv,Z2); n+=1
        setattr(m,name,uvs)
print('Flaechen umgestellt',n,'Zeilen gesamt',mo.lut.shape[0],'Texturhoehe',256*mo.lut.shape[0])
PV=dict(clip=17,bild=47,gier=587,x=-8246,z=-21266)
A=FR.mit_bg(mo,PV); img=A['img'][:480]; bone=A['bone'][:480]; lok=A['lok'][:480]
LM=G.lab(G.videoraum(img))
res={}
for n_,m in {'Oberarm 13':bone==13,'Rumpf 8 (W)-rechts':(bone==8)&(lok[...,0]<0),'Rumpf 8 (W)-links':(bone==8)&(lok[...,0]>=0)}.items():
    res[n_]=round(float((LM[m][:,1]>=10).mean()*100),1); 
print('Probe Rot-Anteil',res)
np.save(S8+'/m22_probe_img.npy',img)
json.dump(dict(ader_480=int(a1.sum()),ader_481=int(a2.sum()),flaechen=n,rot=res),open(S8+'/m22_adersim.json','w'))
