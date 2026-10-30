import sys, math
from PIL import Image, ImageDraw
def render(path, size=360, yaw=0.6, pitch=0.25):
    V=[];F=[]
    for L in open(path):
        p=L.split()
        if not p: continue
        if p[0]=='v': V.append(tuple(map(float,p[1:4])))
        elif p[0]=='f': F.append([int(x)-1 for x in p[1:]])
    if not V: return Image.new('RGB',(size,size),(40,40,40))
    cy,sy=math.cos(yaw),math.sin(yaw); cp,sp=math.cos(pitch),math.sin(pitch)
    T=[]
    for x,y,z in V:
        x1=x*cy+z*sy; z1=-x*sy+z*cy
        y2=y*cp-z1*sp; z2=y*sp+z1*cp
        T.append((x1,y2,z2))
    xs=[t[0] for t in T]; ys=[t[1] for t in T]
    s=0.9*size/max(max(xs)-min(xs),max(ys)-min(ys),1)
    cx=(max(xs)+min(xs))/2; cyy=(max(ys)+min(ys))/2
    img=Image.new('RGB',(size,size),(40,40,48)); d=ImageDraw.Draw(img)
    polys=[]
    for f in F:
        if any(i<0 or i>=len(T) for i in f): continue
        P=[T[i] for i in f]
        ax,ay,az=[P[1][k]-P[0][k] for k in range(3)]; bx,by,bz=[P[2][k]-P[0][k] for k in range(3)]
        nx,ny,nz=ay*bz-az*by,az*bx-ax*bz,ax*by-ay*bx; l=math.sqrt(nx*nx+ny*ny+nz*nz) or 1
        sh=abs(nz/l)*0.7+0.3
        polys.append((sum(p[2] for p in P)/len(P),[( (p[0]-cx)*s+size/2, size/2-(p[1]-cyy)*s) for p in P],sh))
    for z,pts,sh in sorted(polys,key=lambda q:q[0]):
        c=int(200*sh); d.polygon(pts,fill=(c,int(c*0.9),int(c*0.75)))
    return img
if __name__=='__main__':
    out=sys.argv[1]; files=sys.argv[2:]
    tiles=[render(f) for f in files]
    W=Image.new('RGB',(360*len(tiles),360+20),(20,20,20)); d=ImageDraw.Draw(W)
    for i,(t,f) in enumerate(zip(tiles,files)):
        W.paste(t,(360*i,20)); d.text((360*i+5,4),f.split('/')[-1],fill=(255,255,255))
    W.save(out)
