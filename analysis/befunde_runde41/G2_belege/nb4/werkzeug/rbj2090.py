# RE1.5 RDT: RBJ-Zeiger RDT+0x5C (re15_rdt.h animation), Container wie RE2 (u32 dirOff, u32 Anzahl, Paare (Flags+EMR, EDD))
import struct,sys
for p in sys.argv[1:]:
    d=open(p,'rb').read()
    b=struct.unpack_from('<I',d,0x5c)[0]
    print(p.split('/')[-1],'RDT+0x5C =',hex(b))
    if not b: continue
    lst=b+struct.unpack_from('<I',d,b)[0]; cnt=d[b+4]
    for k in range(cnt):
        a,e=struct.unpack_from('<II',d,lst+8*k)
        fl=struct.unpack_from('<I',d,b+a)[0]; edd=b+e
        n=(struct.unpack_from('<I',d,edd)[0]>>16)//4
        print('  Paar',k,'Flags',hex(fl),'Clips',n,[struct.unpack_from('<I',d,edd+4*c)[0]&0xffff for c in range(n)])
