# NB4 Messer (T): Bild-fuer-Bild-Nachrechnung des RE2-Spieler-Hooks 0x80102D08 Sub 2 / Sub 3 (EM23_OVL_0000.BIN, mess_re2_em23_treffer_kanister.dis).
# Clip-Laengen: ROOM40A0.RDT Raum-Bank Satz 0 (Leon) Clips 17/18/19/20 = 16/10/30/21 Bilder (mess_rbj40a0.txt), Basis-Clip 6 = 25 (RE2 PL00.PLD, = RE1.5 PL00 Clip 11).
# Vorschub FUN_8001569c-unabhaengig: FUN_800152c8(pl,0) = Lage += RotY(Gier) * (+0x144,0,+0x148) (RE2_Quellcode_V2/FUN_800152c8.c): +0x144 > 0 = vorwaerts.
# Bild-Zaehler FUN_8002959c -> FUN_80029614: +1, Wrap + Rueckgabe 1 bei frame+1 >= Laenge (@0x80029b28-4c); Blend +0x14E -1 je Aufruf, solange != 0 (@0x800299c0-cc).
L={17:16,18:10,19:30,20:21,'B6':25}
def adv(st):
    if st['blend']>0: st['blend']-=1
    st['f']+=1
    if st['f']>=L[st['clip']]: st['f']=0; return 1
    return 0
def sub2():
    st={'r6':0,'clip':None,'f':0,'blend':0,'spd':0,'inv':0}; out=[];weg=0;F=0
    while True:
        z=[]; r=st['r6']
        if r==0:   # 0x801030bc
            st.update(clip=18,f=0,blend=3,r6=1,spd=-800,inv=1); z.append('SE 0x04010001, Clip 18 Blend 3, +0x144=-800, +0x1D3|=0x80'); r=1
        if r==1:   # 0x80103108
            if st['blend']==0: st['r6']=2
            weg+=st['spd']; mv=st['spd']; adv(st); st['spd']+=150; z.append('Schritt %d'%mv)
        elif r==2 or r==3:
            if r==2: st.update(clip=19,f=0,blend=3,r6=3); z.append('Clip 19 Blend 3')
            weg+=st['spd']; mv=st['spd']; d=adv(st); d+=adv(st); st['r6']+=d; st['spd']+=15
            if st['spd']>0: st['spd']=0
            z.append('Schritt %d, Clip 19 Bild %d'%(mv,st['f']))
        elif r==4:
            z.append('Ende: +4 = 1 (Routine 1), +0x1D3 = 0, +0x1C0 &= 0xED (@0x801031c8-dc)'); out.append((F,z,weg)); return out,weg,F+1
        out.append((F,z,weg)); F+=1
def sub3():
    st={'r6':0,'clip':None,'f':0,'blend':0,'spd':0}; out=[];weg=0;F=0
    while True:
        z=[]; r=st['r6']
        if r==0:   # 0x80103238
            st.update(clip=20,f=0,blend=3,r6=1,spd=800); z.append('SE 0x04020001, Clip 20 Blend 3, +0x144=800, +0x1D3|=0x80'); r=1
        if r==1:   # 0x80103284
            weg+=st['spd']; mv=st['spd']; d=adv(st); st['r6']+=d; st['spd']-=120
            if st['spd']<0: st['spd']=0
            z.append('Schritt %d, Clip 20 Bild %d'%(mv,st['f']))
        elif r in (2,4,5):
            if r in (2,4): st.update(clip=17,f=0,blend=3,r6=5); z.append('Clip 17 Blend 3, +0x1D3 = 0 (wieder treffbar)')
            d=adv(st); d+=adv(st)
            if d: st.update(r6=6,clip='B6',f=0,blend=3); z.append('Clip 17 fertig -> Basis-Clip 6 rueckwaerts (0x00030006, a3 0x10400)')
            else: z.append('Clip 17 Bild %d (doppelt)'%st['f'])
        elif r==6:
            d=adv(st)
            if d: z.append('Ende: +4 = 1, +0x1D3 = 0, +0x1C0 &= 0xED (@0x80103358-70)'); out.append((F,z,weg)); return out,weg,F+1
            z.append('Basis-Clip 6 Bild %d'%st['f'])
        out.append((F,z,weg)); F+=1
for name,fn in (('Sub 2 (Figur sieht den Gator an: Stoss rueckwaerts)',sub2),('Sub 3 (Gator hinter der Figur: Stoss vorwaerts)',sub3)):
    out,weg,n=fn()
    print('== %s: %d Bilder, Weg %d'%(name,n,weg))
    for F,z,w in out: print('  Bild %2d  Weg bis hier %5d  %s'%(F,w,'; '.join(z)))
