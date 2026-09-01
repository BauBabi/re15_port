# -*- coding: utf-8 -*-
"""FRAGE 4: malt das ORIGINAL einen Durchgang einmal oder zweimal?"""
import sys, os, csv, collections, math
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL

G = []
with open(r'analysis\kartensymbole\symbolkatalog.csv', encoding='utf-8') as f:
    for t in csv.DictReader(f):
        if t['typ'] not in ('TUER','TUER2'): continue
        G.append(dict(pg=int(t['seite']), r=int(t['rect']),
                      x=int(t['screen_x'])+int(t['px_w'])//2,
                      y=int(t['screen_y'])+int(t['px_h'])//2,
                      w=int(t['px_w']), h=int(t['px_h']),
                      wand=t['wand'], typ=t['typ'], raeume=t['raeume']))
print("TUER/TUER2-Symbole:", len(G), " auf", len(set((g['pg'],g['r']) for g in G)), "Rechtecken")
c = collections.Counter((g['pg'],g['r']) for g in G)
print("Symbole je Rechteck:", dict(collections.Counter(c.values())))

print("\n=== ZWEI GEMALTE SYMBOLE FUER DENSELBEN DURCHGANG? ===")
print("Kriterium: zwei Symbole auf DERSELBEN Seite, VERSCHIEDENE Rechtecke, dicht beieinander.")
print("Ein Durchgang zwischen A und B, den der Kuenstler zweimal malt, muss so aussehen:")
print("zwei Stempel beidseits derselben Wandlinie, also 4..10 px auseinander,")
print("mit GEGENUEBERLIEGENDER Wandseite (N<->S bzw. E<->W).\n")
GEG = {'N':'S','S':'N','E':'W','W':'E'}
for tol in (4,6,8,10,14,20):
    n = ngeg = 0
    for i in range(len(G)):
        for j in range(i+1, len(G)):
            a, b = G[i], G[j]
            if a['pg'] != b['pg']: continue
            d = math.hypot(a['x']-b['x'], a['y']-b['y'])
            if d > tol: continue
            n += 1
            if GEG.get(a['wand']) == b['wand']: ngeg += 1
    print(f"   d<={tol:2d}px: {n} Symbolpaare, davon gegenueberliegende Wand: {ngeg}")

print("\n--- alle Symbolpaare auf derselben Seite mit d<=20 px ---")
for i in range(len(G)):
    for j in range(i+1, len(G)):
        a, b = G[i], G[j]
        if a['pg'] != b['pg']: continue
        d = math.hypot(a['x']-b['x'], a['y']-b['y'])
        if d > 20: continue
        print(f"   S{a['pg']:<2d} r{a['r']:<2d}({a['x']:3d},{a['y']:3d}) {a['wand']:<3s} {a['typ']:<5s} [{a['raeume']}]"
              f"   <-> r{b['r']:<2d}({b['x']:3d},{b['y']:3d}) {b['wand']:<3s} {b['typ']:<5s} [{b['raeume']}]   d={d:4.1f}"
              f"   {'SELBES RECT' if a['r']==b['r'] else 'versch. Rect'}")

# --- Gegenprobe: Symbole je Raum-Rechteck vs. Tuer-Datensaetze dieses Raums ---
print("\n=== GEGENPROBE: Symbole je Rechteck vs. Tuer-Datensaetze der zugehoerigen Zone ===")
read_rdt = SL.G['read_rdt']
tab = []
for (base, zi), z in sorted(ZONES.items()):
    key = (z['page'], z['rect'])
    nsym = c.get(key, 0)
    nd = sum(1 for d in doors_of(base) if zone_at(base, d['lx'], d['lz']) == zi)
    tab.append((base, zi, z['page'], z['rect'], nd, nsym))
print(f"{'Raum':6s} {'z':2s} {'S':>2s} {'r':>2s} {'Tueren':>6s} {'Symbole':>7s}")
sd = ss = 0
for base, zi, pg, r, nd, nsym in tab:
    if nd == 0 and nsym == 0: continue
    sd += nd; ss += nsym
    print(f"{base:04X}  {zi}  {pg:2d} {r:2d} {nd:6d} {nsym:7d}")
print(f"SUMME              {sd:6d} {ss:7d}")
