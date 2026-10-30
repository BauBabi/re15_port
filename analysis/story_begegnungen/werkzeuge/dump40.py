import sys,struct,os,glob,json
sys.path.insert(0,sys.argv[1]); out=sys.argv[1]
import jp
rows=[]
for s in range(1,7):
    for p in sorted(glob.glob(f'Version_40%/PSX/STAGE{s}/ROOM*.RDT')):
        d=open(p,'rb').read(); name=os.path.basename(p)[4:8]
        ms=struct.unpack_from('<I',d,0x3C)[0]
        if ms==0 or ms>=len(d):
            rows.append((name,None,None)); continue
        first=d[ms]|d[ms+1]<<8; n=first//2
        tbl=[d[ms+2*i]|d[ms+2*i+1]<<8 for i in range(n)]
        for i,o in enumerate(tbl):
            st=ms+o; en=ms+tbl[i+1] if i+1<n else len(d)
            rows.append((name,i,jp.decode(d,st,min(en,st+600))))
txt=[]
cur=None
for r,i,t in rows:
    if r!=cur: txt.append(f'\n=== ROOM{r}'); cur=r
    if i is None: txt.append('  (kein Nachrichtenblock: 0x3C = 0)')
    else: txt.append(f'  {i:2d}: {t}')
open(f'{out}/texte40.txt','w',encoding='utf-8').write('\n'.join(txt))
json.dump(rows,open(f'{out}/texte40.json','w',encoding='utf-8'),ensure_ascii=False)
bad=sum(1 for r in rows if r[2] and '⟦' in r[2].replace('⟦▼⟧','').replace('⟦YN⟧',''))
print('Eintraege',len(rows),'Raeume',len(set(r[0] for r in rows)),'ohne Block',sum(1 for r in rows if r[1] is None),'mit Sondercode',bad)
