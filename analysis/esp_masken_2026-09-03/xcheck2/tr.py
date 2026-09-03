import glob,struct,collections,sys,os
sys.path.insert(0,r"C:/workspace/git/reAi_v2/analysis/esp_masken_2026-09-03")
from sld import find_sld
fs=sorted(glob.glob(r"C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX/BSS/ROOM*/BG*.BSS"))
print("files",len(fs))
sizes=collections.Counter(); nhit=0; mism=[]
for f in fs:
    d=open(f,'rb').read()
    L=len(d)
    while L>0 and d[L-1]==0: L-=1
    end=(L+3)&~3
    tr=None
    if end>=8:
        flag=struct.unpack_from('<I',d,end-4)[0]
        off=struct.unpack_from('<I',d,end-8)[0]
        if flag!=0 and off+4<=len(d):
            raw=struct.unpack_from('<I',d,off)[0]
            tr=(off,raw)
    bf=find_sld(d)
    bfset=set(bf)
    if tr is not None:
        nhit+=1; sizes[tr[1]]+=1
        if tr not in bfset: mism.append((f,'tr-not-in-bf',tr,bf))
    else:
        if bf: mism.append((f,'bf-but-no-tr',None,bf))
print("trailer hits",nhit,"sizes",sizes.most_common())
print("mismatches",len(mism))
for m in mism[:10]: print(m)
