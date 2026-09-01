# -*- coding: utf-8 -*-
import sys, math, statistics, itertools, json
sys.path.insert(0, r'analysis\karte_0901\s')
from base import *
from reg2 import bmask, raster0, best_shift


def stage_med(st, ausser=None):
    v = [zeile(x)[2] for x in ECHT if x >> 12 == st and x != ausser]
    if not v:
        return None
    return int(statistics.median([x[2] for x in v])), int(statistics.median([x[3] for x in v]))


def zone_haupt(r):
    ks = [k for k in zonen(r) if ZONES[k]['sx'] > 1]
    return (ks or zonen(r))[0]


def schaetze(k, sm, spanne=0.20, schritt=8, voll=False):
    z = ZONES[k]
    B, R = bmask(z['page'], z['rect'], voll)
    beste = None
    for sx in range(int(sm[0] * (1 - spanne)), int(sm[0] * (1 + spanne)) + 1, schritt):
        for sy in range(int(sm[1] * (1 - spanne)), int(sm[1] * (1 + spanne)) + 1, schritt):
            A, ax, ay = raster0(ZCELLS[k], sx, sy)
            o1, o2, ov = best_shift(A, ax, ay, B, R)
            u = int(A.sum()) + int(B.sum()) - ov
            v = ov / float(u)
            if beste is None or v > beste[0]:
                beste = (v, o1, o2, sx, sy)
    return beste


def _paar(dp, gl):
    if not gl or not dp:
        return None
    n = len(dp); m = len(gl); kk = min(n, m); best = None
    for wahl in itertools.permutations(range(m), kk):
        for sub in itertools.combinations(range(n), kk):
            f = sum(math.hypot(dp[sub[t]][0] - gl[wahl[t]][0], dp[sub[t]][1] - gl[wahl[t]][1])
                    for t in range(kk)) / kk
            if best is None or f < best:
                best = f
    return best


def symbolabstand(r, k, zl):
    z = ZONES[k]
    gl = [(g[0], g[1]) for g in GLYPHEN.get((z['page'], z['rect']), ())]
    dp = [proj(d['lx'], d['lz'], *zl) for d in doors_of(r) if zone_at(r, d['lx'], d['lz']) == k[1]]
    return _paar(dp, gl), len(dp), len(gl)


def pg_of(r):
    for k in zonen(r):
        if ZONES[k]['sx'] > 1:
            return ZONES[k]['page']
    ks = zonen(r)
    return ZONES[ks[0]]['page'] if ks else None


PG = {r: pg_of(r) for r in ROOMS}
D = {r: doors_of(r) for r in ROOMS}


def bb(k, wx, wz):
    z = ZONES[k]; R = rects(z['page'])[z['rect']]
    x0, x1, z0, z1 = z['x0'], z['x1'], z['z0'], z['z1']
    fx = min(max(wx - x0, 0), x1 - x0); fz = min(max(wz - z0, 0), z1 - z0)
    return (R[0] + fx * R[2] // (x1 - x0), R[1] + R[3] - 1 - fz * R[3] // (z1 - z0))


def tuerabstand(r, k, zl):
    """zl=None -> Bbox-Streckung. Nachbar immer mit seiner ECHTEN Zeile."""
    ds = []
    for da in D[r]:
        Bd = da['dest'] & 0xFFF0
        if Bd == r or Bd not in ECHT:
            continue
        if PG.get(Bd) is None or PG[Bd] != PG[r]:
            continue
        if zone_at(r, da['lx'], da['lz']) != k[1]:
            continue
        oB = zeile(Bd)[2]
        p = proj(da['lx'], da['lz'], *zl) if zl else bb(k, da['lx'], da['lz'])
        c = [db for db in D.get(Bd, ()) if (db['dest'] & 0xFFF0) == r]
        if c:
            db = min(c, key=lambda d: (d['lx'] - da['nx']) ** 2 + (d['lz'] - da['nz']) ** 2)
            q = proj(db['lx'], db['lz'], *oB)
        else:
            q = proj(da['nx'], da['nz'], *oB)
        ds.append(math.hypot(p[0] - q[0], p[1] - q[1]))
    return (statistics.mean(ds), len(ds)) if ds else (None, 0)


def klemm(k, zl):
    z = ZONES[k]; R = rects(z['page'])[z['rect']]; P = pts_of(k, 4)
    Q = [proj(a, c, *zl) if zl else bb(k, a, c) for a, c in P]
    inn = [p for p in Q if R[0] <= p[0] <= R[0] + R[2] - 1 and R[1] <= p[1] <= R[1] + R[3] - 1]
    return 100 * (1 - len(inn) / len(Q)), 100 * len(set(inn)) / float(R[2] * R[3])


def bbox_sym(r, k):
    z = ZONES[k]
    gl = [(g[0], g[1]) for g in GLYPHEN.get((z['page'], z['rect']), ())]
    dp = [bb(k, d['lx'], d['lz']) for d in doors_of(r) if zone_at(r, d['lx'], d['lz']) == k[1]]
    return _paar(dp, gl)


print('E2-Schaetzung fuer die %d STUB-Raeume (Maske ohne Palettenindex 4)' % len(STUB))
print('%-5s %-6s | %-26s %7s %7s %7s | %8s %8s | %9s %9s | %s' % (
    'Raum', 'pg/rc', 'E2-Zeile {ox,oy,sx,sy}', 'klemm%', 'fuellB%', 'fuellE%',
    'Sym_bbox', 'Sym_E2', 'Tuer_bbox', 'Tuer_E2', 'nSym/nTuer'))
out = []
for r in STUB:
    k = zone_haupt(r); z = ZONES[k]
    sm = stage_med(r >> 12)
    if sm is None:
        print('%04X  keine geeichte Stage-Referenz' % r)
        continue
    b = schaetze(k, sm)
    est = (b[1], b[2], b[3], b[4])
    kl, fu = klemm(k, est)
    kb, fb = klemm(k, None)
    se, nd, ng = symbolabstand(r, k, est)
    sb = bbox_sym(r, k)
    te, nt = tuerabstand(r, k, est)
    tb, _ = tuerabstand(r, k, None)
    out.append(dict(r=r, pg=z['page'], rc=z['rect'], est=est, iou=b[0], klemm=kl, fuell=fu,
                    fuell_bbox=fb, sym_bbox=sb, sym_e2=se, tuer_bbox=tb, tuer_e2=te,
                    nsym=ng, ntuer=nt))
    f = lambda v: ('%.1f' % v) if v is not None else '-'
    print('%04X  %2d/%-2d | {%5d,%5d,%5d,%5d} %7.1f %7.1f %7.1f | %8s %8s | %9s %9s | %d/%d' % (
        r, z['page'], z['rect'], est[0], est[1], est[2], est[3], kl, fb, fu,
        f(sb), f(se), f(tb), f(te), ng, nt))
    sys.stdout.flush()
json.dump(out, open(r'analysis\karte_0901\s\stub_e2.json', 'w'))
print()
sy_b = [o['sym_bbox'] for o in out if o['sym_bbox'] is not None]
sy_e = [o['sym_e2'] for o in out if o['sym_e2'] is not None]
tu_b = [o['tuer_bbox'] for o in out if o['tuer_bbox'] is not None]
tu_e = [o['tuer_e2'] for o in out if o['tuer_e2'] is not None]
if sy_b:
    print('Symbolabstand  Bbox Median %.1f px (n=%d)  ->  E2 Median %.1f px (n=%d)' % (
        statistics.median(sy_b), len(sy_b), statistics.median(sy_e), len(sy_e)))
if tu_b:
    print('Tuerabstand    Bbox Median %.1f px (n=%d)  ->  E2 Median %.1f px (n=%d)' % (
        statistics.median(tu_b), len(tu_b), statistics.median(tu_e), len(tu_e)))
print('Klemmrate      Bbox 0.0 %% (per Konstruktion)  ->  E2 Median %.1f %%  max %.1f %%' % (
    statistics.median([o['klemm'] for o in out]), max(o['klemm'] for o in out)))
print('Fuellgrad      Bbox Median %.1f %%  ->  E2 Median %.1f %%' % (
    statistics.median([o['fuell_bbox'] for o in out]), statistics.median([o['fuell'] for o in out])))
