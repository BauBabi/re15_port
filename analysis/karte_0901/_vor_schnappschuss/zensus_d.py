# -*- coding: utf-8 -*-
import sys, os, pickle, collections, math
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
D = pickle.load(open(r'analysis\karte_0901\_zensus.pkl','rb'))
vor, paare, un = D['vor'], D['paare'], D['un']
hat_rect = lambda v: v.get('pg') is not None

def naechstes_glyph(v, tol=None):
    """(abstand, glyph) zum naechsten TUER-Symbol AUF DEM EIGENEN RECHTECK."""
    best = None
    for gx, gy, gw, gh, wd in SL.GLYPHEN.get((v['pg'], v['r']), ()):
        dx = max(abs(v['mx']-gx) - gw//2, 0); dy = max(abs(v['my']-gy) - gh//2, 0)
        d = dx + dy
        if best is None or d < best[0]: best = (d, (gx, gy, gw, gh, wd))
    return best

print("=== FRAGE 4: WAS MALT DAS ORIGINAL JE DURCHGANG? ===")
print("Kriterium = das Kachel-Symbol, das der Generator selbst benutzt")
print("(kachel_zeigt_tuer, tol=4 + halbe Symbolbreite, GLYPHEN[(seite,rect)]).\n")
beide_rect = [p for p in paare if hat_rect(vor[p['i']]) and hat_rect(vor[p['j']])]
print("Paare, bei denen BEIDE Enden ein Rechteck haben:", len(beide_rect), "von", len(paare))
kz = SL.G['kachel_zeigt_tuer']
c = collections.Counter(); rows = []
for p in beide_rect:
    A, B = vor[p['i']], vor[p['j']]
    ga = kz(A['pg'], A['r'], A['mx'], A['my'])
    gb = kz(B['pg'], B['r'], B['mx'], B['my'])
    n = int(ga) + int(gb)
    c[n] += 1
    rows.append((n, A, B, ga, gb))
print("  Kachel malt ZWEI Symbole (je Raum eins):", c[2])
print("  Kachel malt EIN Symbol                 :", c[1])
print("  Kachel malt KEIN Symbol                :", c[0])

print("\n--- Paare mit ZWEI gemalten Symbolen ---")
for n, A, B, ga, gb in rows:
    if n != 2: continue
    da = naechstes_glyph(A); db = naechstes_glyph(B)
    print(f"  {A['room']:04X}z{A['zi']} S{A['pg']}r{A['r']:<2d}({A['mx']:3d},{A['my']:3d}) d={da[0]}"
          f"   <->   {B['room']:04X}z{B['zi']} S{B['pg']}r{B['r']:<2d}({B['mx']:3d},{B['my']:3d}) d={db[0]}"
          f"   {'GLEICHES BLATT' if A['pg']==B['pg'] else 'versch. Blaetter'}")

print("\n--- Paare mit EINEM gemalten Symbol (= die Doppelung Kachel+Marke) ---")
for n, A, B, ga, gb in rows:
    if n != 1: continue
    X, Y = (A, B) if ga else (B, A)     # X = Kachel malt, Y = Port malt Marke
    d = naechstes_glyph(Y)
    print(f"  Kachel: {X['room']:04X} S{X['pg']}r{X['r']:<2d}({X['mx']:3d},{X['my']:3d})   "
          f"Port-Marke: {Y['room']:04X} S{Y['pg']}r{Y['r']:<2d}({Y['mx']:3d},{Y['my']:3d}) "
          f"naechstes Symbol dort {('%d px' % d[0]) if d else 'KEINS'}   "
          f"{'GLEICHES BLATT' if A['pg']==B['pg'] else 'versch.'}")
