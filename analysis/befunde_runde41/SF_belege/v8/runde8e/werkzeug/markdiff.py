import sys, numpy as np
from PIL import Image
a,b=sys.argv[1],sys.argv[2]
tot=[]
for f in range(255,346):
    x=np.asarray(Image.open(a+'/f%06d.ppm'%f).convert('RGB'),dtype=int)
    y=np.asarray(Image.open(b+'/f%06d.ppm'%f).convert('RGB'),dtype=int)
    m=(np.abs(x-y).sum(2)>0); g=m&(y[...,1]>y[...,0]+30)&(y[...,1]>y[...,2]+30)
    tot.append((f,int(m.sum()),int(g.sum())))
print(' '.join('%d:%d/%d'%t for t in tot))
