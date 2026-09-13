#!/usr/bin/env python3
"""EM23 Paar-1: Netto-Root-Versatz je Clip (kf-Bytes 6..11 = kumulativer Lauf-Versatz)."""
import struct
REPO="c:/workspace/git/reAi_v2"
EXE=open(REPO+"/info/re2leon/PSX.EXE","rb").read()
taddr=struct.unpack_from("<I",EXE,0x18)[0]
def eo(a): return 0x800+(a-taddr)
def toc(kind,rec):
    i=eo(0x8009adf4)+((kind-0x10)*4+rec)*8
    s,z=struct.unpack_from("<II",EXE,i); return s*0x800,z
EMS=open(REPO+"/re15_port/shared_assets/RE2/CDEMD0.EMS","rb").read()
o,s=toc(0x23,3); emd=EMS[o:o+s]
diroff,dircnt=struct.unpack_from("<II",emd,0)
dirs=[struct.unpack_from("<I",emd,diroff+4*i)[0] for i in range(dircnt)]
order=sorted(range(dircnt),key=lambda i:dirs[i]); ends={}
for k,i in enumerate(order): ends[i]=dirs[order[k+1]] if k+1<len(order) else diroff
def parse_edd(b,sz):
    d=emd[b:b+sz]; c0,o0=struct.unpack_from("<HH",d,0); n=o0//4
    cl=[(0,c0)]; cur=4
    for i in range(1,n):
        c,oo=struct.unpack_from("<HH",d,cur); cl.append(((oo-o0)//4,c)); cur+=4
    return [[struct.unpack_from("<I",d,o0+4*(f+j))[0] for j in range(c)] for (f,c) in cl]
def parse_emr(b,sz):
    d=emd[b:b+sz]; bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,0)
    return dict(bones=bc,kfs=kfs,kfo=kfo,d=d)
edd=parse_edd(dirs[1],ends[1]-dirs[1]); emr=parse_emr(dirs[2],ends[2]-dirs[2])
print("EMR1: bones=%d kfsize=%d kfo=0x%X"%(emr['bones'],emr['kfs'],emr['kfo']))
for ci,fw in enumerate(edd):
    kf=[w&0xFFF for w in fw]
    pts=[]
    for k in kf:
        b=emr['kfo']+k*emr['kfs']
        x,y,z,vx,vy,vz=struct.unpack_from("<hhhhhh",emr['d'],b)
        pts.append((x,y,z,vx,vy,vz))
    if not pts: continue
    dx=pts[-1][3]-pts[0][3]; dy=pts[-1][4]-pts[0][4]; dz=pts[-1][5]-pts[0][5]
    ys=[p[1] for p in pts]
    print("  clip %2d: %3d F  netto dx=%+6d dy=%+6d dz=%+6d  (%.1f/F)  poseY %d..%d (erst %d -> letzt %d)"%(
        ci,len(fw),dx,dy,dz,dx/max(1,len(fw)),min(ys),max(ys),ys[0],ys[-1]))
