import struct, os
ROOT = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX"
HDRN = ["snd0_edt","snd0_vh","snd0_vb","snd1_edt","snd1_vh","snd1_vb","sca","rid","rvd","lit",
        "md1tim_ptr","flr","blk","msg","main_scd","sub_scd","extra_scd","esp","eff","esp_tim","model_tim","rbj"]
import sys
for room in sys.argv[1:]:
    p=os.path.join(ROOT,"STAGE%s"%room[4],room+".RDT")
    b=open(p,'rb').read(); nCut=b[1]; nMd=b[2]; hdr7=b[7]
    dirs={n:struct.unpack_from("<I",b,8+i*4)[0] for i,n in enumerate(HDRN)}
    pris=[struct.unpack_from("<I",b,0x60+i*32+0x1c)[0] for i in range(nCut)]
    marks=[(0,"HEADER (0x00..0x5F)"),(0x60,"rid  camera cuts  %d x 32B"%nCut)]
    for n,v in dirs.items():
        if v: marks.append((v,n))
    marks.append((min(pris),"sprite.pri BLOCK (no pointer; = lit + nCut*40)"))
    marks.append((len(b),"<EOF>"))
    marks=sorted(set(marks))
    print("### %s  size=%d(0x%X) nCut=%d nMd1=%d hdr7(sprite_max)=%d"%(room,len(b),len(b),nCut,nMd,hdr7))
    for i,(o,n) in enumerate(marks[:-1]):
        nxt=marks[i+1][0]
        print("   0x%06X .. 0x%06X  (%7d B)  %s"%(o,nxt-1,nxt-o,n))
    print()
