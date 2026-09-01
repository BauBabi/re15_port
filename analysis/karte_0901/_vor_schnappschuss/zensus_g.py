# -*- coding: utf-8 -*-
"""Vollstaendiger Paar-Zensus: Blatt, Verschmelzung, Pixel-Abstand, Kachel-Symbole."""
import sys, os, pickle, collections, math, csv, json
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
D = pickle.load(open(r'analysis\karte_0901\_zensus.pkl','rb'))
vor, paare, un = D['vor'], D['paare'], D['un']

# Symbole mit ihrem Rechteck
GS = collections.defaultdict(list)   # page -> [(x,y,w,h,wand,rect,raeume)]
with open(r'analysis\kartensymbole\symbolkatalog.csv', encoding='utf-8') as f:
    for t in csv.DictReader(f):
        if t['typ'] not in ('TUER','TUER2'): continue
        GS[int(t['seite'])].append((int(t['screen_x'])+int(t['px_w'])//2,
                                    int(t['screen_y'])+int(t['px_h'])//2,
                                    int(t['px_w']), int(t['px_h']), t['wand'],
                                    int(t['rect']), t['raeume']))
def sym_bei(pg, mx, my, tol=4):
    """naechstes gemaltes Symbol auf DIESER SEITE (egal welches Rect)."""
    best = None
    for gx, gy, gw, gh, wd, r, rm in GS.get(pg, ()):
        if abs(mx-gx) <= tol+gw//2 and abs(my-gy) <= tol+gh//2:
            d = abs(mx-gx)+abs(my-gy)
            if best is None or d < best[0]: best = (d, gx, gy, r, wd, rm)
    return best

lebt = lambda v: v['aus'] is None
hat_rect = lambda v: v.get('pg') is not None

# ============ TABELLE ALLER PAARE ============
Z = []
for p in paare:
    A, B = vor[p['i']], vor[p['j']]
    e = dict(a=A, b=B, d1=p['d1'], d2=p['d2'], tol=p['tol'])
    e['beide_rect'] = hat_rect(A) and hat_rect(B)
    e['gleiche_seite'] = hat_rect(A) and hat_rect(B) and A['pg'] == B['pg']
    e['dpx'] = math.hypot(A['mx']-B['mx'], A['my']-B['my']) if e['gleiche_seite'] else None
    e['verschmolzen'] = lebt(A) and lebt(B) and e['gleiche_seite'] and p['tol']
    e['marken'] = int(lebt(A)) + int(lebt(B)) - (1 if e['verschmolzen'] else 0)
    e['symA'] = sym_bei(A['pg'], A['mx'], A['my']) if hat_rect(A) else None
    e['symB'] = sym_bei(B['pg'], B['mx'], B['my']) if hat_rect(B) else None
    Z.append(e)

print("=== 1. ZENSUS ALLER DURCHGANGS-PAARE ===")
print("Paare gesamt                        :", len(Z))
print("  beide Enden auf einem Blatt       :", sum(1 for e in Z if e['beide_rect']))
print("     davon DASSELBE Blatt           :", sum(1 for e in Z if e['gleiche_seite']))
print("     davon verschiedene Blaetter    :", sum(1 for e in Z if e['beide_rect'] and not e['gleiche_seite']))
print("  nur ein Ende gezeichnet           :", sum(1 for e in Z if hat_rect(e['a'])!=hat_rect(e['b'])))
print("  kein Ende gezeichnet              :", sum(1 for e in Z if not hat_rect(e['a']) and not hat_rect(e['b'])))
print()
print("  VERSCHMOLZEN (Durchgang 2)        :", sum(1 for e in Z if e['verschmolzen']))
print("  erzeugt 2 Port-Marken             :", sum(1 for e in Z if e['marken']==2))
print("  erzeugt 1 Port-Marke              :", sum(1 for e in Z if e['marken']==1))
print("  erzeugt 0 Port-Marken             :", sum(1 for e in Z if e['marken']==0))
print("  Summe Port-Marken aus Paaren      :", sum(e['marken'] for e in Z))

print("\n=== 2. WARUM NICHT VERSCHMOLZEN? (nur Paare mit >=1 lebendem Ende) ===")
g = collections.Counter()
for e in Z:
    if e['verschmolzen']: continue
    A, B = e['a'], e['b']
    if not lebt(A) and not lebt(B): g['beide Enden schon in Durchgang 1 ausgefallen'] += 1
    elif lebt(A) != lebt(B):
        tot = B if lebt(A) else A
        g['ein Ende faellt in Durchgang 1: ' + tot['aus']] += 1
    else:
        if not e['beide_rect']: g['?? kein Rect'] += 1
        elif not e['gleiche_seite']: g['beide leben, aber verschiedene BLAETTER (continue)'] += 1
        elif not e['tol']: g['beide leben, aber Positionstoleranz 4000 gerissen'] += 1
        else: g['??'] += 1
for k, n in g.most_common(): print(f"   {k:58s} {n}")

print("\n=== 3. PIXEL-ABSTAND DER BEIDEN ENDEN (gleiches Blatt) ===")
ds = sorted(e['dpx'] for e in Z if e['dpx'] is not None)
import statistics
print("   n =", len(ds))
print("   Median %.1f px   Mittel %.1f px   Min %.1f   Max %.1f" % (
      statistics.median(ds), sum(ds)/len(ds), ds[0], ds[-1]))
for q in (0.10,0.25,0.5,0.75,0.90):
    print("   p%-3d = %5.1f px" % (q*100, ds[int(q*(len(ds)-1))]))
print("   <= 2 px:", sum(1 for d in ds if d<=2), "  <= 5 px:", sum(1 for d in ds if d<=5),
      "  <=10 px:", sum(1 for d in ds if d<=10), "  >20 px:", sum(1 for d in ds if d>20))
print("   Schlimmste:")
for e in sorted([e for e in Z if e['dpx'] is not None], key=lambda e:-e['dpx'])[:8]:
    A,B = e['a'],e['b']
    print(f"     {A['room']:04X}z{A['zi']} r{A['r']}({A['mx']},{A['my']}) <-> {B['room']:04X}z{B['zi']} r{B['r']}({B['mx']},{B['my']})  {e['dpx']:6.1f} px")

print("\n=== 4. WAS MALT DIE KACHEL JE DURCHGANG (Symbol auf der SEITE, tol=4) ===")
cc = collections.Counter()
for e in Z:
    if not e['beide_rect']: continue
    cc[int(e['symA'] is not None)+int(e['symB'] is not None)] += 1
print("   beide Enden ein gemaltes Symbol :", cc[2])
print("   genau ein Ende                  :", cc[1])
print("   keines                          :", cc[0])
print("   (Paare mit beiden Enden auf einem Blatt: %d)" % sum(cc.values()))
print("\n   Paare, bei denen BEIDE Enden ein Symbol treffen:")
for e in Z:
    if not e['beide_rect'] or e['symA'] is None or e['symB'] is None: continue
    A,B = e['a'],e['b']
    print(f"     {A['room']:04X} S{A['pg']}r{A['r']}({A['mx']},{A['my']})->Sym r{e['symA'][3]}({e['symA'][1]},{e['symA'][2]}) "
          f"| {B['room']:04X} S{B['pg']}r{B['r']}({B['mx']},{B['my']})->Sym r{e['symB'][3]}({e['symB'][1]},{e['symB'][2]}) "
          f"| Symbolabstand {math.hypot(e['symA'][1]-e['symB'][1], e['symA'][2]-e['symB'][2]):.1f} px"
          f" | {'SELBES Symbol' if (e['symA'][1],e['symA'][2])==(e['symB'][1],e['symB'][2]) else 'zwei Symbole'}")
pickle.dump(Z, open(r'analysis\karte_0901\_paare.pkl','wb'))
