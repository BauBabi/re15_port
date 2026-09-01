# -*- coding: utf-8 -*-
import sys, os, pickle, collections, math
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
D = pickle.load(open(r'analysis\karte_0901\_zensus.pkl','rb'))
vor, paare, un = D['vor'], D['paare'], D['un']

lebt = lambda v: v['aus'] is None
print("=== PAARE NACH UEBERLEBEN IN DURCHGANG 1 ===")
c = collections.Counter()
for p in paare:
    A, B = vor[p['i']], vor[p['j']]
    c[(lebt(A), lebt(B))] += 1
print("  beide erzeugen Marke  :", c[(True,True)])
print("  genau eine erzeugt    :", c[(True,False)] + c[(False,True)])
print("  keine erzeugt         :", c[(False,False)])
tot = c[(True,True)]*2 + c[(True,False)] + c[(False,True)]
print("  -> Marken aus Paaren  :", tot)
print("  -> Marken aus Einzeln :", sum(1 for k in un if lebt(vor[k])))
print("  -> Summe (== 134?)    :", tot + sum(1 for k in un if lebt(vor[k])))

print("\n=== DIE 'BEIDE LEBEN'-PAARE: BLATT-VERGLEICH ===")
bp = [p for p in paare if lebt(vor[p['i']]) and lebt(vor[p['j']])]
gl, ug = [], []
for p in bp:
    A, B = vor[p['i']], vor[p['j']]
    (gl if A['pg'] == B['pg'] else ug).append(p)
print("  gleiche Seite (verschmolzen moeglich):", len(gl))
print("  verschiedene Seiten (continue)       :", len(ug))
# Toleranz-Filter des Generators
gl_tol = [p for p in gl if p['tol']]
print("  davon in 4000er-Toleranz             :", len(gl_tol))

print("\n--- verschmolzene Paare (gleiche Seite, Toleranz ok) ---")
for p in sorted(gl_tol, key=lambda p: (vor[p['i']]['room'], vor[p['j']]['room'])):
    A, B = vor[p['i']], vor[p['j']]
    dpx = math.hypot(A['mx']-B['mx'], A['my']-B['my'])
    print(f"  {A['room']:04X}z{A['zi']} r{A['r']:<2d} ({A['mx']:3d},{A['my']:3d}) <-> "
          f"{B['room']:04X}z{B['zi']} r{B['r']:<2d} ({B['mx']:3d},{B['my']:3d})  "
          f"Seite {A['pg']}  dpx={dpx:5.1f}  weltd={p['d1']}/{p['d2']}")

print("\n--- NICHT verschmolzen: verschiedene Blaetter ---")
for p in sorted(ug, key=lambda p: (vor[p['i']]['room'], vor[p['j']]['room'])):
    A, B = vor[p['i']], vor[p['j']]
    print(f"  {A['room']:04X}z{A['zi']} S{A['pg']}r{A['r']:<2d} ({A['mx']:3d},{A['my']:3d}) <-> "
          f"{B['room']:04X}z{B['zi']} S{B['pg']}r{B['r']:<2d} ({B['mx']:3d},{B['my']:3d})")
