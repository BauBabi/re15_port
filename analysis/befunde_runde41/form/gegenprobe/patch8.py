#!/usr/bin/env python3
"""Achter Umbau: Gesicht in Bild 2 zur Kamera (Nutzer: von vorn), Stand-Datum in abgabe.py."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """        tk['%d_kopfroll' % bi.nr] = roll.detach()
""", """        tk['%d_kopfroll' % bi.nr] = roll.detach()
        if bi.nr == 2:
            k = k + 40.0 * torch.relu((ex * bi.vorn[None]).sum(1) + math.cos(math.radians(opt.get('kopf_blick', 15.0)))) ** 2
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)

p = os.path.join(H, 'abgabe.py')
t = open(p, encoding='utf-8').read()
t = ers(t, "aus = dict(stand='2026-10-31',", "aus = dict(stand='2026-11-01',")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
