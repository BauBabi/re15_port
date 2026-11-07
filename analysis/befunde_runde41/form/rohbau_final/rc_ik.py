#!/usr/bin/env python3
"""Rohbau C, Haende und Finger auf die Spitzen des Nutzer-Umrisses. Nur Winkel von Hand und Fingern, je Pose. Schreibt zw/pose_ik.json. Ziele sind Lesungen des Umrisses (Spitzen) und Vorgaben [v] fuer die Hoehe."""
import json, os
import numpy as np, torch
from rc_kern import *
from rc_kern import GERAET, Kamera, Kette, E, HIER, lade_gerippe, namen

import sys
p = os.path.join(HIER, 'zw', 'pose_ik.json')
alt = json.load(open(p)) if os.path.exists(p) else None
if alt:
    os.rename(p, p + '.alt')
NUR = [int(x) for x in sys.argv[1:]] or [1, 2]
g = lade_gerippe()
ix = namen(g)
ke = Kette(g)
dev = GERAET
vers = torch.tensor([k['versatz'] for k in g['knochen']], dtype=torch.float32, device=dev)
LANG = {'1': 800, '2': 800, '3': 800, '4': 620}
# (Knochen, Punkt lokal in x, Bild (x,y) oder None, Hoehe oder None, Gewicht)
ZIELE = {
 2: [('hand_r', 689, (222, 420), 300, 1), ('klaue_r1', 760, (224, 474), 120, 1), ('klaue_r2', 800, (197, 476), 120, 1), ('klaue_r3', 980, (147, 352), None, 1), ('klaue_r4', 700, (274, 443), 120, 1),
     ('hand_l', 689, (438, 382), 280, 1), ('klaue_l1', 620, (461, 392), None, 1), ('klaue_l2', 760, (469, 416), 90, 1), ('klaue_l3', 700, (452, 411), 90, .5), ('klaue_l4', 640, (438, 417), 90, 1)],
 1: [('hand_r', 689, (397, 360), 240, 1), ('klaue_r1', 760, (392, 357), 60, 1), ('klaue_r2', 800, (399, 356), 60, 1), ('klaue_r3', 960, (426, 363), 120, 1), ('klaue_r4', 700, (383, 364), 60, 1),
     ('hand_l', 689, (326, 347), 240, 1), ('klaue_l1', 760, (331, 342), 60, 1), ('klaue_l2', 800, (322, 340), 60, 1), ('klaue_l3', 800, (314, 341), 60, 1), ('klaue_l4', 700, (334, 348), 60, 1)]}
FREI = ['hand_r', 'klaue_r1', 'klaue_r2', 'klaue_r3', 'klaue_r4', 'hand_l', 'klaue_l1', 'klaue_l2', 'klaue_l3', 'klaue_l4']
GR = {'hand': ((-70, 70), (-80, 80), (-110, 110)), 'klaue': ((-25, 25), (-55, 55), (-70, 95))}
aus = dict(alt) if alt else {}
for n in NUR:
    po = g['posen']['bild%d' % n]
    w0 = torch.tensor(po['winkel'], dtype=torch.float32, device=dev)
    mas = torch.zeros_like(w0)
    for b in FREI:
        mas[ix[b]] = 1
    # Start: Finger gerade, Hand wie Gerippe
    w0[[ix[b] for b in FREI if b.startswith('klaue')]] = torch.tensor([0., 0., 100.], device=dev)
    w0[[ix[b] for b in FREI if b.startswith('hand')]] = torch.tensor([0., 0., 0.], device=dev)
    dw = torch.zeros_like(w0, requires_grad=True)
    kam = Kamera(n)
    wurzel = torch.tensor(po['wurzel'], dtype=torch.float32, device=dev)
    lage = torch.tensor(po['lage'], dtype=torch.float32, device=dev)
    opt = torch.optim.Adam([dw], lr=4.0)
    for i in range(2500):
        opt.zero_grad()
        w = w0 + dw * mas
        R, T = ke(w, vers, wurzel, float(po['gier']), lage)
        L = 0.0
        for b, x, xy, h, gw in ZIELE[n]:
            i_ = ix[b]
            pt = T[i_] + R[i_][:, 0] * x
            if xy is not None:
                q = kam(pt[None])[0]
                L = L + gw * ((q - torch.tensor(xy, dtype=torch.float32, device=dev)) ** 2).sum()
            if h is not None:
                L = L + gw * ((-pt[1] - h) / 12.0) ** 2
        for b in FREI:
            k = 'hand' if b.startswith('hand') else 'klaue'
            for c, (lo, hi) in enumerate(GR[k]):
                a = w[ix[b], c] * 360 / 4096
                L = L + 0.5 * torch.relu(lo - a) ** 2 + 0.5 * torch.relu(a - hi) ** 2
            L = L + 1e-5 * (w[ix[b]] ** 2).sum()
        L.backward()
        opt.step()
    w = (w0 + dw * mas).detach()
    R, T = ke(w, vers, wurzel, float(po['gier']), lage)
    print('Bild', n, 'Rest', float(L))
    for b, x, xy, h, gw in ZIELE[n]:
        i_ = ix[b]
        pt = T[i_] + R[i_][:, 0] * x
        q = kam(pt[None])[0].cpu().numpy()
        print('  %-9s Bild (%.0f,%.0f) Soll %s Hoehe %.0f Soll %s  Winkel %s' % (b, q[0], q[1], xy, -float(pt[1]), h, np.round(w[i_].cpu().numpy() * 360 / 4096).astype(int).tolist()))
    aus[str(n)] = np.round(w.cpu().numpy()).astype(int).tolist()
json.dump(aus, open(p, 'w'))
