"""Texturierter Software-Renderer fuer die OBJ+TIM-Ausgabe von probe_ems_bloecke (Ansicht, kein Verhalten)."""
import sys, math, struct
import numpy as np
from PIL import Image, ImageDraw
def load(obj, timf):
    V=[]; F=[]; cur=None
    for L in open(obj):
        p=L.split()
        if not p: continue
        if p[0]=='v': V.append(tuple(map(float,p[1:4])))
        elif p[0]=='f': cur=[int(x)-1 for x in p[1:]]
        elif p[0]=='t':
            n=len(cur); q=list(map(int,p[1:])); uv=[(q[2*i],q[2*i+1]) for i in range(n)]
            F.append((cur,uv,q[2*n],q[2*n+1]))
    d=open(timf,'rb').read(); bpp,w,h,cx,cy,ne=struct.unpack_from('<6i',d,0)
    clut=struct.unpack_from('<%dH'%ne,d,24); pix=d[24+2*ne:]
    return np.array(V),F,(bpp,w,h,cx,cy,clut,pix)
def texel(T,u,v,clutw,page):
    bpp,w,h,cx,cy,clut,pix=T
    row=((clutw>>6)&0x1ff)-cy; col=((clutw&0x3f)<<4)-cx
    x=u+(page&0xF)*128 if bpp==8 else u+(page&0xF)*256
    if bpp==8:
        if x>=w or v>=h: return (255,0,255)
        idx=pix[v*w+x]
    else:
        if x>=w or v>=h: return (255,0,255)
        b=pix[(v*w+x)//2]; idx=(b>>4) if x&1 else (b&15)
    ci=row*256+col+idx if bpp==8 else row*16+col+idx
    c=clut[ci] if 0<=ci<len(clut) else 0
    return ((c&31)*8,((c>>5)&31)*8,((c>>10)&31)*8)
def render(V,F,T,size,yaw,pitch):
    cy_,sy_=math.cos(yaw),math.sin(yaw); cp,sp=math.cos(pitch),math.sin(pitch)
    x1=V[:,0]*cy_+V[:,2]*sy_; z1=-V[:,0]*sy_+V[:,2]*cy_
    y2=V[:,1]*cp-z1*sp; z2=V[:,1]*sp+z1*cp
    s=0.85*size/max(x1.max()-x1.min(),y2.max()-y2.min())
    X=(x1-(x1.max()+x1.min())/2)*s+size/2; Y=size/2-(y2-(y2.max()+y2.min())/2)*s; Z=z2
    img=np.zeros((size,size,3),np.uint8); img[:]=(48,52,60); zb=np.full((size,size),-1e18)
    for idx,uv,cl,pg in F:
        tris=[(0,1,2)] if len(idx)==3 else [(0,1,2),(0,2,3)]
        for a,b,c in tris:
            ia,ib,ic=idx[a],idx[b],idx[c]
            if max(ia,ib,ic)>=len(V): continue
            xa,ya,xb,yb,xc,yc=X[ia],Y[ia],X[ib],Y[ib],X[ic],Y[ic]
            den=(yb-yc)*(xa-xc)+(xc-xb)*(ya-yc)
            if abs(den)<1e-9: continue
            # Licht: Normale in Weltraum
            P=V[[ia,ib,ic]]; n=np.cross(P[1]-P[0],P[2]-P[0]); n=n/(np.linalg.norm(n)+1e-9)
            sh=0.45+0.55*abs(n@np.array([0.3,0.8,0.5])/np.linalg.norm([0.3,0.8,0.5]))
            for py in range(max(0,int(min(ya,yb,yc))),min(size,int(max(ya,yb,yc))+1)):
                for px in range(max(0,int(min(xa,xb,xc))),min(size,int(max(xa,xb,xc))+1)):
                    l1=((yb-yc)*(px-xc)+(xc-xb)*(py-yc))/den; l2=((yc-ya)*(px-xc)+(xa-xc)*(py-yc))/den; l3=1-l1-l2
                    if l1<0 or l2<0 or l3<0: continue
                    z=l1*Z[ia]+l2*Z[ib]+l3*Z[ic]
                    if z<=zb[py,px]: continue
                    zb[py,px]=z
                    u=int(l1*uv[a][0]+l2*uv[b][0]+l3*uv[c][0]); v=int(l1*uv[a][1]+l2*uv[b][1]+l3*uv[c][1])
                    col=texel(T,u,v,cl,pg); img[py,px]=[min(255,int(k*sh)) for k in col]
    return Image.fromarray(img)
if __name__=='__main__':
    out,obj,timf=sys.argv[1:4]
    V,F,T=load(obj,timf)
    views=[(0.0,0.15,'Seite'),(0.9,0.3,'schraeg'),(math.pi/2,0.2,'vorn'),(0.0,1.45,'oben')]
    W=Image.new('RGB',(400*len(views),420),(20,20,20)); d=ImageDraw.Draw(W)
    for i,(yw,pt,name) in enumerate(views):
        W.paste(render(V,F,T,400,yw,pt),(400*i,20)); d.text((400*i+6,4),name,fill=(255,255,255))
    W.save(out)
