import struct, sys, hashlib, collections
p = sys.argv[1]
d = open(p,'rb').read()
u16=lambda o: struct.unpack_from('<H',d,o)[0]
u32=lambda o: struct.unpack_from('<I',d,o)[0]
print('datei', p, len(d), hashlib.sha256(d).hexdigest())
begin, cnt = u32(0), u32(4)
dirs=[u32(begin+4*i) for i in range(cnt)]
print('dir', cnt, [hex(x) for x in dirs])
# EDD banks
for b in range(3):
    ei=1+2*b
    if ei<cnt and dirs[ei]: print(' bank',b,'EDD clips', u16(dirs[ei]+2)//4, 'EMR bones', u16(dirs[ei+1]+4), 'kfoff', u16(dirs[ei+1]+2))
emr=dirs[2]; print('EMR0 bones', u16(emr+4))
md1=dirs[7]; tim=dirs[8]
oc=u32(md1+8); nm=oc//2; print('MD1 object_count',oc,'meshes',nm)
# TIM
assert u32(tim)==0x10, hex(u32(tim))
flags=u32(tim+4); bpp={0:4,1:8,2:16,3:24}[flags&3]; hasclut=bool(flags&8)
o=tim+8
cl_len=u32(o); cx,cy,cw,ch=struct.unpack_from('<4H',d,o+4)
clut_entries=cw*ch
o+=cl_len
il=u32(o); ix,iy,iw,ih=struct.unpack_from('<4H',d,o+4)
w = iw*2 if bpp==8 else (iw*4 if bpp==4 else iw)
print('TIM bpp',bpp,'clut x,y,w,h',cx,cy,cw,ch,'entries',clut_entries,'img',w,'x',ih)
row=256 if bpp==8 else 16
n_cluts = clut_entries//row
print('n_cluts',n_cluts,'Port-Texturhoehe',ih*n_cluts, '<=4096', ih*n_cluts<=4096)
base=md1+12
hist=collections.Counter(); bad=0; total=0; per_mesh={}
for i in range(nm):
    h=md1+12+i*56
    f=struct.unpack_from('<14I',d,h)
    tf_cnt,tu_off=f[5],f[6]; qf_cnt,qu_off=f[12],f[13]
    mh=collections.Counter()
    for k in range(tf_cnt):
        c=u16(base+tu_off+12*k+2); idx=((c>>6)&0x1ff)-cy; mh[idx]+=1
    for k in range(qf_cnt):
        c=u16(base+qu_off+16*k+2); idx=((c>>6)&0x1ff)-cy; mh[idx]+=1
    for k,v in mh.items():
        total+=v; hist[k]+=v
        if k<0 or k>=n_cluts: bad+=v
    per_mesh[i]=dict(sorted(mh.items()))
print('Flaechen gesamt',total,'ungueltige CLUT-Indizes',bad)
print('CLUT-Index-Histogramm (Index: Flaechen; VRAM-y = %d+Index)'%cy, dict(sorted(hist.items())))
for i,m in per_mesh.items(): print('  mesh',i, {cy+k:v for k,v in m.items()})
