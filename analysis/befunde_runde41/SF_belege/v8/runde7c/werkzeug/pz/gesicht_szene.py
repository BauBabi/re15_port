"""Pruefer P5 (D) Gesicht in echten Dumps F225-F450: Gesicht = Klasse 4 (tuerkis) im Markerlauf. Verlust = Bezug Gesicht und Kand nicht Gesicht.
Aufruf: python gesicht_szene.py"""
import numpy as np
L_ = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/laeufe/'
K = {n: np.load(L_ + n + '_kl.npz')['K'] for n in ('R3', 'R2', 'V7', 'R6', 'R4')}
NAME = {-1: 'kein Birkin', 0: 'Rest', 1: 'Mesh8 unten', 2: 'Mesh8 oben', 3: 'Kopf hinten', 5: 'Kopf-Anhang'}
for kand, bez in (('R3', 'V7'), ('R3', 'R6'), ('R3', 'R2'), ('R2', 'V7'), ('R6', 'V7'), ('R4', 'V7')):
    tot = 0; bilder = []
    for i in range(K[kand].shape[0]):
        g = K[bez][i] == 4; v = g & (K[kand][i] != 4)
        if v.any():
            z = {NAME[int(c)]: int((K[kand][i][v] == c).sum()) for c in np.unique(K[kand][i][v])}
            bilder.append('F%d %d %s' % (225 + i, int(v.sum()), z)); tot += int(v.sum())
    print('%s gegen %s: %d Bildpunkte (960x720) in %d von %d Bildern' % (kand, bez, tot, len(bilder), K[kand].shape[0]), '|', '; '.join(bilder[:25]))
