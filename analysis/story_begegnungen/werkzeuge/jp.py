# Zeichentabelle RE1.5 40%-Disc: Raster aus PSX.EXE @0x800289bc/@0x800289ec/@0x80028a44/@0x80028b00 (18 Spalten, 14x14),
# Glyphen abgelesen aus DATA/TEX.TIM (Seite 1 x=256, Seite 2 x=512).
D={}
ascii_rows={0x00:' ',0x16:':',0x17:';',0x18:',',0x19:'"',0x1A:'！',0x1B:'？',0x1C:'!?',0x37:'[',0x38:'/',0x39:']',0x3A:"'",0x3B:'ー',0x3C:'.'}
D.update(ascii_rows)
for i,c in enumerate('0123456789'): D[0x0C+i]=c
for i in range(26): D[0x1D+i]=chr(65+i); D[0x3D+i]=chr(97+i)
kana=('あいう'
 'えおかきくけこさしすせそたちつてとな'
 'にぬねのはひふへほまみむめもやゆよら'
 'りるれろわをんがぎぐげござじずぜぞだ'
 'ぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇ'
 'ぉゃゅょっアイウエオカキクケコサシス'
 'セソタチツテトナニヌネノハヒフヘホマ'
 'ミムメモヤユヨラリルレロワヲンガギグ'
 'ゲゴザジズゼゾダヂヅデドバビブベボパ'
 'ピプペポァィゥェォャュョッヴーー「」')
for i,c in enumerate(kana): D[0x57+i]=c
P9={}
row0='ピプペポァィゥェォャュョッヴーー「」'
for i,c in enumerate(row0): P9[i]=c
r1=['S.','T.','A.','R.','（','）','『','』','゛','゜','・','×','上','右','下','左','悪','安']
for i,c in enumerate(r1): P9[0x12+i]=c
r2='暗穴員運育竜違一炎役押俺奥応回拡界火'
for i,c in enumerate(r2): P9[0x24+i]=c
PA={}
a=('壊薬何外確感関開間館方会怪家完救急機'
   '御寄究記器起況机気危許凶強供給空君具'
   '決血剣研係見形険撃元経験構後号古行合'
   '攻刻光今口効言向降酸剤査残最殺作縮小'
   '進信室宿舎盾書飼写真取指子射除紙状純'
   '仕手私食時出事銃術丈少死使実失持助身'
   '心自重消蛇緒守思者図水前清青赤制生成'
   '石先整性洗全戸跡探切染草捜造装像増存'
   '続族騒大退弾単誰棚台男他待体脱地調置'
   '中知通転定電庭動当踏逃毒倒得特日人入'
   '認任燃年配白敗反破発美備必物譜不風夫'
   '分部普閉並別変放宝本保報味無面迷滅目'
   '戻薬屋奴用鎧要溶様来頼落力硫料理流立'
   '裏令連練路話…ゝ々溜液絡四六角抜所同')
for i,c in enumerate(a): PA[i]=c
PB={}
b=('久階計月告索女習準充社喋傷受冗製届巣'
   '多短対断談治仲丸点堂度内途判品怖墓油'
   '療送枯願玉遇固根好天紫星神情遭滝答録'
   '息二十西東　未誌証番有化学長警資植絵')
for i,c in enumerate(b): PB[i]=c
def decode(d,s,e):
    """liefert Liste (text, ctrl) bis 01"""
    out=[];i=s
    while i<e and i<len(d):
        c=d[i]
        if c==0x01:
            out.append(f'⟦END{d[i+1]:02x}⟧' if i+1<len(d) and d[i+1] else ''); break
        if c==0x02: out.append(f'⟦WAIT{d[i+1]:02x}⟧' if d[i+1] else '⟦▼⟧'); i+=2; continue
        if c==0x03: out.append('⟦YN⟧'); i+=1; continue
        if c==0x04: i+=2; continue
        if c==0x05: i+=2; continue
        if c==0x06: out.append(f'⟦INS{d[i+1]:02x}⟧'); i+=2; continue
        if c==0x07: i+=1; continue
        if c==0x08: out.append(' / '); i+=1; continue
        if c==0x09: out.append(P9.get(d[i+1],f'⟦9:{d[i+1]:02x}⟧')); i+=2; continue
        if c==0x0A: out.append(PA.get(d[i+1],f'⟦A:{d[i+1]:02x}⟧')); i+=2; continue
        if c==0x0B: out.append(PB.get(d[i+1],f'⟦B:{d[i+1]:02x}⟧')); i+=2; continue
        out.append(D.get(c,f'⟦{c:02x}⟧')); i+=1
    return ''.join(out)
