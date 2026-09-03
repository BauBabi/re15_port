import glob,struct,collections,sys
sys.path.insert(0,r"C:/workspace/git/reAi_v2/analysis/esp_masken_2026-09-03")
from sld import find_sld, sld_decompress
fs=sorted(glob.glob(r"C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX/BSS/ROOM*/BG*.BSS"))
bfc=collections.Counter(); trc=collections.Counter()
agree=0; only_bf=[]; only_tr=[]; diff=[]
nbf=0; ntr=0
for f in fs:
    d=open(f,'rb').read()
    L=len(d)
    while L>0 and d[L-1]==0: L-=1
    end=(L+3)&~3
    tr=None
    if end>=8:
        flag=struct.unpack_from('<I',d,end-4)[0]
        off=struct.unpack_from('<I',d,end-8)[0]
        if flag!=0 and off+4<=len(d) and off < end-8:
            raw=struct.unpack_from('<I',d,off)[0]
            if 0x2000<=raw<=0x40000:
                try:
                    head,_=sld_decompress(d,off+4,raw,limit=16)
                    if head[:4]==b'\x10\x00\x00\x00':
                        tr=(off,raw)
                except Exception: pass
    bf=find_sld(d)
    if bf: nbf+=1; bfc[bf[0][1]]+=1
    if tr: ntr+=1; trc[tr[1]]+=1
    if tr and bf:
        if tr in set(bf): agree+=1
        else: diff.append((f,tr,bf))
    elif tr and not bf: only_tr.append((f,tr))
    elif bf and not tr: only_bf.append((f,bf))
print("files",len(fs))
print("brute-force files with SLD:",nbf, bfc.most_common())
print("trailer files with SLD:",ntr, trc.most_common())
print("agree",agree,"only_tr",len(only_tr),"only_bf",len(only_bf),"diff",len(diff))
for x in only_bf[:6]: print("ONLY_BF",x)
for x in only_tr[:6]: print("ONLY_TR",x)
for x in diff[:6]: print("DIFF",x)
