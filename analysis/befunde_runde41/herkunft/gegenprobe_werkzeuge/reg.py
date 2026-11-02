import struct,sys
R='C:/workspace/git/reAi_v2/'
# Alle "lui v0,hi; addiu v0,v0,lo; lui at,0x8007; sw v0,off(at)" -> Dispatch-Eintrag
for b in ('Version_40%','info/Re1.5'):
  for s in range(1,7):
    d=open(R+b+'/PSX/BIN/STAGE%d.BIN'%s,'rb').read()
    regs=[]
    for i in range(0,len(d)-16,4):
        w=[struct.unpack_from('<I',d,i+4*k)[0] for k in range(4)]
        if (w[2]&0xffff0000)==0x3c010000 and (w[2]&0xffff)==0x8007 and (w[3]>>26)==0x2b and ((w[3]>>21)&31)==1:
            off=w[3]&0xffff; 
            if off&0x8000: off-=0x10000
            addr=0x80070000+off
            if 0x80072bac<=addr<0x80072bac+0x80*4:
                # Wert aus lui/addiu davor
                hi=w[0]&0xffff if (w[0]>>26)==0x0f else None
                lo=w[1]&0xffff; lo=lo-0x10000 if lo&0x8000 else lo
                val=(hi<<16)+lo if hi is not None else None
                regs.append('%02x->%s'%((addr-0x80072bac)//4, hex(val) if val else '?'))
    print(b.split('/')[0][:10],'STAGE%d'%s,' '.join(regs))
