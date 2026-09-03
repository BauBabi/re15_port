import struct, zlib, math
RDT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
OUT = r"C:\workspace\git\reAi_v2\analysis\esp_masken_2026-09-03\room1140\plan_room1140.png"
d=open(RDT,'rb').read()
S=0x570
sca=[]
for i in range(19):
    o=S+24+i*12
    w,dn,x,z,ty,u0,u1,fl = struct.unpack_from("<HHhhBBBB", d, o); sca.append((x,z,w,dn,ty,i))
cuts=[struct.unpack_from("<HHiiiiiiI", d, 0x60+i*32) for i in range(10)]
# RVD zones
zs=[]
zst=struct.unpack_from("<I",d,0x28)[0]
o=zst
while o+20<=len(d):
    raw=struct.unpack_from("<I",d,o)[0]
    if raw==0xFFFFFFFF: break
    b0,b1,b2,b3 = d[o],d[o+1],d[o+2],d[o+3]
    v=struct.unpack_from("<8h",d,o+4)
    zs.append((b0,b1,b2,b3,v)); o+=20
    if len(zs)>40: break
X0,X1,Z0,Z1 = -13000, 19000, -28000, 4000
W,H = 800, 800
def sx(x): return int((x-X0)*(W-1)/(X1-X0))
def sy(z): return int((z-Z0)*(H-1)/(Z1-Z0))
img=bytearray(b'\x10'*(W*H*3))
def line(x0,y0,x1,y1,c):
    dx=abs(x1-x0); dy=abs(y1-y0); s1=1 if x0<x1 else -1; s2=1 if y0<y1 else -1; err=dx-dy
    for _ in range(6000):
        if 0<=x0<W and 0<=y0<H:
            o=(y0*W+x0)*3; img[o]=c[0]; img[o+1]=c[1]; img[o+2]=c[2]
        if x0==x1 and y0==y1: break
        e2=2*err
        if e2>-dy: err-=dy; x0+=s1
        if e2<dx: err+=dx; y0+=s2
def rect(x,z,w,dd,c):
    a,b_,c2,d2 = sx(x),sy(z),sx(x+w),sy(z+dd)
    line(a,b_,c2,b_,c); line(c2,b_,c2,d2,c); line(c2,d2,a,d2,c); line(a,d2,a,b_,c)
def dot(x,z,c,r=3):
    for dy in range(-r,r+1):
        for dx in range(-r,r+1):
            px,py=sx(x)+dx, sy(z)+dy
            if 0<=px<W and 0<=py<H:
                o=(py*W+px)*3; img[o]=c[0]; img[o+1]=c[1]; img[o+2]=c[2]
for (x,z,w,dd,ty,i) in sca:
    rect(x,z,w,dd,(0,200,200) if ty==1 else (200,200,0))
# prop bbox
rect(-7124-1494, -6976-112, 2987, 223, (255,0,0))
# cameras + view ray
for i,(flag,fov,px,py,pz,tx,ty,tz,pri) in enumerate(cuts):
    dot(px,pz,(255,255,0),4)
    line(sx(px),sy(pz),sx(px+(tx-px)//3),sy(pz+(tz-pz)//3),(120,120,60))
# RVD zones
for (b0,b1,b2,b3,v) in zs:
    pts=[(v[0],v[1]),(v[2],v[3]),(v[4],v[5]),(v[6],v[7])]
    for k in range(4):
        a=pts[k]; b=pts[(k+1)%4]
        line(sx(a[0]),sy(a[1]),sx(b[0]),sy(b[1]),(255,0,255))
raw=b''.join(b'\x00'+bytes(img[y*W*3:(y+1)*W*3]) for y in range(H))
def ch(t,dd):
    c=t+dd; return struct.pack(">I",len(dd))+c+struct.pack(">I",zlib.crc32(c)&0xffffffff)
open(OUT,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack(">IIBBBBB",W,H,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw,9))+ch(b'IEND',b''))
print("wrote",OUT)
print("RVD zones (%d):"%len(zs))
for (b0,b1,b2,b3,v) in zs:
    print("   from=%d to=%d b2=%d b3=%d quad=%s"%(b0,b1,b2,b3,v))
