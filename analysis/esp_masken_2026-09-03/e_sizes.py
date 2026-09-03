import struct, os, sys, glob, json
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRNAMES = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb",
            "sca","rid","rvd","lit","md1tim_ptr","flr","blk","msg",
            "main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]

def sect_len(b, off):
    """byte length of one pri section, byte-true per FUN_800392d4."""
    v = struct.unpack_from("<I", b, off)[0]
    if v == 0xFFFFFFFF: return 4, 0, 0
    ng = v & 0xFFFF; decl = v >> 16
    p = off+4
    counts=[]
    for i in range(ng):
        counts.append(struct.unpack_from("<H", b, p)[0]); p += 8
    mp = p
    total = sum(counts)
    for i in range(total):
        size_field = struct.unpack_from("<H", b, mp+6)[0]
        mp += 8
        if (size_field & 0xf000) == 0: mp += 4
    return mp-off, ng, total

rdts = sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT")))
bad=[]; ok=0
for p in rdts:
    room = os.path.basename(p)[:-4]
    b = open(p,'rb').read()
    if len(b)<0x60: continue
    nCut=b[1]; hdr7=b[7]
    dirs = {n: struct.unpack_from("<I", b, 8+i*4)[0] for i,n in enumerate(HDRNAMES)}
    pris = [struct.unpack_from("<I", b, 0x60+i*32+0x1c)[0] for i in range(nCut)]
    pmin=min(pris); pmax=max(pris)
    nxt=min([v for v in dirs.values() if v>pmax and v<len(b)] or [len(b)])
    tbl_start = nxt-4*(nCut+1)
    end=pmin; per=[]
    for i,o in enumerate(pris):
        L,ng,tot = sect_len(b,o)
        per.append((i,o,L,ng,tot))
        end = max(end, o+L)
    slack = tbl_start - end
    # also: is every section contiguous with the previous?
    contiguous = all(pris[i]+per[i][2] == pris[i+1] for i in range(nCut-1))
    if slack!=0 or not contiguous:
        bad.append((room,slack,contiguous,nCut,hdr7))
    else: ok+=1
print("contiguous+zero-slack:",ok,"/",len(rdts))
for x in bad[:40]: print("  ",x)
