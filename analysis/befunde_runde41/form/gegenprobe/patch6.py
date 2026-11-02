#!/usr/bin/env python3
"""Sechster Umbau: gemessene Kante und Verdeckungen aus FORMBLATT als Kosten, Wirbelsaeule ohne Zickzack."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """        if bi.nr == 2 and opt.get('kopfrand'):
""", """        if bi.nr == 2 and opt.get('kante'):
            # FORMBLATT Abschnitt 1.4: gemessene Kante R und Verdeckungen V1-V5 gelten weiter
            kx, ky, ks, dx, dy = opt['kante']
            sd, _ = RG.abstand(kx, ky, s['u'], s['v'], *s['q'], s['reff'])
            ka = ((sd.min(1).values / ks[None]) ** 2).mean(1)
            sd, _ = RG.abstand(dx, dy, s['u'], s['v'], *s['q'], s['reff'])
            de = ((torch.relu(sd.min(1).values + 2.0) / 3.0) ** 2).mean(1)
            k = k + opt.get('gew_kante', 3.0) * ka + opt.get('gew_deck', 3.0) * de
            tk['2_kante'] = ka.detach()
            tk['2_deckung'] = de.detach()
        wb = [RG.IDX['wirbel'], RG.IDX['brust']]
        k = k + opt.get('gew_wirbel', 3.0) * ((par.ang[i][:, wb, 1] ** 2).sum(1) + (par.ang[i][:, wb, 0] ** 2).sum(1))
        if bi.nr == 2 and opt.get('kopfrand'):
""")
t = ers(t, """    if a.kopfrand:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
""", """    if a.kante:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
        r = [q for q in fb['teile_bild2'] if q['id'] == 'R'][0]
        ku = np.array([p + [4.0 if p[0] >= 354 else 7.0] for p in r['kante']], float)
        pu = []
        for q in fb['verdeckungen_bild2']:
            if q['id'] in ('V1', 'V2', 'V3', 'V4', 'V5'):
                (x0, y0), (x1, y1) = q['stelle']
                pu += [[x0 + (x1 - x0) * w, y0 + (y1 - y0) * w] for w in np.linspace(0, 1, 14)]
        pu = np.array(pu, float)
        t_ = lambda x: torch.tensor(x, dtype=DT, device=GER)
        opt['kante'] = (t_(ku[:, 0]), t_(ku[:, 1]), t_(ku[:, 2]), t_(pu[:, 0]), t_(pu[:, 1]))
    if a.kopfrand:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
""")
t = ers(t, """    a.add_argument('--kopfrand', action='store_true', help='Kopfumriss auf Zug Q (Vorgabe)')
""", """    a.add_argument('--kopfrand', action='store_true', help='Kopfumriss auf Zug Q (Vorgabe)')
    a.add_argument('--kante', action='store_true', help='gemessene Kante R und Verdeckungen V1-V5 aus FORMBLATT')
""")
t = ers(t, "{k: v for k, v in opt.items() if k != 'kopfrand'}", "{k: v for k, v in opt.items() if k not in ('kopfrand', 'kante')}")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
