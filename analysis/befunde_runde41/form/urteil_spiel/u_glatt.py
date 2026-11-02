# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Versuch, eine Normale je Ecke wie RE2. Gleiches Netz, nur Normalen geglaettet (flaechengewichtet je Eckposition und Teil). Nur Blatt, die Abgaben bleiben unberuehrt."""
import copy
from u_basis import *
from u_ton import Ton, tafel


def glaetten(form):
    f2 = copy.deepcopy(form)
    n_b = len(f2['knochen'])
    for m in f2['meshes'][:n_b]:
        if not m['ecken']:
            continue
        V = np.array(m['ecken'], float)
        key = [tuple(np.round(v, 0)) for v in V]
        idx = {}
        for k in key:
            idx.setdefault(k, len(idx))
        acc = np.zeros((len(idx), 3))
        for f in m['dreiecke']:
            e = list(f['e'])
            p = V[e]
            c = np.cross(p[1] - p[0], p[2] - p[0])
            for x in e:
                acc[idx[key[x]]] += c
        for f in m['vierecke']:
            a, b, c, d = f['e']
            for e in ((a, b, d), (a, d, c)):
                p = V[list(e)]
                n = np.cross(p[1] - p[0], p[2] - p[0])
                for x in e:
                    acc[idx[key[x]]] += n
        # Richtung wie gespeichert: mit der alten Normale abgleichen
        N0 = np.array(m['normalen'], float)
        s = 0.0
        for f in m['dreiecke'] + m['vierecke']:
            for x, y in zip(f['e'], f['n']):
                s += acc[idx[key[x]]] @ N0[y]
        vz = 1.0 if s >= 0 else -1.0
        L = np.linalg.norm(acc, axis=1, keepdims=True)
        neu = vz * acc / np.maximum(L, 1e-9) * 4096
        neu = np.round(neu).astype(int)
        m['normalen'] = [tuple(int(c) for c in r) + (0,) * (len(m['normalen'][0]) - 3) for r in neu]
        for f in m['dreiecke'] + m['vierecke']:
            f['n'] = tuple(idx[key[x]] for x in f['e'])
    return f2


if __name__ == '__main__':
    t = []
    SICHT = (('links', 180, 6), ('3/4 vorn', 50, 20), ('3/4 hinten', -130, 24))
    for k in ROHBAU:
        form = lade(k)
        tim = open(K[k][1], 'rb').read()
        mo = FE.modell(FE.schreibe_emd(form, pruefen=False), tim)
        mg = FE.modell(FE.schreibe_emd(glaetten(form), pruefen=False), tim)
        tex = FB.Textur(tim)
        kf = kf_von(mo, 2)
        for s, az, el in SICHT:
            t.append(('%s %s wie abgegeben' % (k, s), tafel(mo, tex, kf, az, el, 480)))
            t.append(('%s %s eine Normale je Ecke' % (k, s), tafel(mg, tex, kf, az, el, 480)))
    FB.montage(t, 4, AUS + '/glatt_versuch.png', 'Versuch: gleiche Netze, links je wie abgegeben, rechts eine Normale je Ecke wie RE2. Ton, Ruhe')
