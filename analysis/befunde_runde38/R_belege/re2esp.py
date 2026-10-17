import struct,sys
def u16(b,o): return b[o]|(b[o+1]<<8)
def ids(raw):
    al=(len(raw)+3)&~3; ende=al-4; out={}
    for n in range(8):
        i=raw[n]
        if i==0xFF: break
        out[i]=struct.unpack_from('<I',raw,ende-4*n)[0]
    return out
raw=open(sys.argv[1],'rb').read()
want=int(sys.argv[2],0)
d=ids(raw)
print('ids',[hex(k) for k in d])
off=d[want]
w=struct.unpack_from('<I',raw,off)[0]
tab=off+((w&0xffff)*2+(w>>16)+2)*4
print('bank@0x%x w=0x%08x tab@0x%x'%(off,w,tab))
for sub in range(int(sys.argv[3]) if len(sys.argv)>3 else 4):
    woff=u16(raw,tab+(sub&7)*2); sk=tab+woff*4; np_=u16(raw,sk); p=sk+4
    print(' sub %d: skript@0x%x nparts=%d'%(sub,sk,np_))
    for k in range(min(np_,6)):
        ns=u16(raw,p)
        print('  part%d nsteps=%d'%(k,ns))
        for s in range(min(ns,8)):
            st=raw[p+4+s*24:p+4+s*24+24]
            h=struct.unpack('<12h',st)
            print('   step%d '%s + ' '.join('%d'%x for x in h))
        p+=ns*24+4
