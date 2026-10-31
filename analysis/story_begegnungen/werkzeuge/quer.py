import sys,json,re,glob,os,struct
sp=sys.argv[1]
TOOL=open('re15_port/tools/scd_dump_room.py',encoding='utf-8').read().replace('\nmain()\n','\n')
ns={}; exec(compile(TOOL,'scd','exec'),ns)
op_size=ns['op_size'];rdt_section_end=ns['rdt_section_end'];section_regions=ns['section_regions'];u32=ns['u32'];u16=ns['u16']
rows=json.load(open(sp+r'\texte40.json',encoding='utf-8'))
T={}
for r,i,t in rows:
    if i is None: continue
    t=re.sub(r'⟦END..⟧','',t); t=re.sub(r'⟦WAIT..⟧',' ',t); t=t.replace('⟦▼⟧',' ▼ '); t=re.sub(r'(⟦YN⟧).*$',' [J/N]',t); t=re.sub(r' +',' ',t).strip()
    T[(r,i)]=t
used={}
for s in range(1,7):
    for p in sorted(glob.glob(f'Version_40%/PSX/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); n=os.path.basename(p)[4:8]
        ms,ss=u32(d,0x40),u32(d,0x44)
        for sec,tag in ((ms,'main'),(ss,'sub')):
            if sec==0 or sec>=len(d): continue
            se=rdt_section_end(d,sec)
            for (o,e,idx) in section_regions(d,sec,se):
                pc=sec+o; end=sec+e
                while pc<end:
                    op=d[pc]; sz=op_size(d,pc)
                    if sz is None or pc+sz>end: break
                    if op==0x2B: used.setdefault((n,d[pc+1]),set()).add(f'{tag}{idx:02d}')
                    if op==0x2C and d[pc+2]==1: used.setdefault((n,u16(d,pc+(22 if d[pc+3]&0x80 else 14))),set()).add('AOT')   # lange AOT: Nutzlast ab pc+22
                    if op==0x46 and d[pc+2]==1: used.setdefault((n,u16(d,pc+4)),set()).add('AOT')
                    pc+=sz
json.dump({f'{k[0]}:{k[1]}':sorted(v) for k,v in used.items()},open(sp+r'\used.json','w'))
leon=lambda r:r[-1]=='0'; elza=lambda r:r[-1]=='1'
print('Ausgeloeste Zeilen je Datei:',sum(1 for k in used if k in T))
print('--- Leon-Dateien, ausgeloest, mit Elza-Seite im Text:')
for (r,i),u in sorted(used.items()):
    if (r,i) in T and leon(r) and re.search('エルザ|ジョン|シェリー',T[(r,i)]): print(r,i,T[(r,i)])
print('--- Elza-Dateien, ausgeloest, mit Leon-Seite im Text:')
for (r,i),u in sorted(used.items()):
    if (r,i) in T and elza(r) and re.search('レオン|マーヴ|エイダ',T[(r,i)]): print(r,i,T[(r,i)])
print('--- Texte, die in KEINER Datei ausgeloest werden (nach Raumpaar):')
pairs={}
for (r,i),t in T.items():
    pairs.setdefault((r[:3],i),[]).append(r)
for (b,i),rs in sorted(pairs.items()):
    if not any((r,i) in used for r in rs):
        print(b,i,T[(rs[0],i)][:50])
