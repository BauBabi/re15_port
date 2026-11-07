#!/usr/bin/env python3
"""Zeilenschnitte der aufrechten Teile: je 15 Zeilen linke Kante, rechte Kante, Mitte, Breite. Quer laufende Teile spaltenweise. Schreibt lauf/teile_zeilen.json."""
import json, os
import numpy as np, cv2

F = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
T = os.path.join(F, 'nutzer_teile')
A = os.path.join(F, 'gerippe', 'lauf')


def maske(n):
    m = cv2.imread(os.path.join(T, 'maske_%s.png' % n), cv2.IMREAD_UNCHANGED)
    return (m[..., -1] if m.ndim == 3 else m) > 127


aus = {}
for n, art in (('tail', 'z'), ('left_leg', 'z'), ('right_leg', 'z'), ('right_arm', 'z'), ('left_arm_front', 'z'), ('part_of_Body', 'z'), ('face_and_upper_body', 'z'), ('right_arm_front', 's')):
    m = maske(n)
    ys, xs = np.nonzero(m)
    z = []
    if art == 'z':
        for y in range(int(ys.min()) + 5, int(ys.max()), 15):
            q = np.nonzero(m[y])[0]
            if q.size:
                z.append((y, int(q.min()), int(q.max()), round((q.min() + q.max()) / 2, 1), int(q.max() - q.min() + 1)))
    else:
        for x in range(int(xs.min()) + 5, int(xs.max()), 15):
            q = np.nonzero(m[:, x])[0]
            if q.size:
                z.append((x, int(q.min()), int(q.max()), round((q.min() + q.max()) / 2, 1), int(q.max() - q.min() + 1)))
    aus[n] = dict(art='zeilen' if art == 'z' else 'spalten', schnitte=z)
    print(n, art, ' '.join('%d:%d-%d' % (a, b, c) for a, b, c, _, _ in z))
json.dump(aus, open(os.path.join(A, 'teile_zeilen.json'), 'w', encoding='utf-8'), indent=0)
