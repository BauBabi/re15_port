# -*- coding: utf-8 -*-
"""Schirm-Ebene: jede Tuermarke gegen ALLE gemalten Symbole DER SEITE."""
import sys, os, pickle, collections, math, csv, re
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
D = pickle.load(open(r'analysis\karte_0901\_zensus.pkl','rb'))
vor = D['vor']; paare = D['paare']
lebt = lambda v: v['aus'] is None

GS = collections.defaultdict(list)
with open(r'analysis\kartensymbole\symbolkatalog.csv', encoding='utf-8') as f:
    for t in csv.DictReader(f):
        if t['typ'] not in ('TUER','TUER2'): continue
        GS[int(t['seite'])].append(dict(x=int(t['screen_x'])+int(t['px_w'])//2,
            y=int(t['screen_y'])+int(t['px_h'])//2, w=int(t['px_w']), h=int(t['px_h']),
            wand=t['wand'], r=int(t['rect']), rm=t['raeume']))

# Rect eines Raums (aus ZONES) -> welche Raeume gehoeren zu welchem Rect
rect_room = {}
for (b, zi), z in ZONES.items(): rect_room.setdefault((z['page'], z['rect']), []).append((b, zi))

M = [v for v in vor if lebt(v)]
print("lebende Marken (Tueren):", len(M))
res = collections.Counter(); zeilen = []
for v in M:
    tr = None
    for g in GS.get(v['pg'], ()):
        if abs(v['mx']-g['x']) <= 4+g['w']//2 and abs(v['my']-g['y']) <= 4+g['h']//2:
            d = abs(v['mx']-g['x'])+abs(v['my']-g['y'])
            if tr is None or d < tr[0]: tr = (d, g)
    if tr is None: res['kein Symbol in 4 px'] += 1; continue
    g = tr[1]
    same = (g['r'] == v['r'])
    res['Symbol im EIGENEN Rect (Filter haette greifen muessen)' if same
         else 'Symbol im NACHBAR-Rect (Filter greift NICHT)'] += 1
    # gehoert das Symbol zum ZIELRAUM dieser Tuer?
    ziel = v['d']['dest'] & 0xFFF0
    zr = [b for (b, zi) in rect_room.get((v['pg'], g['r']), [])]
    zeilen.append((v, g, same, ziel in zr, zr))
for k, n in res.most_common(): print(f"   {k:56s} {n}")

print("\n--- Marke ueber einem Symbol des NACHBAR-Rects ---")
print("   'Ziel?' = das Nachbar-Rect gehoert dem ZIELRAUM dieser Tuer = derselbe Durchgang")
nz = 0
for v, g, same, istziel, zr in sorted(zeilen, key=lambda t: (t[0]['pg'], t[0]['r'])):
    if same: continue
    nz += istziel
    print(f"   S{v['pg']:<2d} Marke r{v['r']:<2d}({v['mx']:3d},{v['my']:3d}) Raum {v['room']:04X}->"
          f"{v['d']['dest']:04X}   Symbol r{g['r']:<2d}({g['x']:3d},{g['y']:3d}) "
          f"Rect-Raeume {[('%04X'%b) for b in zr]}   Ziel? {'JA' if istziel else 'nein'}")
print(f"   -> davon derselbe Durchgang (Symbol im Zielraum-Rect): {nz}")
