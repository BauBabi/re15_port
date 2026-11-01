import struct,sys
R=r'C:/workspace/git/reAi_v2'
sys.path.insert(0,R+'/re15_port/tools/maske')
import geom
for nm,p in (('DISC',R+'/Version_40%/PSX/STAGE2/ROOM2090.RDT'),('MOD',R+'/info/Re1.5/PSX/STAGE2/ROOM2090.RDT')):
    d=open(p,'rb').read()
    Z=geom.sca_zellen(d)
    print(nm,len(Z),'Zellen')
    s=struct.unpack_from('<I',d,0x20)[0]
    for i,(x,z,w,dp,fl) in enumerate(Z):
        e=d[s+24+12*i:s+24+12*(i+1)]
        print(f'  {i:2d} x {x:7d}..{x+w:7d} z {z:7d}..{z+dp:7d} band {fl} raw {e.hex()}')
    # Boden @0x34
    f=struct.unpack_from('<I',d,0x34)[0]
    print(' floor@',hex(f),d[f:f+24].hex())
