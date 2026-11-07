# RE2 ROOM40A0.RDT Raum-Animationsbank. Leser FUN_80018e08 (RE2 EXE): piVar4 = *(RDT+0x60); Anzahl = (char)piVar4[1];
# Liste piVar7 = piVar4 + *piVar4, je Eintrag 2 Woerter: [0] -> puVar1 = Flags-Wort (Bit 0 Leon, Bit 1 Claire), puVar1+1 = EMR (-> PL+0x190, sw @0x80018ea4);
# [1] -> EDD (-> PL+0x194, sw @0x80018eb8). FUN_8002959c liest EDD[clip] = (Bilder & 0xffff) | (Offset << 16).
import struct,sys
p=sys.argv[1]
d=open(p,'rb').read()
base=struct.unpack_from('<I',d,0x60)[0]
lst=base+struct.unpack_from('<I',d,base)[0]
cnt=d[base+4]
print('RDT+0x60 ->',hex(base),'Anzahl',cnt,'Liste',hex(lst))
for k in range(cnt):
    a,b=struct.unpack_from('<II',d,lst+8*k)
    fl=struct.unpack_from('<I',d,base+a)[0]
    emr=base+a+4; edd=base+b
    w0=struct.unpack_from('<I',d,edd)[0]
    n=(w0>>16)//4
    print('Paar %d Flags 0x%x EMR @0x%x EDD @0x%x Clips %d'%(k,fl,emr,edd,n))
    for c in range(n):
        w=struct.unpack_from('<I',d,edd+4*c)[0]
        print('   Clip %2d: %3d Bilder'%(c,w&0xffff))
