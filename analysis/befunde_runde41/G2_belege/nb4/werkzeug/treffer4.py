# NB4 Messer (T/W): je Treffer (hp-Flanke, Messhaken RE15_R39_MESS_SPIELER_HP=100) der Port HEUTE (RE1.5-Niederwurf) gegen das RE2-Modell.
# Port heute: Weg = Summe der Bild-Schritte waehrend des Niederwurfs, Luftlinie Treffer -> Ende Niederwurf, Dauer = Bilder mit Spieler-Clip 11..16
# (Niederwurf-Clips 0xb..0x10, game_step_common.c kd_clip) ab dem Treffer.
# RE2-Modell (EM23_OVL_0000.BIN, selbst gelesen): Treffer @0x80101578: Unterzustand = 2 (+5 = 2, `addiu v0,zero,517 / sw` @0x801015b4-c4) bzw. 3
# (FUN_80015910(Gator, Spieler) wahr -> +5 += 1 @0x801015c8-ec): ((pl.gier - gator.gier + 0x400) & 0xfff) < 0x800.
#   Sub 2 (Figur sieht den Gator an): Schritte RUECKWAERTS 800,650,500,350 (+0x144 = -800 @0x801030e0-e4, += 150 @0x8010313c-4c, Blend 3 = 4 Bilder
#   @0x80103108-1c) dann 200,185,...,5 (+= 15 @0x801031a4-c4, Klemme 0); Summe 3735, 20 Bilder (Clip 19 30 Bilder doppelt @0x80103170-98).
#   Sub 3 (Gator hinter der Figur): VORWAERTS 800,680,560,440,320,200,80 (+0x144 = 800 @0x8010325c-60, -= 120 @0x801032b4-d8); Summe 3080;
#   Clip 20 (21) + Clip 17 (16, doppelt) + Basis-Clip 6 rueckwaerts (25) = 54 Bilder.
# Richtung (PORT-Lesart A, s. Messblatt): RE2 dreht die Figur ueber FUN_8001569c(pl, Achse, 1536) (@0x80101684-88) auf die Korridorachse der Gator-
#   Fahrt -> der Stoss laeuft in beiden Faellen laengs der Gator-Blickrichtung (cos g, -sin g). Port: Gator-Gier unrastet (A) bzw. auf 90 Grad gerastet (A90).
# Kante (W): ueber die Kantenlinie = Mittelpunkt jenseits z -18100 / -14500 (Wasser), Waende bei Radius 450. Deck = SCA [5] x 1850..7200 z -18100..-14500 (ROOM2090.RDT @0x730), Absprungleisten [8] z -14500..-13500 @0x754 / [9] z -19100..-18100
#   @0x760 (u1 0x02 = Abwaerts-Bit, climb_common.c @0x8002d74c-50), Waende [6] Ostwand x >= 7200 @0x73c, [7] Plattform x <= 1850 @0x748 (u1 0).
import re,sys,math,os,glob
S2=[800,650,500,350]+list(range(200,0,-15))
S3=[800,680,560,440,320,200,80]
def c12(a): return math.cos(a*2*math.pi/4096)
def s12(a): return math.sin(a*2*math.pi/4096)
def deck_ziel(x,z,g,steps,raste):
    if raste: g=((g+512)&0xC00)&0xfff
    dx,dz=c12(g),-s12(g)
    for i,s in enumerate(steps):
        x+=dx*s; z+=dz*s
        if z < -18100: return 'Wasser Sued (Leiste [9]) nach %d Bildern'%(i+1),(int(x),int(z))
        if z > -14500: return 'Wasser Nord (Leiste [8]) nach %d Bildern'%(i+1),(int(x),int(z))
        if x > 7200-450: return 'Ostwand [6]',(int(x),int(z))
        if x < 1850+450: return 'Plattform [7]',(int(x),int(z))
    return 'bleibt oben',(int(x),int(z))
rs=re.compile(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+) ")
rc=re.compile(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)")
rt=re.compile(r'TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\)')
rr=re.compile(r'RAMMEN F(\d+)')
PH={2:'CHASE',3:'LUNGE',5:'GUARD',6:'CROSS_APPR',7:'CROSS',12:'RE2_DASH'}
tot={}
for run in sys.argv[1:]:
    name=os.path.basename(run.rstrip('/'))
    sl=[l for l in open(run+'/state.log',errors='replace') if l.startswith('F')]
    cl=[l for l in open(run+'/climb.log',errors='replace') if l.startswith('T')]
    # letzter Besuch (Rueckkehr): Bildzaehler faellt
    start=0;prev=None
    for i,l in enumerate(sl):
        f=int(l[1:l.index(' ')])
        if prev is not None and f<prev: start=i
        prev=f
    st=[]
    for i in range(start,len(sl)):
        m=rs.match(sl[i]); c=rc.search(cl[i]) if i<len(cl) else None
        if m and c: st.append((int(m.group(1)),int(m.group(2)),int(m.group(3)),int(m.group(4)),int(m.group(5)),int(m.group(6)),int(c.group(2))))
    trag={}
    if os.path.exists(run+'/gator_trag.log'):
        for l in open(run+'/gator_trag.log',errors='replace'):
            m=rt.match(l)
            if m: trag[int(m.group(1))]=tuple(map(int,m.groups()[1:]))
    ramm=set()
    if os.path.exists(run+'/gator_boss.log'):
        for l in open(run+'/gator_boss.log',errors='replace'):
            m=rr.match(l)
            if m: ramm.add(int(m.group(1)))
    for k in range(1,len(st)):
        if not (st[k][4]<100 and st[k-1][4]>=100): continue
        F=st[k][0]; x0,z0,rot0=st[k-1][1],st[k-1][2],st[k-1][3]; oben=st[k-1][6]<-900
        j=k; ruhe=0; weg=0.0
        while j+1<len(st) and ruhe<10:
            d=math.hypot(st[j+1][1]-st[j][1],st[j+1][2]-st[j][2]); weg+=d; ruhe=ruhe+1 if d==0 else 0; j+=1
        luft=int(math.hypot(st[j][1]-x0,st[j][2]-z0))
        dauer=0; q=k+1
        while q<len(st) and 11<=st[q][5]<=16: dauer+=1; q+=1
        # Weg NUR in den Niederwurf-Bildern (Spieler-Clip 11..16), ohne das Gehen der Eingabe danach
        weg=0.0
        for u in range(k,q):
            if u+1<len(st): weg+=math.hypot(st[u+1][1]-st[u][1],st[u+1][2]-st[u][2])
        luft=int(math.hypot(st[min(q,len(st)-1)][1]-x0,st[min(q,len(st)-1)][2]-z0))
        art='?'; sub='-'; zielA='-'; zielA90='-'
        tg=trag.get(F)
        if tg:
            ph,gmo,gaf,gg,gx,gz=tg
            art=('Rammen' if F in ramm else PH.get(ph,'ph%d'%ph))
            s3=((rot0-gg+0x400)&0xfff)<0x800; sub='3' if s3 else '2'
            if oben:
                zielA='%s %s'%deck_ziel(x0,z0,gg,S3 if s3 else S2,0)
                zielA90='%s %s'%deck_ziel(x0,z0,gg,S3 if s3 else S2,1)
        elif F in ramm: art='Rammen'
        print('%-30s F%-5d %-6s %-10s Lage (%d,%d) Blick %4d | heute Weg %5d Luftlinie %5d Dauer %3d | RE2 Sub %s %s | A: %s | A90: %s'%(
            name,F,'oben' if oben else 'Wasser',art,x0,z0,rot0,int(weg),luft,dauer,sub,
            ('3735/20' if sub=='2' else '3080/54' if sub=='3' else '-'),zielA,zielA90))
