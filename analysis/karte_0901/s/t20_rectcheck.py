# -*- coding: utf-8 -*-
"""Der Tuer-Abstand der Stub-Raeume ist 30-190 px. Das ist zu gross fuer einen
   Versatzfehler INNERHALB eines 16..88-px-Rechtecks. Pruefung: gibt es auf der Seite
   ein ANDERES Rechteck, auf dem die Tuer passt?  Anker = die Tuer selbst (Versatz),
   Massstab = Stage-Median.  Unabhaengige Gegengroessen: Klemmrate + Symbolabstand."""
import sys, math, statistics, itertools
sys.path.insert(0, r'analysis\karte_0901\s')
from base import *

def stage_med(st):
    v = [zeile(x)[2] for x in ECHT if x >> 12 == st]
    return (int(statistics.median([x[2] for x in v])), int(statistics.median([x[3] for x in v]))) if v else None

def pg_of(r):
    for k in zonen(r):
        if ZONES[k]['sx'] > 1: return ZONES[k]['page']
    ks = zonen(r); return ZONES[ks[0]]['page'] if ks else None
PG = {r: pg_of(r) for r in ROOMS}
D  = {r: doors_of(r) for r in ROOMS}

def kx(wx, sx): return ((((wx + 32000) * 10 * sx) >> 20) + 5) // 10
def kz(wz, sy): return -((((wz + 32000) * 10 * sy) >> 20) + 5) // 10

def anker(r, k):
    """Stuetzpunkte (welt_x, welt_z, karte_x, karte_y) aus Tueren zu geeichten Nachbarn."""
    S = []
    for da in D[r]:
        Bd = da['dest'] & 0xFFF0
        if Bd == r or Bd not in ECHT: continue
        if PG.get(Bd) is None or PG[Bd] != PG[r]: continue
        if zone_at(r, da['lx'], da['lz']) != k[1]: continue
        oB = zeile(Bd)[2]
        c = [db for db in D.get(Bd, ()) if (db['dest'] & 0xFFF0) == r]
        if c:
            db = min(c, key=lambda d: (d['lx'] - da['nx']) ** 2 + (d['lz'] - da['nz']) ** 2)
            q = proj(db['lx'], db['lz'], *oB)
        else:
            q = proj(da['nx'], da['nz'], *oB)
        S.append((da['lx'], da['lz'], q[0], q[1], Bd))
    return S

def sym(r, k, pg, ri, zl):
    gl = [(g[0], g[1]) for g in GLYPHEN.get((pg, ri), ())]
    dp = [proj(d['lx'], d['lz'], *zl) for d in doors_of(r) if zone_at(r, d['lx'], d['lz']) == k[1]]
    if not gl or not dp: return None
    n = len(dp); m = len(gl); kk = min(n, m); best = None
    for w in itertools.permutations(range(m), kk):
        for s in itertools.combinations(range(n), kk):
            f = sum(math.hypot(dp[s[t]][0] - gl[w[t]][0], dp[s[t]][1] - gl[w[t]][1]) for t in range(kk)) / kk
            if best is None or f < best: best = f
    return best

print('%-5s %-5s %-8s | %s' % ('Raum', 'Port', 'TuerFhl', 'alle Rechtecke der Seite: rc(Tuerfehler px / Klemm% / Sym px)'))
for r in STUB:
    k = zonen(r)[0]
    for kk2 in zonen(r):
        if ZONES[kk2]['sx'] > 1: k = kk2
    S = anker(r, k)
    if not S: continue
    pg = PG[r]; sm = stage_med(r >> 12)
    if sm is None: continue
    P = pts_of(k, 4)
    zeilen = []
    for ri, R in enumerate(rects(pg)):
        if R[2] <= 0 or R[3] <= 0: continue
        ox = int(round(statistics.mean([m - kx(w, sm[0]) for (w, _, m, _, _) in S])))
        oy = int(round(statistics.mean([m - kz(w, sm[1]) for (_, w, _, m, _) in S])))
        zl = (ox, oy, sm[0], sm[1])
        fhl = statistics.mean([math.hypot(proj(a, c, *zl)[0] - mx, proj(a, c, *zl)[1] - my)
                               for (a, c, mx, my, _) in S])
        Q = [proj(a, c, *zl) for a, c in P]
        inn = sum(1 for p in Q if R[0] <= p[0] <= R[0] + R[2] - 1 and R[1] <= p[1] <= R[1] + R[3] - 1)
        kl = 100 * (1 - inn / len(Q))
        sy = sym(r, k, pg, ri, zl)
        zeilen.append((kl, ri, fhl, sy))
    # Der Tuer-Anker ist rechteck-UNABHAENGIG - er bestimmt EINE Position auf der Seite.
    # Die Frage ist also: auf welches Rechteck faellt diese Position?
    zeilen.sort()
    best = zeilen[0]
    txt = ' '.join('%d(%.0f/%.0f/%s)' % (ri, fhl, kl, ('%.0f' % sy) if sy is not None else '-')
                   for (kl, ri, fhl, sy) in zeilen[:4])
    print('%04X  %2d/%-2d %8.1f | %s' % (r, ZONES[k]['page'], ZONES[k]['rect'], best[2], txt))
