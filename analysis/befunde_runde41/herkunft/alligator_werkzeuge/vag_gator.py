# RE2-Alligator-Baenke (Paar-Tabelle @0x800a7400: Bank 22 = (0x23,0), 62 = (0x23,0x1f), 63 = (0x23,0x20))
# + RE2 ROOM40A0 (beide Tonbaenke) gegen ALLE Dateien der 40%-Disc, jede Byte-Lage (8 Verschiebungen x u64-Schluessel).
import struct,glob,os
import numpy as np
R='C:/workspace/git/reAi_v2'
x=open(R+'/info/re2leon/PSX.EXE','rb').read()
o=0x800a7b1c-0x80010000+0x800
E=open(R+'/re15_port/shared_assets/RE2/ENEMSE.VBS','rb').read()
def bank(b):
    a=struct.unpack_from('<4I',x,o+b*16)
    vs=(a[3]&0xffffff)*0x800; return E[vs:vs+a[2]]
def bl(b):
    s=set()
    for i in range(0,len(b)-15,16):
        y=b[i:i+16]
        if len(set(y))<6: continue
        s.add(y)
    return s
S={}
for b in (22,62,63): S['ENEMSE%d'%b]=bl(bank(b))
r=open(R+'/info/re2leon/PL0/RDT/ROOM40A0.RDT','rb').read()
of=[struct.unpack_from('<I',r,q)[0] for q in range(8,0x20,4)]
print('40A0 offs',[hex(v) for v in of])
# beide Baenke: (vh,vb)=(of[1],of[2]) und (of[4],of[5]); VB-Laenge bis naechster Abschnitt -> grob: ganzer Rest bis zum naechsten bekannten Offset
offs=sorted(set(v for v in struct.unpack_from('<23I',r,8) if v))
def ende(a): 
    n=[v for v in offs if v>a]; return n[0] if n else len(r)
for k,(vh,vb) in enumerate(((of[1],of[2]),(of[4],of[5]))):
    if vb: S['40A0.b%d'%k]=bl(r[vb:ende(vb)])
for k,v in S.items(): print(k,'Bloecke',len(v))
keys={k:np.array(sorted(set(struct.unpack('<Q',y[:8])[0] for y in v)),dtype=np.uint64) for k,v in S.items()}
hits={k:{} for k in S}
for p in sorted(glob.glob(R+'/Version_40%/**/*',recursive=True)):
    if os.path.isdir(p) or p.endswith(('ZNULL.DAT','.bin','.cue')): continue
    d=open(p,'rb').read()
    for sh in range(8):
        n=(len(d)-sh)//8
        if n<=0: continue
        a=np.frombuffer(d,dtype='<u8',count=n,offset=sh)
        for k,K in keys.items():
            idx=np.nonzero(np.isin(a,K))[0]
            for i in idx:
                q=sh+int(i)*8
                if d[q:q+16] in S[k]: hits[k][os.path.basename(p)]=hits[k].get(os.path.basename(p),0)+1
for k,v in hits.items(): print(k,'Treffer',sum(v.values()),dict(sorted(v.items(),key=lambda t:-t[1])[:8]))
