import sys, struct
T="C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration"
for r in sys.argv[1:]:
    d=open(T+"/re15_port/shared_assets/PSX/STAGE1/ROOM%s.RDT"%r,"rb").read()
    so=struct.unpack_from("<I",d,0x20)[0]
    cx,cz=struct.unpack_from("<hh",d,so)
    cnt=struct.unpack_from("<5I",d,so+4); o=so+24; seen=set()
    print("== ROOM%s ceil(%d,%d) rgn %s"%(r,cx,cz,cnt))
    for g in range(5):
        for i in range(cnt[g]):
            w,dn,x,z,t,u0,u1,fl=struct.unpack_from("<HHhhBBBB",d,o); o+=12
            k=(w,dn,x,z,t,u0,u1,fl)
            if k in seen: continue
            seen.add(k); print("  %d.%d x %d..%d z %d..%d t%d u0 %02x u1 %02x fl %02x"%(g,i,x,x+w,z,z+dn,t,u0,u1,fl))
