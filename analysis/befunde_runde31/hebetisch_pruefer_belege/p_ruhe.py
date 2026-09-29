# -*- coding: utf-8 -*-
"""Pruefer r31/hebetisch: wertet hebetisch.log (RE15_HEBETISCH_LOG) aus. p_ruhe.py <hebetisch.log>
Je Aufnahme-Oeffnung: Bild, y, PC, y der 3 Bilder davor; Bilder mit Aufnahme bei y != -1205;
je Fahrt: letzte Bewegung vor der Ruhe, Abfahrt, Ruhebilder ohne Aufnahme."""
import re, sys
rows = []
for l in open(sys.argv[1]):
    m = re.match(r'F(\d+) y=(-?\d+) ruht=(\d) pc=(-?\d+) modal=(\d+)', l)
    if m: rows.append(tuple(int(x) for x in m.groups()))
by = {r[0]: r for r in rows}
print('Zeilen %d, F%d..F%d' % (len(rows), rows[0][0], rows[-1][0]))
for i in range(1, len(rows)):
    a, b = rows[i - 1], rows[i]
    if a[4] == 0 and b[4] != 0:
        f = a[0]   # Modal wurde in Bild f aufgemacht (Protokoll liegt VOR dem Sicherungs-/Granaten-Tick)
        vor = [by.get(f - k, (0, None))[1] for k in (3, 2, 1)]
        print('Aufnahme auf in F%d: y=%d ruht=%d pc=0x%X | y F-3..F-1 = %s | zeichnet ab F%d' % (f, a[1], a[2], a[3] & 0xffff, vor, b[0]))
bad = [r for r in rows if r[4] != 0 and r[1] != -1205]
print('Bilder mit Aufnahme: %d, davon y != -1205: %d' % (sum(1 for r in rows if r[4]), len(bad)))
# Ruhe-Laeufe
run = None
for r in rows + [(10**9, 0, 0, -1, 0)]:
    if r[2] and run is None: run = [r[0], r[0], 0]
    if run is not None:
        if r[2]:
            run[1] = r[0]; run[2] += (r[4] == 0)
        else:
            print('Ruhe F%d..F%d (%d Bilder), davon ohne Aufnahme %d; naechstes Bild F%d y=%d' % (run[0], run[1], run[1] - run[0] + 1, run[2], r[0], r[1]))
            run = None
