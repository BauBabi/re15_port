# NB1 M1: Spieler-Clip-13-Abschnitte (fc 40) im 3091-Block eines RE15_ANIM_TRACE: Umlaeufe, Haltebilder, Abweichung
# von slot == i % 40 (Soll aus dem Original-GDB-Protokoll k2_orig_3091_gdb_lauf2: 0 Abweichungen, 19/12/5/3/10/4 Spruenge).
import sys
rows=[l.split() for l in open(sys.argv[1]) if l.strip()]
bl=[];cur=[];last=-1
for r in rows:
    f=int(r[0])
    if f<last-5: bl.append(cur); cur=[]
    last=f; cur.append(r)
bl.append(cur)
B=[b for b in bl if any(x[1]=='2' and x[2]=='66' for x in b)][0]
sp=[r for r in B if r[1]=='0']
seg=[];cs=None
for r in sp:
    f,clip,fc,cu,sl=int(r[0]),int(r[4]),int(r[5]),int(r[6]),int(r[7])
    if clip==13 and fc==40:
        if cs is None or f!=cs['f1']+1: cs={'f0':f,'f1':f,'slots':[],'cur':[]}; seg.append(cs)
        cs['f1']=f; cs['slots'].append(sl); cs['cur'].append(cu)
    else: cs=None
tot=0
for s in seg:
    sl=s['slots']; wraps=sum(1 for a,b in zip(sl,sl[1:]) if b<a)
    hold=sum(1 for i in range(1,len(sl)) if sl[i]==sl[i-1])
    abw=sum(1 for i,x in enumerate(sl) if x!=i%40)
    tot+=len(sl)
    print("F%d-%d (%d Bilder): Spruenge 39->0 %d, gleiche Folgebilder %d, Abweichung von i%%40 %d, erste %s letzte %s, cur max %d"%(s['f0'],s['f1'],len(sl),wraps,hold,abw,sl[:3],sl[-3:],max(s['cur'])))
print("Summe Clip-13-Bilder",tot)
