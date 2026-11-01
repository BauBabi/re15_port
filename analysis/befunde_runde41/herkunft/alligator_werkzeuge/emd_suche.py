# Sucht in ALLEN Dateien des Abbilds (ohne ZNULL) nach EMD-Verzeichnis-Mustern und TIM-Koepfen, 4-Byte-ausgerichtet.
# EMD-Kopf: u32 dir_off, u32 n; Verzeichnis @dir_off: n u32 steigend (emd_ansichtsblatt.py emd_dir).
import struct,os,json,sys,collections
import numpy as np
R=r'C:/workspace/git/reAi_v2'
F=json.load(open('iso_files.json'))
def lade(p):
    lp=R+'/Version_40%'+p
    if os.path.exists(lp): return open(lp,'rb').read()
    return None
res=[];tims=[]
slack=[]
f=open(R+'/Version_40%/BH1.5-40%.bin','rb')
for p,e,s in F:
    if p.endswith('ZNULL.DAT'): continue
    d=lade(p)
    if d is None: continue
    # Datei-Schlupf bis Sektorende
    rest=(-s)%2048
    if rest:
        l=e+(s+2047)//2048-1
        f.seek(l*2352+24+(2048-rest)); b=f.read(rest)
        if any(b): slack.append((p,rest,sum(1 for x in b if x)))
    a=np.frombuffer(d[:len(d)&~3],dtype='<u4')
    n=len(a)
    # EMD: a[i]=dir_off, a[i+1]=n (3..40); dir_off vielfach 4
    idx=np.where((a[:-1]>=0x20)&(a[:-1]<0x100000)&((a[:-1]&3)==0)&(a[1:]>=3)&(a[1:]<=40))[0]
    for i in idx:
        off=i*4; dir_off=int(a[i]); cnt=int(a[i+1])
        if off+dir_off+cnt*4>len(d): continue
        en=struct.unpack_from('<%dI'%cnt,d,off+dir_off)
        if en[0]<8 or en[0]>0x400: continue
        if any(en[k]<=en[k-1] for k in range(1,cnt)): continue
        if en[-1]>dir_off: continue
        res.append((p,off,dir_off,cnt,en[0],en[1] if cnt>1 else 0,en[-1]))
    # TIM: 0x00000010, flag
    idx=np.where((a[:-1]==0x10)&(np.isin(a[1:],[0,1,2,3,8,9])))[0]
    for i in idx:
        off=i*4; fl=int(a[i+1]); o=off+8
        ok=True; cl=None
        try:
            if fl&8:
                bl,cx,cy,cw,ch=struct.unpack_from('<I4H',d,o)
                if bl!=12+cw*ch*2 or cw==0 or ch==0 or cx>=1024 or cy>=512: ok=False
                cl=(cx,cy,cw,ch)
                o+=bl
            bl,ix,iy,iw,ih=struct.unpack_from('<I4H',d,o)
            if bl!=12+iw*ih*2 or iw==0 or ih==0 or ix>=1024 or iy>=512: ok=False
        except struct.error:
            ok=False
        if ok: tims.append((p,off,fl,cl,(ix,iy,iw,ih)))
print('EMD-Muster',len(res))
c=collections.Counter(os.path.dirname(r[0]) for r in res); print(c)
for r in res:
    if 'EMD' in r[0] or 'PLD' in r[0] and False: pass
print('Treffer ausserhalb EMS/PLD/PLW:')
for r in res:
    if not r[0].endswith(('.EMS','.PLD','.PLW')):
        print(' ',r)
print('EMS-Treffer je Datei',collections.Counter(r[0] for r in res if r[0].endswith('.EMS')))
print('TIM',len(tims),collections.Counter(os.path.splitext(t[0])[1] for t in tims))
print('Schlupf nicht null',len(slack),slack[:20])
json.dump(res,open("emd_muster.json","w"),default=int); json.dump(tims,open("tim_liste.json","w"),default=int)
