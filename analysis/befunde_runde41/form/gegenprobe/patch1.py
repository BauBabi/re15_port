#!/usr/bin/env python3
"""Einmaliger Umbau von gerippe/rig.py und gerippe/fit.py fuer die Gegenprobe. Originale liegen in gegenprobe/vorher/."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'rig.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "    'unterarm': ((-12, 12), (-20, 20), (-155, 12)),",
        "    'unterarm': ((-12, 12), (-20, 20), (-155, 2)),      # Gegenprobe: Ellbogen nicht ueberstrecken (frueher +12)")
t = ers(t, "    'unterschenkel': ((-12, 12), (-20, 20), (-10, 165)),",
        "    'unterschenkel': ((-12, 12), (-20, 20), (-2, 165)),  # Gegenprobe: Knie nicht ueberstrecken (frueher -10)")
t = ers(t, "# Der Oberarm gehoert in Bild 2 zu beiden Teilen: sein Ansatz liegt unter dem Rumpf, sein Ende im Armteil.",
        "# Gegenprobe: In Bild 2 gehoeren die Oberarme zu keinem Teil. Beide Arme kommen unter der Rumpfkante hervor, die Oberarme muessen verdeckt bleiben.")
t = ers(t, "'kopf', 'kiefer', 'oberarm_l', 'oberarm_r'],", "'kopf', 'kiefer'],")
t = ers(t, "'left_arm_front': ['oberarm_l', 'unterarm_l',", "'left_arm_front': ['unterarm_l',")
t = ers(t, "'right_arm_front': ['oberarm_r', 'unterarm_r',", "'right_arm_front': ['unterarm_r',")
open(p, 'w', encoding='utf-8', newline='\n').write(t)

p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "            ids = [RG.IDX[b] for b in RG.TEIL_KNOCHEN[self.nr][n] if not (self.nr == 2 and b.startswith('oberarm'))]\n",
        "            ids = [RG.IDX[b] for b in RG.TEIL_KNOCHEN[self.nr][n]]\n")
t = ers(t, """            alle = np.ones(ger.K, bool)
            alle[arme.cpu().numpy()] = False
            alle[sw] = False
            self.prim['face_and_upper_body'] = torch.tensor(np.nonzero(alle)[0], device=GER)
            self.prim_verdeckt = torch.tensor(sw, device=GER)
""", """            ob = np.nonzero(np.isin(kn, [RG.IDX['oberarm_l'], RG.IDX['oberarm_r']]))[0]    # Gegenprobe: Oberarme bleiben unter der Rumpfkante
            alle = np.ones(ger.K, bool)
            alle[arme.cpu().numpy()] = False
            alle[sw] = False
            alle[ob] = False
            self.prim['face_and_upper_body'] = torch.tensor(np.nonzero(alle)[0], device=GER)
            self.prim_verdeckt = torch.tensor(np.concatenate([sw, ob]), device=GER)
""")
t = ers(t, "def startwerte(par, ger, bilder, rng, streu=0.12):", '''_ANA = None


def ana_tabellen(ger):
    """Stationen fuer die Anatomie-Kosten (Gegenprobe): Rumpf, Glieder ausserhalb des Rumpfes, Klauen."""
    global _ANA
    if _ANA is None:
        kn = ger.st_kn.cpu().numpy()
        s = ger.st_s.cpu().numpy()
        rumpf = np.nonzero(np.isin(kn, [RG.IDX[n] for n in ('huefte', 'wirbel', 'brust')]))[0]
        gl = []
        for i, n in enumerate(RG.NAMEN):
            k = RG.klasse(n)
            if k in ('oberschenkel', 'oberarm'):
                gl += [q for q in np.nonzero(kn == i)[0] if s[q] > 0.45]
            elif k in ('unterschenkel', 'fuss', 'unterarm', 'hand'):
                gl += list(np.nonzero(kn == i)[0])
        kl = [np.nonzero(kn == RG.IDX[n])[0][-1] for n in RG.NAMEN if RG.klasse(n) == 'klaue']
        _ANA = dict(rumpf=torch.tensor(rumpf, device=GER), glied=torch.tensor(np.array(gl), device=GER), klaue=torch.tensor(np.array(kl), device=GER),
                    kopf=(ger.st_kn == RG.IDX['kopf']).nonzero()[:, 0])
    return _ANA


def anatomie(par, ger, st, bilder, a3, b3, opt, tk):
    """Was kein Bild sieht: Kopf aufrecht, Glieder nicht im Rumpf, Klauen am Boden, Oberarm nicht zur Schulter duenner, Kopfrand auf Zug Q."""
    t = ana_tabellen(ger)
    qa, qb = ger.querschnitte(a3, b3)
    rg = torch.minimum(qa, qb)[:, t['glied']]
    k = torch.zeros(par.B, dtype=DT, device=GER)
    ik = RG.IDX['kopf']
    wo = torch.tensor([0.0, -1.0, 0.0], dtype=DT, device=GER)
    for i, bi in enumerate(bilder):
        s = st[i]
        ex = s['R'][:, ik, :, 0]
        up = -s['R'][:, ik, :, 1]
        w = wo[None] - ex * (ex @ wo)[:, None]
        roll = torch.relu(math.cos(math.radians(opt.get('kopf_roll', 12.0))) * w.norm(dim=1) - (up * w).sum(1))
        k = k + 200.0 * roll ** 2
        tk['%d_kopfroll' % bi.nr] = roll.detach()
        c = s['c'][:, t['rumpf']]
        e = [s[q][:, t['rumpf']] for q in ('e0', 'e1', 'e2')]
        p = s['c'][:, t['glied']]
        d = p[:, :, None, :] - c[:, None, :, :]
        q2 = sum(((d * x[:, None]).sum(-1) / (x * x).sum(-1)[:, None]) ** 2 for x in e)
        n = torch.sqrt(q2 + 1e-6)
        abst = (n - 1.0) * d.norm(dim=-1) / n
        pen = torch.relu(0.7 * rg[:, :, None] - abst) / 100.0
        vol = (pen ** 2).sum((1, 2))
        k = k + opt.get('gew_rumpf', 0.15) * vol
        tk['%d_glied_im_rumpf' % bi.nr] = vol.detach()
        tief = s['c'][..., 1] + s['ry']
        kl = (tief[:, t['klaue']] / 60.0) ** 2
        k = k + opt.get('gew_klaue', 0.1) * kl.sum(1)
        tk['%d_klaue' % bi.nr] = kl.sum(1).detach()
        if bi.nr == 2 and opt.get('kopfrand'):
            ar = opt['kopfrand']
            sel = t['kopf']
            sd, _ = RG.abstand(ar[0], ar[1], s['u'][:, sel], s['v'][:, sel], *[q[:, sel] for q in s['q']], s['reff'][:, sel])
            rd = (sd.min(1).values / 6.0) ** 2
            k = k + rd.mean(1)
            tk['2_kopfrand'] = rd.mean(1).detach()
    io = RG.D_KEYS.index('oberarm')
    k = k + (torch.relu(0.85 * a3[:, io, 2] - a3[:, io, 0]) / 40.0) ** 2
    return k


def startwerte(par, ger, bilder, rng, streu=0.12):''')
t = ers(t, """    summe = summe + gew['stetig'] * (sw_l[:, 1:] - sw_l[:, :-1]).pow(2).sum(1)
    if details:""", """    summe = summe + gew['stetig'] * (sw_l[:, 1:] - sw_l[:, :-1]).pow(2).sum(1)
    if opt.get('anatomie'):
        summe = summe + anatomie(par, ger, st, bilder, a3, b3, opt, tk)
    if details:""")
t = ers(t, """    a.add_argument('--punkt', action='append', default=[], help='bild,knochen,start|ende,x,y,sigma: Bildpunkt als weiche Vorgabe')
""", """    a.add_argument('--punkt', action='append', default=[], help='bild,knochen,start|ende,x,y,sigma: Bildpunkt als weiche Vorgabe')
    a.add_argument('--anatomie', action='store_true', help='Gegenprobe: Kopf aufrecht, Glieder nicht im Rumpf, Klauen am Boden')
    a.add_argument('--kopfrand', action='store_true', help='Kopfumriss auf Zug Q (Vorgabe)')
    a.add_argument('--gew', default='', help='name=wert,...: Gewichte der Anatomie-Kosten (gew_rumpf, gew_klaue, kopf_roll)')
""")
t = ers(t, """    print('Primitive', ger.K, 'Pixel', [len(b.px) for b in fein], opt, flush=True)
""", """    opt['anatomie'] = a.anatomie
    for q in a.gew.split(','):
        if q:
            k, w = q.split('=')
            opt[k] = float(w)
    if a.kopfrand:
        fb = json.load(open(os.path.join(F, 'formblatt.json'), encoding='utf-8'))
        zug = np.array([[262, 225], [268, 212], [277, 206], [286, 183]] + fb['zeichnung']['zug_q']['linie'][:-1], float)
        opt['kopfrand'] = (torch.tensor(zug[:, 0], dtype=DT, device=GER), torch.tensor(zug[:, 1], dtype=DT, device=GER))
    print('Primitive', ger.K, 'Pixel', [len(b.px) for b in fein], {k: v for k, v in opt.items() if k != 'kopfrand'}, flush=True)
""")
t = ers(t, """    opt2 = dict(hand2=opt['hand2'], punkte=opt['punkte'], zwang={k: (v.cpu().numpy().tolist() if torch.is_tensor(v) else v) for k, v in opt['zwang'].items()})
""", """    opt2 = dict(hand2=opt['hand2'], punkte=opt['punkte'], zwang={k: (v.cpu().numpy().tolist() if torch.is_tensor(v) else v) for k, v in opt['zwang'].items()},
                anatomie=bool(opt.get('anatomie')), kopfrand=bool(opt.get('kopfrand')), gew=a.gew)
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
