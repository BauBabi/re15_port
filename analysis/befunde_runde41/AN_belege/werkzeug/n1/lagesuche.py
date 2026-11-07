import math, numpy as np, sys
sys.argv=[sys.argv[0],"11"]
exec(open(r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/an/proj5090.py").read().split("pts=")[0])
def figur(cut,x,z,h):
    f=proj(cut,(x,0,z)); k=proj(cut,(x,-h,z))
    if f is None or k is None: return None
    return f,k
def gut_an(cut,x,z):
    r=figur(cut,x,z,1750)
    if r is None: return False
    f,k=r
    return 10<f[0]<310 and 10<k[0]<310 and k[1]>5 and f[1]<238
def form_ok(cut,x,z):
    r=figur(cut,x,z,4000)
    if r is None: return None
    f,k=r
    if not (20<f[0]<300): return None
    return f[2], f[1]-k[1]
best=[]
for lx in range(-3000,6001,250):
  for lz in range(-24800,-22599,200):
    for th in range(-40,41,5):
      t=math.radians(th); sx=lx+3760*math.cos(t); sz=lz+3760*math.sin(t)
      if not (-25100<sz<-22500): continue
      if not (gut_an(11,sx,sz) and gut_an(15,sx,sz)): continue
      f11=form_ok(11,lx,lz); f15=form_ok(15,lx,lz)
      if f11 is None or f15 is None: continue
      best.append((f15[1],lx,lz,th,int(sx),int(sz),int(f11[1]),int(f15[0])))
best.sort()
print(len(best))
for b in best[:25]: print("formhoehe15 %d L=(%d,%d) th=%d S=(%d,%d) formhoehe11 %d tiefe15 %d"%b)
