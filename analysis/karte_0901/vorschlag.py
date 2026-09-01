# -*- coding: utf-8 -*-
"""Vorschlag mit Zahlen: welche Regel bringt 'ein Durchgang = ein Symbol'?"""
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
def blatt_fuer_band(room, zi, band):
    e = etagen.get((room, zi))
    if not e or len(e) < 2: return None
    best = bestd = None
    for (fb, fpg, frr) in e:
        d = abs(fb - band)
        if bestd is None or d < bestd: best, bestd = (fpg, frr), d
    return best

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
        roh.append(dict(room=b, idx=n, d=m, zi=zi, pg=pg, r=r, mx=mx, my=my, seite=seite))
part = {}; K = []
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

def eig_rect(X, tol=4):   return sym_treffer(SYM_RECT[(X['pg'], X['r'])], X['mx'], X['my'], tol) is not None
def seite_alle(X, tol=4): return sym_treffer(SYM_PAGE[X['pg']], X['mx'], X['my'], tol) is not None
def spiegel(X, tol=8):
    mw = WAND[X['seite']]
    for g in SYM_PAGE.get(X['pg'], ()):
        if GEG.get(mw) != g['wand']: continue
        if math.hypot(X['mx'] - g['x'], X['my'] - g['y']) <= tol: return g
    return None

def lauf(name, test, paar_erst=True):
    mark = []; fertig = set()
    for k in range(len(roh)):
        if k in fertig: continue
        j = part.get(k)
        enden = [roh[k]] + ([roh[j]] if j is not None else [])
        fertig.add(k)
        if j is not None: fertig.add(j)
        tr = [test(X, enden) for X in enden]
        if paar_erst:
            if any(tr): continue
            fuer = {}
            for X in enden: fuer.setdefault(X['pg'], X)
        else:
            fuer = {}
            for X, t in zip(enden, tr):
                if not t: fuer.setdefault(X['pg'], X)
        for X in fuer.values(): mark.append((X['pg'], X['r'], X['mx'], X['my'], X['seite']))
    mark = sorted(set(mark))
    P("   %-58s %3d Tuermarken" % (name, len(mark)))
    return mark

def t_rect(X, e): return eig_rect(X)
def t_paar(X, e):
    L = list(SYM_RECT[(X['pg'], X['r'])])
    for Y in e:
        if Y is not X and Y['pg'] == X['pg']: L += SYM_RECT[(Y['pg'], Y['r'])]
    return sym_treffer(L, X['mx'], X['my'], 4) is not None
def t_sp(X, e):   return eig_rect(X) or spiegel(X) is not None
def t_sp_paar(X, e): return t_paar(X, e) or spiegel(X) is not None
def t_seite(X, e): return seite_alle(X)

P("=" * 78)
P("VORSCHLAG - REGELN UND IHRE ZAHLEN")
P("=" * 78)
P("Ausgangsmenge: %d Tuer-Datensaetze mit Rechteck, %d davon zu %d Paaren gebunden."
  % (len(roh), len(part), len(part) // 2))
P("")
IST = lauf("(a) HEUTE: Kachel/eigenes Rect, DANN Paarung  [= Header]", t_rect, False)
A = lauf("(b) nur Paarung vorziehen", t_rect, True)
B = lauf("(c) Paarung vorziehen + Kachel-Test eig.+Partner-Rect", t_paar, True)
C = lauf("(d) Paarung vorziehen + SPIEGELWAND-Test (8 px)", t_sp, True)
D = lauf("(e) Paarung vorziehen + Partner-Rect + SPIEGELWAND", t_sp_paar, True)
E = lauf("(f) Paarung vorziehen + Kachel-Test ganze Seite (4 px)", t_seite, True)
P("")
P("Verlust gegenueber heute: (b) %d   (c) %d   (d) %d   (e) %d   (f) %d"
  % (len(IST) - len(A), len(IST) - len(B), len(IST) - len(C), len(IST) - len(D), len(IST) - len(E)))
P("")
P("RESTDOPPELUNG NACH JEDER REGEL (Marke <=8 px vor einem Symbol der Spiegelwand):")
def rest(mark, name):
    n = 0
    for (pg, r, mx, my, se) in mark:
        X = dict(pg=pg, r=r, mx=mx, my=my, seite=se)
        if spiegel(X): n += 1
    P("   %-58s %3d Restfaelle" % (name, n))
rest(IST, "(a) heute")
rest(A, "(b)"); rest(B, "(c)"); rest(C, "(d)"); rest(D, "(e)"); rest(E, "(f)")
P("")
P("PORT-MARKE GEGEN PORT-MARKE (zwei Stempel derselben Seite, Mittelpunkt <=8 px):")
def rest2(mark, name):
    n = 0
    for i in range(len(mark)):
        for j in range(i + 1, len(mark)):
            if mark[i][0] != mark[j][0]: continue
            if math.hypot(mark[i][2] - mark[j][2], mark[i][3] - mark[j][3]) <= 8: n += 1
    P("   %-58s %3d Paare" % (name, n))
rest2(IST, "(a) heute"); rest2(A, "(b)"); rest2(B, "(c)"); rest2(C, "(d)"); rest2(D, "(e)")
P("")
P("WAS (e) GEGENUEBER HEUTE ZUSAETZLICH WEGLAESST:")
for m in sorted(set(IST) - set(D)):
    X = dict(pg=m[0], r=m[1], mx=m[2], my=m[3], seite=m[4])
    g = spiegel(X)
    P("   S%-2d r%-2d (%3d,%3d) Wand %s   %s" % (m[0], m[1], m[2], m[3], WAND[m[4]],
      ("Spiegelsymbol r%d(%d,%d) Wand %s, %.1f px" % (g['r'], g['x'], g['y'], g['wand'],
       math.hypot(m[2]-g['x'], m[3]-g['y']))) if g else "Partner-Rect-Symbol / Paar-Verschmelzung"))
P("")
P("WAS (e) NEU ZEIGT, das heute fehlt (Gegenrichtung, darf nicht leer bleiben):")
for m in sorted(set(D) - set(IST)):
    P("   S%-2d r%-2d (%3d,%3d) Wand %s" % (m[0], m[1], m[2], m[3], WAND[m[4]]))
open(r'analysis\karte_0901\_vorschlag_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
