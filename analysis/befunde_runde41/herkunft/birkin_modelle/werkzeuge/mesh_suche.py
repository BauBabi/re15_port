#!/usr/bin/env python3
"""Suche je M1-Mesh ueber ALLE Modelle (RE2 CDEMD0 alle kinds, Disc- und Mod-EMS alle Blobs): Topologie-Gleichheit
(V/T/Q) und Lagepassung (mittlerer Abstand naechster Nachbar, Translation zugelassen und nicht)."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from blobs_kette import kette
import numpy as np
M1 = zerlege(blob('M1'))
ALLE = []
for kind in range(0x10, 0x5B):
    e, s, l = re2_rec(kind, 3)
    if not e: continue
    try:
        ALLE.append((f'RE2 k{kind:02X}', zerlege(e)))
    except Exception as ex:
        print('RE2', hex(kind), 'nicht lesbar', ex)
for nm, pf in (('Disc0', DISC+'CDEMD0.EMS'), ('Disc1', DISC+'CDEMD1.EMS'), ('Mod0', MOD+'CDEMD0.EMS'), ('Mod1', MOD+'CDEMD1.EMS')):
    a = open(pf, 'rb').read()
    for s, ln, c in kette(a):
        try:
            ALLE.append((f'{nm} S{s}', zerlege(a[s*2048:s*2048+ln])))
        except Exception as ex:
            print(nm, s, 'nicht lesbar', ex)
print(len(ALLE), 'Modelle')
def arr(m):
    return np.array([v[:3] for v in m.tv], float)
for i in (0, 7, 15, 16):
    m = M1.meshes[i]
    A = arr(m)
    print(f'=== M1 mesh {i}: V={len(A)} T={len(m.tf)} Q={len(m.qf)} Box {A.min(0)}..{A.max(0)}')
    res = []
    for nm, q in ALLE:
        if nm.startswith('Mod') and ' S2012' in nm or ' S2015' in nm: continue
        for j, n in enumerate(q.meshes):
            if not n.tv: continue
            B = arr(n)
            # NN-Abstand beidseitig ohne Verschiebung
            d = np.sqrt(((A[:, None, :] - B[None, :, :])**2).sum(-1))
            ab = d.min(1); ba = d.min(0)
            res.append((ab.mean()+ba.mean(), (ab < 6).mean(), (ba < 6).mean(), nm, j, len(B), len(n.tf), len(n.qf)))
    res.sort()
    for r in res[:8]:
        print(f'   Abstand {r[0]:7.1f} | Anteil M1 nahe {r[1]:.2f} | Anteil Kand. nahe {r[2]:.2f} | {r[3]} mesh {r[4]} V={r[5]} T={r[6]} Q={r[7]}')
    print('   hoechste Teilabdeckung (Anteil M1 < 6 Einheiten an einem Kandidaten):')
    res.sort(key=lambda r: -r[1])
    for r in res[:8]:
        print(f'   Anteil {r[1]:.2f} Gegenanteil {r[2]:.2f} | {r[3]} mesh {r[4]} V={r[5]} T={r[6]} Q={r[7]}')
