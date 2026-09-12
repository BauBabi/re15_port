import struct
REPO="c:/workspace/git/reAi_v2"
emd=open(REPO+"/re15_port/shared_assets/RE2/EM23.EMD","rb").read()
diroff,dircnt=struct.unpack_from("<II",emd,0)
dirs=[struct.unpack_from("<I",emd,diroff+4*i)[0] for i in range(dircnt)]
order=sorted(range(dircnt),key=lambda i:dirs[i]); ends={}
for k,i in enumerate(order): ends[i]=dirs[order[k+1]] if k+1<len(order) else diroff
for name,mi in (("EMR1",2),("EMR3",6)):
    base=dirs[mi]; size=ends[mi]-dirs[mi]; d=emd[base:base+size]
    bto,kfo,bc,kfs=struct.unpack_from("<HHHH",d,0)
    print("%s @0x%X size=0x%X: hdr bto=0x%X kfo=0x%X bones=%d kfsize=%d"%(name,base,size,bto,kfo,bc,kfs))
    relpos=[struct.unpack_from("<hhh",d,8+6*b) for b in range(bc)]
    print("  relpos:",relpos)
    # armature/child table at bto: bc*(u16 count,u16 off), offsets rel to bto
    links=[]
    for b in range(bc):
        c,o=struct.unpack_from("<HH",d,bto+4*b)
        kids=list(d[bto+o:bto+o+c])
        links.append((b,c,kids))
    print("  tree:",links)
    par=[-1]*bc
    for b,c,kids in links:
        for k in kids:
            if k<bc and par[k]==-1 and k!=b: par[k]=b
    print("  parents:",par)
