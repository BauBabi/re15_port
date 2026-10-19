"""Python-Nachbau von re15_schwenke_entfernen (platform/pc/src/inv_render_pc.c) fuer den Kompositor."""
import struct, numpy as np
EXE=r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/info/Re1.5/PSX.EXE'
RECTFIX3=[(128,16,102,116,40,40),(168,16,118,134,24,24),(192,16,135,155,56,32),(0,32,135,76,72,88),(168,40,109,134,16,16),
          (184,48,146,114,72,32),(72,64,186,114,40,48),(112,74,179,67,48,48),(160,80,102,76,48,48),(208,80,156,76,48,40)]
def rects(seite):
    if seite==3: return [(x,y,w,h,u,v) for (u,v,x,y,w,h) in RECTFIX3]
    exe=open(EXE,'rb').read(); t=struct.unpack_from('<I',exe,0x18)[0]; fo=lambda a:a-t+0x800
    cnt=struct.unpack_from('<H',exe,fo(0x80076840+8*seite))[0]; ptr=struct.unpack_from('<I',exe,fo(0x80076844+8*seite))[0]
    out=[]
    for i in range(cnt):
        x,y,w,h=struct.unpack_from('<hhhh',exe,fo(ptr+12*i)); u,_,v,_=struct.unpack_from('<BBBB',exe,fo(ptr+12*i+8))
        out.append((x,y,w,h,u,v))
    return out
def entfernen(m, seite):
    m=m.copy().astype(np.int16)
    WAND,S_SAAT,S_WACHS,MAX_AUSD=4,4,10,10
    lauf=np.zeros((256,256),np.int16); ben=np.zeros((256,256),bool); benr=np.zeros((256,256),np.int16)
    R=rects(seite)
    for ri,(rx,ry,rw,rh,ru,rv) in enumerate(R):
        for iy in range(rv,min(rv+rh,256)):
            for ix in range(ru,min(ru+rw,256)):
                ben[iy,ix]=True
                if not benr[iy,ix] and ri<254: benr[iy,ix]=ri+1
    for y in range(256):
        for x in range(256):
            if not ben[y,x] or m[y,x]!=WAND: continue
            u0,u1,v0,v1=0,255,0,255
            if benr[y,x]:
                rx,ry,rw,rh,ru,rv=R[benr[y,x]-1]
                u0,u1,v0,v1=max(ru,0),min(ru+rw-1,255),max(rv,0),min(rv+rh-1,255)
            lx=ly=0
            k=x
            while k<=u1 and m[y,k]==WAND: lx+=1; k+=1
            k=x-1
            while k>=u0 and m[y,k]==WAND: lx+=1; k-=1
            k=y
            while k<=v1 and m[k,x]==WAND: ly+=1; k+=1
            k=y-1
            while k>=v0 and m[k,x]==WAND: ly+=1; k-=1
            lauf[y,x]=min(max(lx,ly),255)
    def pass_(wachs, n_max):
        weg=np.zeros((256,256),np.int8)
        weg[(ben)&(m==WAND)&(lauf>0)&(lauf<=S_SAAT)]=1
        for _ in range(24):
            ge=False
            for y in range(1,255):
                for x in range(1,255):
                    if weg[y,x] or not ben[y,x] or m[y,x]!=WAND: continue
                    if lauf[y,x]>wachs: continue
                    if weg[y-1,x] or weg[y+1,x] or weg[y,x-1] or weg[y,x+1]: weg[y,x]=1; ge=True
            if not ge: break
        for y in range(256):
            for x in range(256):
                if weg[y,x]!=1: continue
                st=[(x,y)]; weg[y,x]=2; t=0; x0=x1=x; y0=y1=y
                while t<len(st):
                    cx,cy=st[t]; t+=1
                    x0=min(x0,cx); x1=max(x1,cx); y0=min(y0,cy); y1=max(y1,cy)
                    for dy in (-1,0,1):
                        for dx in (-1,0,1):
                            nx,ny=cx+dx,cy+dy
                            if 0<=nx<=255 and 0<=ny<=255 and weg[ny,nx]==1: weg[ny,nx]=2; st.append((nx,ny))
                if x1-x0+1<3 or y1-y0+1<3: continue
                if x1-x0+1>MAX_AUSD or y1-y0+1>MAX_AUSD or (n_max and len(st)>n_max):
                    for cx,cy in st: weg[cy,cx]=0
                else:
                    for cx,cy in st: weg[cy,cx]=3
        for y in range(256):
            for x in range(256):
                if weg[y,x]!=3: continue
                z=[0]*16
                for dy in range(-2,3):
                    for dx in range(-2,3):
                        nx,ny=x+dx,y+dy
                        if not(0<=nx<=255 and 0<=ny<=255): continue
                        if weg[ny,nx]==3 or m[ny,nx]==WAND: continue
                        z[m[ny,nx]&15]+=1
                bn=0;best=-1
                for b in range(16):
                    if z[b]>bn: bn=z[b]; best=b
                if best>=0: m[y,x]=best
    pass_(S_WACHS,0)
    pass_(6,13)
    b2=np.zeros((256,256),np.int8); b2[ben&(m==2)]=1
    for y in range(256):
        for x in range(256):
            if b2[y,x]!=1: continue
            st=[(x,y)]; b2[y,x]=2; t=0; wandnah=0
            while t<len(st):
                cx,cy=st[t]; t+=1; hat=False
                for dy in (-1,0,1):
                    for dx in (-1,0,1):
                        nx,ny=cx+dx,cy+dy
                        if not(0<=nx<=255 and 0<=ny<=255): continue
                        if m[ny,nx]==WAND: hat=True
                        if b2[ny,nx]==1: b2[ny,nx]=2; st.append((nx,ny))
                if hat: wandnah+=1
            if len(st)<=64 and wandnah*100>=len(st)*60:
                for px2,py2 in st:
                    z=[0]*16
                    for dy in range(-2,3):
                        for dx in range(-2,3):
                            nx,ny=px2+dx,py2+dy
                            if not(0<=nx<=255 and 0<=ny<=255): continue
                            if m[ny,nx]==WAND or m[ny,nx]==2: continue
                            z[m[ny,nx]&15]+=1
                    bn=0;best=-1
                    for b in range(16):
                        if z[b]>bn: bn=z[b]; best=b
                    if best>=0: m[py2,px2]=best
    return m
