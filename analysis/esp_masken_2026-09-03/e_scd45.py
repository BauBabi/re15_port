import struct, os, re, sys
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRN = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb","sca","rid","rvd","lit",
        "md1tim_ptr","flr","blk","msg","main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
src=open(r"C:\workspace\git\reAi_v2\re15_port\engine\src\scd_vm.c",encoding="utf-8",errors="replace").read()
blk=src.split("static const uint8_t s_opcode_sizes[256] = {")[1].split("};")[0]
blk=re.sub(r"/\*.*?\*/","",blk,flags=re.S)
SZ=[0]*256
for m in re.finditer(r"\[(0x[0-9A-Fa-f]+)\]\s*=\s*(0x[0-9A-Fa-f]+|\d+)",blk):
    SZ[int(m.group(1),16)]=int(m.group(2),0)
assert SZ[0x45]==3

def walk(b, base, limit):
    """walk the SCD block at base; return histogram of opcodes."""
    hist={}
    if base==0 or base>=len(b): return hist
    first=struct.unpack_from("<H",b,base)[0]
    if first==0 or first%2: return hist
    n=first//2
    if n>256: return hist
    offs=[struct.unpack_from("<H",b,base+2*i)[0] for i in range(n)]
    for i,o in enumerate(offs):
        p=base+o
        end = base+offs[i+1] if i+1<n else limit
        while p<end and p<len(b):
            op=b[p]; s=SZ[op]
            if s==0 or s==0xFF: s=1
            hist[op]=hist.get(op,0)+1
            p+=s
    return hist

rooms=sys.argv[1:]
for room in rooms:
    p=os.path.join(ROOT,"STAGE%s"%room[4],room+".RDT")
    b=open(p,'rb').read(); nCut=b[1]
    dirs={n:struct.unpack_from("<I",b,8+i*4)[0] for i,n in enumerate(HDRN)}
    tot={}
    for key,nxtkeys in [("main_scd",["sub_scd"]),("sub_scd",["extra_scd","msg"]),("extra_scd",["msg","esp","rbj"])]:
        base=dirs[key]
        if not base: continue
        cands=[v for v in dirs.values() if v>base]
        lim=min(cands) if cands else len(b)
        h=walk(b,base,lim)
        for k,v in h.items(): tot[k]=tot.get(k,0)+v
    print("%-9s opcode 0x45 (sprite.pri mask toggle) count = %d   [hdr7=%d]"%(room,tot.get(0x45,0),b[7]))
