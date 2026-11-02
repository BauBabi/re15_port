# 16-Byte-ADPCM-Block-Vergleich: alle Disc-VAGs (RDT-Banken, SOUND/*.VB) gegen RE2 ROOM40A0 (beide Banken) und ENEMSE.VBS.
import struct,glob,os,collections
R=r'C:/workspace/git/reAi_v2'
def blocks(b):
    s=set()
    for i in range(0,len(b)-15,16):
        x=b[i:i+16]
        if x[2:]==b'\0'*14 or x[2:]==b'\x77'*14: continue
        if len(set(x))<6: continue
        s.add(x)
    return s
r=open(R+'/info/re2leon/PL0/RDT/ROOM40A0.RDT','rb').read()
of=[struct.unpack_from('<I',r,o)[0] for o in range(8,0x20,4)]
print('RE2 40A0 Offsets',[hex(x) for x in of],len(r))
# VB0 von of[2] bis of[3]-Grenze? VB-Ende aus VH-Tabelle
def vbsize(d,vh):
    ps=struct.unpack_from('<H',d,vh+0x12)[0]; vs=struct.unpack_from('<H',d,vh+0x16)[0]
    tab=vh+32+2048+ps*512
    return sum(struct.unpack_from('<H',d,tab+i*2)[0]*8 for i in range(1,vs+1)),vs
sets={}
for k,(vh,vb) in enumerate(((of[1],of[2]),(of[4],of[5]))):
    if vh==0: continue
    n,vs=vbsize(r,vh); print('bank',k,'vs',vs,'bytes',n)
    sets[f'40A0.b{k}']=blocks(r[vb:vb+n])
sets['ENEMSE.VBS']=blocks(open(R+'/re15_port/shared_assets/RE2/ENEMSE.VBS','rb').read())
print({k:len(v) for k,v in sets.items()})
# Disc
res=[]
D=R+'/Version_40%/PSX'
for s in range(1,7):
    for p in sorted(glob.glob(f'{D}/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
        of=[struct.unpack_from('<I',d,o)[0] for o in range(8,0x20,4)]
        for k,(vh,vb) in enumerate(((of[1],of[2]),(of[4],of[5]))):
            if vh==0 or d[vh:vh+4]!=b'pBAV': continue
            ps=struct.unpack_from('<H',d,vh+0x12)[0]; vs=struct.unpack_from('<H',d,vh+0x16)[0]
            tab=vh+32+2048+ps*512; o=vb
            for i in range(1,vs+1):
                sz=struct.unpack_from('<H',d,tab+i*2)[0]*8
                B=blocks(d[o:o+sz]); o+=sz
                if len(B)<8: continue
                res.append((n,k,i,sz,len(B),{t:len(B&S) for t,S in sets.items()}))
for p in sorted(glob.glob(D+'/SOUND/*.VB')):
    d=open(p,'rb').read(); B=blocks(d)
    res.append((os.path.basename(p),0,0,len(d),len(B),{t:len(B&S) for t,S in sets.items()}))
ges=collections.Counter()
seen=set()
for n,k,i,sz,b,m in sorted(res,key=lambda x:-max(x[5].values())/max(1,x[4])):
    key=(sz,b,tuple(m.items()))
    if key in seen: continue
    seen.add(key)
    if max(m.values())>0: print(n,k,i,'Bytes',sz,'Bloecke',b,m)
print('Eintraege',len(res),'eindeutig',len(seen))
