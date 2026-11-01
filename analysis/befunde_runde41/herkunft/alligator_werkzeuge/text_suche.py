import sys,io,re,os,json,struct,glob
sys.stdout=io.TextIOWrapper(sys.stdout.buffer,encoding='utf-8',errors='replace')
R=r'C:/workspace/git/reAi_v2'
sys.path.insert(0,R+'/analysis/story_begegnungen/werkzeuge')
import jp
# Kodierung Klartext -> Disc-Bytes (Tabellen aus jp.py)
inv={}
for k,v in jp.D.items(): inv.setdefault(v,bytes([k]))
for k,v in jp.P9.items(): inv.setdefault(v,bytes([9,k]))
for k,v in jp.PA.items(): inv.setdefault(v,bytes([0xA,k]))
for k,v in jp.PB.items(): inv.setdefault(v,bytes([0xB,k]))
def enc(s):
    o=b''
    for c in s:
        if c not in inv: return None
        o+=inv[c]
    return o
W=['ワニ','わに','アリゲーター','クロコダイル','ALLIGATOR','Alligator','GATOR','wani','WANI','CROCO','クロコ','アリゲ','水路','水槽','排水','下水','怪物','巨大','化物','爬虫','爬','ポンプ','貯水','水位']
files=[]
for r,d,f in os.walk(R+'/Version_40%'):
    for x in f:
        if x.endswith('.bin') or x=='ZNULL.DAT' or x.endswith('.cue'): continue
        files.append(os.path.join(r,x))
big={}
for p in files:
    big[p]=open(p,'rb').read()
print('Dateien',len(files))
# 1) Disc-Kodierung
for w in W:
    e=enc(w)
    if e is None:
        print(f'{w}: nicht kodierbar (Zeichen fehlt in der Disc-Schrift)',[c for c in w if c not in inv]); continue
    hits=[]
    for p,d in big.items():
        i=d.find(e)
        n=0
        while i>=0:
            hits.append((os.path.basename(p),hex(i))); n+=1
            i=d.find(e,i+1)
            if n>4: break
    print(w,e.hex(),len(hits),hits[:8])
# 2) ASCII + Shift-JIS roh
pats=[b'GATOR',b'gator',b'Gator',b'ALLIG',b'allig',b'Allig',b'WANI',b'wani',b'CROC',b'croc',b'EM023',b'em023',b'EM23',b'em23',b'EM_23',b'\x83\x8f\x83\x6a',b'\x82\xed\x82\xc9',b'\xe9\xb0']
for t in pats:
    hits=[]
    for p,d in big.items():
        if p.endswith(('.BSS','.STR','.VB','.DAT','.BGM','.TIM','.PIX')): continue
        i=d.find(t)
        if i>=0: hits.append((os.path.basename(p),hex(i),d[max(0,i-16):i+24]))
    print(t,len(hits),hits[:5])
