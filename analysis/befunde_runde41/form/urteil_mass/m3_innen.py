#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 3: sichtbare Pixel je Knochen; Glieder im Rumpf (Windungszahl echte Meshes); Luecken an Gelenken.
Windungszahl: Summe der Raumwinkel je Dreieck / 4 pi. Bei offenen Enden gilt |w| > 0.5 als innen. Faecher auf Torch-GPU, sonst numpy."""
import json, sys
import numpy as np
from m1_skelett import F, W, NAME, lade, fk, welt
sys.path.insert(0, W + '/re15_port/tools')
import form_pruefung as P  # noqa

try:
    import torch
    GER = 'cuda' if torch.cuda.is_available() else 'cpu'
except Exception:
    torch = None
    GER = None

GRUPPEN = dict(rumpf=[0, 7, 10], kopf=[21, 22, 23], schwanz=[1, 2, 3, 4, 5, 6])
GLIEDER = dict(oberarm_l=[11], unterarm_l=[12], hand_l=[13, 14, 15, 28, 30], oberarm_r=[16], unterarm_r=[17], hand_r=[18, 19, 20, 29, 31],
               oberschenkel_l=[8], unterschenkel_l=[24], fuss_l=[25], oberschenkel_r=[9], unterschenkel_r=[26], fuss_r=[27], hals=[21], kopf=[22], kiefer=[23])


def tris_bone(f, i):
    m = f['meshes'][i]
    e = np.array(m['ecken'], float)
    t = []
    for d in m['dreiecke']:
        t.append(d['e'])
    for q in m['vierecke']:
        a, b, c, d = q['e']
        t += [[a, b, d], [a, d, c]]
    return e, np.array(t, int) if t else np.zeros((0, 3), int)


def pose_tris(f, R, T, i):
    e, t = tris_bone(f, i)
    return (e @ R[i].T + T[i]), t


def proben(v, t, n, rng):
    """Flaechengleich verteilte Punkte."""
    a, b, c = v[t[:, 0]], v[t[:, 1]], v[t[:, 2]]
    fl = np.linalg.norm(np.cross(b - a, c - a), axis=1)
    if fl.sum() <= 0:
        return np.zeros((0, 3))
    w = rng.choice(len(t), n, p=fl / fl.sum())
    u = rng.random((n, 2))
    u[u.sum(1) > 1] = 1 - u[u.sum(1) > 1]
    return a[w] + u[:, :1] * (b[w] - a[w]) + u[:, 1:] * (c[w] - a[w])


def windung(q, v, t):
    """q Nx3, v, t Mesh. Rueckgabe: |Windungszahl| je Punkt."""
    if torch is None:
        raise SystemExit('torch fehlt')
    Q = torch.tensor(q, dtype=torch.float32, device=GER)
    A = torch.tensor(v[t[:, 0]], dtype=torch.float32, device=GER)
    B = torch.tensor(v[t[:, 1]], dtype=torch.float32, device=GER)
    C = torch.tensor(v[t[:, 2]], dtype=torch.float32, device=GER)
    aus = []
    for s in range(0, len(Q), 2000):
        p = Q[s:s + 2000][:, None, :]
        a, b, c = A[None] - p, B[None] - p, C[None] - p
        la, lb, lc = a.norm(dim=2), b.norm(dim=2), c.norm(dim=2)
        z = (a * torch.cross(b, c, dim=2)).sum(2)
        n = la * lb * lc + (a * b).sum(2) * lc + (a * c).sum(2) * lb + (b * c).sum(2) * la
        w = 2 * torch.atan2(z, n)
        aus.append((w.sum(1) / (4 * np.pi)).abs().cpu())
    return torch.cat(aus).numpy()


def sichtbar(x):
    k = P.Kandidat(F + '/urteil_mass/streng_%s/form.json' % x)
    aus = {}
    for n in (1, 2):
        zu, _ = P.teil_zuordnung(k, n)
        tb = P.teilbild(k, n, zu)
        kn = tb['kn']
        aus[n] = {int(b): int((kn == b).sum()) for b in range(32) if (kn == b).any()}
        np.save(F + '/urteil_mass/kn_%s_%d.npy' % (x, n), kn)
    return aus


def main():
    rng = np.random.default_rng(1)
    E = {}
    for x in 'abc':
        j, f = lade(x)
        print('\n=========== Rohbau', x.upper())
        vis = sichtbar(x)
        for n in (1, 2):
            s = vis[n]
            gr = {'Rumpf': [0, 7, 10], 'Schwanz': [1, 2, 3, 4, 5, 6], 'Hals/Kopf/Kiefer': [21, 22, 23], 'Oberarm L': [11], 'Unterarm+Hand L': [12, 13, 14, 15, 28, 30], 'Oberarm R': [16],
                  'Unterarm+Hand R': [17, 18, 19, 20, 29, 31], 'Bein L': [8, 24, 25], 'Bein R': [9, 26, 27]}
            print('Bild %d sichtbare Pixel: %s' % (n, ', '.join('%s %d' % (g, sum(s.get(b, 0) for b in l)) for g, l in gr.items())))
        ERG = {}
        for c, nm in ((0, 'Pose 1'), (1, 'Pose 2'), (2, 'Ruhe')):
            R, T, k = fk(f, c)
            M = {i: pose_tris(f, R, T, i) for i in range(32)}
            # Koerper-Solids
            print('%s: Anteil der Oberflaeche im Rumpf / im Kopf / im Schwanz' % nm)
            for g, l in GLIEDER.items():
                vv, tt = [], []
                of = 0
                for i in l:
                    v, t = M[i]
                    vv.append(v)
                    tt.append(t + of)
                    of += len(v)
                v, t = np.vstack(vv), np.vstack(tt)
                q = proben(v, t, 1500, rng)
                zeile = []
                for gn, gl in GRUPPEN.items():
                    if set(gl) & set(l):
                        zeile.append(None)
                        continue
                    inn = np.zeros(len(q), bool)
                    for b in gl:
                        vb, tb_ = M[b]
                        if len(tb_):
                            inn |= windung(q, vb, tb_) > 0.5
                    zeile.append(float(inn.mean()))
                ERG.setdefault(nm, {})[g] = zeile
                print('   %-16s %s' % (g, '  '.join('-   ' if z is None else '%.2f' % z for z in zeile)))
            # Fremddurchdringung Gelenkmitte: liegt die Knochenachse im Rumpf
            print('   Gelenkpunkte im Rumpf:', end=' ')
            pkt = np.array(T, float)
            inn = np.zeros(len(pkt), bool)
            for b in GRUPPEN['rumpf']:
                vb, tb_ = M[b]
                inn |= windung(pkt, vb, tb_) > .5
            print(', '.join(NAME[i] for i in range(32) if inn[i] and i not in (0, 7, 10)))
            # Luecke Eltern-Kind: naechster Abstand der Ecken
            lue = []
            for i, kn in enumerate(f['knochen']):
                p = kn['eltern']
                if p < 0 or not len(M[i][0]) or not len(M[p][0]):
                    continue
                a, b = M[i][0], M[p][0]
                d = np.sqrt(((a[:, None, :] - b[None, :, :]) ** 2).sum(2)).min()
                lue.append((float(d), NAME[i]))
            lue.sort(reverse=True)
            print('   groesste Luecken Eltern-Kind (Ecke zu Ecke):', ', '.join('%s %.0f' % (n_, d) for d, n_ in lue[:5]))
        E[x] = dict(sichtbar=vis, innen=ERG)
    json.dump(E, open(F + '/urteil_mass/m3.json', 'w'), indent=1)


if __name__ == '__main__':
    main()
