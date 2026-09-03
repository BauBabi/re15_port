import struct,os,sys,glob,json,hashlib,collections
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from x_sld import STAGE_TBL, PSX, depack
ROOT=os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','..','..'))
BSSD=os.path.join(ROOT,'re15_port','shared_assets','PSX','BSS')

want={}   # (roomkey, cut) -> bytes
for stg in range(1,7):
    d=open(os.path.join(PSX,'BIN','STAGE%d.BIN'%stg),'rb').read(); tb=STAGE_TBL[stg][0]
    for p in sorted(glob.glob(os.path.join(PSX,'STAGE%d'%stg,'ROOM*.BSS'))):
        name=os.path.basename(p)[:-4]; room=int(name[4:],16)&0xff
        data=open(p,'rb').read(); nch=len(data)//0x10000
        for cut in range(min(nch,16)):
            e=struct.unpack('<H',d[tb+room*0x20+cut*2:tb+room*0x20+cut*2+2])[0]
            if e==0x0008: continue
            ch=data[cut*0x10000:(cut+1)*0x10000]
            if struct.unpack('<I',ch[e-4:e])[0]==0: continue
            off=struct.unpack('<i',ch[e-8:e-4])[0]
            us=struct.unpack('<I',ch[off:off+4])[0]
            want[(name,cut)]=depack(ch,off+4,us)
print('want',len(want))

files=sorted(glob.glob(os.path.join(BSSD,'ROOM????','PRI*.TIM')))
print('files',len(files))
ident=0; diff=0; nofile=0; extra=[]
have=set()
for f in files:
    rd=os.path.basename(os.path.dirname(f))     # ROOM1170
    cut=int(os.path.basename(f)[3:5])
    key=('ROOM%03X'%(int(rd[4:8],16)>>4), cut)   # ROOM1170 -> 0x117
    have.add(key)
    b=open(f,'rb').read()
    if key not in want:
        extra.append((f,len(b))); continue
    if b==want[key]: ident+=1
    else:
        diff+=1; print('DIFF',f,len(b),len(want[key]))
print('byte-identisch',ident,'differing',diff,'extra(no-want)',len(extra))
for x in extra: print('  EXTRA',x[0],x[1])
missing=sorted(set(want)-have)
print('missing (want without file):',len(missing))
c=collections.Counter(k[0] for k in missing)
print(dict(sorted(c.items())))
