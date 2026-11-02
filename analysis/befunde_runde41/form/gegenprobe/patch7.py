#!/usr/bin/env python3
"""Siebter Umbau: gelesene Kante (dunkel auf dunkel) gilt nur einseitig, FORMBLATT: Silhouette kann weiter aussen liegen."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """            ka = ((sd.min(1).values / ks[None]) ** 2).mean(1)
""", """            m = sd.min(1).values
            m = torch.where(ks[None] < 5.0, m, torch.relu(m))     # belegt: beidseitig, gelesen: nur Mindestausdehnung
            ka = ((m / ks[None]) ** 2).mean(1)
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
