import subprocess,glob,os,re,sys,collections
R='C:/workspace/git/reAi_v2'
res=collections.defaultdict(list)
for tag,base in (('DISC',R+'/Version_40%/PSX'),('MOD',R+'/info/Re1.5/PSX')):
    for f in sorted(glob.glob(base+'/STAGE*/ROOM*.RDT')):
        out=subprocess.run([sys.executable,R+'/re15_port/tools/scd_dump_room.py',f],capture_output=True,text=True).stdout
        for l in out.splitlines():
            if 'Sce_em_set' in l:
                raw=l.split('Sce_em_set')[1].split()
                b=[int(x,16) for x in raw if re.fullmatch('[0-9a-f]{2}',x)]
                res[tag].append((os.path.basename(f),b))
for tag in res:
    c=collections.Counter(b[2] for _,b in res[tag] if len(b)>3)
    print(tag,'Sce_em_set gesamt',len(res[tag]),'Typen(Byte3):',{hex(k):v for k,v in sorted(c.items())})
    for f,b in res[tag]:
        if len(b)>3 and b[2] in (0x22,0x23): print('  ',f,' '.join('%02x'%x for x in b))
print('Beispiel',res['DISC'][:3])
print('--- STAGE2-Raeume, Typen je Datei')
for tag in res:
    per=collections.defaultdict(collections.Counter)
    for f,b in res[tag]:
        if f.startswith('ROOM2'): per[f][hex(b[2])]+=1
    for f in sorted(per): print(tag,f,dict(per[f]))
for tag in res:
    for f,b in res[tag]:
        if f.startswith(('ROOM205','ROOM209')): print(tag,f,' '.join('%02x'%x for x in b))
