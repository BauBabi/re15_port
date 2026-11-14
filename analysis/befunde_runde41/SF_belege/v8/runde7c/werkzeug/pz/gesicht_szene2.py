"""wie gesicht_szene.py (Pruefer P5 R3), Paare per Aufruf: python gesicht_szene2.py KAND:BEZUG ..."""
import sys, numpy as np
import szene as SZ
NAME = {-2: 'fehlt', -1: 'kein Birkin', 0: 'Rest', 1: 'Mesh8 unten', 2: 'Mesh8 oben', 3: 'Kopf hinten', 5: 'Kopf-Anhang'}
for paar in sys.argv[1:]:
    kand, bez = paar.split(':')
    A, B = np.load(SZ.L_ + kand + '_kl.npz')['K'], np.load(SZ.L_ + bez + '_kl.npz')['K']
    tot = 0; m8 = 0; bilder = []
    for i in range(A.shape[0]):
        v = (B[i] == 4) & (A[i] != 4)
        if v.any():
            z = {NAME[int(c)]: int((A[i][v] == c).sum()) for c in np.unique(A[i][v])}
            bilder.append('F%d %d %s' % (225 + i, int(v.sum()), z)); tot += int(v.sum()); m8 += int(np.isin(A[i][v], (1, 2)).sum())
    print('%s gegen %s: %d Bildpunkte (960x720) in %d von %d Bildern, davon Mesh 8: %d |' % (kand, bez, tot, len(bilder), A.shape[0], m8), '; '.join(bilder[:25]))
