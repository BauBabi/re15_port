"""Massstabszeile {ox, oy, sx, sy} fuer die Original-Formel FUN_800473f8 (@0x8004741c-0x80047528):
    mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
    my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
aus Paaren (Welt-Wandflaeche <-> gemalte Wandlinie) je Achse, kleinste Quadrate, dann ganzzahlig
gerundet und mit der EXAKTEN Formel nachgerechnet (C-Trunkierung wie der Port, re15_map_zones.c)."""
import numpy as np
def tdiv(a, b):  # C-Division (Richtung 0)
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q
# re15_map_zone_marker (Zeilen-Pfad): t = ((((w+32000)*10*s)>>20)+5)/10; px = (flip_x ? -t : t) + ox,
# py = (flip_z ? t2 : -t2) + oy (Spiegel fuer gedrehte Raumrahmen, re15_map_zones.c)
FLIP = {'x': 0, 'z': 0}
def _t(w, s): return tdiv((((w + 32000) * 10 * s) >> 20) + 5, 10)
def fx(w, s, o): return (-_t(w, s) if FLIP['x'] else _t(w, s)) + o
def fz(w, s, o): return (_t(w, s) if FLIP['z'] else -_t(w, s)) + o
def fit(paare, achse):
    W = np.array([p[0] for p in paare], float); P = np.array([p[1] for p in paare], float)
    sgn = (-1 if FLIP['x'] else 1) if achse == 'x' else (1 if FLIP['z'] else -1)
    A = np.vstack([sgn * (W + 32000) / 2**20, np.ones_like(W)]).T
    (s, o), *_ = np.linalg.lstsq(A, P, rcond=None)
    best = None
    for si in range(int(s) - 3, int(s) + 4):
        for oi in range(int(o) - 3, int(o) + 4):
            f = fx if achse == 'x' else fz
            res = [f(w, si, oi) - p for w, p in paare]
            sc = (max(abs(r) for r in res), sum(r * r for r in res))
            if best is None or sc < best[0]: best = (sc, si, oi, res)
    return best
if __name__ == '__main__':
    import sys, json
    d = json.loads(sys.argv[1])
    FLIP['x'] = d.get('flip_x', 0); FLIP['z'] = d.get('flip_z', 0)
    for achse in ('x', 'z'):
        sc, s, o, res = fit(d[achse], achse)
        print(f'{achse}: s {s} o {o} (Einheiten/px {2**20/s:.1f}) Reste {res} max {sc[0]}')
