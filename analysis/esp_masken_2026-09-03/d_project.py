import struct, zlib, os
RDT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1\ROOM1140.RDT"
OUT = r"C:\workspace\git\reAi_v2\analysis\esp_masken_2026-09-03\room1140"
d=open(RDT,'rb').read()
def s16(o): return struct.unpack_from("<h",d,o)[0]
MD1=0xE84; base=MD1+12
verts=[(s16(base+0x38+k*8),s16(base+0x38+k*8+2),s16(base+0x38+k*8+4)) for k in range(24)]
quads=[]
for i in range(15):
    o=base+0x170+i*16
    f=struct.unpack_from("<8H",d,o)
    quads.append((f[1],f[3],f[5],f[7]))
PROP=(-7124,0,-6976)

def isqrt_bios(n):   # plain isqrt is close enough for a locator overlay
    import math
    return int(math.isqrt(int(n)))

def build_view(px,py,pz,tx,ty,tz,fov):
    dx,dy,dz = tx-px, ty-py, tz-pz
    dist = isqrt_bios(dx*dx+dy*dy+dz*dz)
    horiz= isqrt_bios(dx*dx+dz*dz)
    sp = int((-dy*4096)//dist) if dist else 0
    cp = int((horiz*4096)//dist) if dist else 4096
    def s16c(v):
        v &= 0xffff
        return v-0x10000 if v>=0x8000 else v
    sp=s16c(sp); cp=s16c(cp)
    if horiz:
        sy=s16c(int((dx*4096)//horiz)); cy=s16c(int((dz*4096)//horiz))
        R=[cy,0,-sy,(sp*sy)>>12,cp,(sp*cy)>>12,(cp*sy)>>12,-sp,(cp*cy)>>12]
    else:
        R=[4096,0,0,0,cp,sp,0,-sp,cp]
    t=[ (R[0]*-px+R[1]*-py+R[2]*-pz)>>12,
        (R[3]*-px+R[4]*-py+R[5]*-pz)>>12,
        (R[6]*-px+R[7]*-py+R[8]*-pz)>>12 ]
    return R,t,(fov>>7)

def readpng(p):
    raw=open(p,'rb').read(); i=8; w=h=None; idat=b''
    while i<len(raw):
        ln=struct.unpack_from(">I",raw,i)[0]; typ=raw[i+4:i+8]; data=raw[i+8:i+8+ln]; i+=12+ln
        if typ==b'IHDR': w,h=struct.unpack_from(">II",data,0)
        elif typ==b'IDAT': idat+=data
    dec=zlib.decompress(idat); px=bytearray(w*h*3); stride=w*3; prev=bytearray(stride)
    pos=0
    for y in range(h):
        ft=dec[pos]; pos+=1; line=bytearray(dec[pos:pos+stride]); pos+=stride
        if ft==1:
            for x in range(3,stride): line[x]=(line[x]+line[x-3])&0xff
        elif ft==2:
            for x in range(stride): line[x]=(line[x]+prev[x])&0xff
        elif ft==3:
            for x in range(stride):
                a=line[x-3] if x>=3 else 0; line[x]=(line[x]+((a+prev[x])>>1))&0xff
        elif ft==4:
            for x in range(stride):
                a=line[x-3] if x>=3 else 0; b=prev[x]; c=prev[x-3] if x>=3 else 0
                p=a+b-c; pa=abs(p-a); pb=abs(p-b); pc=abs(p-c)
                pr=a if (pa<=pb and pa<=pc) else (b if pb<=pc else c)
                line[x]=(line[x]+pr)&0xff
        px[y*stride:(y+1)*stride]=line; prev=line
    return w,h,px

def writepng(p,w,h,px):
    raw=b''.join(b'\x00'+bytes(px[y*w*3:(y+1)*w*3]) for y in range(h))
    def ch(t,dd):
        c=t+dd; return struct.pack(">I",len(dd))+c+struct.pack(">I",zlib.crc32(c)&0xffffffff)
    open(p,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack(">IIBBBBB",w,h,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw,9))+ch(b'IEND',b''))

def line(px,w,h,x0,y0,x1,y1,col):
    dx=abs(x1-x0); dy=abs(y1-y0); sx=1 if x0<x1 else -1; sy=1 if y0<y1 else -1; err=dx-dy
    for _ in range(4000):
        if 0<=x0<w and 0<=y0<h:
            o=(y0*w+x0)*3; px[o]=col[0]; px[o+1]=col[1]; px[o+2]=col[2]
        if x0==x1 and y0==y1: break
        e2=2*err
        if e2>-dy: err-=dy; x0+=sx
        if e2<dx:  err+=dx; y0+=sy

for ci in range(10):
    flag,fov,px_,py_,pz_,tx,ty,tz,pri = struct.unpack_from("<HHiiiiiiI", d, 0x60+ci*32)
    R,t,H = build_view(px_,py_,pz_,tx,ty,tz,fov)
    pts=[]
    for (vx,vy,vz) in verts:
        wx,wy,wz = vx+PROP[0], vy+PROP[1], vz+PROP[2]
        cx=(R[0]*wx+R[1]*wy+R[2]*wz)>>12; cx+=t[0]
        cy=(R[3]*wx+R[4]*wy+R[5]*wz)>>12; cy+=t[1]
        cz=(R[6]*wx+R[7]*wy+R[8]*wz)>>12; cz+=t[2]
        if cz<=1: pts.append(None); continue
        pts.append((160+cx*H//cz, 120+cy*H//cz, cz))
    vis=[p for p in pts if p]
    onscreen=[p for p in vis if -400<p[0]<720 and -300<p[1]<540]
    print("cut%d: fov=%d H=%d  verts in front=%d  bbox=%s"%(ci,fov,H,len(vis),
        ("x[%d..%d] y[%d..%d] z[%d..%d]"%(min(p[0] for p in vis),max(p[0] for p in vis),
        min(p[1] for p in vis),max(p[1] for p in vis),min(p[2] for p in vis),max(p[2] for p in vis))) if vis else "-"))
    src=os.path.join(OUT,"bg%02d.png"%ci)
    w,h,img = readpng(src)
    for q in quads:
        for a,b in ((0,1),(1,2),(2,3),(3,0)):
            pa=pts[q[a]]; pb=pts[q[b]]
            if pa and pb: line(img,w,h,pa[0],pa[1],pb[0],pb[1],(255,0,0))
    writepng(os.path.join(OUT,"overlay_cut%d.png"%ci),w,h,img)
