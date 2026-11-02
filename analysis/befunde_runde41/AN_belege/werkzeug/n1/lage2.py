import math, sys
sys.argv=["x","11"]
exec(open("proj5090.py").read().split("pts=")[0])
def spanne(cut, x, z, halb, h):
    xs=[]; ys=[]
    for dx,dz in ((halb,0),(-halb,0),(0,halb),(0,-halb)):
        for y in (0,-h):
            p=proj(cut,(x+dx,y,z+dz))
            if p is None: return None
            xs.append(p[0]); ys.append(p[1])
    return min(xs),max(xs),min(ys),max(ys)
best=[]
for lx in range(-3000,4001,250):
  for lz in range(-25000,-22399,200):
    for th in range(-60,61,5):
      t=math.radians(th); sx=lx+3760*math.cos(t); sz=lz+3760*math.sin(t)
      if not (-25000<sz<-22500): continue
      a15=spanne(15,sx,sz,250,1750); f15=spanne(15,lx,lz,1300,4000)
      a11=spanne(11,sx,sz,250,1750); f11=spanne(11,lx,lz,1300,4000)
      if None in (a15,f15,a11,f11): continue
      if not (5<a15[0] and a15[1]<315 and a15[2]>-10 and a15[3]<245): continue
      if not (5<a11[0] and a11[1]<315 and a11[3]<250): continue
      gap=max(a15[0]-f15[1], f15[0]-a15[1])       # >0 = keine Ueberdeckung
      if gap<0: continue
      if not (f11[1]>20 and f11[0]<300): continue
      best.append((-gap,lx,lz,th,int(sx),int(sz),[int(v) for v in a15],[int(v) for v in f15],[int(v) for v in a11],[int(v) for v in f11]))
best.sort()
print(len(best))
for b in best[:15]: print(b)
