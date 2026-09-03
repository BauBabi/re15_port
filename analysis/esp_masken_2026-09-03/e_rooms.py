import struct, os, glob, json, sys
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRNAMES = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb",
            "sca","rid","rvd","lit","md1tim_ptr","flr","blk","msg",
            "main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
ANOM = {
 "ROOM1220":[0,1,2,3,4,5,6,8], "ROOM1221":[0,1,2,3,4,5,6,8],
 "ROOM3040":list(range(14)),  "ROOM3041":list(range(14)),
 "ROOM3050":[0,1,2,3,4,5,6,7,8,9,11,12,13,14,15], "ROOM3051":[0,1,2,3,4,5,6,7,8,9,11,12,13,14,15],
 "ROOM3090":[15], "ROOM3091":[15],
 "ROOM30A0":[0,1,2,3,4,5], "ROOM30A1":[0,1,2,3,4,5],
 "ROOM30E0":[6,7,8,9], "ROOM30E1":[6,7,8,9],
 "ROOM4040":[2,4,5,6,7,9,10,11,12], "ROOM4041":[2,4,5,6,7,9,10,11,12],
 "ROOM4080":[1,2,3,5,6,7,8], "ROOM4081":[1,2,3,5,6,7,8],
 "ROOM40A0":[7], "ROOM40A1":[7],
}
def rdt(room):
    return os.path.join(ROOT,"STAGE%s"%room[4],room+".RDT")
tot=0
for room in sorted(ANOM):
    b=open(rdt(room),'rb').read()
    nCut=b[1]; hdr7=b[7]
    dirs={n:struct.unpack_from("<I",b,8+i*4)[0] for i,n in enumerate(HDRNAMES)}
    pris=[struct.unpack_from("<I",b,0x60+i*32+0x1c)[0] for i in range(nCut)]
    pmin=min(pris); pmax=max(pris)
    nxt=min([v for v in dirs.values() if v>pmax and v<len(b)] or [len(b)])
    print("### %s  nCut=%d hdr7=%d  lit=0x%X  pri_block=[0x%X,0x%X) next=%s@0x%X  block_len=%d  = nCut*4(sect)+nCut*4(tbl)+4 ? %s"
          % (room,nCut,hdr7,dirs['lit'],pmin,nxt,[k for k,v in dirs.items() if v==nxt],nxt,nxt-pmin,
             "yes" if nxt-pmin == 4*nCut+4*nCut+4 else "no"))
    print("    dirs 0x4C..0x5C: esp=0x%X eff=0x%X esp_tim=0x%X model_tim=0x%X rbj=0x%X"
          % (dirs['esp'],dirs['eff'],dirs['esp_tim'],dirs['model_tim'],dirs['rbj']))
    for i in range(nCut):
        o=pris[i]; w=struct.unpack_from("<I",b,o)[0]
        mark="ANOM" if i in ANOM[room] else "    "
        print("    %s cut %2d  rec+0x1C @0x%04X stores 0x%08X (=lit+%d*40+%d) -> RDT[0x%04X..0x%04X] = %s"
              % (mark,i,0x60+i*32+0x1c,o,nCut,o-pmin,o,o+3," ".join("%02X"%x for x in b[o:o+4])))
        if i in ANOM[room]:
            assert w==0xFFFFFFFF, (room,i,hex(w))
            tot+=1
    print()
print("ANOM cuts verified as 0xFFFFFFFF:", tot)
