# -*- coding: utf-8 -*-
"""Zensus der Durchgangs-Paare + Doppel-Marken. Nutzt sonde_lib (Ist-Header)."""
import sys, os, math, itertools, collections, json
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL

GAP = SL.GAP
kachel_zeigt_tuer = SL.G['kachel_zeigt_tuer']
read_rdt = SL.G['read_rdt']

# ---- ALLE Raeume aus der Raumliste (nicht nur die mit Zone) ----
room_ids = []
for line in open(r're15_port\include\re15_room_list.h'):
    for tok in line.replace(',', ' ').split():
        if tok.startswith('0x') and len(tok) == 6:
            try: room_ids.append(int(tok, 16))
            except ValueError: pass
BASES = sorted(set(r & 0xFFF0 for r in room_ids))
RDT2 = {}
for b in BASES:
    got = read_rdt(b) or read_rdt(b+1)
    if got: RDT2[b] = got
print("Basis-Raeume in Liste:", len(BASES), " mit RDT:", len(RDT2),
      " mit Zone im Header:", len(set(k[0] for k in ZONES)))

def doors_all(b):
    if b not in RDT2: return []
    return [d for d in RDT2[b][1] if not (d['rw'] == 0 and d['rd'] == 0)]

def to_map(room, zi, wx, wz):
    z = ZONES.get((room, zi))
    if z is None: return None
    pg, r = z['page'], z['rect']
    R = rects(pg)[r]
    if z['sx'] > 0 and z['sy'] > 0:
        mx, my = proj_cal(z, wx, wz)
        mx = max(R[0], min(R[0]+R[2]-1, mx)); my = max(R[1], min(R[1]+R[3]-1, my))
        return pg, r, mx, my, 'cal'
    p = proj_bbox(z, wx, wz)
    if p is None: return None
    return pg, r, p[0], p[1], 'bbox'

def snap_wall(pg, r, mx, my, senk):
    R = rects(pg)
    if r >= len(R): return mx, my, (3 if senk else 0)
    RX, RY, RW, RH = R[r]
    pix = page_pix(pg)
    if pix is None: return mx, my, (3 if senk else 0)
    U, V = rect_uv(pg, r)
    def drawn(i, j):
        if not (0 <= i < RW and 0 <= j < RH): return False
        yy, xx = V+j, U+i
        if not (0 <= yy < 256 and 0 <= xx < 256): return False
        return pix[yy][xx] != 0
    QUER = 3
    i0, j0 = mx-RX, my-RY
    best = None
    if senk:
        for dj in range(-QUER, QUER+1):
            j = j0+dj
            if not (0 <= j < RH): continue
            for i in range(RW):
                if not drawn(i, j): continue
                if not (i == 0 or not drawn(i-1, j) or i == RW-1 or not drawn(i+1, j)): continue
                k = abs(i-i0)+2*abs(dj); s = 3 if drawn(i+1, j) else 1
                if best is None or k < best[0]: best = (k, i, j, s)
        if best is None: return mx, my, 3
        return RX+best[1], RY+best[2], best[3]
    for di in range(-QUER, QUER+1):
        i = i0+di
        if not (0 <= i < RW): continue
        for j in range(RH):
            if not drawn(i, j): continue
            if not (j == 0 or not drawn(i, j-1) or j == RH-1 or not drawn(i, j+1)): continue
            k = abs(j-j0)+2*abs(di); s = 0 if drawn(i, j+1) else 2
            if best is None or k < best[0]: best = (k, i, j, s)
    if best is None: return mx, my, 0
    return RX+best[1], RY+best[2], best[3]

# ---------- DURCHGANG 1 ----------
vor = []
for b in BASES:
    for n, d in enumerate(doors_all(b)):
        wx, wz = d['lx'], d['lz']
        rec = dict(room=b, idx=n, d=d, zi=None, aus=None, pg=None, r=None)
        if (b, 0) not in ZONES:
            rec['aus'] = 'raum_ohne_zone'; vor.append(rec); continue
        zi = zone_at(b, wx, wz); rec['zi'] = zi
        if zi is None:
            rec['aus'] = 'punkt_ausserhalb_aller_zonen'; vor.append(rec); continue
        mp = to_map(b, zi, wx, wz)
        if mp is None:
            rec['aus'] = 'keine_projektion'; vor.append(rec); continue
        pg, r, mx, my, mode = mp
        rec.update(pg=pg, r=r, mode=mode, roh=(mx, my))
        senk = (d['rd'] > d['rw']) if d['rw'] != d['rd'] else \
               (min(mx-rects(pg)[r][0], rects(pg)[r][0]+rects(pg)[r][2]-1-mx) <
                min(my-rects(pg)[r][1], rects(pg)[r][1]+rects(pg)[r][3]-1-my))
        mx, my, seite = snap_wall(pg, r, mx, my, senk)
        rec.update(mx=mx, my=my, seite=seite, senk=senk)
        if kachel_zeigt_tuer(pg, r, mx, my):
            rec['aus'] = 'kachel_malt_schon'
        vor.append(rec)

verl = collections.Counter(v['aus'] for v in vor)
print("\n=== DURCHGANG 1 ===")
print("Tuer-Datensaetze gesamt :", len(vor))
for k, n in verl.most_common(): print(f"   {str(k):32s} {n}")

# ---------- PAARE ----------
kand = []
for i in range(len(vor)):
    A = vor[i]
    for j in range(i+1, len(vor)):
        B = vor[j]
        if A['d']['dest'] != B['room'] or B['d']['dest'] != A['room']: continue
        d1 = abs(A['d']['nx']-B['d']['lx']) + abs(A['d']['nz']-B['d']['lz'])
        d2 = abs(B['d']['nx']-A['d']['lx']) + abs(B['d']['nz']-A['d']['lz'])
        kand.append((d1+d2, i, j, d1, d2))
paare = []; belegt = set()
for s, i, j, d1, d2 in sorted(kand):
    if i in belegt or j in belegt: continue
    belegt.add(i); belegt.add(j)
    paare.append(dict(i=i, j=j, d1=d1, d2=d2, tol=(d1 <= 4000 and d2 <= 4000)))
un = [k for k in range(len(vor)) if k not in belegt]
print("\n=== PAARBILDUNG (dest-reziprok) ===")
print("Paare gesamt              :", len(paare))
print("  davon Generator-Toleranz:", sum(1 for p in paare if p['tol']))
print("  Toleranz gerissen       :", sum(1 for p in paare if not p['tol']))
print("Ungepaarte Datensaetze    :", len(un))

# Warum ungepaart?
grund = collections.Counter()
for k in un:
    A = vor[k]; de = A['d']['dest']
    if de == A['room']: grund['SELBST-Tuer (dest == eigener Raum)'] += 1
    elif de not in RDT2: grund['Zielraum hat kein RDT / nicht in Raumliste'] += 1
    else:
        rz = [d for d in doors_all(de) if d['dest'] == A['room']]
        if not rz: grund['Zielraum hat keinen Rueckweg-Datensatz'] += 1
        else: grund['Rueckweg da, aber Greedy schon belegt'] += 1
print("  Gruende:")
for k, n in grund.most_common(): print(f"     {k:44s} {n}")
import pickle
pickle.dump(dict(vor=vor, paare=paare, un=un), open(r'analysis\karte_0901\_zensus.pkl','wb'))
