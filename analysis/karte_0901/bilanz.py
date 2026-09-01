# -*- coding: utf-8 -*-
"""Schlussbilanz: wie viele Tuer-Zeichnungen sieht der Spieler JE DURCHGANG?
Gezaehlt wird, was auf EINEM Blatt sichtbar ist: gemalte Symbole der Kachel plus
Port-Marken aus s_map_marks[] des Schnappschusses."""
import sys, os, math, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S
OUT = []
def P(*a):
    s = ' '.join(str(x) for x in a); print(s); OUT.append(s)

WAND = {0: 'N', 1: 'E', 2: 'S', 3: 'W'}
GEG = {'N': 'S', 'S': 'N', 'E': 'W', 'W': 'E'}
zid_of = {k: z['zid'] for k, z in ZONES.items()}
etagen = {}
for (fr, fz, fb, fpg, frr) in FLOORS: etagen.setdefault((fr, fz), []).append((fb, fpg, frr))
def blatt(room, zi, band):
    e = etagen.get((room, zi))
    if not e or len(e) < 2: return None
    best = bestd = None
    for (fb, fpg, frr) in e:
        d = abs(fb - band)
        if bestd is None or d < bestd: best, bestd = (fpg, frr), d
    return best

alle = []
for b in BASES:
    for n, m in enumerate(doors_of(b)):
        rec = dict(room=b, idx=n, d=m, pg=None)
        zi = zone_at(b, m['lx'], m['lz']) if any(k[0] == b for k in ZONES) else None
        if zi is not None:
            mp = to_map(b, zi, m['lx'], m['lz'])
            if mp:
                pg, r, mx, my = mp
                bl = blatt(b, zi, m.get('band', 0))
                if bl and bl != (pg, r):
                    R0 = rects(pg)[r]; R1 = rects(bl[0])[bl[1]]
                    mx += R1[0] - R0[0]; my += R1[1] - R0[1]; pg, r = bl
                if m['rw'] != m['rd']: senk = m['rd'] > m['rw']
                else:
                    R = rects(pg)[r]
                    senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) < min(my - R[1], R[1] + R[3] - 1 - my))
                mx, my, se = snap_wall(pg, r, mx, my, senk)
                rec.update(zi=zi, pg=pg, r=r, mx=mx, my=my, seite=se,
                           kachel=kachel_zeigt_tuer(pg, r, mx, my))
        alle.append(rec)
K = []
for i in range(len(alle)):
    A = alle[i]
    for j in range(i + 1, len(alle)):
        B = alle[j]
        if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
        d1 = abs(A['d']['nx'] - B['d']['lx']) + abs(A['d']['nz'] - B['d']['lz'])
        d2 = abs(B['d']['nx'] - A['d']['lx']) + abs(B['d']['nz'] - A['d']['lz'])
        K.append((d1 + d2, i, j))
paare = []; bel = set()
for s, i, j in sorted(K):
    if i in bel or j in bel: continue
    bel.add(i); bel.add(j); paare.append((i, j))

MK = set((m[0], m[1], m[2], m[3], m[4]) for m in MARKS if m[4] <= 3)
def marke_da(X):
    return X['pg'] is not None and (X['pg'], X['r'], X['mx'], X['my'], X['seite']) in MK
def sym_da(X):
    if X['pg'] is None: return None
    g = sym_treffer(SYM_PAGE[X['pg']], X['mx'], X['my'], 4)
    if g: return (g['x'], g['y'])
    for g in SYM_PAGE.get(X['pg'], ()):
        if GEG.get(WAND[X['seite']]) == g['wand'] and math.hypot(X['mx']-g['x'], X['my']-g['y']) <= 8:
            return (g['x'], g['y'])
    return None

P("=" * 78)
P("SCHLUSSBILANZ - SICHTBARE TUER-ZEICHNUNGEN JE DURCHGANG (IST-Stand)")
P("=" * 78)
P("Ein Durchgang = ein Paar reziproker Tuer-Datensaetze. Gezaehlt werden je BLATT:")
P("die getroffenen gemalten Symbole (nach Identitaet dedupliziert) + die Port-Marken.")
P("")
hist = collections.Counter(); det = []
for (i, j) in paare:
    A, B = alle[i], alle[j]
    proBlatt = collections.defaultdict(lambda: [set(), 0])
    for X in (A, B):
        if X['pg'] is None: continue
        s = sym_da(X)
        if s: proBlatt[X['pg']][0].add(s)
        if marke_da(X): proBlatt[X['pg']][1] += 1
    if not proBlatt: hist['nichts gezeichnet'] += 1; continue
    mx_ = max(len(v[0]) + v[1] for v in proBlatt.values())
    hist['%d Zeichnung(en) auf dem vollsten Blatt' % mx_] += 1
    if mx_ >= 2:
        pg = max(proBlatt, key=lambda p: len(proBlatt[p][0]) + proBlatt[p][1])
        det.append((A, B, pg, len(proBlatt[pg][0]), proBlatt[pg][1]))
for k, n in sorted(hist.items()): P("   %-42s %3d" % (k, n))
P("")
P("Die Durchgaenge mit >=2 Zeichnungen auf EINEM Blatt:")
P("   Raum A  ->  Raum B          Blatt  gemalte Symbole  Port-Marken")
for A, B, pg, ns, nm in sorted(det, key=lambda t: (t[2], t[0]['room'])):
    P("   %04X <-> %04X                S%-2d        %d              %d"
      % (A['room'], B['room'], pg, ns, nm))
P("")
P("Zum Vergleich: was das ORIGINAL je Durchgang malt (nur gemalte Symbole):")
h2 = collections.Counter()
for (i, j) in paare:
    A, B = alle[i], alle[j]
    if A['pg'] is None and B['pg'] is None: continue
    s = set()
    for X in (A, B):
        v = sym_da(X)
        if v: s.add(v)
    h2[len(s)] += 1
for k in sorted(h2): P("   %d gemaltes Symbol / Durchgang: %3d Durchgaenge" % (k, h2[k]))
open(r'analysis\karte_0901\_bilanz_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
