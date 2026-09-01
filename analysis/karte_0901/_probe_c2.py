# -*- coding: utf-8 -*-
"""Erreichbare Flaeche eines Raums: Flutfuellung im band-freien Komplement (PR=450)."""
import struct, os, sys, collections
ROOT=r'C:\workspace\git\reAi_v2'
NAMES = ["collision","camera","zone","light","md1ptr","floor","block","message",
         "mainScd","subScd","extraScd","effect","x50","espTim","modelTim","anim"]
PR=450
def sca_cells(rid, band=0):
    stage=rid>>12
    d=open(os.path.join(ROOT,'re15_port','shared_assets','PSX','STAGE%d'%stage,'ROOM%04X.RDT'%rid),'rb').read()
    offs={nm:struct.unpack_from('<I',d,0x20+4*i)[0] for i,nm in enumerate(NAMES)}
    s=offs['collision']; cnt=struct.unpack_from('<5I',d,s+4)
    out=set()
    for i in range(cnt[0]):
        w,dep,x,z,t,u0,u1,fl=struct.unpack_from('<HHhhBBBB',d,s+24+12*i)
        if (fl>>4)==band and (u0&1): out.add((x,z,w,dep,t))
    return sorted(out)

def reach(rid, band, seeds, step=100, pad=2000, radius=PR):
    C=sca_cells(rid,band)
    xs0=min(c[0] for c in C)-pad; xs1=max(c[0]+c[2] for c in C)+pad
    zs0=min(c[1] for c in C)-pad; zs1=max(c[1]+c[3] for c in C)+pad
    W=(xs1-xs0)//step+1; H=(zs1-zs0)//step+1
    blocked=[[False]*W for _ in range(H)]
    for (x,z,w,dd,t) in C:
        i0=max(0,(x-radius-xs0)//step); i1=min(W-1,(x+w+radius-xs0)//step)
        j0=max(0,(z-radius-zs0)//step); j1=min(H-1,(z+dd+radius-zs0)//step)
        for j in range(j0,j1+1):
            for i in range(i0,i1+1): blocked[j][i]=True
    seen=[[False]*W for _ in range(H)]
    st=[]
    for (sx,sz) in seeds:
        i=(sx-xs0)//step; j=(sz-zs0)//step
        if 0<=i<W and 0<=j<H and not blocked[j][i] and not seen[j][i]:
            seen[j][i]=True; st.append((i,j))
    while st:
        i,j=st.pop()
        for di,dj in ((1,0),(-1,0),(0,1),(0,-1)):
            a,b=i+di,j+dj
            if 0<=a<W and 0<=b<H and not blocked[b][a] and not seen[b][a]:
                seen[b][a]=True; st.append((a,b))
    pts=[(xs0+i*step+step//2, zs0+j*step+step//2) for j in range(H) for i in range(W) if seen[j][i]]
    return pts, (xs0,xs1,zs0,zs1), seen, (W,H,step)

def ascii_of(pts, W,H, marks=()):
    if not pts: return
    x0=min(p[0] for p in pts); x1=max(p[0] for p in pts)
    z0=min(p[1] for p in pts); z1=max(p[1] for p in pts)
    S=set()
    for (x,z) in pts:
        i=int((x-x0)*(W-1)/max(1,x1-x0)); j=int((z1-z)*(H-1)/max(1,z1-z0))
        S.add((j,i))
    M={}
    for (nm,mx,mz) in marks:
        i=int((mx-x0)*(W-1)/max(1,x1-x0)); j=int((z1-mz)*(H-1)/max(1,z1-z0))
        M[(max(0,min(H-1,j)),max(0,min(W-1,i)))]=nm
    print('   erreichbare Bbox: x %d..%d (%d)  z %d..%d (%d)  AR=%.3f  Zellen=%d'%(
        x0,x1,x1-x0,z0,z1,z1-z0,(x1-x0)/max(1,z1-z0),len(pts)))
    for j in range(H):
        print('   |'+''.join(M.get((j,i), '#' if (j,i) in S else '.') for i in range(W))+'|')
