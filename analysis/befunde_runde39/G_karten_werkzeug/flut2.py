import sys, collections
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/re15_port/tools')
import gen_map_zones as G
import numpy as np
from PIL import Image
rid=int(sys.argv[1],16); bands=[int(b) for b in sys.argv[2].split(',')]; R=int(sys.argv[3]); out=sys.argv[4]
pts=[tuple(map(int,p.split(','))) for p in sys.argv[5:]]
sca=G.read_rdt(rid)[0]
cells=[c for c in sca if (c[4]>>4) in bands]
walls=[c for c in cells if (c[6]&1)]
allc=sca
xs=[c[0] for c in allc]+[c[0]+c[2] for c in allc]; zs=[c[1] for c in allc]+[c[1]+c[3] for c in allc]
x0,x1,z0,z1=min(xs)-2000,max(xs)+2000,min(zs)-2000,max(zs)+2000
S=100
nx,nz=(x1-x0)//S+1,(z1-z0)//S+1
solid=np.zeros((nz,nx),bool)
for (x,z,w,d,flr,typ,u0) in walls:
    a=max(0,(x-R-x0)//S); b=min(nx-1,(x+w+R-x0)//S); c=max(0,(z-R-z0)//S); e=min(nz-1,(z+d+R-z0)//S)
    solid[c:e+1,a:b+1]=True
img=np.zeros((nz,nx,3),np.uint8); img[solid]=(120,120,120)
cols=[(0,200,0),(0,120,255),(255,200,0),(255,0,255)]
for i,p in enumerate(pts):
    sx,sz=(p[0]-x0)//S,(p[1]-z0)//S
    if solid[sz,sx]: print('Punkt',p,'in Wand'); continue
    seen=np.zeros_like(solid); q=collections.deque([(sz,sx)]); seen[sz,sx]=True
    while q:
        z,x=q.popleft()
        for dz,dx in ((1,0),(-1,0),(0,1),(0,-1)):
            a,b=z+dz,x+dx
            if 0<=a<nz and 0<=b<nx and not seen[a,b] and not solid[a,b]: seen[a,b]=True; q.append((a,b))
    zz,xx=np.nonzero(seen)
    edge=(xx.min()==0 or zz.min()==0 or xx.max()==nx-1 or zz.max()==nz-1)
    print('Punkt',p,'Bereich x %d..%d z %d..%d n=%d %s'%(x0+xx.min()*S,x0+xx.max()*S,z0+zz.min()*S,z0+zz.max()*S,len(xx),'LECKT' if edge else ''))
    img[seen]=cols[i%4]
    img[max(0,sz-2):sz+3,max(0,sx-2):sx+3]=(255,0,0)
Image.fromarray(img[::-1]).resize((nx*2,nz*2),Image.NEAREST).save(out)
print('Bild',out,'x',x0,x1,'z',z0,z1,'(oben=+z)')
