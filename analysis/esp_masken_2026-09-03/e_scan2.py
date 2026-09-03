import struct, os, glob
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRN = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb","sca","rid","rvd","lit",
        "md1tim_ptr","flr","blk","msg","main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
BASES={0x7800,0x7c00,0x7c10,0x8080,0x8088}

def try_section(b,off,maxdepth=1024):
    """Return (length, ngroups, nmasks) if a valid sprite.pri section starts at off, else None."""
    if off+4>len(b): return None
    ng,decl=struct.unpack_from("<HH",b,off)
    if not (1<=ng<=8): return None
    if not (1<=decl<=105): return None
    p=off+4; counts=[]
    if p+ng*8>len(b): return None
    for i in range(ng):
        c,base,dx,dy=struct.unpack_from("<HHhh",b,p)
        if base not in BASES: return None
        if not (1<=c<=105): return None
        if not (-512<=dx<=512 and -512<=dy<=512): return None
        counts.append(c); p+=8
    tot=sum(counts)
    if not (decl-4<=tot<=decl+4): return None
    mp=p
    for i in range(tot):
        if mp+8>len(b): return None
        depth=struct.unpack_from("<H",b,mp+4)[0]
        sf=struct.unpack_from("<H",b,mp+6)[0]
        if depth>=maxdepth: return None
        mp+=8
        if (sf&0xf000)==0:
            if mp+4>len(b): return None
            w,h=struct.unpack_from("<HH",b,mp)
            if w==0 or h==0 or w>256 or h>256: return None
            mp+=4
    return (mp-off,ng,tot)

rooms=["ROOM1220","ROOM1221","ROOM3040","ROOM3041","ROOM3050","ROOM3051","ROOM3090","ROOM3091",
       "ROOM30A0","ROOM30A1","ROOM30E0","ROOM30E1","ROOM4040","ROOM4041","ROOM4080","ROOM4081",
       "ROOM40A0","ROOM40A1","ROOM1150","ROOM1170","ROOM1210"]
for room in rooms:
    p=os.path.join(ROOT,"STAGE%s"%room[4],room+".RDT")
    b=open(p,'rb').read(); nCut=b[1]
    dirs={n:struct.unpack_from("<I",b,8+i*4)[0] for i,n in enumerate(HDRN)}
    pris=[struct.unpack_from("<I",b,0x60+i*32+0x1c)[0] for i in range(nCut)]
    pmin,pmax=min(pris),max(pris)
    nxt=min([v for v in dirs.values() if v>pmax and v<len(b)] or [len(b)])
    hits_in=[]; hits_out=[]
    for off in range(0,len(b)-4,2):
        r=try_section(b,off)
        if r:
            (hits_in if pmin<=off<nxt else hits_out).append((off,)+r)
    print("%-9s pri=[0x%X,0x%X)  valid-sections inside=%d outside=%d %s"
          %(room,pmin,nxt,len(hits_in),len(hits_out),
            "  OUTSIDE: "+", ".join("0x%X(g%d,m%d,%dB)"%(o,g,m,L) for o,L,g,m in hits_out[:8]) if hits_out else ""))
