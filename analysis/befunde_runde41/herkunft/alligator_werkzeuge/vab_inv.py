# VAG-Inventar: alle RDT-Tonbaenke (Disc, Mod) + SOUND/*.EDH+VB (Disc); md5 je Sample; ROOM2090-Banken gegen Rest.
import struct,glob,os,hashlib,collections,json,sys
R=r'C:/workspace/git/reAi_v2'
def vab(d,vh,vb,lim):
    if d[vh:vh+4]!=b'pBAV': return None
    ps,ts,vs=struct.unpack_from('<HHH',d,vh+0x12)
    tab=vh+32+2048+ps*512
    sz=[struct.unpack_from('<H',d,tab+i*2)[0]*8 for i in range(256)]
    out=[];o=vb
    for i in range(1,vs+1):
        n=sz[i]
        out.append((i,n,hashlib.md5(d[o:o+n]).hexdigest()[:10],o))
        o+=n
    return ps,ts,vs,out,o
ALL=collections.defaultdict(list)
def rdt(base,tag):
    r={}
    for s in range(1,7):
        for p in sorted(glob.glob(f'{base}/STAGE{s}/ROOM*.RDT')):
            d=open(p,'rb').read()
            if len(d)<0x60: continue
            n=os.path.basename(p)[4:8]
            of=[struct.unpack_from('<I',d,o)[0] for o in range(8,0x20,4)]
            for k,(e,vh,vb) in enumerate(((of[0],of[1],of[2]),(of[3],of[4],of[5]))):
                if vh==0 or vh>=len(d): continue
                v=vab(d,vh,vb,len(d))
                if v is None: continue
                r[(n,k)]=v
                for i,sz,h,o in v[3]:
                    if sz>0: ALL[h].append((tag,n,k,i,sz))
    return r
D=rdt(R+'/Version_40%/PSX','D')
M=rdt(R+'/info/Re1.5/PSX','M')
print('Disc Banken',len(D),'Mod',len(M))
for k in sorted(D):
    if k[0].startswith('20'):
        ps,ts,vs,L,e=D[k]
        print(k,'ps',ps,'ts',ts,'vs',vs,'Bytes',sum(x[1] for x in L),'sizes',[x[1] for x in L])
print('--- ROOM2090 Disc: wo kommen die Samples noch vor?')
for k in sorted(D):
    if k[0]=='2090':
        for i,sz,h,o in D[k][3]:
            where=[(t,n,kk,ii) for t,n,kk,ii,s in ALL[h] if not (t=='D' and n=='2090')]
            print(k,i,sz,h,'@',hex(o),'sonst:',len(where),where[:6])
# ROOM2090 Mod
for k in sorted(M):
    if k[0]=='2090':
        print('MOD',k,M[k][:3],[x[1] for x in M[k][3]])
json.dump({h:v for h,v in ALL.items()},open('vag_index.json','w'))
print('Samples gesamt',len(ALL))
