# PLD: Wort0 = Verzeichnis-Offset; Verzeichnis[0] = EDD (RE2 FUN_8003baf0: +0x17c = base + *(base + *base)), [1] = EMR (+0x108)
import struct,sys
for p in sys.argv[1:]:
    d=open(p,'rb').read()
    dr=struct.unpack_from('<I',d,0)[0]
    edd=struct.unpack_from('<I',d,dr)[0]
    n=(struct.unpack_from('<I',d,edd)[0]>>16)//4
    print(p.split('/')[-1],'EDD @',hex(edd),'Clips',n)
    print('  ',[ (c,struct.unpack_from('<I',d,edd+4*c)[0]&0xffff) for c in range(n)])
