#!/usr/bin/env python3
"""Profile lesen: je erzwungenem Wert Gesamtkosten und reiner Bildanteil. python profil_lesen.py lauf/pr_*.pt"""
import glob, json, os, sys
import numpy as np, torch

HIER = os.path.dirname(os.path.abspath(__file__))
aus = {}
for a in sys.argv[1:]:
    for pf in sorted(glob.glob(os.path.join(HIER, a))):
        d = torch.load(pf, weights_only=False)
        z = d['meta']['opt']['zwang']
        name = [k for k, v in z.items() if isinstance(v, list)][0]
        k = d['kosten'].numpy()
        da = np.array(d['meta']['daten'])
        print(os.path.basename(pf), name)
        print('   Wert   ', ' '.join('%8.0f' % v for v in z[name]))
        print('   Kosten ', ' '.join('%8.2f' % v for v in k))
        print('   Bild   ', ' '.join('%8.2f' % v for v in da))
        aus[name] = dict(werte=z[name], kosten=[float(x) for x in k], bild=[float(x) for x in da])
json.dump(aus, open(os.path.join(HIER, 'lauf', 'profile.json'), 'w', encoding='utf-8'), indent=0)
