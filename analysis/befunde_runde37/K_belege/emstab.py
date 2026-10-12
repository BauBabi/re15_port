import struct,sys
exe=open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/info/Re1.5/PSX.EXE','rb').read()
t_addr=struct.unpack_from('<I',exe,0x18)[0]
def rd(addr,n): o=0x800+addr-t_addr; return exe[o:o+n]
for name,tab,btab in (('CDEMD0',0x80072f38,0x80073138),('CDEMD1',0x80073178,0x80073378)):
    print(name, 'table @%08x bytes @%08x'%(tab,btab))
    for i in range(64):
        off,sz=struct.unpack_from('<II',rd(tab+i*8,8))
        b=rd(btab+i,1)[0]
        if sz: print('  type 0x%02x idx %2d sector %5d size %7d byte %02x'%(i+0x10,i,off,sz,b))
