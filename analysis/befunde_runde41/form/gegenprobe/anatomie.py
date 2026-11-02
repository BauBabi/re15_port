#!/usr/bin/env python3
"""Anatomie-Pruefung aus gerippe.json allein: Winkelgrenzen, Boden, Durchdringung (Voxel), Beugerichtung, Spiegelbild.
python anatomie.py [gerippe.json]"""
import sys, json
import numpy as np
from fk import *


def dicke(k, s):
    """a(s), b(s): quadratisch durch s = 0, .5, 1."""
    d = k['dicke']
    w = np.stack([2 * (s - .5) * (s - 1), -4 * s * (s - 1), 2 * s * (s - .5)], -1)
    a = w @ np.array([q['a'] for q in d], float)
    b = w @ np.array([q['b'] for q in d], float)
    return a, b


def grenzen(g):
    aus = []
    for pose in ('bild1', 'bild2'):
        w = g['posen'][pose]['winkel']
        for k in g['knochen']:
            gr = g['winkelgrenzen_grad'][k['klasse']]
            for j, ach in enumerate('xyz'):
                lo, hi = gr[j]
                if k['seite'] == 'rechts' and j < 2:
                    lo, hi = -hi, -lo
                v = ((w[k['index']][j] + 2048) % 4096 - 2048) * 360 / 4096
                rand = min(v - lo, hi - v)
                if rand < 0.5 and not (lo == hi == 0 and v == 0):
                    aus.append((pose, k['name'], ach, round(v, 1), lo, hi, round(rand, 1)))
    return aus


def inside(k, Ri, Ti, P):
    """Bool je Punkt P[n,3]: im Kegelstumpf des Knochens mit gerundeten Enden."""
    v = (P - Ti) @ Ri     # R^T (P-T)
    a0 = np.array(k['achse_von'], float)
    a1 = np.array(k['achse_bis'], float)
    L = np.linalg.norm(a1 - a0)
    e = (a1 - a0) / L
    q = v - a0
    t = q @ e
    s = np.clip(t / L, 0, 1)
    # Querachsen: a = Y bei X-Knochen, X bei Y-Knochen; b = Z. Klauen: a = Y, b quer zu e in XZ
    if k['querachse_a'] == 'Y':
        ea = np.array([0, 1, 0.])
    else:
        ea = np.array([1, 0, 0.])
    eb = np.cross(e, ea)
    if np.linalg.norm(eb) < 1e-6:
        eb = np.array([0, 0, 1.])
    eb = eb / np.linalg.norm(eb)
    x = q @ ea
    y = q @ eb
    a, b = dicke(k, s)
    r2 = (x / a) ** 2 + (y / b) ** 2
    u = np.where(t < 0, t, np.where(t > L, t - L, 0.0))
    c = 0.75 * np.minimum(a, b)
    return r2 + (u / c) ** 2 <= 1.0


def voxel(g, R, T, schritt=40.0):
    lo = np.full(3, 1e9)
    hi = -lo
    for k in g['knochen']:
        i = k['index']
        for e in (anfang(g, R, T, i), ende(g, R, T, i)):
            lo = np.minimum(lo, e)
            hi = np.maximum(hi, e)
    lo -= 1100
    hi += 1100
    xs, ys, zs = [np.arange(lo[j], hi[j], schritt) for j in range(3)]
    X, Y, Z = np.meshgrid(xs, ys, zs, indexing='ij')
    P = np.stack([X.ravel(), Y.ravel(), Z.ravel()], 1)
    M = {}
    for k in g['knochen']:
        i = k['index']
        p0, p1 = anfang(g, R, T, i), ende(g, R, T, i)
        rmax = max(max(q['a'], q['b']) for q in k['dicke'])
        pl = np.minimum(p0, p1) - rmax * 1.8
        ph = np.maximum(p0, p1) + rmax * 1.8
        sel = np.nonzero(np.all((P >= pl) & (P <= ph), 1))[0]
        M[i] = sel[inside(k, R[i], T[i], P[sel])]
    return P, M, schritt


def namen(g, *worte):
    return [k['index'] for k in g['knochen'] if any(k['name'].startswith(w) for w in worte)]


def main():
    g = lade(sys.argv[1] if len(sys.argv) > 1 else None)
    print('--- Winkelgrenzen (Rand unter 0.5 Grad oder drueber)')
    for z in grenzen(g):
        print(z)
    for nr in (1, 2):
        pose = 'bild%d' % nr
        kam = FK.lade_kamera(nr)
        R, T = fk(g, pose)
        print('=== Pose', nr)
        print('--- Boden: tiefste Oberflaeche je Glied (Hoehe ueber Boden, positiv = schwebt)')
        P, M, sch = voxel(g, R, T)
        ix = {k['name']: k['index'] for k in g['knochen']}
        for n in ('hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'fuss_r', 'fuss_l', 'schwanz6', 'unterschenkel_r', 'unterschenkel_l'):
            pts = P[M[ix[n]]]
            print('  %-16s tief %6.0f  hoch %6.0f  Vol %5.1f' % (n, -pts[:, 1].max(), -pts[:, 1].min(), len(pts) * sch ** 3 / 1e9))
        print('  niedrigster Punkt Figur Hoehe %.0f, hoechster %.0f' % (min(-P[M[i]][:, 1].max() for i in M if len(M[i])), max(-P[M[i]][:, 1].min() for i in M if len(M[i]))))
        rumpf = np.unique(np.concatenate([M[i] for i in namen(g, 'huefte', 'wirbel', 'brust')]))
        kopf = np.unique(np.concatenate([M[i] for i in namen(g, 'hals', 'kopf', 'kiefer')]))
        print('--- Durchdringung: Anteil des Knochen-Volumens im Rumpf / Kopf (Voxel 40)')
        for k in g['knochen']:
            i = k['index']
            if k['name'] in ('huefte', 'wirbel', 'brust'):
                continue
            n = len(M[i])
            a = len(np.intersect1d(M[i], rumpf))
            b = len(np.intersect1d(M[i], kopf)) if k['name'] not in ('hals', 'kopf', 'kiefer') else 0
            if (a + b) / max(1, n) > 0.03:
                print('  %-16s im Rumpf %5.1f %%  im Kopf %5.1f %%' % (k['name'], a * 100 / max(1, n), b * 100 / max(1, n)))
        arme = {s: np.unique(np.concatenate([M[i] for i in namen(g, 'oberarm_' + s, 'unterarm_' + s, 'hand_' + s, 'klaue_' + s)])) for s in 'lr'}
        beine = {s: np.unique(np.concatenate([M[i] for i in namen(g, 'oberschenkel_' + s, 'unterschenkel_' + s, 'fuss_' + s)])) for s in 'lr'}
        schw = np.unique(np.concatenate([M[i] for i in namen(g, 'schwanz')]))

        def ue(a, b, t):
            print('  %-28s %6.1f Mio Einh3  (%.1f %% von A)' % (t, len(np.intersect1d(a, b)) * sch ** 3 / 1e6, len(np.intersect1d(a, b)) * 100 / max(1, len(a))))
        ue(beine['l'], schw, 'Bein l - Schwanz')
        ue(beine['r'], schw, 'Bein r - Schwanz')
        ue(beine['l'], beine['r'], 'Bein l - Bein r')
        ue(arme['l'], arme['r'], 'Arm l - Arm r')
        ue(arme['l'], kopf, 'Arm l - Kopf')
        ue(arme['r'], kopf, 'Arm r - Kopf')
        ue(arme['l'], beine['l'], 'Arm l - Bein l')
        ue(arme['r'], beine['r'], 'Arm r - Bein r')
        ue(kopf, rumpf, 'Kopf - Rumpf')
        print('--- Gelenke')

        def achse(n):
            i = ix[n]
            return (ende(g, R, T, i) - anfang(g, R, T, i)) / g['knochen'][i]['laenge']
        for s in 'lr':
            ob, un = achse('oberarm_' + s), achse('unterarm_' + s)
            sch1, sch2 = achse('oberschenkel_' + s), achse('unterschenkel_' + s)
            print('  Ellbogen %s Beugung %5.1f Grad; Knie %s Beugung %5.1f Grad' % (s, math.degrees(math.acos(np.clip(ob @ un, -1, 1))), s, math.degrees(math.acos(np.clip(sch1 @ sch2, -1, 1)))))
            h = achse('hand_' + s)
            print('    Hand zu Unterarm %5.1f Grad; Hand Nick %5.1f Grad; Fuss Nick %5.1f Grad' % (math.degrees(math.acos(np.clip(h @ un, -1, 1))), math.degrees(math.asin(np.clip(-h[1], -1, 1))), math.degrees(math.asin(np.clip(-achse('fuss_' + s)[1], -1, 1)))))
        Rb = R[ix['brust']]
        Rh = R[ix['huefte']]
        for n in ('oberarm_r', 'oberarm_l'):
            d = Rb.T @ achse(n)
            print('  %s Richtung im Brustraum (vorn %.2f, unten %.2f, links %.2f)' % (n, d[0], d[1], d[2]))
        for n in ('oberschenkel_r', 'oberschenkel_l'):
            d = Rh.T @ achse(n)
            print('  %s Richtung im Hueftraum (vorn %.2f, unten %.2f, links %.2f)' % (n, d[0], d[1], d[2]))
        for n in ('huefte', 'wirbel', 'brust', 'hals', 'kopf'):
            d = achse(n)
            print('  %s Achse Welt (%.2f %.2f %.2f) Nick %.1f Grad' % (n, d[0], d[1], d[2], math.degrees(math.asin(np.clip(-d[1], -1, 1)))))


if __name__ == '__main__':
    main()
