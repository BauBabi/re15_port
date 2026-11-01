# EMS-Inventar: jede Datei sektorweise abgehen, Block-Grenzen nach EMD-Regel (u32 dir_off @0, Laenge dir_off+36),
# Abgleich gegen TOC der jeweiligen EXE. Ausgabe je Block: Sektor, Laenge, TOC-Typ(en), md5.
import struct,hashlib,sys
R=r'C:/workspace/git/reAi_v2'
def toc(exe,a):
    d=open(exe,'rb').read(); o=a-0x80010000+0x800
    return [struct.unpack_from('<II',d,o+8*i) for i in range(64)]
def walk(d):
    cur=0; out=[]
    while cur+4<=len(d):
        dir_off=struct.unpack_from('<I',d,cur)[0]
        if dir_off<36 or dir_off&3 or cur+dir_off+36>len(d):
            out.append(('BRUCH',cur,dir_off)); break
        ln=dir_off+36
        out.append(('B',cur,ln))
        nxt=(cur+ln+2047)&~2047
        cur=nxt
        sk=0
        while cur+4<=len(d) and struct.unpack_from('<I',d,cur)[0]==0:
            cur+=4; sk+=4
        if sk: cur=(cur+2047)&~2047
    return out
for v,base in (('DISC',R+'/Version_40%'),('MOD',R+'/info/Re1.5')):
    exe=base+'/PSX.EXE'
    for nm,ta in (('CDEMD0',0x80072f38),('CDEMD1',0x80073178)):
        d=open(f'{base}/PSX/EMD/{nm}.EMS','rb').read()
        T=toc(exe,ta)
        bysec={}
        for i,(s,l) in enumerate(T):
            if l: bysec.setdefault(s,[]).append((0x10+i,l))
        print(f'== {v} {nm}.EMS Groesse {len(d)} ({len(d)/2048:.0f} Sektoren), TOC-Eintraege {sum(1 for s,l in T if l)}')
        for k,(t,c,l) in enumerate(walk(d)):
            if t=='BRUCH':
                print(f'  BRUCH @ {c:#x} sek {c/2048} wert {l:#x}'); continue
            tag=','.join(f'{tt:02X}' for tt,ll in bysec.get(c//2048,[]))
            print(f'  {k:2d} sek {c//2048:5d} len {l:7d} typ [{tag or "-- OHNE TOC --"}] md5 {hashlib.md5(d[c:c+l]).hexdigest()[:8]}')
        # raw TOC incl. 0-len
        print('  roh 0x22:',T[0x12],' 0x23:',T[0x13],' ende:',max(s*2048+l for s,l in T))
