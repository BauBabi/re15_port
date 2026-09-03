import struct, glob, os, json
DIR = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\STAGE1"
rows=[]
for p in sorted(glob.glob(os.path.join(DIR,"ROOM*.RDT"))):
    d=open(p,'rb').read()
    if len(d)<0x60: continue
    hdr=d[:8]; nCut=hdr[1]; hdr7=hdr[7]; nOm=hdr[2]
    rid=struct.unpack_from("<I",d,0x24)[0]
    nnull=0; nsec=0; tot=0
    for i in range(nCut):
        o=rid+i*32
        if o+32>len(d): break
        pri=struct.unpack_from("<I",d,o+0x1c)[0]
        if pri==0 or pri+4>len(d): nnull+=1; continue
        first=struct.unpack_from("<I",d,pri)[0]
        if first==0xFFFFFFFF: nnull+=1
        else:
            nsec+=1
            gc=first&0xffff; mc=first>>16
            s=0
            for g in range(gc):
                go=pri+4+g*8
                if go+8<=len(d): s+=struct.unpack_from("<H",d,go)[0]
            tot=max(tot,s)
    rows.append((os.path.basename(p),nCut,hdr7,nOm,nnull,nsec,tot))
print("%-14s %4s %5s %4s %6s %6s %8s"%("ROOM","nCut","hdr7","nOm","NULL","SECT","maxmask"))
for r in rows:
    mark = "   <<<" if r[5]>0 else ""
    print("%-14s %4d %5d %4d %6d %6d %8d%s"%(r[0],r[1],r[2],r[3],r[4],r[5],r[6],mark))
print()
print("STAGE1: %d RDTs, %d with at least one real pri section"%(len(rows), sum(1 for r in rows if r[5]>0)))
