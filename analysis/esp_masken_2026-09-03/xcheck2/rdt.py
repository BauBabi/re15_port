import os,struct,glob,collections
ROOT=r"C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"
files=sorted(glob.glob(ROOT+"/STAGE*/ROOM*.RDT"))
grpfield=collections.Counter(); ngrp=0; ncut_with=0; ncut_tot=0; nrdt=0
maskbits=collections.Counter(); sizelow=collections.Counter()
per=collections.Counter()
for f in files:
    d=open(f,'rb').read()
    if len(d)<0x28: continue
    nrdt+=1
    ncut=d[1]; hdr7=d[7]
    base=struct.unpack_from('<I',d,0x24)[0]
    if base==0 or base>=len(d): continue
    for c in range(ncut):
        o=base+c*32
        if o+32>len(d): break
        ncut_tot+=1
        po=struct.unpack_from('<I',d,o+0x1c)[0]
        if po+4>len(d) or po==0: continue
        w=struct.unpack_from('<I',d,po)[0]
        if w==0xFFFFFFFF: continue
        ncut_with+=1
        gc=w&0xffff; dc=w>>16
        gp=po+4
        mp=gp+gc*8
        ok=True
        for g in range(gc):
            if gp+g*8+8>len(d): ok=False;break
            mc,f2,dx,dy=struct.unpack_from('<HHhh',d,gp+g*8)
            ngrp+=1
            grpfield[f2]+=1
            for m in range(mc):
                if mp+8>len(d): ok=False;break
                sz=struct.unpack_from('<H',d,mp+6)[0]
                sizelow[sz&0x0fff]+=1
                if (sz&0xf000)==0: mp+=12
                else: mp+=8
            if not ok: break
print("rdts",nrdt,"cuts",ncut_tot,"cuts_with_masks",ncut_with,"groups",ngrp)
print("group[+2] top:",grpfield.most_common(10))
print("size low12 top:",sizelow.most_common(8))
