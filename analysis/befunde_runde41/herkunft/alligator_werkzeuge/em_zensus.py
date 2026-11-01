# Sce_em_set-Zensus Disc + Mod: alle Skripte (main/sub/extra) UND Rohsuche nach op44-Muster im gesamten RDT.
import sys,struct,os,glob,collections,io
sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8',errors='replace')
R=r'C:/workspace/git/reAi_v2'
T=open(R+'/re15_port/tools/scd_dump_room.py',encoding='utf-8').read().replace('\nmain()\n','\n')
ns={}; exec(compile(T,'scd','exec'),ns)
op_size=ns['op_size'];rdt_section_end=ns['rdt_section_end'];section_regions=ns['section_regions'];u16=ns['u16'];u32=ns['u32'];s16=ns['s16']
def census(base):
    per=collections.defaultdict(list)
    allt=collections.Counter()
    for s in range(1,7):
        for p in sorted(glob.glob(f'{base}/STAGE{s}/ROOM*.RDT')):
            d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
            if len(d)<0x60: continue
            ms,ss=u32(d,0x40),u32(d,0x44)
            for sec,tag in ((ms,'main'),(ss,'sub')):
                if sec==0 or sec>=len(d): continue
                se=rdt_section_end(d,sec)
                for (o,e,idx) in section_regions(d,sec,se):
                    pc=sec+o; end=sec+e
                    while pc<end:
                        op=d[pc]; sz=op_size(d,pc)
                        if sz is None or pc+sz>end: break
                        if op==0x44:
                            t=d[pc+2]
                            allt[t]+=1
                            per[t].append((n,tag,idx,d[pc+1],s16(d,pc+8),s16(d,pc+10),s16(d,pc+12),hex(pc)))
                        pc+=sz
    return per,allt
for nm,b in (('DISC',R+'/Version_40%/PSX'),('MOD',R+'/info/Re1.5/PSX')):
    per,allt=census(b)
    print('==',nm,'Typen',sorted((hex(k),v) for k,v in allt.items()))
    for t in (0x22,0x23,0x24,0x25,0x26,0x2D):
        L=per.get(t,[])
        print(f' Typ {t:02X}: {len(L)}x', sorted(set(x[0] for x in L))[:40])
    print(' 0x23:',per.get(0x23,[])[:20])
# Rohsuche: 0x44 ss 23 pp 00.. im ganzen Disc-RDT, Lage ausserhalb bekannter Skripte zaehlt
print('--- Rohsuche Muster 44 xx 23 (Disc RDT)')
for s in range(1,7):
    for p in sorted(glob.glob(f'{R}/Version_40%/PSX/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
        i=0
        ms,ss=u32(d,0x40),u32(d,0x44)
        hits=[]
        while True:
            i=d.find(b'\x44',i)
            if i<0: break
            if i+20<=len(d) and d[i+2]==0x23 and d[i+1]<16 and d[i+3]==0 and d[i+16:i+20]==b'\0\0\0\0' and d[i+5:i+6] in (b'\0',):
                hits.append(hex(i))
            i+=1
        if hits: print(n,hits[:8],'msc',hex(ms),hex(ss))
