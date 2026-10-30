import sys,json,glob,os,struct
TOOL=open('re15_port/tools/scd_dump_room.py',encoding='utf-8').read().replace('\nmain()\n','\n')
ns={}; exec(compile(TOOL,'scd','exec'),ns)
op_size=ns['op_size'];rdt_section_end=ns['rdt_section_end'];section_regions=ns['section_regions'];u32=ns['u32'];s16=ns['s16'];u16=ns['u16']
def scan(p):
    d=open(p,'rb').read(); forced=[];sets=[];cks=set();objs=[]
    ms,ss=u32(d,0x40),u32(d,0x44)
    for sec,tag in ((ms,'main'),(ss,'sub')):
        if sec==0 or sec>=len(d): continue
        se=rdt_section_end(d,sec)
        for (o,e,idx) in section_regions(d,sec,se):
            pc=sec+o; end=sec+e; depth=[]
            lbl=f'{tag}{idx:02d}'
            while pc<end:
                while depth and pc>=depth[-1]: depth.pop()
                op=d[pc]; sz=op_size(d,pc)
                if sz is None or pc+sz>end: break
                if op==0x06: depth.append(pc+4+u16(d,pc+2))
                if op==0x07 and depth: depth[-1]=pc+s16(d,pc+2)
                if op==0x22 and d[pc+1] in (3,4,9):
                    sets.append((d[pc+1],d[pc+2],d[pc+3]))
                    if lbl in ('main00','sub00') and not depth: forced.append((d[pc+1],d[pc+2],d[pc+3],hex(pc)))
                if op==0x21 and d[pc+1] in (3,4,9): cks.add((d[pc+1],d[pc+2]))
                if op==0x2D and lbl in ('main00','sub00') and not depth: objs.append((d[pc+1],d[pc+2],s16(d,pc+10),s16(d,pc+14),s16(d,pc+16),s16(d,pc+18),s16(d,pc+20),hex(pc)))
                pc+=sz
    return forced,sets,cks,objs
for s in range(1,7):
    fs=sorted(glob.glob(f'Version_40%/PSX/STAGE{s}/ROOM*0.RDT'))
    for a in fs:
        b=a[:-5]+'1.RDT'
        if not os.path.exists(b): continue
        n=os.path.basename(a)[4:8]
        fa,sa,ca,oa=scan(a); fb,sb,cb,ob=scan(b)
        ka={(x[0],x[1],x[2]) for x in fa}; kb={(x[0],x[1],x[2]) for x in fb}
        onlya=[x for x in fa if (x[0],x[1],x[2]) not in kb]; onlyb=[x for x in fb if (x[0],x[1],x[2]) not in ka]
        # nur interessant: Flag, das die Gegenseite prueft/setzt
        ia=[x for x in onlya if (x[0],x[1]) in cb or any(y[0]==x[0] and y[1]==x[1] for y in sb)]
        ib=[x for x in onlyb if (x[0],x[1]) in ca or any(y[0]==x[0] and y[1]==x[1] for y in sa)]
        oda=set((o[0],o[2],o[3],o[4],o[5],o[6]) for o in oa); odb=set((o[0],o[2],o[3],o[4],o[5],o[6]) for o in ob)
        if ia or ib: print(n,'nur Leon erzwingt',ia,'| nur Elza erzwingt',ib)
        if oda!=odb and len(oa)==len(ob): print(n,'Objektlage verschieden',sorted(oda-odb)[:4],'vs',sorted(odb-oda)[:4])
