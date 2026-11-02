#!/usr/bin/env python3
"""Rohbau final, Clip 2: neutrale, spiegelgleiche Ruhepose. Alle Viere flach am Boden, Rumpf fast waagerecht und tief, Kopf gehoben, Schwanz laeuft aus dem Ruecken nach hinten zum Boden aus.
Nur wenige freie Winkel, links ist der Spiegel von rechts: (ax, ay, az) -> (-ax, -ay, az). python rf_ruhe.py STAND.json"""
import json
import math
import os
import sys

import numpy as np
import torch

from rf_kern import E, GERAET, HIER, Kette, Netz, namen, innen_tiefe
from rf_blick import netz_von
import rf_plan


def grad(*a):
    return [x * 4096 / 360.0 for x in a]


def start(ix):
    w = np.zeros((32, 3))
    # Rumpf: Becken wenig nach vorn geneigt, Brust waagerecht
    w[ix['huefte']] = grad(0, 0, -5)
    w[ix['wirbel']] = grad(0, 0, 3)
    w[ix['brust']] = grad(0, 0, 3)
    w[ix['hals']] = grad(0, 0, -10)
    w[ix['kopf']] = grad(0, 0, 8)
    w[ix['kiefer']] = grad(0, 0, 4)
    for n, a in (('schwanz1', -16), ('schwanz2', -12), ('schwanz3', -9), ('schwanz4', -3), ('schwanz5', 6), ('schwanz6', 8)):
        w[ix[n]] = grad(0, 0, a)
    for s, vz in (('r', -1), ('l', 1)):
        w[ix['oberarm_' + s]] = grad(vz * 12, 0, 40)
        w[ix['unterarm_' + s]] = grad(0, 0, -74)
        w[ix['hand_' + s]] = grad(0, 0, 34)
        w[ix['oberschenkel_' + s]] = grad(vz * 10, 0, -70)
        w[ix['unterschenkel_' + s]] = grad(0, 0, 118)
        w[ix['fuss_' + s]] = grad(0, 0, -44)
        for f in '1234':
            w[ix['klaue_%s%s' % (s, f)]] = grad(0, 0, 8)
    return w


def main():
    stand = json.load(open(os.path.join(HIER, sys.argv[1])))
    g, T = rf_plan.plan()
    ix = namen(g)
    nz, lr, c = netz_von(T, stand)
    v, L_, C_ = nz.ecken(lr, c)
    v = v.detach()
    L_ = [x.detach() for x in L_]
    C_ = [x.detach() for x in C_]
    ke = Kette(g)
    vers = torch.tensor([k['versatz'] for k in g['knochen']], dtype=torch.float32, device=GERAET)
    w0 = torch.tensor(start(ix), dtype=torch.float32, device=GERAET)
    wz = torch.tensor([0.0, -1300.0, 0.0], device=GERAET, requires_grad=True)
    # frei je rechter Seite: ax, az von oberarm, unterarm, hand, oberschenkel, unterschenkel, fuss; Finger az gemeinsam
    frei = ['oberarm_r', 'unterarm_r', 'hand_r', 'oberschenkel_r', 'unterschenkel_r', 'fuss_r']
    dw = torch.zeros(len(frei), 2, device=GERAET, requires_grad=True)
    dr = torch.zeros(3, device=GERAET, requires_grad=True)   # Huefte, Wirbel, Brust az
    dl = torch.zeros(3, device=GERAET, requires_grad=True)   # linker Arm: Hand ist kleiner, Wurzelhoehe gleich, also minimal anders gebeugt
    df = torch.zeros(8, device=GERAET, requires_grad=True)   # Finger az, je Hand: die rechte Hand ist groesser, Spiegel ginge durch den Boden
    ds = torch.zeros(6, device=GERAET, requires_grad=True)   # Schwanz az: M6 Abnahme 1, laeuft zum Boden
    pn = []
    for p in T:
        pn += [p.name] * (p.T * p.n + 2)
    pn = np.array(pn)
    grp = {}
    for s in 'rl':
        grp['hand_' + s] = torch.tensor(np.nonzero(np.isin(pn, ['hand_' + s, 'kralle_' + s] + ['klaue_%s%s' % (s, f) for f in '1234']))[0], device=GERAET)
        grp['fuss_' + s] = torch.tensor(np.nonzero(np.isin(pn, ['fuss_' + s, 'zeh0_' + s, 'zeh1_' + s, 'zeh2_' + s]))[0], device=GERAET)
    schw = torch.tensor(np.nonzero(np.char.startswith(pn, 'schwanz'))[0], device=GERAET)
    opt = torch.optim.Adam([dict(params=[dw], lr=3.0), dict(params=[wz], lr=6.0), dict(params=[dr, df, dl, ds], lr=2.0)])

    def winkel():
        w = w0.clone()
        for a, b in enumerate(frei):
            i = ix[b]
            j = ix[b[:-1] + 'l']
            w[i, 0] = w0[i, 0] + dw[a, 0]
            w[i, 2] = w0[i, 2] + dw[a, 1]
            w[j, 0] = w0[j, 0] - dw[a, 0]
            w[j, 2] = w0[j, 2] + dw[a, 1] + (dl[a] if a < 3 else 0.0)
        for a, b in enumerate(('huefte', 'wirbel', 'brust')):
            w[ix[b], 2] = w0[ix[b], 2] + dr[a]
        for a in range(6):
            b = ix['schwanz%d' % (a + 1)]
            w[b, 2] = w0[b, 2] + ds[a]
        for f in range(4):
            for q_, s in enumerate('rl'):
                b = ix['klaue_%s%d' % (s, f + 1)]
                w[b, 2] = w0[b, 2] + df[f + 4 * q_]
        return w
    z = torch.zeros(3, device=GERAET)
    for it in range(2500):
        opt.zero_grad()
        w = winkel()
        R, Tt = ke(w, vers, wz * torch.tensor([0, 1.0, 0], device=GERAET), 0.0, z)
        q = nz.welt(v, R, Tt)
        y = q[:, 1]
        L = 0.0
        for k, sel in grp.items():
            ys = y[sel]
            top = torch.topk(ys, 6).values.mean()
            L = L + 4 * (top / 30.0) ** 2
        L = L + (torch.relu(y + 2) / 20.0).pow(2).mean() * 80
        L = L + (torch.topk(torch.relu(y + 2), 6).values.mean() / 10.0) ** 2
        # M6: tiefster Punkt des Schwanzes rund 30 ueber dem Boden, Schwanz gleichmaessig gebogen
        L = L + 4 * ((torch.topk(y[schw], 6).values.mean() + 30.0) / 30.0) ** 2
        L = L + (ds / 150.0).pow(2).sum() + ((ds[1:] - ds[:-1]) / 60.0).pow(2).sum()
        # Haende und Fuesse flach: X-Achse waagerecht, Knochen quer auch
        for b in ('hand_r', 'hand_l', 'fuss_r'):
            L = L + 40 * (R[ix[b]][1, 0] ** 2) + 10 * (R[ix[b]][1, 2] ** 2)
        # Haende unter den Schultern, leicht vor; Fuesse unter dem Knie
        hr = Tt[ix['hand_r']]
        sr = Tt[ix['oberarm_r']]
        L = L + ((hr[0] - sr[0] - 150) / 200.0) ** 2 + ((hr[2] - sr[2] + 180) / 200.0) ** 2
        fr = Tt[ix['fuss_r']]
        hu = Tt[ix['oberschenkel_r']]
        L = L + ((fr[0] - hu[0] + 120) / 200.0) ** 2 + ((fr[2] - hu[2] + 200) / 200.0) ** 2
        # Rumpf waagerecht und tief: Bauch rund 600 ueber dem Boden, Becken wie Brust
        bauch = -(R[ix['wirbel']][1, 1] * 820 + Tt[ix['wirbel']][1])
        L = L + 3 * ((bauch - 620) / 120.0) ** 2
        L = L + (dw / 400.0).pow(2).sum() * .1 + (dr / 100.0).pow(2).sum() + (df / 100.0).pow(2).sum()
        # Glieder nicht im Rumpf
        L.backward()
        opt.step()
    # Zweiter Gang: Rumpf steht, nur Arme und Finger. Handflaeche flach am Boden, jeder Finger liegt auf, kein Punkt unter dem Boden (form_emd.boden_nachfuehren hebt sonst die ganze Figur).
    handv = {s_: torch.tensor(np.nonzero(pn == 'hand_' + s_)[0], device=GERAET) for s_ in 'rl'}
    fing = {f_: torch.tensor(np.nonzero(pn == f_)[0], device=GERAET) for f_ in ['klaue_%s%d' % (s_, i) for s_ in 'rl' for i in (1, 2, 3, 4)]}
    opt2 = torch.optim.Adam([dict(params=[dw], lr=1.5), dict(params=[dl, df], lr=1.5)])
    dw0 = dw.detach().clone()
    for it in range(1500):
        opt2.zero_grad()
        w = winkel()
        w = torch.cat([w], 0)
        R, Tt = ke(w, vers, wz.detach() * torch.tensor([0, 1.0, 0], device=GERAET), 0.0, z)
        q = nz.welt(v, R, Tt)
        y = q[:, 1]
        L = 0.0
        for s_ in 'rl':
            b = ix['hand_' + s_]
            L = L + 60 * (R[b][1, 0] ** 2) + 10 * (R[b][1, 2] ** 2)
            L = L + ((torch.topk(y[handv[s_]], 3).values.mean() + 6.0) / 8.0) ** 2
        for f_, sel in fing.items():
            L = L + .3 * ((torch.topk(y[sel], 3).values.mean() + 12.0) / 12.0) ** 2
        for k, sel in grp.items():
            L = L + 2 * (torch.topk(torch.relu(y[sel] + 2), 3).values.mean() / 6.0) ** 2
        L = L + ((dw[3:] - dw0[3:]) / 2.0).pow(2).sum() + (df / 200.0).pow(2).sum()
        L.backward()
        opt2.step()
    with torch.no_grad():
        w = winkel()
        R, Tt = ke(w, vers, wz * torch.tensor([0, 1.0, 0], device=GERAET), 0.0, z)
        q = nz.welt(v, R, Tt)
        for k, sel in grp.items():
            print(k, 'Hoehe', round(-float(q[sel, 1].max())))
        for p_ in ('klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'kralle_r', 'hand_r'):
            print('  ', p_, 'Hoehe', round(-float(q[np.nonzero(pn == p_)[0], 1].max())))
        print('Schwanz tiefster Punkt Hoehe', round(-float(q[schw, 1].max())), 'Winkel', np.round(w[[ix['schwanz%d' % a] for a in range(1, 7)], 2].cpu().numpy() * 360 / 4096, 1).tolist())
        print('tiefster Punkt', round(float(q[:, 1].max())), 'Wurzel', round(float(wz[1])), 'Bauch', round(float(-(R[ix['wirbel']][1, 1] * 820 + Tt[ix['wirbel']][1]))))
        for b in frei:
            print(b, np.round(w[ix[b]].cpu().numpy() * 360 / 4096, 1).tolist())
    json.dump(dict(winkel=w.cpu().numpy().tolist(), wurzel=[0.0, float(wz[1]), 0.0]), open(os.path.join(HIER, 'zw', 'ruhe.json'), 'w'))


if __name__ == '__main__':
    main()
