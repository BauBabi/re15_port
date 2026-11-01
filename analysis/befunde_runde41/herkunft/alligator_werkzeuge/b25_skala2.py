import sys,struct
sys.path.insert(0,'.')
from emd_tab import *
import numpy as np
re2=open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read()
D=E.emd_dir(re2); m2=E.md1_parse(re2[D[7]:u32(re2,0)])
b25=blocks(R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')[25][1]
D5=E.emd_dir(b25); m25=E.md1_parse(b25[D5[7]:D5[8]])
pairs=[(7,1),(8,2),(9,3),(10,4),(11,5),(12,6),(13,7),(14,None)]
for i,j in pairs:
    if j is None: continue
    A=np.array(sorted(set(v[:3] for v in m25[i].tv+m25[i].qv)),float)
    B=np.array(sorted(set(v[:3] for v in m2[j].tv+m2[j].qv)),float)
    best=None
    for s in np.arange(0.15,0.30,0.001):
        d=np.sqrt(((A[:,None,:]-s*B[None,:,:])**2).sum(-1)).min(1)
        q=(d.mean(),d.max(),s)
        if best is None or q<best: best=q
    print(f'B25 Mesh {i} vs RE2 Mesh {j}: |A|={len(A)} |B|={len(B)} beste Skala {best[2]:.3f} mittl. Abstand {best[0]:.2f} max {best[1]:.2f}')
# Vergleich gegen das Bind-Modell: Dreieck-Indizes (nur Struktur)
for i,j in pairs:
    if j is None: continue
    tf=[(t[1],t[3],t[5]) for t in m25[i].tf]; tg=[(t[1],t[3],t[5]) for t in m2[j].tf]
    print(i,j,'Tri-Indizes gleich',tf==tg,'Quad-Indizes gleich',[(q[1],q[3],q[5],q[7]) for q in m25[i].qf]==[(q[1],q[3],q[5],q[7]) for q in m2[j].qf])
print('UV gleich?',[(m25[i].tuv==m2[j].tuv) for i,j in pairs if j is not None])
print(m25[12].tuv[:3]); print(m2[6].tuv[:3])
