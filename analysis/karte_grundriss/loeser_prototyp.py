# -*- coding: utf-8 -*-
"""GRUNDRISS-LOESER: eine zusammenhaengende Karte je Blatt aus Kollision + Tuergraph.

Guetemass (das, was optimiert wird):
  A) UEBERLAPPUNG  - Anteil der begehbaren Pixel, die zwei Raeumen zugleich gehoeren.
  B) TUERVERSATZ   - Abstand der beiden Enden eines Durchgangs auf dem Blatt.
  C) PASSFORM      - passt alles ins Kartenfeld (132 x 140 px).
Massstab: Median der ausgelieferten Zeilen @0x800768b0 derselben Stage (gemessen).
Freiheitsgrade je Raum: Drehung (0/90/180/270), Spiegelung, Verschiebung.
"""
import sys, statistics, collections, random, math
sys.path.insert(0,r'analysis/karte_audit_0901')
from sonde_lib import *
u16=G['u16']; s16=G['s16']; BASES=G['BASES']; read_rdt=G['read_rdt']; page_of=G['page_of']

def zeile(rid):
    idx=BASES[(rid>>12)-1]+((rid>>4)&0xff); a=0x800768b0+idx*8
    return s16(a),s16(a+2),u16(a+4),u16(a+6)
MED={}
for st in range(1,7):
    xs=[];ys=[]
    for r in sorted(set(k[0] for k in ZONES)):
        if (r>>12)!=st: continue
        o,p,a,b=zeile(r)
        if a>1 and b>1: xs.append(a); ys.append(b)
    if xs: MED[st]=((1<<20)/statistics.median(xs),(1<<20)/statistics.median(ys))

raeume=[]
for line in open('re15_port/include/re15_room_list.h'):
    for tok in line.replace(',',' ').split():
        if tok.startswith('0x') and len(tok)==6:
            try: raeume.append(int(tok,16))
            except ValueError: pass
BASEN=sorted(set(r & 0xFFF0 for r in raeume))
RDTS={}
for b in BASEN:
    got=read_rdt(b) or read_rdt(b+1)
    if got: RDTS[b]=got

# ⛔ SCHABLONEN-BEREICHE AUSSORTIEREN. Zwoelf Kollisions-Bereiche in elf Raeumen sind
# derselbe leere Rahmen 23950x17000 (vier duenne Waende, innen unbegehbar) - eine
# Schablone im RDT, kein Ort. Ohne diesen Filter kommt ROOM10D0 auf eine Bbox von
# 42350x62500 statt 32145x41362 und ueberlagert auf Seite 3 alles.
def _form(zellen_, bb):
    x0,x1,z0,z1=bb
    return frozenset((cx-x0,cz-z0,cw,cd) for (cx,cz,cw,cd) in zellen_
                     if x0<=cx<=x1 and z0<=cz<=z1)
_zonen_of=G['zones_of']
_formen=collections.defaultdict(set)
for _b in RDTS:
    _al=list(set(RDTS[_b][0]))
    for _bb in _zonen_of(_al):
        _f=_form(_al,_bb)
        if _f: _formen[_f].add(_b)
_ECHT={}
for _b in RDTS:
    _al=list(set(RDTS[_b][0]))
    gut=[]
    for _bb in _zonen_of(_al):
        _f=_form(_al,_bb)
        if _f and len(_formen[_f])>=2: continue
        gut.extend(c for c in _al if _bb[0]<=c[0]<=_bb[1] and _bb[2]<=c[1]<=_bb[3])
    _ECHT[_b]=gut or _al

def zellen(rid):
    """Begehbare Zellen: duenne Waende raus, Schablonen raus."""
    alle=_ECHT.get(rid, list(set(RDTS[rid][0])))
    dick=[c for c in alle if min(c[2],c[3])>1000]
    return dick or list(alle)

def tr(wx,wz,k,sp):
    """Drehung k*90 Grad, optional gespiegelt (sp)."""
    if sp: wx=-wx
    if k==0: return ( wx,  wz)
    if k==1: return (-wz,  wx)
    if k==2: return (-wx, -wz)
    return ( wz, -wx)

class Blatt:
    def __init__(self, seite):
        self.seite=seite
        self.zimmer=[b for b in BASEN if page_of(b)==seite and b in RDTS]
        st=self.zimmer[0]>>12 if self.zimmer else 1
        self.ex,self.ey=MED.get(st,(460.0,460.0))
        self.kanten=[]
        for b in self.zimmer:
            for d in RDTS[b][1]:
                if d['rw']==0 and d['rd']==0 or d['dest'] not in self.zimmer: continue
                for e in RDTS[d['dest']][1]:
                    if e['rw']==0 and e['rd']==0 or e['dest']!=b: continue
                    if abs(d['nx']-e['lx'])+abs(d['nz']-e['lz'])>4000: continue
                    if b < d['dest']:
                        self.kanten.append((b,self.anker(b,d),d['dest'],self.anker(d['dest'],e)))
                    break
    def anker(self, rid, d):
        """Der Punkt, an dem die WAND steht - nicht die Mitte des Trigger-Rechtecks.
        Das Trigger-Rechteck liegt INNEN im Raum; seine Mitte ist um die halbe
        Quer-Ausdehnung von der Wand entfernt. Heftet man beide Mittelpunkte aufeinander,
        dringen die Raeume um genau diese Tiefe ineinander (gemessen ROOM1120<->ROOM1130:
        224 von 1079 Pixeln doppelt). Die Aussenseite ist die, auf der KEINE begehbaren
        Zellen mehr liegen - das entscheiden die Daten, nicht eine Annahme."""
        senk = d['rd'] > d['rw']
        quer = (d['rw'] if senk else d['rd']) / 2.0
        cx, cz = d['lx'], d['lz']
        zs = zellen(rid)
        def frei(px, pz):
            for (ax, az, aw, ad) in zs:
                if ax <= px < ax + aw and az <= pz < az + ad: return False
            return True
        if senk:
            a = (cx - quer * 2, cz); b2 = (cx + quer * 2, cz)
            return (cx - quer, cz) if frei(*a) else (cx + quer, cz)
        else:
            a = (cx, cz - quer * 2); b2 = (cx, cz + quer * 2)
            return (cx, cz - quer) if frei(*a) else (cx, cz + quer)

    def pixel(self,b,st):
        ox,oy,k,sp=st
        s=set()
        for (cx,cz,cw,cd) in zellen(b):
            pts=[tr(cx,cz,k,sp),tr(cx+cw,cz,k,sp),tr(cx,cz+cd,k,sp),tr(cx+cw,cz+cd,k,sp)]
            X=[p[0] for p in pts]; Z=[p[1] for p in pts]
            x0=int(ox+min(X)/self.ex); x1=int(ox+max(X)/self.ex)
            y0=int(oy-max(Z)/self.ey); y1=int(oy-min(Z)/self.ey)
            for y in range(y0,y1+1):
                for x in range(x0,x1+1): s.add((x,y))
        return s
    def punkt(self,b,st,wx,wz):
        ox,oy,k,sp=st
        dx,dz=tr(wx,wz,k,sp)
        return (ox+dx/self.ex, oy-dz/self.ey)
    def kosten(self,lage,pix=None):
        pix = pix or {b:self.pixel(b,lage[b]) for b in lage}
        alle=set(); doppelt=0
        for b in lage:
            doppelt += len(alle & pix[b]); alle |= pix[b]
        tuer=0.0; n=0
        for (a,pa,b,pb) in self.kanten:
            if a not in lage or b not in lage: continue
            p1=self.punkt(a,lage[a],*pa); p2=self.punkt(b,lage[b],*pb)
            tuer += math.hypot(p1[0]-p2[0],p1[1]-p2[1]); n+=1
        xs=[p[0] for p in alle]; ys=[p[1] for p in alle]
        br=(max(xs)-min(xs)+1) if xs else 0; ho=(max(ys)-min(ys)+1) if ys else 0
        raus=max(0,br-132)+max(0,ho-140)
        # ⛔ Gewichte so, dass die UEBERLAPPUNG fuehrt. Zuerst stand hier
        # raus*3.0 bei raus bis 152 - die Passform dominierte alles, und die
        # vollstaendige Suche waehlte kompakte, aber ineinander liegende Grundrisse
        # (Seite 3: 15,7 % Ueberlappung statt 11,6 %). Faustregel jetzt:
        # 1 % Ueberlappung wiegt so viel wie 5 px Tuerversatz und 5 px Ueberstand.
        ueb = doppelt/max(1,len(alle))*100.0
        return (ueb, tuer/max(1,n), br, ho, ueb*10.0 + tuer/max(1,n)*2.0 + raus*2.0)
    def start(self):
        if not self.zimmer: return {}
        grad=collections.Counter()
        for (a,_,b,_) in self.kanten: grad[a]+=1; grad[b]+=1
        wurzel=max(self.zimmer,key=lambda b: grad[b])
        lage={wurzel:(0.0,0.0,0,0)}
        q=[wurzel]
        nb=collections.defaultdict(list)
        for (a,pa,b,pb) in self.kanten:
            nb[a].append((b,pa,pb)); nb[b].append((a,pb,pa))
        pix={wurzel:self.pixel(wurzel,lage[wurzel])}
        while q:
            a=q.pop(0)
            for (b,pa,pb) in nb[a]:
                if b in lage: continue
                p=self.punkt(a,lage[a],*pa)
                best=None
                for k in range(4):
                    for sp in (0,1):
                        d=tr(pb[0],pb[1],k,sp)
                        st=(p[0]-d[0]/self.ex, p[1]+d[1]/self.ey, k, sp)
                        # ⛔ EINDRINGTIEFE HERAUSRECHNEN. Der Tueranker heftet zwei
                        # Punkte aufeinander, die BEIDE im Inneren ihres Raums liegen -
                        # die Raeume dringen dadurch entlang der Tuer-Normalen ineinander
                        # (gemessen ROOM1120<->ROOM1130: 241 px, ein Band laengs der
                        # gemeinsamen Wand). Der Raum wird deshalb genau so weit
                        # zurueckgeschoben, bis die Ueberlappung verschwindet - die
                        # kleinste Verschiebung, die das leistet, und LAENGS der Wand
                        # aendert sie nichts.
                        st=self.ausruecken(b,st,pix,k,sp,pb)
                        pb2=self.pixel(b,st)
                        ueb=sum(len(pb2 & v) for v in pix.values())
                        if best is None or ueb<best[0]: best=(ueb,st,pb2)
                lage[b]=best[1]; pix[b]=best[2]; q.append(b)
        return lage
    def ausruecken(self, b, st, pix, k, sp, pb):
        """Schiebt den Raum laengs der Tuer-NORMALEN zurueck, bis er sich mit dem schon
        gesetzten Bestand nicht mehr ueberlagert. Die Normale steht senkrecht auf der
        Wand, in der die Tuer sitzt; laengs der Wand wird nichts veraendert, die Tuer
        bleibt also auf ihrer Linie."""
        if not pix: return st
        ox, oy, kk, ss = st
        # Wandachse der Tuer im gedrehten Rahmen: die Verbindung Tueranker -> Raummitte
        zs = zellen(b)
        mx = sum(c[0] + c[2] / 2.0 for c in zs) / len(zs)
        mz = sum(c[1] + c[3] / 2.0 for c in zs) / len(zs)
        dmx, dmz = tr(mx - pb[0], mz - pb[1], k, sp)
        # Richtung "vom Anker in den Raum" in Kartenpixeln
        vx = dmx / self.ex; vy = -dmz / self.ey
        n = (vx * vx + vy * vy) ** 0.5
        if n < 1e-6: return st
        vx /= n; vy /= n
        beste = st
        for schritt in range(0, 41):
            kand = (ox + vx * schritt * 0.5, oy + vy * schritt * 0.5, kk, ss)
            pb2 = self.pixel(b, kand)
            ueb = sum(len(pb2 & v) for v in pix.values())
            if ueb == 0: return kand
            if schritt == 0: bester_ueb = ueb; beste = kand
            elif ueb < bester_ueb: bester_ueb = ueb; beste = kand
        return beste

    def vollstaendig(self, grenze=6):
        """Alle Lage-Kombinationen durchprobieren. Die VERSCHIEBUNGEN liegen durch die
        Tueranker fest, sobald die Lage (Drehung/Spiegelung) je Raum gewaehlt ist - frei
        sind also nur 8 Posen je Raum. Der greedy Aufbau waehlt je Raum einzeln und in
        der Reihenfolge der Breitensuche; das ist reihenfolgeabhaengig und findet nicht
        das Beste. Bei bis zu `grenze` Raeumen geht die vollstaendige Suche."""
        import itertools
        if not self.zimmer: return {}
        grad=collections.Counter()
        for (a,_,b,_) in self.kanten: grad[a]+=1; grad[b]+=1
        wurzel=max(self.zimmer,key=lambda b: grad[b])
        nb=collections.defaultdict(list)
        for (a,pa,b,pb) in self.kanten:
            nb[a].append((b,pa,pb)); nb[b].append((a,pb,pa))
        # Spannbaum
        ordn=[wurzel]; kante={}; gesehen={wurzel}
        q=[wurzel]
        while q:
            a=q.pop(0)
            for (b,pa,pb) in nb[a]:
                if b in gesehen: continue
                gesehen.add(b); kante[b]=(a,pa,pb); ordn.append(b); q.append(b)
        frei=ordn[1:]
        if len(frei) > grenze: return None
        best=None
        for kombi in itertools.product(range(8), repeat=len(frei)):
            lage={wurzel:(0.0,0.0,0,0)}
            for b,c in zip(frei,kombi):
                k,sp=c%4,c//4
                a,pa,pb=kante[b]
                p=self.punkt(a,lage[a],*pa)
                d=tr(pb[0],pb[1],k,sp)
                lage[b]=(p[0]-d[0]/self.ex, p[1]+d[1]/self.ey, k, sp)
            kk=self.kosten(lage)
            if best is None or kk[4]<best[0][4]: best=(kk,dict(lage))
        return best

    def relax(self, lage, runden=250):
        """Federmodell: Durchgaenge ziehen zusammen, ueberlappende Raeume druecken
        auseinander. Der Tuer-Anker allein reicht nicht - das Trigger-Rechteck sitzt
        INNEN im Raum (halbe Tiefe), beide Mittelpunkte aufeinander zu heften laesst die
        Raeume um genau diese Tiefe ineinander dringen (gemessen ROOM1120<->ROOM1130:
        224 von 1079 Pixeln). Gedreht/gespiegelt wird hier nicht mehr, nur verschoben."""
        import math as _m
        for _ in range(runden):
            schub = {b: [0.0, 0.0] for b in lage}
            pix = {b: self.pixel(b, lage[b]) for b in lage}
            mitte = {}
            for b in lage:
                if not pix[b]: mitte[b] = (0.0, 0.0); continue
                mitte[b] = (sum(p[0] for p in pix[b]) / len(pix[b]),
                            sum(p[1] for p in pix[b]) / len(pix[b]))
            # auseinander, wo sich Flaechen ueberlagern
            for i, a in enumerate(sorted(lage)):
                for b in sorted(lage)[i + 1:]:
                    n = len(pix[a] & pix[b])
                    if n <= 2: continue
                    dx = mitte[b][0] - mitte[a][0]; dy = mitte[b][1] - mitte[a][1]
                    d = _m.hypot(dx, dy) or 1.0
                    kraft = min(2.0, n / 40.0)
                    schub[a][0] -= dx / d * kraft; schub[a][1] -= dy / d * kraft
                    schub[b][0] += dx / d * kraft; schub[b][1] += dy / d * kraft
            # zusammen, wo ein Durchgang ist
            for (a, pa, b, pb) in self.kanten:
                if a not in lage or b not in lage: continue
                p1 = self.punkt(a, lage[a], *pa); p2 = self.punkt(b, lage[b], *pb)
                dx = p2[0] - p1[0]; dy = p2[1] - p1[1]
                schub[a][0] += dx * 0.30; schub[a][1] += dy * 0.30
                schub[b][0] -= dx * 0.30; schub[b][1] -= dy * 0.30
            beweg = 0.0
            for b in lage:
                ox, oy, k, sp = lage[b]
                lage[b] = (ox + schub[b][0] * 0.5, oy + schub[b][1] * 0.5, k, sp)
                beweg += abs(schub[b][0]) + abs(schub[b][1])
            if beweg < 0.5: break
        return lage

    def zusammen(self, lage, runden=300):
        """Zieht die Tueren zusammen, ohne je eine Ueberlappung zuzulassen.
        Der Aufbau rueckt jeden Raum so weit aus, bis nichts mehr doppelt liegt - das ist
        die kleinste Verschiebung, die das leistet, aber die Tueren stehen danach
        auseinander. Hier wird jeder Raum schrittweise zurueckgezogen, solange KEINE
        Ueberlappung entsteht. Harte Schranke, kein Gewicht: Ueberlappung ist verboten,
        der Tuerversatz wird nur so klein wie moeglich."""
        import math as _m
        pix = {b: self.pixel(b, lage[b]) for b in lage}
        for _ in range(runden):
            bewegt = False
            for (a, pa, b, pb) in self.kanten:
                if a not in lage or b not in lage: continue
                p1 = self.punkt(a, lage[a], *pa); p2 = self.punkt(b, lage[b], *pb)
                dx = p1[0] - p2[0]; dy = p1[1] - p2[1]
                d = _m.hypot(dx, dy)
                if d < 0.6: continue
                for wer, vz in ((b, 1.0), (a, -1.0)):
                    ox, oy, k, sp = lage[wer]
                    schritt = min(1.0, d) * 0.5 * vz
                    kand = (ox + dx / d * schritt, oy + dy / d * schritt, k, sp)
                    pk = self.pixel(wer, kand)
                    if any(len(pk & pix[o]) > 0 for o in lage if o != wer): continue
                    lage[wer] = kand; pix[wer] = pk; bewegt = True
                    break
            if not bewegt: break
        return lage

    def einpassen(self, lage, feld=(100, 55, 132, 140)):
        """Den fertigen Grundriss als GANZES ins Kartenfeld skalieren.
        Ein Stockwerk misst im ausgelieferten Massstab bis zu 203 x 299 px, das Feld ist
        132 x 140 - das Gebaeude passt so nicht auf ein Blatt. Genau deshalb hat der
        Kuenstler verkleinert. Skaliert wird GLEICHMAESSIG und um den Schwerpunkt, damit
        die relative Lage - und damit die Nachbarschaften - erhalten bleibt."""
        pix = {b: self.pixel(b, lage[b]) for b in lage}
        alle = set().union(*pix.values()) if pix else set()
        if not alle: return lage, 1.0
        xs = [p[0] for p in alle]; ys = [p[1] for p in alle]
        br = max(xs) - min(xs) + 1; ho = max(ys) - min(ys) + 1
        f = min(1.0, feld[2] / float(br), feld[3] / float(ho))
        self.ex /= f; self.ey /= f
        neu = {}
        for b, (ox, oy, k, sp) in lage.items():
            neu[b] = (feld[0] + (ox - min(xs)) * f, feld[1] + (oy - min(ys)) * f, k, sp)
        return neu, f

    def feile(self,lage,runden=400,seed=1):
        rng=random.Random(seed)
        pix={b:self.pixel(b,lage[b]) for b in lage}
        beste=self.kosten(lage,pix); bl=dict(lage)
        for _ in range(runden):
            b=rng.choice(list(lage))
            alt=lage[b]
            art=rng.random()
            if art<0.5:
                neu=(alt[0]+rng.choice([-2,-1,1,2]), alt[1]+rng.choice([-2,-1,1,2]), alt[2], alt[3])
            elif art<0.8:
                neu=(alt[0],alt[1],(alt[2]+rng.choice([1,2,3]))%4,alt[3])
            else:
                neu=(alt[0],alt[1],alt[2],1-alt[3])
            lage[b]=neu; alt_pix=pix[b]; pix[b]=self.pixel(b,neu)
            k=self.kosten(lage,pix)
            if k[4] < beste[4]: beste=k; bl=dict(lage)
            else: lage[b]=alt; pix[b]=alt_pix
        return bl,beste

for seite in (4,3,2):
    B=Blatt(seite)
    if not B.zimmer: continue
    l0=B.start()
    k0=B.kosten(l0)
    lz=B.zusammen(dict(l0))
    lf,f=B.einpassen(lz); kz=B.kosten(lf)
    print('Seite %d: %d Raeume, %d Durchgaenge' % (seite,len(B.zimmer),len(B.kanten)))
    print('   nach Aufbau:     Ueberlappung %5.1f %%, Tuerversatz %5.1f px, Ausdehnung %dx%d'
          % (k0[0],k0[1],k0[2],k0[3]))
    print('   eingepasst (%.2fx): Ueberlappung %5.1f %%, Ausdehnung %dx%d  (Feld 132x140)'
          % (f,kz[0],kz[2],kz[3]))
    if seite==4:
        pix={b:B.pixel(b,lf[b]) for b in lf}
        W,H=140,150; X0,Y0=95,50
        g=[[' ']*W for _ in range(H)]
        for idx,b in enumerate(sorted(pix)):
            ch='#$%&@'[idx%5]
            for (x,y) in pix[b]:
                if X0<=x<X0+W and Y0<=y<Y0+H: g[y-Y0][x-X0]=ch
        print('   3F-Grundriss (je Raum ein Zeichen):')
        for j in range(0,H,3):
            zl=''.join(g[j][i] for i in range(0,W,1))
            if zl.strip(): print('   %4d %s' % (Y0+j,zl))
