import sys,struct,os,glob,json,hashlib,re
sys.argv_saved=sys.argv
TOOL=open('re15_port/tools/scd_dump_room.py',encoding='utf-8').read().replace('\nmain()\n','\n')
ns={}
exec(compile(TOOL,'scd','exec'),ns)
op_size=ns['op_size'];rdt_section_end=ns['rdt_section_end'];section_regions=ns['section_regions'];u16=ns['u16'];u32=ns['u32'];s16=ns['s16'];NAMES=ns['NAMES']
EM={0x40:'MARVIN',0x41:'0x41',0x42:'ADA',0x43:'0x43',0x44:'0x44',0x45:'IRONS',0x46:'0x46',0x47:'ELLIOT',0x48:'0x48',0x49:'JOHN',0x4A:'0x4A',0x4B:'SHERRY',0x4C:'0x4C',0x4D:'ANNETTE',0x4E:'0x4E',0x4F:'0x4F'}
def walk(d,start,end):
    """liefert Ereignisliste mit Bedingungs-Stack"""
    pc=start; ev=[]; stack=[]   # stack: (ende_pc, text)
    pending=[]
    while pc<end:
        while stack and pc>=stack[-1][0]: stack.pop()
        op=d[pc]; sz=op_size(d,pc)
        if sz is None or pc+sz>end: ev.append((pc,'ABBRUCH',f'op={op:02x}',list(s[1] for s in stack))); break
        if op==0x06:
            pending=[]; ifend=pc+4+u16(d,pc+2); # sammelt folgende Ck/Cmp
            q=pc+4; conds=[]
            while q<end and d[q] in (0x21,0x23,0x3E,0x51,0x4F,0x5E,0x28):
                o=d[q]
                if o==0x21: conds.append(f'Ck({d[q+1]},{d[q+2]})={d[q+3]}')
                elif o==0x23: conds.append(f'Cmp(idx{d[q+2]} op{d[q+3]} {s16(d,q+4)})')
                else: conds.append(NAMES.get(o,hex(o)))
                q+=op_size(d,q)
            stack.append((ifend,' & '.join(conds) or '?'))
        elif op==0x07:
            t=pc+s16(d,pc+2)
            if stack: stack[-1]=(t,'NOT['+stack[-1][1]+']')
        elif op==0x2B:
            ev.append((pc,'MSG',(d[pc+1],d[pc+2],d[pc+3]),[s[1] for s in stack]))
        elif op==0x44:
            ev.append((pc,'EM',(d[pc+1],d[pc+2],s16(d,pc+4) if 0 else d[pc+3]),[s[1] for s in stack]))
        elif op==0x3B:
            ev.append((pc,'DOOR',d[pc:pc+32].hex(),[s[1] for s in stack]))
        elif op==0x22:
            ev.append((pc,'SET',(d[pc+1],d[pc+2],d[pc+3]),[s[1] for s in stack]))
        elif op==0x3F:
            ev.append((pc,'MOTION',(d[pc+1],d[pc+2],d[pc+3]),[s[1] for s in stack]))
        elif op==0x5A:
            ev.append((pc,'WEAPON',d[pc:pc+6].hex(),[s[1] for s in stack]))
        if op==0x01 and not stack: 
            pass
        pc+=sz
    return ev
res={}
for s in range(1,7):
    for p in sorted(glob.glob(f'Version_40%/PSX/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); name=os.path.basename(p)[4:8]
        ms,ss=u32(d,0x40),u32(d,0x44)
        r={'subs':{}, 'scdhash':''}
        h=hashlib.md5()
        for sec,tag in ((ms,'main'),(ss,'sub')):
            if sec==0 or sec>=len(d): continue
            se=rdt_section_end(d,sec)
            h.update(d[sec:se])
            for (o,e,idx) in section_regions(d,sec,se):
                r['subs'][f'{tag}{idx:02d}']=walk(d,sec+o,sec+e)
        r['scdhash']=h.hexdigest()[:8]
        res[name]=r
json.dump(res,open(sys.argv[1]+'/zensus.json','w'),default=str)
# Uebersicht
pairs={}
for k in res: pairs.setdefault(k[:3],[]).append(k)
same=[];diff=[];single=[]
for b,l in sorted(pairs.items()):
    if len(l)==2:
        (same if res[l[0]]['scdhash']==res[l[1]]['scdhash'] else diff).append(b)
    else: single.append(l[0])
print('SCD identisch Leon/Elza:',len(same),' '.join(same))
print('SCD verschieden:',len(diff),' '.join(diff))
print('nur eine Datei:',' '.join(single))
print()
print('Raum  | Figuren (Sce_em_set NPC-Typen) je Datei')
for k in sorted(res):
    em=[]
    for sub,ev in res[k]['subs'].items():
        for e in ev:
            if e[1]=='EM' and 0x40<=e[2][1]<=0x4F:
                em.append(f"{EM[e[2][1]]}#{e[2][0]}@{sub}"+('' if not e[3] else '{'+' ; '.join(e[3])+'}'))
    if em: print(k,' '.join(em))
