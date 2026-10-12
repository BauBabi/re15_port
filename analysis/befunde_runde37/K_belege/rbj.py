import struct,sys
base=r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/re15_port/shared_assets/PSX/STAGE1/'
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def u16(b,o): return struct.unpack_from('<H',b,o)[0]
for room in sys.argv[1:]:
    r=open(base+'ROOM%s.RDT'%room,'rb').read()
    aoff=u32(r,0x5C)
    # find end: next pointer in header table > aoff
    ptrs=sorted(set(u32(r,0x8+i*4) for i in range(23) if 0<u32(r,0x8+i*4)<len(r)))
    nxt=[p for p in ptrs if p>aoff]; end=nxt[0] if nxt else len(r)
    b=r[aoff:end]
    total=u32(b,0); nrec=u32(b,4)
    print('ROOM%s anim@%#x size %d total %#x nrec %d'%(room,aoff,len(b),total,nrec))
    for i in range(nrec):
        pre=u32(b,total+i*8); edd=u32(b,total+i*8+4)
        marker=u32(b,pre)
        emr=pre+4
        o_arm,o_frm,cnt,ks=struct.unpack_from('<HHHH',b,emr)
        n=u16(b,edd+2)//4
        clips=[u16(b,edd+j*4) for j in range(n)]
        print('  rec%d prefix %#x marker %#x edd %#x bones %d ksize %d clips %d %s'%(i,pre,marker,edd,cnt,ks,n,clips))
