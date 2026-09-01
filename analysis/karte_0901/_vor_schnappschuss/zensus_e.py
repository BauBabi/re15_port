# -*- coding: utf-8 -*-
"""Was steht WIRKLICH im Header, und wie sieht es auf dem SCHIRM aus?"""
import sys, os, re, collections, math
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL

H = open(r're15_port\engine\src\re15_map_zones.h', encoding='utf-8').read()
blk = H.split('s_map_marks[] = {')[1].split('};')[0]
MK = []
for m in re.finditer(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', blk):
    pg, r, mx, my, kind, zid, zid2 = (int(x) for x in m.groups())
    MK.append(dict(pg=pg, r=r, mx=mx, my=my, kind=kind, zid=zid, zid2=zid2))
print("Marken im Header:", len(MK),
      " Tueren:", sum(1 for k in MK if k['kind'] <= 3),
      " Treppen:", sum(1 for k in MK if k['kind'] >= 4),
      " mit zid2:", sum(1 for k in MK if k['zid2'] != 255))

# ---- alle gemalten TUER-Symbole je SEITE (Bildschirm-Koordinaten) ----
GLY_SEITE = collections.defaultdict(list)
for k, v in SL.GLYPHEN.items():
    if isinstance(k, tuple): continue         # (page,rect)-Schluessel ueberspringen
    for g in v: GLY_SEITE[k].append(g)
print("gemalte TUER/TUER2-Symbole gesamt:", sum(len(v) for v in GLY_SEITE.values()))

print("\n=== A: PORT-MARKE ueber einem GEMALTEN Symbol (gleiche SEITE, egal welches Rect) ===")
for tol in (0, 2, 4, 6, 8):
    n = 0
    for k in MK:
        if k['kind'] > 3: continue
        for gx, gy, gw, gh, wd in GLY_SEITE.get(k['pg'], ()):
            if abs(k['mx']-gx) <= tol + gw//2 and abs(k['my']-gy) <= tol + gh//2:
                n += 1; break
    print(f"   tol={tol}px: {n} von {sum(1 for k in MK if k['kind']<=3)} Tuermarken")

print("\n=== B: ZWEI PORT-MARKEN dicht beieinander (gleiche SEITE) ===")
for tol in (0, 2, 4, 6, 8, 12):
    n = 0
    T = [k for k in MK if k['kind'] <= 3]
    for i in range(len(T)):
        for j in range(i+1, len(T)):
            if T[i]['pg'] != T[j]['pg']: continue
            if abs(T[i]['mx']-T[j]['mx']) <= tol and abs(T[i]['my']-T[j]['my']) <= tol: n += 1
    print(f"   tol={tol}px: {n} Paare")

print("\n--- Liste: Tuermarke direkt auf einem gemalten Symbol (tol=4) ---")
for k in sorted(MK, key=lambda k: (k['pg'], k['r'])):
    if k['kind'] > 3: continue
    for gx, gy, gw, gh, wd in GLY_SEITE.get(k['pg'], ()):
        if abs(k['mx']-gx) <= 4+gw//2 and abs(k['my']-gy) <= 4+gh//2:
            print(f"   Marke S{k['pg']}r{k['r']:<2d}({k['mx']:3d},{k['my']:3d}) kind{k['kind']} "
                  f"zid{k['zid']}/{k['zid2']}   Symbol ({gx},{gy}) {gw}x{gh} wand={wd}")
            break
