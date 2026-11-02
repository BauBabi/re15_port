#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 1: Skelett, Laengen je Pose, Lage, Kette links/rechts, Chiralitaet, Blick.
Eigene Vorwaertsrechnung aus form_emd.lese_emd, unabhaengig von den Rohbau-Skripten."""
import json, os, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
sys.path.insert(0, W + '/re15_port/tools')
import form_emd as FE, form_kamera as FK, emd_ansichtsblatt as EA  # noqa
F = W + '/analysis/befunde_runde41/form'
G = json.load(open(F + '/gerippe.json', encoding='utf-8'))
NAME = {k['index']: k['name'] for k in G['knochen']}
NAME[30], NAME[31] = 'klaue_l4', 'klaue_r4'


def lade(x):
    d = F + '/rohbau_' + x
    j = json.load(open(d + '/form.json', encoding='utf-8'))
    f = FE.lese_emd(open(d + '/' + j['emd'], 'rb').read())
    return j, f


def fk(f, clip):
    """Fliesskomma-FK. R[i], T[i] im Aktorraum (Y unten)."""
    b = f['clips'][clip]['bilder'][0]
    k = f['pool'][b['kf']] if 'kf' in b else b
    ks = f['knochen']
    R, T = [None] * len(ks), [None] * len(ks)
    for i, kn in enumerate(ks):
        a = np.array(k['winkel'][i], float) * 2 * np.pi / 4096
        cx, sx, cy, sy, cz, sz = np.cos(a[0]), np.sin(a[0]), np.cos(a[1]), np.sin(a[1]), np.cos(a[2]), np.sin(a[2])
        Rx = np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]])
        Ry = np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
        Rz = np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])
        loc = Rx @ Ry @ Rz
        p = kn['eltern']
        if p < 0:
            R[i], T[i] = loc, np.array(k['wurzel'], float)
        else:
            R[i] = R[p] @ loc
            T[i] = R[p] @ np.array(kn['versatz'], float) + T[p]
    return R, T, k


def gier_y(g):
    a = g * 2 * np.pi / 4096
    return np.array([[np.cos(a), 0, np.sin(a)], [0, 1, 0], [-np.sin(a), 0, np.cos(a)]])


def welt(j, n, R, T):
    s = j['bild%d' % n]
    Y = gier_y(s['gier'])
    lage = np.array(s['lage'], float)
    return [Y @ r for r in R], [Y @ t + lage for t in T]


def proj(n, p):
    k = FK.lade_kamera(n)
    return FK.projiziere(k, p) if hasattr(FK, 'projiziere') else None


def main():
    print('Funktionen form_kamera:', [x for x in dir(FK) if not x.startswith('_')][:40])
    print('Kamera Bild1 Schluessel', list(FK.lade_kamera(1).keys()))
    aus = {}
    for x in 'abc':
        j, f = lade(x)
        ks = f['knochen']
        print('\n===== Rohbau', x.upper(), 'Knochen', len(ks), 'Clips', len(f['clips']))
        # Skelett gegen Gerippe
        ab = []
        for i, k in enumerate(ks):
            if i < 30:
                g = G['knochen'][i]
                d = np.array(k['versatz'], float) - np.array(g['versatz'], float)
                if k['eltern'] != g['eltern'] or np.abs(d).max() > 0:
                    ab.append((i, NAME[i], k['eltern'], g['eltern'], k['versatz'], g['versatz']))
        print('Abweichung Skelett gegen gerippe.json (i, name, eltern, g-eltern, versatz, g-versatz):')
        for a in ab:
            print('  ', a)
        print('  Knochen 30, 31:', ks[30], ks[31])
        # Laengen je Pose
        P = [fk(f, c) for c in range(3)]
        m = 0
        for i, k in enumerate(ks):
            if k['eltern'] < 0:
                continue
            d = [np.linalg.norm(P[c][1][i] - P[c][1][k['eltern']]) for c in range(3)]
            m = max(m, max(d) - min(d))
        print('  Gelenkabstand je Pose: groesster Unterschied %.3f Einheiten' % m)
        # Links rechts gleich?
        paare = [(8, 9), (24, 26), (25, 27), (11, 16), (12, 17), (13, 18), (14, 19), (15, 20), (28, 29), (30, 31)]
        for a, b in paare:
            va, vb = np.array(ks[a]['versatz'], float), np.array(ks[b]['versatz'], float)
            spiegel = vb * np.array([1, 1, -1])
            if np.abs(va - spiegel).max() > 1:
                print('  Links/Rechts Versatz weicht ab: %s %s gegen %s %s' % (NAME[a], ks[a]['versatz'], NAME[b], ks[b]['versatz']))
        print('  Vorzeichen Z: oberschenkel_l %d, oberschenkel_r %d, oberarm_l %d, oberarm_r %d' % (ks[8]['versatz'][2], ks[9]['versatz'][2], ks[11]['versatz'][2], ks[16]['versatz'][2]))
        # Rahmen: det je Knochen
        for c in range(3):
            d = [np.linalg.det(r) for r in P[c][0]]
            if min(d) < 0.99:
                print('  Spiegelung in Clip', c)
        aus[x] = dict(j=j)
    json.dump({}, open(F + '/urteil_mass/m1.json', 'w'))


if __name__ == '__main__':
    main()
