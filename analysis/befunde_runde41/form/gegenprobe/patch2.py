#!/usr/bin/env python3
"""Zweiter Umbau: Vorlauf nur Pose 2 auf Punkte, Gier-Starts, Restfehler zaehlt Oberarm nicht mehr als Rumpf."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "def plan(p):", '''def vorlauf2(par, ger, bilder, opt, n=400, lr=0.02):
    """Nur Pose 2: Bildpunkte aus --punkt und Kopfzwang, Grenzen, Boden. Bringt Glieder um, ohne die Bildkosten zu fragen."""
    op = torch.optim.Adam([par.ang[1], par.wurzel[1], par.gier[1]], lr=lr)
    a0 = par.ang[1].detach().clone()
    bi = bilder[1]
    for it in range(n):
        op.zero_grad(set_to_none=True)
        st = zustand(par, ger, bilder)[-1][1]
        k = torch.zeros(par.B, dtype=DT, device=GER)
        for nr, kn, wo, x, y, sig in opt.get('punkte', []):
            if nr == 2:
                sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                k = k + ((st['u'][:, j] - x) ** 2 + (st['v'][:, j] - y) ** 2) / (2.0 * sig) ** 2
        sel = (ger.st_kn == RG.IDX['kopf']).nonzero()[:, 0]
        z = opt.get('zwang', {})
        if 'kopfx2' in z:
            k = k + ((st['u'][:, sel].mean(1) - z['kopfx2']) / 8.0) ** 2 + ((st['v'][:, sel].mean(1) - z['kopfy2']) / 8.0) ** 2
        ang = par.ang[1]
        k = k + 60.0 * ((torch.relu(ger.lo - ang) ** 2 + torch.relu(ang - ger.hi) ** 2) * ger.frei).sum((1, 2))
        k = k + 0.3 * ((ang - a0) * ger.frei).pow(2).sum((1, 2))
        tief = st['c'][..., 1] + st['ry']
        k = k + 2.0 * torch.relu(tief + 10.0).pow(2).sum(1) / (60.0 ** 2)
        k.sum().backward()
        op.step()
    return k.detach()


def plan(p):''')
t = ers(t, """    a.add_argument('--anatomie', action='store_true',""", """    a.add_argument('--vorpunkte', type=int, default=0, help='Schritte Vorlauf nur Pose 2 auf die Punkte')
    a.add_argument('--gier2start', default='', help='Grad je Stapelzeile, wiederholt')
    a.add_argument('--anatomie', action='store_true',""")
t = ers(t, """    gruppen = [dict(params=list(par.ang.parameters()), lr=a.lr),""", """    if a.gier2start:
        ws = [float(x) for x in a.gier2start.split(',')]
        for b in range(par.B):
            par.gier[1].data[b] = math.radians(ws[b % len(ws)])
    if a.vorpunkte:
        print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
    gruppen = [dict(params=list(par.ang.parameters()), lr=a.lr),""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)

p = os.path.join(H, 'ansicht.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """        if nr == 2 and n == 'face_and_upper_body':
            ob = np.isin(kn, [RG.IDX['oberarm_l'], RG.IDX['oberarm_r']])
            s = s | (ob & u)
""", "")
t = ers(t, """        if nr == 2 and n != 'face_and_upper_body':
            # Oberarm liegt in beiden Teilen: seine Pixel im Oberkoerper zaehlen dort, nicht als Armueberstand
            ob = np.isin(kn, [RG.IDX['oberarm_l'], RG.IDX['oberarm_r']])
            s = s & ~(ob & m['face_and_upper_body'])
""", "")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
