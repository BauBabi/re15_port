# -*- coding: utf-8 -*-
"""Regel-Simulation + Schirmbilder, gegen den eingefrorenen Schnappschuss."""
import sys, os, math, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S
OUT = []
def P(*a):
    s = ' '.join(str(x) for x in a); print(s); OUT.append(s)

zid_of = {k: z['zid'] for k, z in ZONES.items()}
etagen = {}
for (fr, fz, fb, fpg, frr) in FLOORS: etagen.setdefault((fr, fz), []).append((fb, fpg, frr))
def blatt_fuer_band(room, zi, band):
    e = etagen.get((room, zi))
    if not e or len(e) < 2: return None
    best = bestd = None
    for (fb, fpg, frr) in e:
        d = abs(fb - band)
        if bestd is None or d < bestd: best, bestd = (fpg, frr), d
    return best

# ---- Durchgang 1 OHNE den Kachel-Filter (der Filter wird spaeter als Regel angewandt) ----
roh = []
for b in sorted(set(k[0] for k in ZONES)):
    for n, m in enumerate(doors_of(b)):
        zi = zone_at(b, m['lx'], m['lz'])
        if zi is None: continue
        mp = to_map(b, zi, m['lx'], m['lz'])
        if not mp: continue
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
        roh.append(dict(room=b, idx=n, d=m, zi=zi, pg=pg, r=r, mx=mx, my=my, seite=seite,
                        zid=zid_of.get((b, zi), 0)))
P("Tuer-Datensaetze mit Rechteck (Durchgang 1 ohne Kachel-Filter): %d" % len(roh))

part = {}
K = []
for i in range(len(roh)):
    A = roh[i]
    for j in range(i + 1, len(roh)):
        B = roh[j]
        if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
        d1 = abs(A['d']['nx'] - B['d']['lx']) + abs(A['d']['nz'] - B['d']['lz'])
        d2 = abs(B['d']['nx'] - A['d']['lx']) + abs(B['d']['nz'] - A['d']['lz'])
        if d1 > 4000 or d2 > 4000: continue
        K.append((d1 + d2, i, j))
bel = set()
for _d, i, j in sorted(K):
    if i in bel or j in bel: continue
    bel.add(i); bel.add(j); part[i] = j; part[j] = i
P("davon zu Paaren gebunden: %d Paare (%d Datensaetze), einzeln: %d"
  % (len(part) // 2, len(part), len(roh) - len(part)))

def sim(name, tol, bereich, paar_erst):
    marken = []; fertig = set()
    for k, v in enumerate(roh):
        if k in fertig: continue
        j = part.get(k)
        enden = [v] + ([roh[j]] if j is not None else [])
        for X in enden: fertig.add(id(X))
        fertig.add(k)
        if j is not None: fertig.add(j)
        def liste(X):
            if bereich == 'rect': return SYM_RECT[(X['pg'], X['r'])]
            if bereich == 'seite': return SYM_PAGE[X['pg']]
            L = list(SYM_RECT[(X['pg'], X['r'])])
            for Y in enden:
                if Y is not X and Y['pg'] == X['pg']: L += SYM_RECT[(Y['pg'], Y['r'])]
            return L
        tr = [sym_treffer(liste(X), X['mx'], X['my'], tol) is not None for X in enden]
        if paar_erst:
            if any(tr): continue
            fuer = {}
            for X in enden: fuer.setdefault(X['pg'], X)
        else:
            fuer = {}
            for X, t in zip(enden, tr):
                if not t: fuer.setdefault(X['pg'], X)
        for X in fuer.values(): marken.append((X['pg'], X['r'], X['mx'], X['my'], X['seite']))
    n = len(set(marken))
    P("   %-56s %3d Tuermarken" % (name, n))
    return set(marken)

P("")
P("IST (Schnappschuss-Header): %d Tuermarken, %d Treppenmarken"
  % (sum(1 for m in MARKS if m[4] <= 3), sum(1 for m in MARKS if m[4] >= 4)))
P("Simulation (dieselbe Pipeline, nur die Regel getauscht; Treppen unveraendert):")
a = sim("(a) HEUTE: Kachel-Filter/eigenes Rect, DANN Paarung", 4, 'rect', False)
b = sim("(b) Paarung zuerst, Kachel-Test nur eigenes Rect", 4, 'rect', True)
c = sim("(c) Paarung zuerst, Kachel-Test eigenes + Partner-Rect", 4, 'paar', True)
d = sim("(d) Paarung zuerst, Kachel-Test ganze Seite", 4, 'seite', True)
e = sim("(e) wie (c), Toleranz 6 px", 6, 'paar', True)
f = sim("(f) wie (d), Toleranz 6 px", 6, 'seite', True)
P("")
P("Differenz (c) gegen (a): %d Marken weniger" % (len(a) - len(c)))
P("Differenz (d) gegen (a): %d Marken weniger" % (len(a) - len(d)))
P("Marken, die (c) zusaetzlich zu (a) faellen laesst:")
for m in sorted(a - c): P("     S%dr%-2d (%3d,%3d) kind%d" % m)
P("Marken, die (d) zusaetzlich zu (c) faellen laesst:")
for m in sorted(c - d): P("     S%dr%-2d (%3d,%3d) kind%d" % m)

# ---------------- SCHIRMBILDER ----------------
def schirm(pg, x0, y0, x1, y1, titel=''):
    pix = page_pix(pg); W, Hh = x1 - x0 + 1, y1 - y0 + 1
    buf = [[' '] * W for _ in range(Hh)]
    for ri, (RX, RY, RW, RH) in enumerate(rects(pg)):
        U, V = rect_uv(pg, ri)
        for j in range(RH):
            for i in range(RW):
                sx, sy = RX + i, RY + j
                if not (x0 <= sx <= x1 and y0 <= sy <= y1): continue
                yy, xx = V + j, U + i
                if not (0 <= yy < 256 and 0 <= xx < 256): continue
                if pix[yy][xx]: buf[sy - y0][sx - x0] = '#' if pix[yy][xx] == 4 else '.'
    for m in MARKS:
        if m[0] != pg: continue
        if x0 <= m[2] <= x1 and y0 <= m[3] <= y1:
            buf[m[3] - y0][m[2] - x0] = 'M' if m[4] <= 3 else 'T'
    P(""); P("--- Seite %d  x %d..%d  y %d..%d   %s" % (pg, x0, x1, y0, y1, titel))
    P('     ' + ''.join(str((x0 + i) // 100 % 10) for i in range(W)))
    P('     ' + ''.join(str((x0 + i) // 10 % 10) for i in range(W)))
    P('     ' + ''.join(str((x0 + i) % 10) for i in range(W)))
    for j in range(Hh): P("%4d " % (y0 + j) + ''.join(buf[j]))
    P("     '#' = Palettenindex 4 (gemalte Linie/Tuerblatt), '.' = sonstige Kachel-Farbe,")
    P("     'M' = Port-Tuermarke aus s_map_marks[], 'T' = Port-Treppenmarke")

schirm(4, 150, 140, 178, 165, "ROOM1130 r4 <-> ROOM1140 r6: Kachel malt EINE Doppeltuer, Port setzt eine Marke daneben")
schirm(4, 138, 78, 176, 100, "ROOM1130 r4 <-> ROOM1150 r2 und ROOM1130 -> ROOM1170")
schirm(4, 142, 116, 168, 134, "ROOM1130 r4 <-> ROOM1120 r5 (VERSCHMOLZENES Paar, zid2 gesetzt)")
schirm(7, 120, 105, 145, 125, "ROOM3000 r0 <-> ROOM3010 r1")
open(r'analysis\karte_0901\_regeln_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
