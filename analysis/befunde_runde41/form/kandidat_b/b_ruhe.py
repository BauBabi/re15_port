#!/usr/bin/env python3
"""Kandidat B: Ruhepose (Clip 2). Frei gesetzt: Stand wie Pose 1, Bein NR und Bein FL auf die gespiegelte Lage von NL und FR gezogen, damit die Figur gerade steht."""
import numpy as np, torch
from b_geom import *
import b_teile as T
import b_fit as F
from b_fit import GERAET, ablage_torch


def ruhe_winkel(fit, z0=-100.0, iters=500):
    mod = F.Modell().to(GERAET)
    mod.pos.data[:] = ablage_torch(fit['pos'][:len(mod.namen)])
    mod.logr.data[:] = torch.log(ablage_torch(fit['rad'][:len(mod.namen)]))
    mod.pos.requires_grad_(False)
    mod.logr.requires_grad_(False)
    p = {n: fit['pos'][i] for i, n in enumerate(mod.namen)}
    sp = lambda a: np.array([a[0], a[1], 2 * z0 - a[2]])    # noqa: E731
    ziele = {}
    for a, b in (('nr_e', 'nl_e'), ('nr_w', 'nl_w'), ('nr_a2', 'nl_a2'), ('nr_i2', 'nl_i2')):
        ziele[a] = sp(p[b])
    for a, b in (('fl_k', 'fr_k'), ('fl_a', 'fr_a'), ('fl_f', 'fr_f'), ('fl_t3', 'fr_t3'), ('fl_t4', 'fr_t4')):
        ziele[a] = sp(p[b])
    frei = [T.IDX[k] for k in ('nr_ober', 'nr_unter', 'nr_faust', 'nr_innen', 'fl_ober', 'fl_unter', 'fl_fuss', 'fl_zeh')]
    maske = torch.zeros_like(mod.w2)
    maske[frei] = 1
    opt = torch.optim.Adam([mod.w2], lr=0.02)
    for it in range(iters):
        po = mod.fk()
        q, _ = mod.knoten_gepost(po)
        l = (mod.w2 ** 2).sum() * 0.001
        for k, z in ziele.items():
            l = l + ((q[mod.ni[k]] - ablage_torch(z)) ** 2).sum() / 1e5
        opt.zero_grad()
        l.backward()
        mod.w2.grad *= maske
        opt.step()
    w = mod.w2.detach().cpu().numpy() * maske.cpu().numpy()
    print('Ruhepose Rest', round(float(l.item()), 3))
    return w
