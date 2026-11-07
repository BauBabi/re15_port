#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 5: offene Mesh-Raender und ob sie frei liegen, Eltern-Kind-Ueberlappung, Schwanzende, Maske-Trefferflaechen der Hand, Textur-Durchsicht."""
import json, sys
import numpy as np
from collections import Counter
from m1_skelett import F, W, G, NAME, lade, fk, welt, FK
from m3_innen import tris_bone, pose_tris, proben, windung
from m5_paare import schnitt
sys.path.insert(0, W + '/re15_port/tools')
import form_emd as FE


def rand_kanten(t):
    c = Counter()
    for a, b, d in t:
        for u, v in ((a, b), (b, d), (d, a)):
            c[tuple(sorted((int(u), int(v))))] += 1
    return [k for k, n in c.items() if n == 1]


def main():
    rng = np.random.default_rng(3)
    for x in ['final']:
        j, f = lade(x)
        print('\n=========== Rohbau', x.upper())
        # offene Raender je Knochen (Ecken in md1 sind nicht zusammengelegt: nach Lage verschmelzen)
        OFF = {}
        for i in range(32):
            v, t = tris_bone(f, i)
            if not len(t):
                continue
            s, inv = np.unique(np.round(v).astype(int), axis=0, return_inverse=True)
            tt = inv.reshape(-1)[t]
            tt = tt[(tt[:, 0] != tt[:, 1]) & (tt[:, 1] != tt[:, 2]) & (tt[:, 0] != tt[:, 2])]
            r = rand_kanten(tt)
            OFF[i] = (s, np.array(r, int).reshape(-1, 2))
        ges = sum(len(o[1]) for o in OFF.values())
        print('Offene Randkanten gesamt: %d; Knochen mit Rand: %s' % (ges, ', '.join('%s %d' % (NAME[i], len(o[1])) for i, o in OFF.items() if len(o[1]) > 0)))
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = fk(f, c)
            M = {i: pose_tris(f, R, T, i) for i in range(32)}
            frei = {}
            for i, (s, e) in OFF.items():
                if not len(e):
                    continue
                mid = (s[e[:, 0]] + s[e[:, 1]]) / 2
                q = mid @ R[i].T + T[i]
                inn = np.zeros(len(q), bool)
                for b in range(32):
                    if b == i or not len(M[b][1]):
                        continue
                    inn |= windung(q, M[b][0], M[b][1]) > .5
                if (~inn).any():
                    frei[NAME[i]] = (int((~inn).sum()), len(q))
            print('  %s: frei liegende Randkanten (Kante nicht in anderem Teil): %s' % (nm, ', '.join('%s %d/%d' % (n, a, b) for n, (a, b) in frei.items()) or 'keine'))
            # Eltern-Kind: Abstand Ring
            ohne = []
            for i, kn in enumerate(f['knochen']):
                p = kn['eltern']
                if p < 0:
                    continue
                vk, tk = M[i]
                vp, tp = M[p]
                if not len(tk) or not len(tp):
                    continue
                q = proben(vk, tk, 500, rng)
                a = (windung(q, vp, tp) > .5).mean()
                q2 = proben(vp, tp, 500, rng)
                b = (windung(q2, vk, tk) > .5).mean()
                dd = np.sqrt(((q[:, None, :] - q2[None, :, :]) ** 2).sum(2)).min()
                if a + b < .005 and dd > 40:
                    ohne.append('%s-%s Abstand %.0f' % (NAME[p], NAME[i], dd))
            print('  %s: Eltern-Kind ohne Ueberlappung und Abstand > 40: %s' % (nm, '; '.join(ohne) or 'keine'))
        # Schwanzende
        print('Schwanz Querschnitte (Hoehe x Breite) je Abschnitt s=.5, Gerippe 2a=2b:')
        z = []
        for i in range(1, 7):
            v, t = tris_bone(f, i)
            L0, L1 = v[:, 0].min(), v[:, 0].max()
            for s_ in (.5, .9) if i == 6 else (.5,):
                s = schnitt(v, t, 0, L0 + (L1 - L0) * s_)
                z.append('%s(%.1f) %4.0f x %4.0f' % (NAME[i], s_, s[:, 1].max() - s[:, 1].min(), s[:, 2].max() - s[:, 2].min()))
        print('  ', '; '.join(z))
        # Schwanzlaenge Mesh
        t = FE.tim_lesen(open(F + '/rohbau_%s/FORM.TIM' % x, 'rb').read())
        px = t['pixel']
        c = np.array(t['cluts'])
        print('Textur: Indizes genutzt %d, Index 0 (durchsichtig bei Wort 0): %d Texel, CLUT[0]=%d' % (len(np.unique(px)), int((px == 0).sum()), int(c[0][0])))


if __name__ == '__main__':
    main()
