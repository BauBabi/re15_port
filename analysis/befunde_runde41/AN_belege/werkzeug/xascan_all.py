import sys, os, struct
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/nutzer_batch_2026-08-27/tools")
from re2_scd_walk import all_subs, walk
B=r"C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/"
tab={}
for s in range(1,8):
    d=open(B+"STAGE%d.BIN"%s,"rb").read()
    i=d.find(bytes.fromhex("44e522ac"))
    w=struct.unpack_from("<I",d,i-12)[0]   # addiu v0,v0,imm
    hi=struct.unpack_from("<I",d,i-16)[0]
    imm=w&0xffff; imm = imm-0x10000 if imm&0x8000 else imm
    addr=((hi&0xffff)<<16)+imm
    tab[s]=(d,addr-0x8011a000)
for fn,d,name,base,i,s,e,last in all_subs():
    st=int(fn[4],16)
    if st not in tab: continue
    ops,stt = walk(d,s,e,last)
    for (a,op,ln) in ops:
        if op == 0x59:
            n=d[a+2]; sd,o=tab[st]
            c,b,l=struct.unpack_from("<3H",sd,o+6*n) if o+6*n+6<=len(sd) else (-1,-1,-1)
            print(fn,name,"sub%02d"%i,"@0x%05X"%a,"xa %02X"%n,"VOICE0%d ch %d blk %d len %d"%(st,c,b,l))
