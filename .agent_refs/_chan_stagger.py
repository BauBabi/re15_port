import struct
f=open('extracted/PSX/EMD/CDEMD0/em10.emd','rb').read()
begin=struct.unpack_from('<I',f,0)[0]
dirs=[struct.unpack_from('<I',f,begin+i*4)[0] for i in range(9)]
print("begin=%#x dirs=%s"%(begin,[hex(d) for d in dirs]))
STR=dirs[2]

def analyze(edd,emr,label):
    # EMR keyframe pool: kf_off @+2, kf_size @+6 (same layout as _zroot.py uses for emr=dir[4])
    kfo=struct.unpack_from('<H',f,emr+2)[0]
    ksize=struct.unpack_from('<H',f,emr+6)[0]
    pool=emr+kfo
    bc=struct.unpack_from('<H',f,emr+4)[0]  # bone count
    # EDD clip table: header count@+0 off@+2, then per-clip pairs (count,off) at edd+ci*4
    hdr_cnt=struct.unpack_from('<H',f,edd+0)[0]
    hdr_off=struct.unpack_from('<H',f,edd+2)[0]
    nclips=hdr_off//4   # number of clip-table entries before frame data
    print("\n=== %s  EDD=%#x EMR=%#x ksize=%d bonecount=%d kfpool=%#x  ~nclips=%d ==="%(label,edd,emr,ksize,bc,pool,nclips))
    def kf(idx):
        o=pool+idx*ksize
        return struct.unpack_from('<3h',f,o)  # root x,y,z
    def kfspeed(idx):
        o=pool+idx*ksize
        return struct.unpack_from('<3h',f,o+6)
    for ci in [0x00,0x01,0x02,0x09,0x16,0x27]:
        if ci>=nclips: 
            print("  clip 0x%02x : OUT OF RANGE (only %d clips this channel)"%(ci,nclips)); continue
        c=struct.unpack_from('<H',f,edd+ci*4)[0]
        o=struct.unpack_from('<H',f,edd+ci*4+2)[0]
        if c==0 or c>2000:
            print("  clip 0x%02x : nf=%d (skip/empty)"%(ci,c)); continue
        rxs=[];rys=[];rzs=[];sxs=[];szs=[]
        ok=True
        for fr in range(c):
            raw=struct.unpack_from('<I',f,edd+o+fr*4)[0]
            ki=raw&0xfff
            if pool+ki*ksize+12>len(f): ok=False;break
            rx,ry,rz=kf(ki); sx,sy,sz=kfspeed(ki)
            rxs.append(rx);rys.append(ry);rzs.append(rz);sxs.append(sx);szs.append(sz)
        if not ok or not rxs:
            print("  clip 0x%02x : nf=%d (bad kf idx)"%(ci,c)); continue
        # net forward = sum of per-frame speedX (root translation accumulation)
        netX=sum(sxs); netZ=sum(szs)
        print("  clip 0x%02x : nf=%2d  rootX[%5d..%5d] dX=%5d  spdXsum=%6d spdZsum=%6d  f0root=(%d,%d,%d) fLastroot=(%d,%d,%d)"%(
            ci,c,min(rxs),max(rxs),rxs[-1]-rxs[0],netX,netZ,rxs[0],rys[0],rzs[0],rxs[-1],rys[-1],rzs[-1]))

analyze(dirs[3],dirs[4],"Channel A (dir3 EDD / dir4 EMR-pool) = ACTIONS")
analyze(dirs[5],dirs[6],"Channel B (dir5 EDD / dir6 EMR-pool) = LOCOMOTION")
