#!/usr/bin/env python3
"""Lage und Richtung je Nutzer-Teil: Mittellinie der Maske gegen die projizierte Knochenkette (aus gerippe.json).
python richtung.py [gerippe.json]"""
import sys
import numpy as np
from fk import *

KETTEN = {'tail': (1, ['schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6']),
          'left_leg': (1, ['oberschenkel_l', 'unterschenkel_l', 'fuss_l']),
          'right_leg': (1, ['oberschenkel_r', 'unterschenkel_r', 'fuss_r']),
          'right_arm': (1, ['oberarm_r', 'unterarm_r', 'hand_r']),
          'left_arm_front': (2, ['oberarm_l', 'unterarm_l', 'hand_l', 'klaue_l2']),
          'right_arm_front': (2, ['oberarm_r', 'unterarm_r', 'hand_r', 'klaue_r2'])}


def main():
    g = lade(sys.argv[1] if len(sys.argv) > 1 else None)
    ms = json.load(open(os.path.join(F, 'gerippe', 'lauf', 'teile_mass.json'), encoding='utf-8'))
    ix = {k['name']: k['index'] for k in g['knochen']}
    for t, (nr, kette) in KETTEN.items():
        kam = FK.lade_kamera(nr)
        R, T = fk(g, 'bild%d' % nr)
        pl = []
        for b in kette:
            i = ix[b]
            pl.append((b, FK.projiziere(kam, anfang(g, R, T, i)), FK.projiziere(kam, ende(g, R, T, i))))
        ml = np.array(ms[t]['mittellinie'], float)
        aus = []
        for q in range(len(ml)):
            p = ml[q, :2]
            best = (1e9, None, None)
            for b, a, e in pl:
                d = e - a
                s = np.clip((p - a) @ d / max(1e-6, d @ d), 0, 1)
                x = a + s * d
                r = np.linalg.norm(p - x)
                if r < best[0]:
                    best = (r, b, d)
            aus.append(best)
        ab = np.array([a[0] for a in aus])
        # Richtung: Mittellinie zwischen Nachbarpunkten gegen Knochen
        wi = []
        for q in range(1, len(ml) - 1):
            d = ml[q + 1, :2] - ml[q - 1, :2]
            b = aus[q][2]
            c = abs(d @ b) / max(1e-6, np.linalg.norm(d) * np.linalg.norm(b))
            wi.append(math.degrees(math.acos(min(1, c))))
        print('%-16s Bild %d  Abstand Mittellinie zur Kette px: Median %5.1f  Max %5.1f  | Richtung Median %5.1f Grad Max %5.1f Grad' % (t, nr, np.median(ab), ab.max(), np.median(wi), max(wi)))
        print('    ' + ' '.join('(%d,%d)%.0f' % (ml[q, 0], ml[q, 1], ab[q]) for q in range(len(ml))))
        for b, a, e in pl:
            print('    %-16s (%3.0f,%3.0f) -> (%3.0f,%3.0f)  %3.0f px' % (b, a[0], a[1], e[0], e[1], np.linalg.norm(e - a)))


if __name__ == '__main__':
    main()
