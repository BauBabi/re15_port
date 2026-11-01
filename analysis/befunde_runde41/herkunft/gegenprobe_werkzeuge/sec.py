import struct, hashlib
R='C:/workspace/git/reAi_v2/'
def blob(p,s,l):
    f=open(p,'rb'); f.seek(s*2048); return f.read(l)
F={'D0':(R+'Version_40%/PSX/EMD/CDEMD0.EMS',1263,226404),'D1':(R+'Version_40%/PSX/EMD/CDEMD1.EMS',1266,226404),
   'M0':(R+'info/Re1.5/PSX/EMD/CDEMD0.EMS',1261,226404),'M1k':(R+'info/Re1.5/PSX/EMD/CDEMD1.EMS',1265,226404),
   'M1':(R+'info/Re1.5/PSX/EMD/CDEMD1.EMS',2015,228460),'M0x':(R+'info/Re1.5/PSX/EMD/CDEMD0.EMS',2012,228460),
   'P1':(R+'re15_port/shared_assets/PSX/EMD/CDEMD1.EMS',2015,228460)}
def secs(b):
    do,=struct.unpack_from('<I',b,0); n,=struct.unpack_from('<I',b,4)
    d=list(struct.unpack_from('<%dI'%n,b,do))
    g=d+[do]
    return [(d[i],b[d[i]:g[i+1]]) for i in range(n)]
for k,(p,s,l) in F.items():
    b=blob(p,s,l)
    S=secs(b)
    print(f'{k:4s} md5 {hashlib.md5(b).hexdigest()[:8]} dir@{struct.unpack_from("<I",b,0)[0]:#x} n={len(S)} ', ' '.join(f'{i}:{len(x)}/{hashlib.md5(x).hexdigest()[:8]}' for i,(o,x) in enumerate(S)))
