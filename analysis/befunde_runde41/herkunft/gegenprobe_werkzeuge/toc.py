import hashlib, zlib, struct, sys
R='C:/workspace/git/reAi_v2/'
def a2o(a): return a-0x80010000+0x800
for tag,base in (('DISC',R+'Version_40%/'),('MOD',R+'info/Re1.5/')):
    exe=open(base+'PSX.EXE','rb').read()
    print(tag,'EXE len',len(exe),'md5',hashlib.md5(exe).hexdigest()[:8], 'tadr', hex(struct.unpack_from('<I',exe,0x18)[0]), 'tsize',hex(struct.unpack_from('<I',exe,0x1c)[0]))
    for fig,toc,arc in (('Leon',0x80072f38,'CDEMD0.EMS'),('Elza',0x80073178,'CDEMD1.EMS')):
        ems=open(base+'PSX/EMD/'+arc,'rb').read()
        for typ in (0x22,0x23,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37):
            o=a2o(toc+(typ-0x10)*8)
            s,l=struct.unpack_from('<II',exe,o)
            blob=ems[s*0x800:s*0x800+l]
            h=hashlib.md5(blob).hexdigest()[:8] if l else '-'
            c='%08X'%(zlib.crc32(blob)&0xffffffff) if l else '-'
            print(f'  {fig} typ {typ:02x} @0x{toc+(typ-0x10)*8:08x} file0x{o:x} raw {exe[o:o+8].hex(" ")} sec {s} len {l} md5 {h} crc {c}')
