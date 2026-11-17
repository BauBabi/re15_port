import sys, numpy as np
from PIL import Image
T,N,M=sys.argv[1:4]
L=lambda d,f: np.asarray(Image.open('%s/f%06d.ppm'%(d,f)).convert('RGB'),dtype=int)
tot=0; per=[]
for f in range(255,351):
    t,n,m=L(T,f),L(N,f),L(M,f)
    sw=np.abs(n-t).sum(2)>0
    g=sw&(np.abs(m-n).sum(2)>0)
    per.append(int(g.sum())); tot+=int(g.sum())
print('Schwanzpunkte aus Texelspalten u54-55: gesamt %d, max je Bild %d, Bilder mit >0: %d'%(tot,max(per),sum(1 for p in per if p)))
