# MD1-Signatur in allen Disc-Dateien: u32 len,u32 0,u32 c (2*Meshes), u32 @+12 == 28*c (erster Vertex-Offset relativ +12).
import struct,os,json,collections
import numpy as np
R=r'C:/workspace/git/reAi_v2'
F=json.load(open('iso_files.json'))
res=[]
for p,e,s in F:
    if p.endswith('ZNULL.DAT'): continue
    lp=R+'/Version_40%'+p
    if not os.path.exists(lp): continue
    d=open(lp,'rb').read()
    a=np.frombuffer(d[:len(d)&~3],dtype='<u4')
    idx=np.where((a[1:-2]==0)&(a[2:-1]>=2)&(a[2:-1]<=120)&((a[2:-1]&1)==0)&(a[3:]==a[2:-1]*28))[0]
    for i in idx:
        o=int(i)*4
        ln,_,c=struct.unpack_from('<III',d,o)
        if ln<100 or ln>0x80000 or o+ln>len(d): continue
        # Summen
        tot=0;q=0;ok=True
        for m in range(c//2):
            t=struct.unpack_from('<14I',d,o+12+m*56)
            tot+=t[1]+t[8]; q+=t[5]+t[12]
            if t[1]>4000 or t[8]>4000: ok=False
        if ok: res.append((p,o,ln,c//2,tot,q))
print(len(res))
print(collections.Counter(os.path.splitext(r[0])[1] for r in res))
gross=[r for r in res if r[0].endswith('.RDT')]
gross.sort(key=lambda r:-r[4])
print('RDT groesste:',gross[:24])
print('ausserhalb:',[r for r in res if not r[0].endswith(('.RDT','.EMS','.PLD','.PLW'))][:20])
# je EMS-Treffer genau in Bloecken?
print('EMS',[(r[1]//2048,r[3],r[4]) for r in res if r[0].endswith('CDEMD0.EMS')])
