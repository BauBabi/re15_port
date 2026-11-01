# Mod-Block 25 gegen RE2 EM023: Skalierungsfaktor je Mesh-Paar, Restfehler; Textur-Vergleich.
import sys,struct
sys.path.insert(0,'.')
from emd_tab import *
import numpy as np
re2=open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read()
D=E.emd_dir(re2); m2=E.md1_parse(re2[D[7]:u32(re2,0)])
b25=blocks(R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')[25][1]
D5=E.emd_dir(b25); m25=E.md1_parse(b25[D5[7]:D5[8]])
# Paare gleicher Zaehlung (tv,qv,tf,qf): Mesh 7..14 in B25 <-> 1..7 u. a. in RE2
sig=lambda x:(len(x.tv),len(x.qv),len(x.tf),len(x.qf))
for i,x in enumerate(m25):
    c=[j for j,y in enumerate(m2) if sig(y)==sig(x)]
    if not c or sig(x)[0]<10: continue
    for j in c:
        a=np.array([v[:3] for v in x.tv+x.qv],float); b=np.array([v[:3] for v in m2[j].tv+m2[j].qv],float)
        s=(a*b).sum()/(b*b).sum()
        r=np.abs(a-s*b).max()
        print(f'B25 Mesh {i} <-> RE2 Mesh {j}: Skala {s:.4f}  max. Abweichung {r:.1f}  (Vertices {len(a)})  gleiche Flaechen {x.tf==m2[j].tf and x.qf==m2[j].qf}  gleiche UV {x.tuv==m2[j].tuv and x.quv==m2[j].quv}')
# Mesh 0 von B25 = Rumpf+Schwanz zusammengezogen?
print('B25 Mesh0',sig(m25[0]),'RE2 0,11..15 Summe',[sig(m2[k]) for k in (0,11,12,13,14,15)],sum(sig(m2[k])[0] for k in (0,11,12,13,14,15)))
# TIM
t25=b25[D5[8]:]; tr=open(R+'/re15_port/shared_assets/RE2/EM23.TIM','rb').read()
def tim(t):
    cs=struct.unpack_from('<I',t,8)[0]; cw,ch=struct.unpack_from('<2H',t,16)
    o=8+cs; bs,x,y,w,h=struct.unpack_from('<I4H',t,o)
    return cw,ch,t[20:8+cs],w,h,t[o+12:o+bs]
c1,h1,cl1,w1,hh1,p1=tim(t25); c2,h2,cl2,w2,hh2,p2=tim(tr)
print('TIM B25',c1,h1,w1*2,hh1,'RE2',c2,h2,w2*2,hh2,'Bytes',len(p1),len(p2))
a=np.frombuffer(p1,np.uint8).reshape(hh1,w1*2); b=np.frombuffer(p2,np.uint8).reshape(hh2,w2*2)
# RGB-Paletten anwenden und Pixel vergleichen
def rgb(p,cl,row):
    c=np.frombuffer(cl,'<u2').reshape(-1,256 if len(cl)>=512 else 16)[row]
    return np.stack([c&31,(c>>5)&31,(c>>10)&31],-1)[p]
for r1 in range(h1):
  for r2 in range(h2):
    A=rgb(a,cl1,r1).astype(int)
    for ofs in (0,128,256,384):
        if ofs+w1*2>w2*2: continue
        B=rgb(b[:, ofs:ofs+w1*2],cl2,r2).astype(int)
        d=np.abs(A-B).mean()
        print(f'  CLUT B25[{r1}] RE2[{r2}] x-Versatz {ofs}: mittl. Abweichung {d:.2f}, gleiche Pixel {(np.abs(A-B).sum(-1)==0).mean()*100:.1f}%')
