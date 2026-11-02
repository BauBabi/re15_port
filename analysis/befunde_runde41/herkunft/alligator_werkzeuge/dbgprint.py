import struct,glob,os
ins=0x0c000000|((0x800279c8&0x0fffffff)>>2)
for base in ('Version_40%','info/Re1.5'):
    for f in sorted(glob.glob(base+'/PSX/BIN/*.BIN')):
        d=open(f,'rb').read()
        n=len(d)//4
        w=struct.unpack_from('<%dI'%n,d)
        hits=[hex(0x80100000+4*i) for i,x in enumerate(w) if x==ins]
        if hits: print(base,os.path.basename(f),hits)
d=open('Version_40%/PSX.EXE','rb').read()
w=struct.unpack_from('<%dI'%((len(d)-0x800)//4),d,0x800)
print('EXE',[hex(0x80010000+4*i) for i,x in enumerate(w) if x==ins][:30])
