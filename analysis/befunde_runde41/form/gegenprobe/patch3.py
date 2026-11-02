#!/usr/bin/env python3
"""Dritter Umbau: Anker im Raum fuer den Vorlauf, Masse setzen, Kopf ohne Trichter, Kiefer vorn, Oberarm-Anteil."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'rig.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "'kopf': (280, 750, 300, 800),", "'kopf': (280, 680, 300, 800),")
open(p, 'w', encoding='utf-8', newline='\n').write(t)

p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """    io = RG.D_KEYS.index('oberarm')
    k = k + (torch.relu(0.85 * a3[:, io, 2] - a3[:, io, 0]) / 40.0) ** 2
    return k
""", """    io = RG.D_KEYS.index('oberarm')
    k = k + (torch.relu(0.85 * a3[:, io, 2] - a3[:, io, 0]) / 40.0) ** 2
    # Kopf ohne Trichter: Ende nicht dicker als die Mitte
    ik = RG.D_KEYS.index('kopf')
    k = k + (torch.relu(a3[:, ik, 2] - a3[:, ik, 1]) / 30.0) ** 2 + (torch.relu(b3[:, ik, 2] - b3[:, ik, 1]) / 30.0) ** 2
    # Oberarm-Anteil und Kiefer: sieht kein Bild
    L = {n: lae[:, RG.L_KEYS.index(n)] for n in ('oberarm', 'unterarm', 'kopf', 'kiefer')}
    k = k + 40.0 * torch.relu(opt.get('oberarm_anteil', 0.72) - L['oberarm'] / L['unterarm']) ** 2
    kx = ver[:, RG.V_KEYS.index('kiefer_x')]
    k = k + ((kx + L['kiefer'] - 0.92 * L['kopf']) / 60.0) ** 2 + (torch.relu(250.0 - kx) / 60.0) ** 2
    return k
""")
t = ers(t, "def anatomie(par, ger, st, bilder, a3, b3, opt, tk):", "def anatomie(par, ger, st, bilder, lae, ver, a3, b3, opt, tk):")
t = ers(t, "summe = summe + anatomie(par, ger, st, bilder, a3, b3, opt, tk)", "summe = summe + anatomie(par, ger, st, bilder, lae, ver, a3, b3, opt, tk)")
t = ers(t, """        for nr, kn, wo, x, y, sig in opt.get('punkte', []):
            if nr == 2:
                sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                k = k + ((st['u'][:, j] - x) ** 2 + (st['v'][:, j] - y) ** 2) / (2.0 * sig) ** 2
""", """        for nr, kn, wo, x, y, sig in opt.get('punkte', []):
            if nr == 2 and not opt.get('anker3d'):
                sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
                j = sel[0] if wo == 'start' else sel[-1]
                k = k + ((st['u'][:, j] - x) ** 2 + (st['v'][:, j] - y) ** 2) / (2.0 * sig) ** 2
        for kn, wo, Pw, sig in opt.get('anker3d', []):
            sel = (ger.st_kn == RG.IDX[kn]).nonzero()[:, 0]
            j = sel[0] if wo == 'start' else sel[-1]
            k = k + ((st['c'][:, j] - Pw) ** 2).sum(1) / sig ** 2
""")
t = ers(t, """    a.add_argument('--vorpunkte', type=int, default=0, help='Schritte Vorlauf nur Pose 2 auf die Punkte')
""", """    a.add_argument('--vorpunkte', type=int, default=0, help='Schritte Vorlauf nur Pose 2 auf die Punkte')
    a.add_argument('--anker3d', default='', help='JSON: Liste [knochen, start|ende, x, y, tiefe, sigma] fuer den Vorlauf, Bild 2')
    a.add_argument('--setze', default='', help='name=wert,...: Laengen setzen (Startwert)')
""")
t = ers(t, """    if a.gier2start:
        ws = [float(x) for x in a.gier2start.split(',')]
""", """    if a.setze:
        with torch.no_grad():
            for q in a.setze.split(','):
                k, w = q.split('=')
                par.l.data[:, RG.L_KEYS.index(k)] = math.log(float(w) / RG.LAENGE0[k])
    if a.anker3d:
        kam = grob[1].kam
        liste = []
        for kn, wo, x, y, d, sig in json.load(open(a.anker3d, encoding='utf-8')):
            auge, r = FK.strahl(kam, np.array([x, y], float))
            Pw = auge + r * (d / float(r @ kam['R'][2]))
            liste.append((kn, wo, torch.tensor(Pw, dtype=DT, device=GER)[None], float(sig)))
        opt['anker3d'] = liste
    if a.gier2start:
        ws = [float(x) for x in a.gier2start.split(',')]
""")
t = ers(t, """    print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
""", """    print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
    opt.pop('anker3d', None)
""") if False else t
t = t.replace("""        print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
""", """        print('Vorlauf Punkte', np.round(vorlauf2(par, ger, grob, opt, a.vorpunkte).cpu().numpy(), 1).tolist(), flush=True)
        opt.pop('anker3d', None)
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
