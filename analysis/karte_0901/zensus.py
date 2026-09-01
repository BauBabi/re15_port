# -*- coding: utf-8 -*-
"""B: DOPPELTE TUEREN - vollstaendiger Zensus gegen den eingefrorenen Schnappschuss."""
import sys, os, re, math, pickle, collections, statistics
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S

OUT = []
def P(*a):
    s = ' '.join(str(x) for x in a)
    print(s); OUT.append(s)

# ================== NACHBAU DES GENERATORS (Durchgang 1-3) ==================
zid_of = {k: z['zid'] for k, z in ZONES.items()}
etagen_der_zone = {}
for (fr, fz, fb, fpg, frr) in FLOORS: etagen_der_zone.setdefault((fr, fz), []).append((fb, fpg, frr))
def blatt_fuer_band(room, zi, band):
    e = etagen_der_zone.get((room, zi))
    if not e or len(e) < 2: return None
    best = bestd = None
    for (fb, fpg, frr) in e:
        d = abs(fb - band)
        if bestd is None or d < bestd: best, bestd = (fpg, frr), d
    return best

vor = []          # ALLE Tuer-Datensaetze, auch die ausgefallenen (mit Grund in 'aus')
for b in BASES:
    for n, m in enumerate(doors_of(b)):
        rec = dict(room=b, idx=n, d=m, zi=None, pg=None, r=None, aus=None)
        if not any(k[0] == b for k in ZONES):
            rec['aus'] = 'raum_ohne_zone'; vor.append(rec); continue
        zi = zone_at(b, m['lx'], m['lz']); rec['zi'] = zi
        if zi is None:
            rec['aus'] = 'punkt_ausserhalb_aller_zonen'; vor.append(rec); continue
        mp = to_map(b, zi, m['lx'], m['lz'])
        if mp is None:
            rec['aus'] = 'keine_projektion'; vor.append(rec); continue
        pg, r, mx, my = mp
        bl = blatt_fuer_band(b, zi, m.get('band', 0))
        if bl and bl != (pg, r):
            R0 = rects(pg)[r]; R1 = rects(bl[0])[bl[1]]
            mx += R1[0] - R0[0]; my += R1[1] - R0[1]; pg, r = bl
        if m['rw'] != m['rd']: senk = m['rd'] > m['rw']
        else:
            R = rects(pg)[r]
            senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) < min(my - R[1], R[1] + R[3] - 1 - my))
        mx, my, seite = snap_wall(pg, r, mx, my, senk)
        rec.update(pg=pg, r=r, mx=mx, my=my, seite=seite, zid=zid_of.get((b, zi), 0))
        if kachel_zeigt_tuer(pg, r, mx, my): rec['aus'] = 'kachel_malt_schon'
        vor.append(rec)
lebt = lambda v: v['aus'] is None
hat  = lambda v: v['pg'] is not None

# ---------- Paare (Generator-Regel: reziprokes dest + Positionstoleranz 4000) ----------
kand = []
for i in range(len(vor)):
    A = vor[i]
    for j in range(i + 1, len(vor)):
        B = vor[j]
        if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
        d1 = abs(A['d']['nx'] - B['d']['lx']) + abs(A['d']['nz'] - B['d']['lz'])
        d2 = abs(B['d']['nx'] - A['d']['lx']) + abs(B['d']['nz'] - A['d']['lz'])
        kand.append((d1 + d2, i, j, d1, d2))
paare = []; bel = set()
for s, i, j, d1, d2 in sorted(kand):
    if i in bel or j in bel: continue
    bel.add(i); bel.add(j)
    paare.append(dict(i=i, j=j, d1=d1, d2=d2, tol=(d1 <= 4000 and d2 <= 4000)))
unpaar = [k for k in range(len(vor)) if k not in bel]

P("=" * 78)
P("TEIL 1 - ZENSUS ALLER DURCHGANGS-PAARE")
P("=" * 78)
P("Basis-Raeume in re15_room_list.h ........ %d" % len(BASES))
P("  davon mit RDT ......................... %d" % len(RDT))
P("  davon mit Karten-Zone im Header ....... %d" % len(set(k[0] for k in ZONES)))
P("Tuer-Datensaetze (Aot_set 0x3b/0x68, ohne 0x0-Warps, ohne sce==0) ... %d" % len(vor))
c = collections.Counter(v['aus'] for v in vor)
for k, n in c.most_common():
    P("   %-34s %3d" % (('ERZEUGT MARKE' if k is None else 'faellt aus: ' + k), n))
P("")
P("Durchgangs-PAARE (A.dest==B.room und B.dest==A.room, greedy nach Weltabstand) %d" % len(paare))
P("   davon in der Generator-Toleranz (<=4000 Welteinheiten je Richtung) . %d" % sum(1 for p in paare if p['tol']))
P("   Toleranz gerissen ................................................. %d" % sum(1 for p in paare if not p['tol']))
P("Ungepaarte Datensaetze ................................................ %d" % len(unpaar))
g = collections.Counter()
for k in unpaar:
    A = vor[k]; de = A['d']['dest']
    if de == A['room']: g['SELBST-Tuer (dest == eigener Raum), ohne Partner'] += 1
    elif de not in RDT: g['Zielraum hat kein RDT / nicht in der Raumliste'] += 1
    elif not [d for d in doors_of(de) if d['dest'] == A['room']]:
        g['Zielraum fuehrt keinen Rueckweg-Datensatz'] += 1
    else: g['Rueckweg vorhanden, aber greedy schon vergeben'] += 1
for k, n in g.most_common(): P("      %-52s %3d" % (k, n))

# ---------- Verschmelzung ----------
Z = []
for p in paare:
    A, B = vor[p['i']], vor[p['j']]
    e = dict(A=A, B=B, **p)
    e['beide_rect'] = hat(A) and hat(B)
    e['gleiche_seite'] = e['beide_rect'] and A['pg'] == B['pg']
    e['dpx'] = math.hypot(A['mx'] - B['mx'], A['my'] - B['my']) if e['gleiche_seite'] else None
    e['verschmolzen'] = lebt(A) and lebt(B) and e['gleiche_seite'] and p['tol']
    e['marken'] = int(lebt(A)) + int(lebt(B)) - (1 if e['verschmolzen'] else 0)
    Z.append(e)
P("")
P("Wo liegen die beiden Enden eines Paares?")
P("   beide Enden auf einem Karten-Rechteck ............ %d" % sum(1 for e in Z if e['beide_rect']))
P("      davon DASSELBE Blatt .......................... %d" % sum(1 for e in Z if e['gleiche_seite']))
P("      davon VERSCHIEDENE Blaetter ................... %d" % sum(1 for e in Z if e['beide_rect'] and not e['gleiche_seite']))
P("   nur ein Ende ueberhaupt gezeichnet ............... %d" % sum(1 for e in Z if hat(e['A']) != hat(e['B'])))
P("   kein Ende gezeichnet ............................. %d" % sum(1 for e in Z if not hat(e['A']) and not hat(e['B'])))
P("")
P("Was macht Durchgang 2 daraus?")
P("   VERSCHMOLZEN (eine Marke, zid2 gesetzt) .......... %d" % sum(1 for e in Z if e['verschmolzen']))
P("   Paar erzeugt 2 Port-Marken ....................... %d" % sum(1 for e in Z if e['marken'] == 2))
P("   Paar erzeugt 1 Port-Marke ........................ %d" % sum(1 for e in Z if e['marken'] == 1))
P("   Paar erzeugt 0 Port-Marken ....................... %d" % sum(1 for e in Z if e['marken'] == 0))
P("   Summe Port-Marken aus Paaren ..................... %d" % sum(e['marken'] for e in Z))
P("   Port-Marken aus ungepaarten Datensaetzen ......... %d" % sum(1 for k in unpaar if lebt(vor[k])))
P("   zid2-Eintraege im Header (Gegenprobe) ............ %d" % sum(1 for m in MARKS if m[6] != 255))

P("")
P("=" * 78)
P("TEIL 2 - WARUM WURDE EIN PAAR NICHT VERSCHMOLZEN?")
P("=" * 78)
g2 = collections.Counter()
for e in Z:
    if e['verschmolzen']: continue
    A, B = e['A'], e['B']
    if not lebt(A) and not lebt(B):
        g2['beide Enden schon in Durchgang 1 ausgefallen (%s / %s)' % (A['aus'], B['aus'])] += 1
    elif lebt(A) != lebt(B):
        g2['ein Ende faellt in Durchgang 1 aus: ' + (B if lebt(A) else A)['aus']] += 1
    elif not e['gleiche_seite']: g2['beide leben, aber VERSCHIEDENE BLAETTER (continue)'] += 1
    elif not e['tol']: g2['beide leben, aber Positionstoleranz 4000 gerissen'] += 1
    else: g2['??'] += 1
for k, n in sorted(g2.items(), key=lambda t: -t[1]): P("   %-62s %3d" % (k, n))

P("")
P("=" * 78)
P("TEIL 3 - GEGENTHESE: LIEGEN DIE ENDEN UEBERHAUPT AN DERSELBEN STELLE?")
P("=" * 78)
ds = sorted(e['dpx'] for e in Z if e['dpx'] is not None)
P("Paare mit beiden Enden auf DEMSELBEN Blatt: %d" % len(ds))
P("Pixel-Abstand der beiden projizierten Tuerpositionen:")
P("   Median %5.1f px   Mittel %5.1f px   Min %4.1f   Max %5.1f" %
  (statistics.median(ds), sum(ds) / len(ds), ds[0], ds[-1]))
for q in (10, 25, 50, 75, 90):
    P("   p%-2d = %5.1f px" % (q, ds[int(q / 100.0 * (len(ds) - 1))]))
P("   <=2 px: %d    <=5 px: %d    <=10 px: %d    >20 px: %d" %
  (sum(1 for d in ds if d <= 2), sum(1 for d in ds if d <= 5),
   sum(1 for d in ds if d <= 10), sum(1 for d in ds if d > 20)))
P("Die zehn schlimmsten:")
for e in sorted([e for e in Z if e['dpx'] is not None], key=lambda e: -e['dpx'])[:10]:
    A, B = e['A'], e['B']
    P("   %04X z%d S%dr%-2d (%3d,%3d)  <->  %04X z%d S%dr%-2d (%3d,%3d)   %6.1f px" %
      (A['room'], A['zi'], A['pg'], A['r'], A['mx'], A['my'],
       B['room'], B['zi'], B['pg'], B['r'], B['mx'], B['my'], e['dpx']))

P("")
P("=" * 78)
P("TEIL 4 - WAS MALT DAS ORIGINAL? (DATA/MAP0x.PIX)")
P("=" * 78)
P("Symbolkatalog analysis/kartensymbole/symbolkatalog.csv, Typ TUER + TUER2:")
P("   Symbole gesamt ......... %d  auf %d Rechtecken" % (len(SYM), len(SYM_RECT)))
cnt = collections.Counter(len(v) for v in SYM_RECT.values())
P("   Symbole je Rechteck .... " + ', '.join("%dx %d Symbol(e)" % (n, k) for k, n in sorted(cnt.items())))
P("")
P("(4a) Zwei Stempel fuer EINEN Durchgang muessten beidseits derselben Wandlinie")
P("     liegen: 4..10 px auseinander, mit gegenueberliegender Wandseite.")
GEG = {'N': 'S', 'S': 'N', 'E': 'W', 'W': 'E'}
for tol in (4, 6, 8, 10, 14, 20):
    n = ng = 0
    for i in range(len(SYM)):
        for j in range(i + 1, len(SYM)):
            a, b = SYM[i], SYM[j]
            if a['pg'] != b['pg']: continue
            if math.hypot(a['x'] - b['x'], a['y'] - b['y']) > tol: continue
            n += 1; ng += (GEG.get(a['wand']) == b['wand'])
    P("     d<=%2d px: %2d Symbolpaare auf einer Seite, davon gegenueberliegende Wand: %d" % (tol, n, ng))
P("")
P("(4b) Fuer jeden Durchgang mit beiden Enden auf einem Rechteck: wie viele GEMALTE")
P("     Symbole trifft er? (tol 4 px + halbe Symbolgroesse, Symbol irgendwo auf der Seite)")
cc = collections.Counter(); doppel = []
for e in Z:
    if not e['beide_rect']: continue
    sa = sym_treffer(SYM_PAGE[e['A']['pg']], e['A']['mx'], e['A']['my'])
    sb = sym_treffer(SYM_PAGE[e['B']['pg']], e['B']['mx'], e['B']['my'])
    if sa and sb:
        gleich = (sa['x'], sa['y']) == (sb['x'], sb['y'])
        cc['zwei Enden -> DASSELBE Symbol' if gleich else 'zwei Enden -> zwei Symbole'] += 1
        doppel.append((e, sa, sb, gleich))
    elif sa or sb: cc['genau ein Ende trifft ein Symbol'] += 1
    else: cc['kein Ende trifft ein Symbol'] += 1
for k, n in sorted(cc.items(), key=lambda t: -t[1]): P("     %-38s %3d" % (k, n))
P("")
P("     Die Faelle 'zwei Symbole' im Detail:")
for e, sa, sb, gleich in doppel:
    if gleich: continue
    A, B = e['A'], e['B']
    P("       %04X S%dr%-2d(%3d,%3d)->Sym r%-2d(%3d,%3d) | %04X S%dr%-2d(%3d,%3d)->Sym r%-2d(%3d,%3d) | Symbolabstand %.1f px" %
      (A['room'], A['pg'], A['r'], A['mx'], A['my'], sa['r'], sa['x'], sa['y'],
       B['room'], B['pg'], B['r'], B['mx'], B['my'], sb['r'], sb['x'], sb['y'],
       math.hypot(sa['x'] - sb['x'], sa['y'] - sb['y'])))
P("")
P("(4c) Gegenprobe ohne Projektion: Symbole je Rechteck gegen Tuer-Datensaetze der Zone")
sd = ss = 0
for (b, zi), z in sorted(ZONES.items()):
    nd = sum(1 for d in doors_of(b) if zone_at(b, d['lx'], d['lz']) == zi)
    ns = len(SYM_RECT.get((z['page'], z['rect']), []))
    sd += nd; ss += ns
P("     Tuer-Datensaetze in gezeichneten Zonen: %d   gemalte Symbole auf deren Rechtecken: %d" % (sd, ss))
P("     -> %.2f Symbole je Tuer-Datensatz (bei 'zweimal gemalt' waeren >=2 zu erwarten)" % (ss / float(sd)))

P("")
P("=" * 78)
P("TEIL 5 - DIE TATSAECHLICHE DOPPELUNG: PORT-MARKE NEBEN GEMALTEM SYMBOL")
P("=" * 78)
M = [v for v in vor if lebt(v)]
P("Lebende Tuer-Marken (vor Verschmelzung/Dedup): %d" % len(M))
for tol in (0, 2, 4, 6, 8):
    eig = nach = 0
    for v in M:
        s = sym_treffer(SYM_PAGE[v['pg']], v['mx'], v['my'], tol)
        if not s: continue
        if s['r'] == v['r']: eig += 1
        else: nach += 1
    P("   tol=%d px: %3d Marken auf einem gemalten Symbol  (eigenes Rect %d, NACHBAR-Rect %d)"
      % (tol, eig + nach, eig, nach))
P("")
P("Der Filter kachel_zeigt_tuer() prueft NUR GLYPHEN[(seite,rect)] des EIGENEN Rechtecks")
P("(gen_map_zones.py, Funktion kachel_zeigt_tuer). Die Rechtecke einer Seite ueberlappen")
P("sich auf dem Schirm, also faellt jedes Symbol des NACHBAR-Rechtecks durch den Filter.")
P("")
P("Liste (tol 4 px):")
rect_room = {}
for (b, zi), z in ZONES.items(): rect_room.setdefault((z['page'], z['rect']), []).append(b)
nziel = 0
for v in sorted(M, key=lambda v: (v['pg'], v['r'], v['my'])):
    s = sym_treffer(SYM_PAGE[v['pg']], v['mx'], v['my'], 4)
    if not s: continue
    zr = rect_room.get((v['pg'], s['r']), [])
    ist = (v['d']['dest'] & 0xFFF0) in zr
    nziel += ist
    P("   S%-2d Marke r%-2d(%3d,%3d)  %04X->%04X   Symbol r%-2d(%3d,%3d) %dx%d  Rect gehoert %s  %s"
      % (v['pg'], v['r'], v['mx'], v['my'], v['room'], v['d']['dest'], s['r'], s['x'], s['y'],
         s['w'], s['h'], [('%04X' % x) for x in zr] or '-', 'ZIELRAUM' if ist else ''))
P("   -> davon nachweislich derselbe Durchgang (Symbol liegt im Rechteck des Zielraums): %d" % nziel)

pickle.dump(dict(vor=vor, paare=paare, Z=[{k: v for k, v in e.items() if k not in ('A', 'B')} | {'i': e['i'], 'j': e['j']} for e in Z]),
            open(r'analysis\karte_0901\_z.pkl', 'wb'))
open(r'analysis\karte_0901\_zensus_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
