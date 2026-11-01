import struct, re, numpy as np
exec(open('md1.py').read().split("D0=md1(")[0])
inc=open(R+'re15_port/engine/src/gen/re2_ems_toc.inc',encoding='utf-8',errors='replace').read()
m=re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};',inc,re.S)
toc=[int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)u',m.group(1))]
i=((0x31-0x10)*4+3)*2; s,l=toc[i],toc[i+1]; print('RE2 G2 (0x31) Sektor',s,'Laenge',l)
g=blob(R+'re15_port/shared_assets/RE2/CDEMD0.EMS',s,l)
do,=struct.unpack_from('<I',g,0); n,=struct.unpack_from('<I',g,4); d=list(struct.unpack_from('<%dI'%n,g,do)); print('RE2 EMD n',n,'dir',d)
G2=md1(g[d[7]:do])
M1=md1(secs(blob(*F['M1']))[7][1])
def V(x): a=np.frombuffer(x,dtype='<i2').reshape(-1,4)[:,:3].astype(int); return a
for mi,gi in zip(range(1,7),range(12,18)):
    a=V(M1[mi][0][0]); b=V(G2[gi][0][0])
    if len(a)!=len(b): print(mi,gi,'Anzahl',len(a),len(b)); continue
    D=np.abs(a[:,None,:]-b[None,:,:]).max(2)
    nn=D.min(1); ex=(D==0).any(1).sum()
    print('M1[%d] vs G2[%d] n=%d: max NN-Abstand %d, exakte Punkte %d, gleiche Reihenfolge exakt %d'%(mi,gi,len(a),nn.max(),ex,(a==b).all(1).sum()))
