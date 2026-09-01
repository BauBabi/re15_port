# -*- coding: utf-8 -*-
"""Wie sonde_lib, aber gegen einen EINGEFRORENEN Schnappschuss von Generator + Header.
Grund: der Arbeitsbaum wurde waehrend der Messung von einem parallelen Lauf neu
generiert (re15_map_zones.h 12:19:39, 134 -> 120 Marken). Ohne Einfrieren misst man
zwei verschiedene Staende gegeneinander.
Schnappschuss: analysis/karte_0901/snapshot/ (sha256 in der Kopfzeile des Dossiers)."""
import os, sys, re, math, itertools, collections
ROOT = r'C:\workspace\git\reAi_v2'
os.chdir(ROOT)
SNAP = os.path.join(ROOT, 'analysis', 'karte_0901', 'snapshot')
src = open(os.path.join(SNAP, 'gen_map_zones.py'), encoding='utf-8').read() \
        .replace('if __name__', 'if False and __name__')
G = {'__file__': os.path.abspath('re15_port/tools/gen_map_zones.py'), '__name__': 'g'}
exec(compile(src, 'g', 'exec'), G)

read_rdt = G['read_rdt']; zones_of = G['zones_of']; rects = G['rects']; rect_uv = G['rect_uv']
page_pix = G['page_pix']; page_of = G['page_of']; GLYPHEN = G['GLYPHEN']; GAP = G['GAP']
kachel_zeigt_tuer = G['kachel_zeigt_tuer']

ZH = os.path.join(SNAP, 're15_map_zones.h')
HDR = open(ZH, encoding='utf-8').read()
_rx = re.compile(r'\{\s*0x([0-9A-Fa-f]{4}),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\}')
ZONES = {}
_in = False
for line in HDR.splitlines():
    if 's_map_zones[]' in line: _in = True; continue
    if _in and line.strip() == '};': break
    if not _in: continue
    m = _rx.search(line)
    if not m: continue
    rid = int(m.group(1), 16); base = rid & 0xFFF0
    if rid != base: continue
    ZONES[(base, int(m.group(8)))] = dict(
        room=base, zi=int(m.group(8)),
        x0=int(m.group(2)), z0=int(m.group(3)), x1=int(m.group(4)), z1=int(m.group(5)),
        page=int(m.group(6)), rect=int(m.group(7)), zid=int(m.group(9)),
        ox=int(m.group(10)), oy=int(m.group(11)), sx=int(m.group(12)), sy=int(m.group(13)))

def _blk(name):
    return HDR.split(name + '[] = {')[1].split('};')[0]
MARKS = [tuple(int(x) for x in m.groups()) for m in
         re.finditer(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}',
                     _blk('s_map_marks'))]
FLOORS = sorted(set((int(m.group(1), 16) & 0xFFF0, int(m.group(2)), int(m.group(3)),
                     int(m.group(4)), int(m.group(5))) for m in
                    re.finditer(r'\{\s*0x([0-9A-Fa-f]{4}),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}',
                                _blk('s_map_floors'))))

ROOM_IDS = []
for line in open(os.path.join(ROOT, 're15_port', 'include', 're15_room_list.h')):
    for tok in line.replace(',', ' ').split():
        if tok.startswith('0x') and len(tok) == 6:
            try: ROOM_IDS.append(int(tok, 16))
            except ValueError: pass
BASES = sorted(set(r & 0xFFF0 for r in ROOM_IDS))
RDT = {}
for b in BASES:
    g = read_rdt(b) or read_rdt(b + 1)
    if g: RDT[b] = g

def doors_of(b):
    if b not in RDT: return []
    return [d for d in RDT[b][1] if not (d['rw'] == 0 and d['rd'] == 0)]
def stairs_of(b):
    return list(RDT[b][2]) if b in RDT else []

def zone_at(base, wx, wz):
    best, best_a = None, 0
    for k, z in sorted(ZONES.items()):
        if k[0] != base: continue
        if z['x0'] - GAP <= wx <= z['x1'] + GAP and z['z0'] - GAP <= wz <= z['z1'] + GAP:
            a = (z['x1'] - z['x0']) * (z['z1'] - z['z0'])
            if best is None or a < best_a: best, best_a = k[1], a
    return best

ASSIGN = {(b, zi): (z['page'], z['rect']) for (b, zi), z in ZONES.items()}
EICH = {(b, zi): (z['ox'], z['oy'], z['sx'], z['sy'])
        for (b, zi), z in ZONES.items() if z['sx'] > 0 and z['sy'] > 0}

def proj_orig(wx, wz, ox, oy, sx, sy):
    """FUN_800473f8 @0x8004741c-0x80047528."""
    return (((((wx + 32000) * 10 * sx) >> 20) + 5) // 10 + ox,
            -(((((wz + 32000) * 10 * sy) >> 20) + 5) // 10) + oy)

def to_map(room, zi, wx, wz):
    if (room, zi) not in ASSIGN: return None
    pg, r = ASSIGN[(room, zi)]; R = rects(pg)[r]
    if (room, zi) in EICH:
        ox, oy, sx, sy = EICH[(room, zi)]
        mx, my = proj_orig(wx, wz, ox, oy, sx, sy)
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

# ---- gemalte Tuersymbole: einmal je (Seite,Rect), einmal je Seite ----
import csv as _csv
SYM_RECT = collections.defaultdict(list)
SYM_PAGE = collections.defaultdict(list)
SYM = []
with open(os.path.join(ROOT, 'analysis', 'kartensymbole', 'symbolkatalog.csv'), encoding='utf-8') as f:
    for t in _csv.DictReader(f):
        if t['typ'] not in ('TUER', 'TUER2'): continue
        g = dict(pg=int(t['seite']), r=int(t['rect']),
                 x=int(t['screen_x']) + int(t['px_w']) // 2,
                 y=int(t['screen_y']) + int(t['px_h']) // 2,
                 w=int(t['px_w']), h=int(t['px_h']), wand=t['wand'], typ=t['typ'])
        SYM.append(g); SYM_RECT[(g['pg'], g['r'])].append(g); SYM_PAGE[g['pg']].append(g)

def sym_treffer(liste, mx, my, tol=4):
    best = None
    for g in liste:
        if abs(mx - g['x']) <= tol + g['w'] // 2 and abs(my - g['y']) <= tol + g['h'] // 2:
            d = abs(mx - g['x']) + abs(my - g['y'])
            if best is None or d < best[0]: best = (d, g)
    return best[1] if best else None
