import sys,struct,hashlib,collections
sys.path.insert(0,'.')
from emd_tab import *
import numpy as np
b25=blocks(R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')[25][1]
D=E.emd_dir(b25)
m25=E.md1_parse(b25[D[7]:D[8]])
print('B25 meshes',len(m25),[ (len(x.tv),len(x.qv),len(x.tf),len(x.qf)) for x in m25])
re2=open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read()
DR=E.emd_dir(re2); print('RE2 dirs',DR,len(re2))
m2=E.md1_parse(re2[DR[7]:u32(re2,0)])
print('RE2 meshes',len(m2),[ (len(x.tv),len(x.qv),len(x.tf),len(x.qf)) for x in m2])
s=lambda M:collections.Counter(tuple(v[:3]) for x in M for v in x.tv+x.qv)
c25=s(m25); c2=s(m2)
print('Punkte B25',len(c25),'RE2',len(c2),'gemeinsam',len(set(c25)&set(c2)))
# Skalierungs-Test: Bbox je Mesh
def bb(x):
    p=[v[:3] for v in x.tv+x.qv]
    if not p: return None
    a=np.array(p); return tuple(a.min(0)),tuple(a.max(0))
for i,x in enumerate(m25): print('B25',i,bb(x))
for i,x in enumerate(m2): print('RE2',i,bb(x))
# TIM
t25=b25[D[8]:]
tr=open(R+'/re15_port/shared_assets/RE2/EM23.TIM','rb').read()
print('TIM25 head',t25[:32].hex(),len(t25)); print('TIMRE2 head',tr[:32].hex(),len(tr))
