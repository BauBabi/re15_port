# NB4 Messer (K): kommt der Kopf des Alligators auf dem Verfolgungsweg OHNE Zielmodus/RUNDE in Aufnahmereichweite der Liegestelle?
# Bahn = TRAG-Zeilen (gator_trag.log, je Bild Phase/Gier/Wurzel) der Abnahme-6-Laeufe mit RE15_G2_OHNE_KANISTER=1 NACH der Aufnahme:
# dort ist die Verfolgung bitgleich der ohne Kanister (Abnahme 6 Abschnitt 3c, NB2 Riegel (a)), der Kanister liegt aber nicht mehr.
# Aufnahme-Praedikat = Port heute (gator_re2_r41.c gr2_kan_takt Stufe 0, Szenen-Kanister): Phase CHASE (2) oder LURK (1),
#   laengs = (cos*dx - sin*dz)>>12 in [0, (8601+0x18f)*2731>>12) (RE2 @0x80100ad0-ec: obj[1].z - z - 8601 < 0x18f; Port: ab Wurzel),
#   |quer| < hit_radius_min 1466 + 267 (PORT-WAHL Koerperbreite).
import sys,re,math,glob,os
S=2731
def q(v): return (v*S)>>12
LEN=q(8601)+q(0x18f)
QMAX=1466+267
def cos12(a): return int(round(4096*math.cos(a*2*math.pi/4096)))
def sin12(a): return int(round(4096*math.sin(a*2*math.pi/4096)))
rx=re.compile(r'TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)')
rk=re.compile(r'Kanister Stufe 0 pos=\((-?\d+),(-?\d+),(-?\d+)\)')
print('Fenster laengs [0,%d) |quer| < %d'%(LEN,QMAX))
for d in sorted(glob.glob(sys.argv[1]+'/*')):
    t=os.path.join(d,'gator_trag.log'); g=os.path.join(d,'debug.log')
    if not os.path.exists(t) or not os.path.exists(g): continue
    m=None
    for l in open(g,errors='ignore'):
        m=rk.search(l)
        if m: break
    if not m: continue
    cx,cz=int(m.group(1)),int(m.group(3))
    n=0;nch=0;hit=0;eps=[];prev=False;dmin=None
    for l in open(t,errors='ignore'):
        r=rx.search(l)
        if not r: continue
        F,ph,mo,af,gi,x,z,px,pz=map(int,r.groups()); n+=1
        dx,dz=cx-x,cz-z
        c,s=cos12(gi),sin12(gi)
        la=(c*dx-s*dz)>>12; qu=(s*dx+c*dz)>>12
        dd=int(math.hypot(dx,dz)); dmin=dd if dmin is None or dd<dmin else dmin
        ok=ph in (1,2)
        nch+=ok
        h= ok and 0<=la<LEN and -QMAX<qu<QMAX
        if h: hit+=1
        if h and not prev: eps.append(F)
        prev=h
    print('%-34s Kanister (%d,%d) Bilder %5d CHASE %5d  im Fenster %4d  Besuche %2d  erste %s  Wurzel min %d'%(os.path.basename(d),cx,cz,n,nch,hit,len(eps),eps,dmin or -1))
