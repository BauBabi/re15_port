import struct,hashlib,glob,os
def load(edh_path, vb_path):
    edh=open(edh_path,'rb').read(); vb=open(vb_path,'rb').read()
    pbav=struct.unpack_from('<I',edh,len(edh)-8)[0]
    nrec=pbav//4
    recs=[edh[i*4:i*4+4] for i in range(nrec)]
    vh=edh[pbav:]
    assert vh[:4]==b'pBAV'
    ver,vabid,fsize,res0,ps,ts,vs=struct.unpack_from('<IIIHHHH',vh,4)
    toneoff=32+16*128
    vagoff=toneoff+32*16*ps
    vagsz=struct.unpack_from('<256H',vh,vagoff)
    sizes=[s<<3 for s in vagsz[1:vs+1]]
    offs=[];o=0
    for s in sizes: offs.append(o); o+=s
    return dict(vb=vb,recs=recs,vh=vh,vs=vs,toneoff=toneoff,sizes=sizes,offs=offs)
def rec_md5(X,i):
    r=X['recs'][i]
    if r in (b'\xff\xff\xff\xff',): return 'leer'
    prog=r[1]&0x7f; tone=r[2]>>4
    t=X['toneoff']+prog*0x200+tone*0x20
    tn=X['vh'][t:t+32]
    vag=struct.unpack_from('<H',tn,22)[0]
    if not (0<vag<=X['vs']): return 'vag?'
    return '%s v%d p%d c%d s%d'%(hashlib.md5(X['vb'][X['offs'][vag-1]:X['offs'][vag-1]+X['sizes'][vag-1]]).hexdigest()[:8],tn[2],tn[3],tn[4],tn[5])
for root,label in (('info/re2leon/COMMON/SOUND','RE2'),('re15_port/shared_assets/PSX/SOUND','RE1.5')):
    for e in sorted(glob.glob(root+'/CORE*.EDH')):
        X=load(e,e[:-4]+'.VB')
        print(label,os.path.basename(e),' | '.join('%d:%s'%(i,rec_md5(X,i)) for i in (4,5,6,8,10)))
