# -*- coding: utf-8 -*-
"""Vollstaendiger Nachbau von Durchgang 1-3 aus dem IST-Header; muss s_map_marks[] exakt treffen."""
import sys, os, re, pickle, collections, math, csv
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901')
from sonde_lib import *
import sonde_lib as SL
kz_rect = SL.G['kachel_zeigt_tuer']; read_rdt = SL.G['read_rdt']

room_ids = []
for line in open(r're15_port\include\re15_room_list.h'):
    for tok in line.replace(',', ' ').split():
        if tok.startswith('0x') and len(tok) == 6:
            try: room_ids.append(int(tok, 16))
            except ValueError: pass
BASES = sorted(set(r & 0xFFF0 for r in room_ids))
RD = {}
for b in BASES:
    g = read_rdt(b) or read_rdt(b + 1)
    if g: RD[b] = g

def doors_a(b):
    if b not in RD: return []
    return [d for d in RD[b][1] if not (d['rw'] == 0 and d['rd'] == 0)]

def stairs_a(b):
    return list(RD[b][2]) if b in RD else []

assign = {(b, zi): (z['page'], z['rect']) for (b, zi), z in ZONES.items()}
eich = {(b, zi): (z['ox'], z['oy'], z['sx'], z['sy']) for (b, zi), z in ZONES.items() if z['sx'] > 0}

def _proj(wx, wz, ox, oy, sx, sy):
    return (((((wx + 32000) * 10 * sx) >> 20) + 5) // 10 + ox,
            -(((((wz + 32000) * 10 * sy) >> 20) + 5) // 10) + oy)

def to_map(room, zi, wx, wz):
    if (room, zi) not in assign: return None
    pg, r = assign[(room, zi)]
    R = rects(pg)[r]
    if (room, zi) in eich:
        ox, oy, sx, sy = eich[(room, zi)]
        mx, my = _proj(wx, wz, ox, oy, sx, sy)
        return pg, r, max(R[0], min(R[0] + R[2] - 1, mx)), max(R[1], min(R[1] + R[3] - 1, my))
    z = ZONES[(room, zi)]
    x0, x1, z0, z1 = z['x0'], z['x1'], z['z0'], z['z1']
    if x1 <= x0 or z1 <= z0: return None
    fx = min(max(wx - x0, 0), x1 - x0); fz = min(max(wz - z0, 0), z1 - z0)
    return pg, r, R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0)

def snap_wall(pg, r, mx, my, senk):
    R = rects(pg)
    if r >= len(R): return mx, my, (3 if senk else 0)
    RX, RY, RW, RH = R[r]
    pix = page_pix(pg)
    if pix is None: return mx, my, (3 if senk else 0)
    U, V = rect_uv(pg, r)
    def dr(i, j):
        if not (0 <= i < RW and 0 <= j < RH): return False
        yy, xx = V + j, U + i
        return 0 <= yy < 256 and 0 <= xx < 256 and pix[yy][xx] != 0
    Q = 3; i0, j0 = mx - RX, my - RY; best = None
    if senk:
        for dj in range(-Q, Q + 1):
            j = j0 + dj
            if not (0 <= j < RH): continue
            for i in range(RW):
                if not dr(i, j): continue
                if not (i == 0 or not dr(i - 1, j) or i == RW - 1 or not dr(i + 1, j)): continue
                k = abs(i - i0) + 2 * abs(dj); s = 3 if dr(i + 1, j) else 1
                if best is None or k < best[0]: best = (k, i, j, s)
        return (RX + best[1], RY + best[2], best[3]) if best else (mx, my, 3)
    for di in range(-Q, Q + 1):
        i = i0 + di
        if not (0 <= i < RW): continue
        for j in range(RH):
            if not dr(i, j): continue
            if not (j == 0 or not dr(i, j - 1) or j == RH - 1 or not dr(i, j + 1)): continue
            k = abs(j - j0) + 2 * abs(di); s = 0 if dr(i, j + 1) else 2
            if best is None or k < best[0]: best = (k, i, j, s)
    return (RX + best[1], RY + best[2], best[3]) if best else (mx, my, 0)

zid_of = {(b, zi): ZONES[(b, zi)]['zid'] for (b, zi) in assign}

# ---- ETAGEN nachbauen ----
belegte = set(assign.values())
floors = []
for (b, zi) in sorted(assign):
    pg0, r0 = assign[(b, zi)]
    uv0 = rect_uv(pg0, r0)
    baender = {}
    for d in doors_a(b):
        if zone_at(b, d['lx'], d['lz']) != zi: continue
        zp = SL.page_of(d['dest'])
        if zp is None or zp == 0xd: continue
        baender.setdefault(d['band'], set()).add(zp)
    if len(baender) < 2: continue
    for band in sorted(baender):
        ziel = sorted(baender[band])
        if len(ziel) != 1: continue
        zp = ziel[0]
        tr = [i for i, _ in enumerate(rects(zp))
              if rect_uv(zp, i) == uv0 and ((zp, i) == (pg0, r0) or (zp, i) not in belegte)]
        if len(tr) != 1: continue
        floors.append((b, zi, band, zp, tr[0]))
_g = {}
for e in floors: _g.setdefault((e[0], e[1]), set()).add((e[3], e[4]))
floors = [e for e in floors if len(_g[(e[0], e[1])]) >= 2]

HF = open(r're15_port\engine\src\re15_map_zones.h', encoding='utf-8').read()
fb = HF.split('s_map_floors[] = {')[1].split('};')[0]
HFL = sorted(set((int(m.group(1), 16) & 0xFFF0, int(m.group(2)), int(m.group(3)),
                  int(m.group(4)), int(m.group(5)))
                 for m in re.finditer(r'\{\s*0x([0-9A-Fa-f]{4}),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', fb)))
print("Etagen nachgebaut:", len(sorted(set(floors))), " im Header:", len(HFL),
      " identisch:", sorted(set(floors)) == HFL)

# ---- DURCHGANG 1 ----
vor = []
for b in sorted(set(k[0] for k in assign)):
    st = stairs_a(b); st_m = {}; used = set(); cand = []
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
    for kind, lst in ((0, doors_a(b)), (1, st)):
        for n, m in enumerate(lst):
            wx = m['lx'] if kind == 0 else m['x']
            wz = m['lz'] if kind == 0 else m['z']
            zi = zone_at(b, wx, wz)
            if zi is None: continue
            if kind == 1 and n in st_m:
                pg, r, mx, my = st_m[n]
            else:
                mp = to_map(b, zi, wx, wz)
                if not mp: continue
                pg, r, mx, my = mp
            if kind == 0:
                if m['rw'] != m['rd']:
                    senk = m['rd'] > m['rw']
                else:
                    R = rects(pg)[r]
                    senk = (min(mx - R[0], R[0] + R[2] - 1 - mx) < min(my - R[1], R[1] + R[3] - 1 - my))
                mx, my, mk = snap_wall(pg, r, mx, my, senk)
                if kz_rect(pg, r, mx, my): continue
            else:
                mk = 5 if m.get('axis') == 12 else 4
            vor.append({'room': b, 'zi': zi, 'idx': n, 'kind': kind, 'pg': pg, 'r': r,
                        'mx': mx, 'my': my, 'seite': mk, 'zid': zid_of.get((b, zi), 0), 'd': m})

# ---- DURCHGANG 2 ----
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
bel = set(); paar_log = []
for _d, i, j in sorted(kand):
    if i in bel or j in bel: continue
    A, B = tu[i], tu[j]
    bel.add(i); bel.add(j)
    if A['pg'] != B['pg']:
        paar_log.append(('blatt', A, B)); continue
    def ab(X, Y):
        RX, RY, RW, RH = rects(Y['pg'])[Y['r']]
        return max(RX - X['mx'], 0, X['mx'] - (RX + RW - 1)) + max(RY - X['my'], 0, X['my'] - (RY + RH - 1))
    W = A if ab(A, B) <= ab(B, A) else B
    paar_log.append(('merge', A, B))
    for X in (A, B): X['mx'], X['my'], X['seite'] = W['mx'], W['my'], W['seite']
    W['zid2'] = (B if W is A else A)['zid']
    (B if W is A else A)['weg'] = True

# ---- DURCHGANG 3 ----
zus = []
for v in vor:
    if v.get('weg'): continue
    for (fr, fz, fb_, fpg, frr) in floors:
        if fr != v['room'] or fz != v['zi'] or (fpg, frr) == (v['pg'], v['r']): continue
        R0 = rects(v['pg'])[v['r']]; R1 = rects(fpg)[frr]
        nx = v['mx'] + (R1[0] - R0[0]); ny = v['my'] + (R1[1] - R0[1])
        if v['kind'] == 0 and kz_rect(fpg, frr, nx, ny): continue
        zus.append(dict(v, pg=fpg, r=frr, mx=nx, my=ny))
print("Etagen-Kopien (zusatz):", len(zus), " davon Tueren:", sum(1 for z in zus if z['kind'] == 0))
vor.extend(zus)
seen = set(); marks = []
for v in vor:
    if v.get('weg'): continue
    k = (v['pg'], v['r'], v['mx'], v['my'], v['seite'])
    if k in seen: continue
    seen.add(k)
    marks.append((v['pg'], v['r'], v['mx'], v['my'], v['seite'], v['zid'], v.get('zid2', 255)))
marks = sorted(marks)
mb = HF.split('s_map_marks[] = {')[1].split('};')[0]
HM = sorted(tuple(int(x) for x in m.groups()) for m in
            re.finditer(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', mb))
print("Marken nachgebaut:", len(marks), " im Header:", len(HM), " IDENTISCH:", marks == HM)
if marks != HM:
    s1 = set(marks); s2 = set(HM)
    print(" nur im Nachbau:", sorted(s1 - s2)[:12])
    print(" nur im Header :", sorted(s2 - s1)[:12])
print("Verworfen durch seen-Dedup:", (len([v for v in vor if not v.get('weg')]) - len(marks)))
pickle.dump(dict(vor=vor, floors=floors, marks=marks, paar_log=[(t, (A['room'], A['zi'], A['pg'], A['r']),
            (B['room'], B['zi'], B['pg'], B['r'])) for t, A, B in paar_log]),
            open(r'analysis\karte_0901\_nachbau.pkl', 'wb'))
