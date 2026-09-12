import struct
d=open("c:/workspace/git/reAi_v2/re15_port/shared_assets/RE2/EM23.EMD","rb").read()
diroff,cnt=struct.unpack_from("<II",d,0)
dirs=[struct.unpack_from("<I",d,diroff+4*i)[0] for i in range(cnt)]
order=sorted(range(cnt),key=lambda i:dirs[i]); ends={}
for k,i in enumerate(order): ends[i]=dirs[order[k+1]] if k+1<len(order) else diroff
def edd(base,size):
    dd=d[base:base+size]; c0,o0=struct.unpack_from("<HH",dd,0); n=o0//4
    cl=[(0,c0)]; cur=4
    for i in range(1,n):
        c,o=struct.unpack_from("<HH",dd,cur); cl.append(((o-o0)//4,c)); cur+=4
    return [[struct.unpack_from("<I",dd,o0+4*(f+j))[0] for j in range(c)] for f,c in cl]
def emr(base,size):
    dd=d[base:base+size]; bto,kfo,bc,kfs=struct.unpack_from("<HHHH",dd,0)
    return dd[kfo:],kfs
for label,ei,mi in (("P1",1,2),("P3",5,6)):
    E=edd(dirs[ei],ends[ei]-dirs[ei]); K,kfs=emr(dirs[mi],ends[mi]-dirs[mi])
    for ci,fw in enumerate(E):
        kfi=[w&0xFFF for w in fw]
        def kf(k): return struct.unpack_from("<hhhhhh",K,k*kfs)
        a=kf(kfi[0]); b=kf(kfi[-1])
        dys=[kf(k)[1] for k in kfi]; dxs=[kf(k)[3] for k in kfi]; dzs=[kf(k)[5] for k in kfi]
        print("%s clip %2d: %3dF  netDX=%6d netDZ=%6d  perF=%5.1f  dx[%d..%d] dz[%d..%d]  y first/last %d/%d min/max %d/%d"%(
            label,ci,len(fw),b[3]-a[3],b[5]-a[5],(b[3]-a[3])/max(1,len(fw)),min(dxs),max(dxs),min(dzs),max(dzs),
            a[1],b[1],min(dys),max(dys)))
