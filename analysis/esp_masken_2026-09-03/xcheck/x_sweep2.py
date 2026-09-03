import struct, os, sys, json, glob, hashlib, collections
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from x_sld import STAGE_TBL, PSX

def depack2(src, spos, outlen):
    out=bytearray(outlen); t3=0;t4=0;t0=0;t5=0
    st=dict(raw=0,flit=0,short=0,long=0,copied=0,flag=0)
    if outlen<=0: return bytes(out),0,st
    while True:
        if t4==0:
            t4=0x80; t5=src[spos+t3]; t3+=1; st['flag']+=1
        a3=src[spos+t3]; t3+=1
        if (a3&0x80)==0:
            out[t0]=a3; t0+=1; st['raw']+=1
        else:
            v1=(a3<<24)&0xffffffff
            if (t5&t4)==0:
                out[t0]=a3;t0+=1; t4=(t4>>1)&0xffff; st['flit']+=1
            else:
                a3=src[spos+t3]
                v0=((a3<<16)|v1)&0xffffffff
                t2=(v0>>20) if v0<0x80000000 else ((v0>>20)-(1<<12))
                t3+=1
                if (a3&0x0f)!=0:
                    t1=(a3&0x0f)+2; st['short']+=1
                else:
                    a3=src[spos+t3]; t3+=1
                    t1=(a3&0x3f)+3
                    v0=((a3>>6)|((t2<<2)&0xffffffff))&0xffffffff
                    v0=(v0<<16)&0xffffffff
                    t2=(v0>>16) if v0<0x80000000 else ((v0>>16)-(1<<16))
                    st['long']+=1
                t4>>=1
                sp2=t0+t2; te=t0+t1
                for _ in range(t1):
                    out[t0]=out[sp2]; sp2+=1; t0+=1
                st['copied']+=t1
                t0=te; t4&=0xffff
        if not (t0<outlen): break
    return bytes(out), t3, st

res={}
gaps=collections.Counter(); emin=1<<30; emax=0
stmax=None; stsum=collections.Counter(); n=0
per_stage=collections.Counter(); per_room=collections.Counter()
ok_all=True
for stg in range(1,7):
    d=open(os.path.join(PSX,'BIN','STAGE%d.BIN'%stg),'rb').read()
    tb=STAGE_TBL[stg][0]
    for p in sorted(glob.glob(os.path.join(PSX,'STAGE%d'%stg,'ROOM*.BSS'))):
        name=os.path.basename(p)[:-4]; room=int(name[4:],16)&0xff
        data=open(p,'rb').read(); nchunk=len(data)//0x10000
        for cut in range(min(nchunk,16)):
            e=struct.unpack('<H',d[tb+room*0x20+cut*2:tb+room*0x20+cut*2+2])[0]
            if e==0x0008: continue
            ch=data[cut*0x10000:(cut+1)*0x10000]
            if struct.unpack('<I',ch[e-4:e])[0]==0: continue
            off=struct.unpack('<i',ch[e-8:e-4])[0]
            us=struct.unpack('<I',ch[off:off+4])[0]
            out,consumed,st=depack2(ch,off+4,us)
            end=off+4+consumed
            if not (off < e-8): print('### off>=e-8',name,cut); ok_all=False
            if not (end <= e-8): print('### end>e-8',name,cut,hex(end),hex(e-8)); ok_all=False
            gaps[(e-8)-end]+=1
            emin=min(emin,e); emax=max(emax,e)
            n+=1
            for k,v in st.items(): stsum[k]+=v
            if stmax is None or st['copied']+st['raw']>stmax[2]['copied']+stmax[2]['raw']:
                pass
            res['%s:%02d'%(name,cut)]=dict(e=e,off=off,us=us,end=end,sha=hashlib.sha1(out).hexdigest(),
                                           stats=st, comp=consumed+4)
            per_stage[stg]+=1; per_room[name]+=1
print('n=%d ok=%s'%(n,ok_all))
print('gap dist', dict(sorted(gaps.items())))
print('e range', hex(emin), hex(emax))
print('mean stats', {k: round(v/n,1) for k,v in stsum.items()})
print('max comp', max(v['comp'] for v in res.values()))
print('per stage', dict(per_stage))
print('STAGE5 rooms', {k:v for k,v in per_room.items() if k.startswith('ROOM5')})
json.dump(res, open('x_res.json','w'), indent=0)
