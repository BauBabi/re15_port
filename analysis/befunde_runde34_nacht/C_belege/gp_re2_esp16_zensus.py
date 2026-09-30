import os, sys, struct, glob
REPO='C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator'
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
from re2_scd_walk import scd_blocks, block_end, walk
def esp_block(d, offs, k):
    """ESP k data block inside effect.esp (RDT offs[18]); table read from the end (offs[19] = end?)."""
    e0=offs[18]; e1=offs[19]
    # end table: entry k at e1-4-4k (as in FUN_8001bd38 for TIMs); take same for esp data
    rel=struct.unpack_from("<I",d,e1-4*k)[0]
    return e0+rel
def streams_of(d, base):
    nsets,ncells=struct.unpack_from('<HH',d,base)
    sets=[tuple(d[base+8+8*i:base+8+8*i+4]) for i in range(nsets)]
    cells=[tuple(d[base+8+8*nsets+4*i:base+8+8*nsets+4*i+4]) for i in range(ncells)]
    tab=base+8+8*nsets+4*ncells
    first=struct.unpack_from('<H',d,tab)[0]
    n=first*2
    so=[struct.unpack_from('<H',d,tab+2*i)[0] for i in range(n)]
    st=[]
    for i,o in enumerate(so):
        if o==0: st.append(None); continue
        a=tab+4*o
        row0=d[a+8:a+8+24]
        st.append(row0[2])
    return sets,cells,st
tot=0
for p in sorted(glob.glob(REPO+'/info/re2leon/PL0/RDT/ROOM*.RDT')):
    d=open(p,'rb').read()
    offs=struct.unpack_from('<23I',d,8)
    if not offs[18]: continue
    ids=list(d[offs[18]:offs[18]+8])
    if 0x16 not in ids: continue
    k=ids.index(0x16)
    try:
        base=esp_block(d,offs,k)
        sets,cells,st=streams_of(d,base)
    except Exception as ex:
        print(os.path.basename(p),'ERR',ex); continue
    # stream -> first set -> cell
    desc=[]
    for i,s in enumerate(st):
        if s is None: desc.append('%d:-'%i); continue
        c=sets[s][0] if s < len(sets) else None
        desc.append('%d:set%d(cell%s)'%(i,s,c))
    uses=[]
    for name, b, subs in scd_blocks(d):
        for i, so in enumerate(subs):
            s=b+so; e=block_end(d, b, subs, i, offs)
            ops, stt = walk(d, s, e, last=(i+1==len(subs)))
            for (a, op, ln) in ops:
                if op in (0x3A,0x64) and d[a+2]==0x16:
                    u=d[a+3]
                    uses.append('%s@%05X op%02X unter=%02x strom%d clut%d'%(name[:1]+str(i),a,op,u,u&7,u>>3))
    print(os.path.basename(p), 'k=%d'%k, ' '.join(desc))
    for u in uses: print('    ',u)
