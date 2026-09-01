# -*- coding: utf-8 -*-
"""Regel-Simulation: wie viele Marken fallen weg?"""
import sys, os, pickle, collections, math, csv
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
D = pickle.load(open(r'analysis\karte_0901\_zensus.pkl','rb'))
vor, paare, un = D['vor'], D['paare'], D['un']

GR = collections.defaultdict(list)   # (page,rect) -> Symbole
GP = collections.defaultdict(list)   # page       -> Symbole
with open(r'analysis\kartensymbole\symbolkatalog.csv', encoding='utf-8') as f:
    for t in csv.DictReader(f):
        if t['typ'] not in ('TUER','TUER2'): continue
        g = (int(t['screen_x'])+int(t['px_w'])//2, int(t['screen_y'])+int(t['px_h'])//2,
             int(t['px_w']), int(t['px_h']))
        GR[(int(t['seite']), int(t['rect']))].append(g); GP[int(t['seite'])].append(g)

def trifft(liste, mx, my, tol=4):
    for gx, gy, gw, gh in liste:
        if abs(mx-gx) <= tol+gw//2 and abs(my-gy) <= tol+gh//2: return True
    return False

hat = lambda v: v.get('pg') is not None
# Zustand nach Projektion (VOR dem Kachel-Filter): alle Records mit Rect
proj = [v for v in vor if hat(v)]
print("Tuer-Datensaetze mit Rechteck (nach Projektion) :", len(proj))
print("  heute unterdrueckt vom Kachel-Filter (eig.Rect):", sum(1 for v in proj if v['aus']=='kachel_malt_schon'))
print("  heute lebend                                   :", sum(1 for v in proj if v['aus'] is None))

part = {}                       # index -> Partnerindex
for p in paare: part[p['i']] = p['j']; part[p['j']] = p['i']
idx = {id(v): k for k, v in enumerate(vor)}

def sim(name, tol, bereich, paar_erst):
    """bereich: 'rect' | 'paar' (eig.+Partner-Rect) | 'seite'
       paar_erst: Paar zuerst zu EINER Marke verschmelzen, dann Kachel pruefen."""
    marken = []          # (pg, r, mx, my)
    erledigt = set()
    for k, v in enumerate(vor):
        if not hat(v) or k in erledigt: continue
        j = part.get(k)
        P = vor[j] if (j is not None and hat(vor[j])) else None
        enden = [v] + ([P] if P is not None else [])
        if P is not None: erledigt.add(j)
        erledigt.add(k)
        # Kachel-Test je Ende
        def test(X):
            if bereich == 'rect': L = GR[(X['pg'], X['r'])]
            elif bereich == 'seite': L = GP[X['pg']]
            else:
                L = list(GR[(X['pg'], X['r'])])
                for Y in enden:
                    if Y is not X and Y['pg'] == X['pg']: L += GR[(Y['pg'], Y['r'])]
            return trifft(L, X['mx'], X['my'], tol)
        tr = [test(X) for X in enden]
        if paar_erst:
            if any(tr): continue            # die Kachel zeigt den Durchgang -> nichts malen
            # eine Marke JE BLATT
            fuer = {}
            for X in enden: fuer.setdefault(X['pg'], X)
            for X in fuer.values(): marken.append((X['pg'], X['r'], X['mx'], X['my']))
        else:
            leben = [X for X, t in zip(enden, tr) if not t]
            fuer = {}
            for X in leben: fuer.setdefault(X['pg'], X)
            for X in fuer.values(): marken.append((X['pg'], X['r'], X['mx'], X['my']))
    ein = len(set(marken))
    print(f"  {name:52s} {len(marken):3d} Tuermarken (dedupliziert {ein})")
    return marken

print("\n=== REGEL-SIMULATION (Tuermarken, ohne Treppen, ohne Etagen-Kopien) ===")
print("  IST-Zustand im Header: 105 Tuermarken (134 gesamt - 29 Treppen)")
sim("(a) HEUTE nachgebaut: Filter/eig.Rect, dann Paar", 4, 'rect', False)
sim("(b) Paar zuerst, Kachel-Test nur eigenes Rect",    4, 'rect', True)
sim("(c) Paar zuerst, Kachel-Test eig.+Partner-Rect",   4, 'paar', True)
sim("(d) Paar zuerst, Kachel-Test ganze Seite",         4, 'seite', True)
sim("(e) wie (c), Toleranz 6 px",                       6, 'paar', True)
sim("(f) wie (d), Toleranz 6 px",                       6, 'seite', True)
