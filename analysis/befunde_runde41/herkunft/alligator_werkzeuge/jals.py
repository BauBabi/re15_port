import struct,collections,sys
def jals(path,lo,hi):
    d=open(path,'rb').read()
    c=collections.Counter()
    for a in range(lo,hi,4):
        x=struct.unpack_from('<I',d,a-0x80100000)[0]
        if x>>26==3: c[hex(0x80000000|((x&0x3ffffff)<<2))]+=1
    return c
B='Version_40%/PSX/BIN/'
regions={'S1 Hund 0x20':(B+'STAGE1.BIN',0x8010d7f8,0x80112020),'S1 Kraehe 0x21':(B+'STAGE1.BIN',0x80112020,0x80116288),
 'S2 Gator 0x23':(B+'STAGE2.BIN',0x8010c448,0x8010ee9c),'S2 0x24':(B+'STAGE2.BIN',0x8010ee9c,0x801109e4),'S2 0x25':(B+'STAGE2.BIN',0x801109e4,0x80115750),
 'S2 Zombie 0x10':(B+'STAGE2.BIN',0x801002b8,0x8010a75c)}
allc={k:jals(*v) for k,v in regions.items()}
keys=set().union(*[set(c) for c in allc.values()])
for k in sorted(keys):
    row=[allc[r].get(k,0) for r in regions]
    print(k,row)
print(list(regions))
