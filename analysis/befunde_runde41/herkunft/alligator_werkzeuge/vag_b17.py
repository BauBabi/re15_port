# Disc-VAGs gegen RE2 ENEMSE-Bank 17 (EM23, TOC @0x800a7b1c); zusaetzlich: in welchen ENEMSE-Baenken liegen die 4 ueberlappenden Disc-Samples?
import struct,glob,os
R=r'C:/workspace/git/reAi_v2'
x=open(R+'/info/re2leon/PSX.EXE','rb').read()
t_addr=struct.unpack_from('<I',x,0x18)[0]
o=0x800a7b1c-t_addr+0x800
E=open(R+'/re15_port/shared_assets/RE2/ENEMSE.VBS','rb').read()
B=[]
for b in range(73):
    a=struct.unpack_from('<4I',x,o+b*16)
    es,ev=a[0],a[2]
    ess=a[1]&0xffffff; evs=a[3]&0xffffff
    B.append((ess*0x800,es,evs*0x800,ev))
print('Bank 17',B[17])
def bl(b):
    s=set()
    for i in range(0,len(b)-15,16):
        y=b[i:i+16]
        if len(set(y))<6: continue
        s.add(y)
    return s
sets={i:bl(E[v:v+n]) for i,(e,ne,v,n) in enumerate(B)}
print('Bloecke Bank 17',len(sets[17]))
samples={'2000 s5':None}
D=R+'/Version_40%/PSX'
res={}
for s in range(1,7):
    for p in sorted(glob.glob(f'{D}/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
        of=[struct.unpack_from('<I',d,o)[0] for o in range(8,0x20,4)]
        for k,(vh,vb) in enumerate(((of[1],of[2]),(of[4],of[5]))):
            if vh==0 or d[vh:vh+4]!=b'pBAV': continue
            ps=struct.unpack_from('<H',d,vh+0x12)[0]; vs=struct.unpack_from('<H',d,vh+0x16)[0]
            tab=vh+32+2048+ps*512; q=vb
            for i in range(1,vs+1):
                sz=struct.unpack_from('<H',d,tab+i*2)[0]*8
                S=bl(d[q:q+sz]); q+=sz
                if len(S)<8: continue
                h=(sz,len(S))
                ov={b:len(S&sets[b]) for b in sets if S&sets[b]}
                if ov: res.setdefault(h,(n,k,i,sz,len(S),ov))
for v in res.values(): print(v)
print('Treffer Bank 17:',[v for v in res.values() if 17 in v[5]])
