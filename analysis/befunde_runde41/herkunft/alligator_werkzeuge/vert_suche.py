# Vertex-Folgen-Suche: Fenster aus 3 aufeinanderfolgenden MD1-Vertices (je 8 B: x,y,z,w) von RE2 EM023 und MOD-Block 25
# gegen alle Dateien der Disc (ohne ZNULL) und alle MOD-EMS-Bloecke. Numpy-Rollhash ueber 4-Byte-Raster.
import sys,struct,os,json,hashlib
import numpy as np
sys.path.insert(0,'.')
from emd_tab import *
def fenster(meshes,k=3):
    S=set()
    for x in meshes:
        for L in (x.tv,x.qv,x.tn,x.qn):
            for i in range(len(L)-k+1):
                b=b''.join(struct.pack('<4h',*L[i+j]) for j in range(k))
                if len(set(b))>4: S.add(b)
    return S
re2=open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read()
D=E.emd_dir(re2)
m2=E.md1_parse(re2[D[7]:u32(re2,0)])
print('RE2 EM023',len(re2),'identisch zu shared_assets/RE2/EM23.EMD:',re2==open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read())
b25=blocks(R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')[25][1]
D5=E.emd_dir(b25); m25=E.md1_parse(b25[D5[7]:D5[8]])
W2=fenster(m2); W25=fenster(m25)
print('Fenster RE2',len(W2),'B25',len(W25))
def such(S,name):
    # Erste 8 Byte als Schluessel, dann Vollvergleich
    keys=np.array(sorted(set(struct.unpack('<Q',b[:8])[0] for b in S)),dtype=np.uint64)
    ges=0; res={}
    F=json.load(open('iso_files.json'))
    for p,e,s in F:
        if p.endswith('ZNULL.DAT'): continue
        lp=R+'/Version_40%'+p
        if not os.path.exists(lp): continue
        d=open(lp,'rb').read()
        a=np.frombuffer(d[:len(d)&~7],dtype='<u8')
        for sh in (0,4):
            a=np.frombuffer(d[sh:(len(d)-sh)&~7 if sh==0 else sh+((len(d)-sh)&~7)],dtype='<u8')
            mask=np.isin(a,keys)
            idx=np.nonzero(mask)[0]
            for i in idx:
                o=sh+int(i)*8
                if d[o:o+24] in S:
                    res.setdefault(p,[]).append(o)
    print(name,'Treffer je Datei',{k:(len(v),v[:4]) for k,v in res.items()})
such(W2,'RE2-EM023 in Disc')
such(W25,'Mod-B25 in Disc')
