import os,re,hashlib,sys,glob
WT=r'C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/re15_port'
A=WT+'/shared_assets/PSX'
zones=open(WT+'/engine/src/re15_map_zones.h',encoding='latin-1').read()
zr={}
for m in re.finditer(r'^\s*\{ 0x([0-9A-F]{4}),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),',zones,re.M):
    zr.setdefault(m.group(1),[]).append((int(m.group(6)),int(m.group(7))))
tab={}
for l in open(sys.argv[1]):
    p=l.split()
    if p and re.match(r'^[0-9A-F]{4}$',p[0]): tab[p[0]]=l
hs={}
for st in range(1,7):
    for f in sorted(glob.glob(A+'/STAGE%d/ROOM*.RDT'%st)):
        rid=os.path.basename(f)[4:8]
        b=open(f,'rb').read()
        h=hashlib.md5(b).hexdigest()[:8]
        dup=hs.get(h); hs.setdefault(h,rid)
        print(rid,len(b),h,'dup='+(dup or '-'),'zones='+(';'.join('%d/%d'%z for z in zr.get(rid,[])) or 'KEINE'),'tab='+('ja' if rid in tab else 'NEIN'))
