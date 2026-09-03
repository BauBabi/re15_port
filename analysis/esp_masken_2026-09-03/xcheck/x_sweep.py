import struct, os, sys, json, glob
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from x_sld import depack, STAGE_TBL, PSX

rows=[]
tot_fg=0; tot_no=0; bad=0
sizes={}
gaps={}
epairs=[]
fgmap={}
for st in range(1,7):
    d=open(os.path.join(PSX,'BIN','STAGE%d.BIN'%st),'rb').read()
    tb=STAGE_TBL[st][0]
    bss=sorted(glob.glob(os.path.join(PSX,'STAGE%d'%st,'ROOM*.BSS')))
    for p in bss:
        name=os.path.basename(p)[:-4]      # ROOM117
        rn=int(name[4:],16)                 # room index low byte e.g. 0x17
        room=rn & 0xff
        data=open(p,'rb').read()
        nchunk=len(data)//0x10000
        row=[struct.unpack('<H',d[tb+room*0x20+i*2:tb+room*0x20+i*2+2])[0] for i in range(16)]
        nonfill=sum(1 for v in row if v!=0x0008)
        rows.append((st,name,room,nchunk,nonfill,row))
        for cut in range(nchunk):
            e=row[cut] if cut<16 else None
            chunk=data[cut*0x10000:(cut+1)*0x10000]
            if e is None or e==0x0008 or e<8 or e>0x10000: 
                continue
            present=struct.unpack('<I',chunk[e-4:e])[0]
            if present==0:
                tot_no+=1; continue
            tot_fg+=1
            off=struct.unpack('<i',chunk[e-8:e-4])[0]
            usize=struct.unpack('<I',chunk[off:off+4])[0]
            try:
                out=depack(chunk,off+4,usize)
            except Exception as ex:
                bad+=1; print('FAIL',name,cut,ex); continue
            ok = len(out)==usize and struct.unpack('<I',out[0:4])[0]==0x10
            if not ok: bad+=1; print('NOTTIM',name,cut,out[:8].hex())
            sizes[usize]=sizes.get(usize,0)+1
            fgmap['%s:%d'%(name,cut)]={'e':e,'off':off,'usize':usize,'present':present,
                                       'sha':__import__('hashlib').sha1(out).hexdigest()}
            epairs.append((name,cut,e,off,usize,present))
print('TOTAL fg=%d no=%d bad=%d'%(tot_fg,tot_no,bad))
print('sizes',{hex(k):v for k,v in sorted(sizes.items())})
json.dump(fgmap,open('x_fgmap.json','w'),indent=0)
# chunk-count vs nonfill for stage1
for st,name,room,nc,nf,row in rows:
    if st==1: print('S1',name,'chunks=%d nonfill=%d'%(nc,nf), 'MATCH' if nc==nf else '### MISMATCH')
