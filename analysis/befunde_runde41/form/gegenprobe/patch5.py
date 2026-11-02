#!/usr/bin/env python3
"""Fuenfter Umbau: Mindestdicken Finger und Hand, Rumpf ohne Kragen und ohne Zelt."""
import os
H = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'gerippe')


def ers(t, a, b):
    assert a in t, a[:60]
    return t.replace(a, b)


p = os.path.join(H, 'fit.py')
t = open(p, encoding='utf-8').read()
t = ers(t, """    # Oberarm-Anteil und Kiefer: sieht kein Bild
""", """    # Finger: Nutzer-Umriss zeigt je Finger 45 px bei Tiefe 4800, Hand in Bild 1 48 px bei Tiefe 6900. Rumpf ohne Kragen hinten, Brust nicht spitz
    ic = RG.D_KEYS.index('klaue')
    k = k + (torch.relu(190.0 - a3[:, ic, 0]) / 30.0) ** 2 + (torch.relu(130.0 - a3[:, ic, 1]) / 30.0) ** 2 + (torch.relu(60.0 - a3[:, ic, 2]) / 20.0) ** 2
    ih = RG.D_KEYS.index('hand')
    k = k + (torch.relu(380.0 - b3[:, ih, 1]) / 30.0) ** 2 + (torch.relu(380.0 - b3[:, ih, 2]) / 30.0) ** 2
    ih = RG.D_KEYS.index('huefte')
    k = k + (torch.relu(b3[:, ih, 0] - 1.02 * b3[:, ih, 1]) / 30.0) ** 2
    ib = RG.D_KEYS.index('brust')
    k = k + (torch.relu(0.62 * a3[:, ib, 0] - a3[:, ib, 2]) / 30.0) ** 2
    # Oberarm-Anteil und Kiefer: sieht kein Bild
""")
open(p, 'w', encoding='utf-8', newline='\n').write(t)
print('ok')
