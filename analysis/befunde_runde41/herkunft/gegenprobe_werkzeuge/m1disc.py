import struct, hashlib, os, glob
exec(open('sec.py').read().split('for k,(p,s,l) in F.items():')[0])
D0=secs(blob(*F['D0'])); M1=secs(blob(*F['M1']))
# Keyframes
e0=D0[2][1]; e1=M1[2][1]
k0=set(e0[184+i*84:184+(i+1)*84] for i in range((len(e0)-184)//84))
k1=[e1[184+i*84:184+(i+1)*84] for i in range((len(e1)-184)//84)]
print('EMR2 Kopf 184 B gleich:',e0[:184]==e1[:184],' M1-Keyframes',len(k1),'davon in D0:',sum(k in k0 for k in k1),' (D0 hat',len(k0),'versch.)')
# alle Disc-Dateien
files=[p for p in glob.glob(R+'Version_40%/**/*',recursive=True) if os.path.isfile(p) and not p.endswith('.bin')]
print('Disc-Dateien',len(files))
data={p:open(p,'rb').read() for p in files}
def where(w):
    return [os.path.relpath(p,R+'Version_40%') for p,d in data.items() if w in d]
for idx,name in ((1,'EDD1'),(2,'EMR2'),(7,'MD1'),(8,'TIM')):
    x=M1[idx][1]; hits=0; tot=0; nohit=0; ex=None
    for o in range(0,len(x)-64,max(64,len(x)//60)):
        w=x[o:o+64]
        if len(set(w))<6: continue
        tot+=1; wh=where(w)
        if wh: hits+=1; ex=ex or wh[:2]
        else: nohit+=1
    print(f'M1 {name}: {tot} Stichproben a 64 B, auf Disc gefunden {hits}, nicht gefunden {nohit}, Bsp {ex}')
# ganzer Abschnitt
for idx,name in ((1,'EDD1'),(2,'EMR2'),(7,'MD1'),(8,'TIM')):
    print(name,'ganz auf Disc:',where(M1[idx][1]))
