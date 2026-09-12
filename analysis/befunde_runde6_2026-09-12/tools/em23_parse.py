#!/usr/bin/env python3
import struct, hashlib, sys
REPO="c:/workspace/git/reAi_v2"
EXE=open(REPO+"/info/re2leon/PSX.EXE","rb").read()
taddr=struct.unpack_from("<I",EXE,0x18)[0]
def exe_off(a): return 0x800+(a-taddr)
TOC=0x8009adf4
def toc(kind,rec):
    i=exe_off(TOC)+((kind-0x10)*4+rec)*8
    sec,size=struct.unpack_from("<II",EXE,i)
    return sec*0x800,size
EMS=open(REPO+"/re15_port/shared_assets/RE2/CDEMD0.EMS","rb").read()
off,size=toc(0x23,3)
emd=EMS[off:off+size]
print("EM23 EMD from CDEMD0.EMS: off=0x%X size=0x%X md5=%s"%(off,size,hashlib.md5(emd).hexdigest()[:12]))
ref=open(REPO+"/re15_port/shared_assets/RE2/EM23.EMD","rb").read()
print("shared_assets/RE2/EM23.EMD md5=%s  identical=%s"%(hashlib.md5(ref).hexdigest()[:12], emd==ref))
o23,s23=toc(0x23,1); print("EM23 overlay rec1: EMS off=0x%X size=%d sector=%d"%(o23,s23,o23//0x800))
ovl_ref=open(REPO+"/info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN","rb").read()
print("overlay identical to info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN:",EMS[o23:o23+s23]==ovl_ref)

diroff,dircnt=struct.unpack_from("<II",emd,0)
print("dir @0x%X count=%d"%(diroff,dircnt))
dirs=[struct.unpack_from("<I",emd,diroff+4*i)[0] for i in range(dircnt)]
print("dir entries:",["0x%X"%d for d,d in enumerate(dirs)])
# section sizes: next dir entry (they are ascending?) 
order=sorted(range(dircnt),key=lambda i:dirs[i])
ends={}
for k,i in enumerate(order):
    end = dirs[order[k+1]] if k+1<len(order) else diroff
    ends[i]=end
for i in range(dircnt):
    print("  dir[%d] @0x%06X  size=0x%X"%(i,dirs[i],ends[i]-dirs[i]))

def parse_edd(base,size):
    d=emd[base:base+size]
    clips=[]
    cnt0=struct.unpack_from("<H",d,0)[0]; off0=struct.unpack_from("<H",d,2)[0]
    nclips=off0//4
    clips.append((0,cnt0))
    cur=4
    for i in range(1,nclips):
        c=struct.unpack_from("<H",d,cur)[0]; o=struct.unpack_from("<H",d,cur+2)[0]
        clips.append(((o-off0)//4,c))
        cur+=4
    frames_off=off0
    out=[]
    for (first,cnt) in clips:
        fw=[struct.unpack_from("<I",d,frames_off+4*(first+j))[0] for j in range(cnt)]
        out.append(fw)
    return out

def parse_emr(base,size):
    d=emd[base:base+size]
    bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,0)
    relpos=[struct.unpack_from("<hhh",d,8+6*b) for b in range(bc)]
    kfdata=d[kfo:]
    nkf=len(kfdata)//kfs
    return dict(bones=bc,kfsize=kfs,kfo=kfo,nkf=nkf,kfdata=kfdata,relpos=relpos,bto=bto)

def dump_pair(name,ei,mi):
    esz=ends[ei]-dirs[ei]; msz=ends[mi]-dirs[mi]
    edd=parse_edd(dirs[ei],esz); emr=parse_emr(dirs[mi],msz)
    print("\n=== %s: EDD dir[%d]@0x%X (%dB, %d clips)  EMR dir[%d]@0x%X (%dB, %d bones, kf %dB, %d kfs @+0x%X)"%(
        name,ei,dirs[ei],esz,len(edd),mi,dirs[mi],msz,emr['bones'],emr['kfsize'],emr['nkf'],emr['kfo']))
    for ci,fw in enumerate(edd):
        kfi=[w&0xFFF for w in fw]
        flags=[(j,w) for j,w in enumerate(fw) if w>>12]
        # root translation per frame
        ys=[];xs=[];zs=[];spd=[]
        for k in kfi:
            b=k*emr['kfsize']
            if b+12<=len(emr['kfdata']):
                x,y,z,sx,sy,sz=struct.unpack_from("<hhhhhh",emr['kfdata'],b)
                xs.append(x);ys.append(y);zs.append(z);spd.append((sx,sy,sz))
        def rng(v): return (min(v),max(v)) if v else (0,0)
        print(" clip %2d: %3dF  kf %d..%d  rootY[%d..%d] first=%s last=%s  rootX[%d..%d] rootZ[%d..%d]"%(
            ci,len(fw),kfi[0] if kfi else -1,kfi[-1] if kfi else -1,
            rng(ys)[0],rng(ys)[1],ys[0] if ys else 0,ys[-1] if ys else 0,
            rng(xs)[0],rng(xs)[1],rng(zs)[0],rng(zs)[1]))
        if spd:
            sxs=[s[0] for s in spd];szs=[s[2] for s in spd];sys_=[s[1] for s in spd]
            print("          speed sx[%d..%d] sy[%d..%d] sz[%d..%d]  sum_sz=%d sum_sx=%d"%(
                min(sxs),max(sxs),min(sys_),max(sys_),min(szs),max(szs),sum(szs),sum(sxs)))
        if flags:
            print("          flags:",", ".join("f%d=0x%08X"%(j,w) for j,w in flags))
    return edd,emr

p1=dump_pair("PAAR1 (Gator-Rig)",1,2)
p2=dump_pair("PAAR2",3,4)
p3=dump_pair("PAAR3 (Opfer-Rig)",5,6)
# dir[0] inspect
print("\ndir[0] @0x%X size=0x%X first bytes:"%(dirs[0],ends[0]-dirs[0]), emd[dirs[0]:dirs[0]+32].hex())
