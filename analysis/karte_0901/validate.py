# -*- coding: utf-8 -*-
"""Gegenprobe: mein Nachbau von Durchgang 1-3 muss s_map_marks[] des Schnappschusses
EXAKT reproduzieren - sonst ist jede Zahl darauf wertlos."""
import sys, os, math, collections
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S

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

vor = []
for b in sorted(set(k[0] for k in ZONES)):
    st = stairs_of(b); st_m = {}; used = set(); cand = []
    for i in range(len(st)):
        for j in range(i + 1, len(st)):
            A, B = st[i], st[j]
            if A['axis'] != B['axis'] or A['count'] != B['count']: continue
            if abs(A['band'] - B['band']) != A['count']: continue
            cand.append((abs(A['x'] - B['x']) + abs(A['z'] - B['z']), i, j))
    for d, i, j in sorted(cand):
        if i in used or j in used: continue
        A, B = st[i], st[j]
        za, zb = zone_at(b, A['x'], A['z']), zone_at(b, B['x'], B['z'])
        if za is None or zb is None: continue
        pa, pb = to_map(b, za, A['x'], A['z']), to_map(b, zb, B['x'], B['z'])
        if not pa or not pb or pa[0] != pb[0]: continue
        cx, cy = (pa[2] + pb[2]) // 2, (pa[3] + pb[3]) // 2
        st_m[i] = (pa[0], pa[1], cx, cy); st_m[j] = (pb[0], pb[1], cx, cy)
        used.add(i); used.add(j)
    for kind, lst in ((0, doors_of(b)), (1, st)):
        for n, m in enumerate(lst):
            wx = m['lx'] if kind == 0 else m['x']
            wz = m['lz'] if kind == 0 else m['z']
            zi = zone_at(b, wx, wz)
            if zi is None: continue
            if kind == 1 and n in st_m: pg, r, mx, my = st_m[n]
            else:
                mp = to_map(b, zi, wx, wz)
                if not mp: continue
                pg, r, mx, my = mp
            bl = blatt_fuer_band(b, zi, m.get('band', 0))
            if bl and bl != (pg, r):
                R0 = rects(pg)[r]; R1 = rects(bl[0])[bl[1]]
                mx += R1[0] - R0[0]; my += R1[1] - R0[1]; pg, r = bl
            if kind == 0:
                if m['rw'] != m['rd']: senk = m['rd'] > m['rw']
                else:
                    R = rects(pg)[r]
                    senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) < min(my - R[1], R[1] + R[3] - 1 - my))
                mx, my, mk = snap_wall(pg, r, mx, my, senk)
                if kachel_zeigt_tuer(pg, r, mx, my): continue
            else: mk = 5 if m.get('axis') == 12 else 4
            vor.append({'room': b, 'zi': zi, 'idx': n, 'kind': kind, 'pg': pg, 'r': r,
                        'mx': mx, 'my': my, 'seite': mk, 'zid': zid_of.get((b, zi), 0), 'd': m})

tu = [v for v in vor if v['kind'] == 0]
kand = []
for i in range(len(tu)):
    A = tu[i]
    for j in range(i + 1, len(tu)):
        B = tu[j]
        if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
        d1 = abs(A['d']['nx'] - B['d']['lx']) + abs(A['d']['nz'] - B['d']['lz'])
        d2 = abs(B['d']['nx'] - A['d']['lx']) + abs(B['d']['nz'] - A['d']['lz'])
        if d1 > 4000 or d2 > 4000: continue
        kand.append((d1 + d2, i, j))
bel = set(); nm = nb = 0
for _d, i, j in sorted(kand):
    if i in bel or j in bel: continue
    A, B = tu[i], tu[j]; bel.add(i); bel.add(j)
    if A['pg'] != B['pg']: nb += 1; continue
    def ab(X, Y):
        RX, RY, RW, RH = rects(Y['pg'])[Y['r']]
        return max(RX - X['mx'], 0, X['mx'] - (RX + RW - 1)) + max(RY - X['my'], 0, X['my'] - (RY + RH - 1))
    W = A if ab(A, B) <= ab(B, A) else B
    for X in (A, B): X['mx'], X['my'], X['seite'] = W['mx'], W['my'], W['seite']
    W['zid2'] = (B if W is A else A)['zid']; (B if W is A else A)['weg'] = True; nm += 1

seen = set(); marks = []
for v in vor:
    if v.get('weg'): continue
    k = (v['pg'], v['r'], v['mx'], v['my'], v['seite'])
    if k in seen: continue
    seen.add(k)
    marks.append((v['pg'], v['r'], v['mx'], v['my'], v['seite'], v['zid'], v.get('zid2', 255)))
marks = sorted(marks)
H = sorted(MARKS)
print("verschmolzene Paare:", nm, "  Paare auf verschiedenen Blaettern (continue):", nb)
print("Marken nachgebaut:", len(marks), "  im Schnappschuss-Header:", len(H))
print("IDENTISCH:", marks == H)
if marks != H:
    a, b = set(marks), set(H)
    print("  nur im Nachbau:", sorted(a - b))
    print("  nur im Header :", sorted(b - a))
print("  davon Tueren:", sum(1 for m in H if m[4] <= 3), " Treppen:", sum(1 for m in H if m[4] >= 4))
print("  vom seen-Dedup verworfen:", len([v for v in vor if not v.get('weg')]) - len(marks))
