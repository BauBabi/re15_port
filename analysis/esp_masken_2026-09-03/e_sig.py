import struct, os, glob
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRN = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb","sca","rid","rvd","lit",
        "md1tim_ptr","flr","blk","msg","main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
def sect_len(b,off):
    v=struct.unpack_from("<I",b,off)[0]
    if v==0xFFFFFFFF: return 4,[],0
    ng=v&0xFFFF
    p=off+4; groups=[]
    for i in range(ng):
        c,base,dx,dy=struct.unpack_from("<HHhh",b,p); groups.append((c,base,dx,dy)); p+=8
    mp=p; tot=sum(g[0] for g in groups)
    for i in range(tot):
        sf=struct.unpack_from("<H",b,mp+6)[0]; mp+=8
        if (sf&0xf000)==0: mp+=4
    return mp-off,groups,tot

allbases=set(); nsect=0; ngrp=0
priblocks={}
for p in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    b=open(p,'rb').read()
    if len(b)<0x60: continue
    room=os.path.basename(p)[:-4]; nCut=b[1]
    dirs={n:struct.unpack_from("<I",b,8+i*4)[0] for i,n in enumerate(HDRN)}
    pris=[struct.unpack_from("<I",b,0x60+i*32+0x1c)[0] for i in range(nCut)]
    pmin,pmax=min(pris),max(pris)
    nxt=min([v for v in dirs.values() if v>pmax and v<len(b)] or [len(b)])
    priblocks[room]=(pmin,nxt,b)
    for o in pris:
        L,g,t=sect_len(b,o)
        if g: nsect+=1
        for c,base,dx,dy in g:
            allbases.add(base); ngrp+=1
print("real pri sections:",nsect,"groups:",ngrp,"distinct group[+2] values:",sorted(hex(x) for x in allbases))

# scan whole RDT for 0x7800 halfwords OUTSIDE the pri block
print()
print("0x7800 halfword occurrences OUTSIDE the sprite.pri block:")
for room in ["ROOM1220","ROOM3040","ROOM3050","ROOM3090","ROOM30A0","ROOM30E0","ROOM4040","ROOM4080","ROOM40A0",
             "ROOM1150","ROOM1170","ROOM1210"]:
    pmin,nxt,b=priblocks[room]
    hits=[]
    for i in range(0,len(b)-1,2):
        if b[i]==0x00 and b[i+1]==0x78:
            if not (pmin<=i<nxt): hits.append(i)
    inside=[i for i in range(0,len(b)-1,2) if b[i]==0x00 and b[i+1]==0x78 and pmin<=i<nxt]
    print("  %-9s pri=[0x%X,0x%X)  inside=%d  outside=%d  first outside: %s"
          % (room,pmin,nxt,len(inside),len(hits),[hex(x) for x in hits[:6]]))
