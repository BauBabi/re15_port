# ISO9660-Inventar des Rohabbilds BH1.5-40%.bin (2352 Byte/Sektor, Mode2/Form1 Nutzdaten @24, 2048 B).
# Liefert: alle Dateien (LBA, Groesse), Vergleich gegen Version_40%-Baum, belegte/freie Sektoren, Nicht-Null-Sektoren in freien Bereichen.
import struct,hashlib,os,sys,json
R=r'C:/workspace/git/reAi_v2'
P=R+'/Version_40%/BH1.5-40%.bin'
f=open(P,'rb')
SZ=os.path.getsize(P); NS=SZ//2352
print('Sektoren',NS,'Rest',SZ%2352)
def raw(l):
    f.seek(l*2352); return f.read(2352)
def sec(l):
    r=raw(l)
    # mode byte @15, subheader @16..23
    if r[15]==2:
        form2=r[18]&0x20
        return r[24:24+2048] if not form2 else r[24:24+2324]
    return r[16:16+2048]
pvd=sec(16)
print('PVD',pvd[:6],pvd[8:40].decode('ascii','replace').strip(),pvd[40:72].decode('ascii','replace').strip(),'vol-size',struct.unpack_from('<I',pvd,80)[0],'date',pvd[813:830])
root=pvd[156:156+34]
def recs(lba,size):
    out=[];
    n=(size+2047)//2048
    for i in range(n):
        s=sec(lba+i); p=0
        while p<2048:
            L=s[p]
            if L==0: break
            r=s[p:p+L]
            e=struct.unpack_from('<I',r,2)[0]; sz=struct.unpack_from('<I',r,10)[0]; fl=r[25]; nl=r[32]; nm=r[33:33+nl]
            dt=r[18:25]
            out.append((nm,e,sz,fl,dt))
            p+=L
    return out
files=[];dirs=[]
def walk(lba,size,path):
    dirs.append((path,lba,size))
    for nm,e,sz,fl,dt in recs(lba,size):
        if nm in (b'\0',b'\1'): continue
        name=nm.decode('ascii','replace').split(';')[0]
        if fl&2: walk(e,sz,path+'/'+name)
        else: files.append((path+'/'+name,e,sz,fl,dt))
e=struct.unpack_from('<I',root,2)[0]; s=struct.unpack_from('<I',root,10)[0]
walk(e,s,'')
print('Dateien',len(files),'Verz',len(dirs))
used=bytearray(NS)
for p,e,s,fl,dt in files:
    for i in range(e,e+(s+2047)//2048):
        if i<NS: used[i]=1
for p,e,s in dirs:
    for i in range(e,e+(s+2047)//2048):
        if i<NS: used[i]=2
for i in range(0,18): used[i]=3  # system area + PVD
# Vergleich mit extrahiertem Baum
ok=0;bad=[]
for p,e,s,fl,dt in files:
    lp=R+'/Version_40%'+p
    if not os.path.exists(lp): bad.append(('fehlt',p,s,e)); continue
    d=b''.join(sec(e+i) for i in range((s+2047)//2048))[:s]
    h=open(lp,'rb').read()
    if d!=h: bad.append(('anders',p,s,len(h),e))
    else: ok+=1
print('Baum-Abgleich: gleich',ok,'probleme',bad[:20])
json.dump([(p,e,s) for p,e,s,fl,dt in files],open('iso_files.json','w'))
# Freie Bereiche
free=[];i=0
while i<NS:
    if used[i]==0:
        j=i
        while j<NS and used[j]==0: j+=1
        free.append((i,j)); i=j
    else: i+=1
print('freie Bereiche',len(free),'Sektoren',sum(b-a for a,b in free))
for a,b in free[:60]:
    nz=0; first=None; form=set()
    for l in range(a,b):
        r=raw(l)
        form.add((r[15],r[18]))
        pay=r[24:2072]
        if any(pay): 
            nz+=1
            if first is None: first=l
    print(f'  frei {a}..{b-1} ({b-a} Sek) nicht-null {nz} erster {first} formen {sorted(form)[:6]}')
print('Letzter Dateisektor',max(e+(s+2047)//2048 for p,e,s,fl,dt in files))
print([ (p,e,s) for p,e,s,fl,dt in sorted(files,key=lambda x:x[1])][:8])
print([ (p,e,s) for p,e,s,fl,dt in sorted(files,key=lambda x:x[1])][-8:])
print('Datei-Flags-Variationen',sorted(set(fl for p,e,s,fl,dt in files)))
