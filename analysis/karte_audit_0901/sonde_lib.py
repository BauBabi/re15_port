# -*- coding: utf-8 -*-
import os, sys, re, math, itertools, collections
ROOT = r'C:\workspace\git\reAi_v2'
os.chdir(ROOT)
src = open('re15_port/tools/gen_map_zones.py', encoding='utf-8').read().replace('if __name__', 'if False and __name__')
G = {'__file__': os.path.abspath('re15_port/tools/gen_map_zones.py'), '__name__': 'g'}
exec(compile(src, 'g', 'exec'), G)

read_rdt = G['read_rdt']; zones_of = G['zones_of']; rects = G['rects']; rect_uv = G['rect_uv']
page_pix = G['page_pix']; page_of = G['page_of']; GLYPHEN = G['GLYPHEN']; GAP = G['GAP']

# ---- Zonen aus dem GENERIERTEN Header lesen (Ist-Stand) ----
ZH = 're15_port/engine/src/re15_map_zones.h'
# Die Struktur hat seit dem 2026-09-01 zwei Felder mehr (flip_x, flip_z am Ende).
_rx = re.compile(r'\{\s*0x([0-9A-Fa-f]{4}),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)(?:,\s*(\d+),\s*(\d+))?(?:,\s*(\d+))?\s*\}')
ZONES = {}   # (base_room, zi) -> dict
_in = False
for line in open(ZH, encoding='utf-8'):
    if 's_map_zones[]' in line: _in = True; continue
    if _in and line.strip() == '};': break
    if not _in: continue
    m = _rx.search(line)
    if not m: continue
    rid = int(m.group(1), 16)
    base = rid & 0xFFF0
    if rid != base: continue          # nur Variante 0
    ZONES[(base, int(m.group(8)))] = dict(
        room=base, zi=int(m.group(8)),
        x0=int(m.group(2)), z0=int(m.group(3)), x1=int(m.group(4)), z1=int(m.group(5)),
        page=int(m.group(6)), rect=int(m.group(7)), zid=int(m.group(9)),
        ox=int(m.group(10)), oy=int(m.group(11)), sx=int(m.group(12)), sy=int(m.group(13)),
        fx=int(m.group(14) or 0), fz=int(m.group(15) or 0),
        synth=int(m.group(16) or 0))

# ---- RDT-Daten ----
RDT = {}
for base in sorted(set(k[0] for k in ZONES)):
    got = read_rdt(base) or read_rdt(base + 1)
    if got: RDT[base] = got

def doors_of(base):
    if base not in RDT: return []
    return [d for d in RDT[base][1] if not (d['rw'] == 0 and d['rd'] == 0)]

def zbox(base):
    return [(z['x0'], z['x1'], z['z0'], z['z1']) for k, z in sorted(ZONES.items()) if k[0] == base]

def zone_at(base, wx, wz):
    """kleinste passende Zone - identisch zum Generator"""
    best, best_a = None, 0
    for k, z in sorted(ZONES.items()):
        if k[0] != base: continue
        x0, x1, z0, z1 = z['x0'], z['x1'], z['z0'], z['z1']
        if x0 - GAP <= wx <= x1 + GAP and z0 - GAP <= wz <= z1 + GAP:
            a = (x1 - x0) * (z1 - z0)
            if best is None or a < best_a: best, best_a = k[1], a
    return best

def proj_cal(z, wx, wz):
    ox, oy, sx, sy = z['ox'], z['oy'], z['sx'], z['sy']
    return (((((wx + 32000) * 10 * sx) >> 20) + 5) // 10 + ox,
            -(((((wz + 32000) * 10 * sy) >> 20) + 5) // 10) + oy)

def proj_bbox(z, wx, wz):
    R = rects(z['page'])[z['rect']]
    x0, x1, z0, z1 = z['x0'], z['x1'], z['z0'], z['z1']
    if x1 <= x0 or z1 <= z0: return None
    fx = min(max(wx - x0, 0), x1 - x0); fz = min(max(wz - z0, 0), z1 - z0)
    if z.get('fx'): fx = (x1 - x0) - fx
    if z.get('fz'): fz = (z1 - z0) - fz
    return (R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0))

def gly_of(z):
    return [(gx, gy, wd) for gx, gy, gw, gh, wd in GLYPHEN.get((z['page'], z['rect']), ())]

def paarfehler(pts, gly):
    n = len(pts); m = len(gly); k = min(n, m)
    if k == 0: return None
    best = None
    for wahl in itertools.permutations(range(m), k):
        for sub in itertools.combinations(range(n), k):
            f = sum(math.hypot(pts[sub[i]][0]-gly[wahl[i]][0], pts[sub[i]][1]-gly[wahl[i]][1]) for i in range(k)) / k
            if best is None or f < best: best = f
    return best
