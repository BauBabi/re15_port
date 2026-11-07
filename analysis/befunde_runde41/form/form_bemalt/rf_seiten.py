#!/usr/bin/env python3
"""Welche Ecken eines Teils sieht welches Bild? python rf_seiten.py STAND.json [teil]"""
import json, os, sys
import numpy as np
from rf_kern import HIER
import rf_bau, rf_plan, rf_textur
st = json.load(open(os.path.join(HIER, sys.argv[1])))
name = sys.argv[2] if len(sys.argv) > 2 else 'unterarm_r'
g, T = rf_plan.plan()
form, lage, d, L, C = rf_bau.baue_form(g, T, st)
kn, pos = rf_textur.texelkarte(form)
bk = rf_textur.Bake(form, st, kn, pos, np.zeros(kn.shape, bool))
for i, (p, r) in enumerate(zip(T, lage)):
    if p.name != name:
        continue
    x, y, w, h = r
    print(p.name, r, 'U-Bins je Ecke, Anteil sichtbar Bild 1 / Bild 2')
    for j in range(p.n):
        a, b = int(x + w * j / p.n), int(x + w * (j + 1) / p.n)
        v1 = bk.vis[1][y:y + h, a:b].mean()
        v2 = bk.vis[2][y:y + h, a:b].mean()
        print('  Ecke %d..%d  %.2f  %.2f' % (j, j + 1, v1, v2))
import rf_marken
for i_, n, m, f in rf_marken.liste():
    if i_ in ('Gg', 'Gr'):
        a = bk.stelle(n, m)
        for i, (p, r) in enumerate(zip(T, lage)):
            if p.name != name:
                continue
            x, y, w, h = r
            s = a[y:y + h, x:x + w]
            v1 = bk.vis[1][y:y + h, x:x + w]
            print(i_, 'Texel', int(s.sum()), 'davon in Bild 1 sichtbar', int((s & v1).sum()), 'je Ecke', [int(s[:, int(w * j / p.n):int(w * (j + 1) / p.n)].sum()) for j in range(p.n)])
