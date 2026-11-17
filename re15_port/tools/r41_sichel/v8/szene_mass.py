"""SF V8 Runde 2: Schwanz in der Szene aus debug.log ("[hangar-takt] hN Schwanz B y ..." = Gelenke 0-7, Welt).

    python szene_mass.py DEBUG_LOG [DEBUG_LOG ...]

Ausgabe je Lauf: Takte, tiefstes Gelenk-y (Riegel S6, y > 0 = unter dem Boden), groesster Gelenkwinkel, Median des Takt-Maximums,
Takte mit einem Gelenk > 30 / > 60 Grad; Takte, in denen die Biegeachse zweier aufeinanderfolgender Gelenke (Knick > 3 Grad) das
Vorzeichen wechselt (Zahlen-Pruefer V8 r1: "die Biegeebene wechselt in 134 von 144 Takten das Vorzeichen"). Nur Messung.
"""
import sys, re, numpy as np
for f in sys.argv[1:]:
    T = []
    for z in open(f, errors='replace'):
        if 'Schwanz B y' in z:
            pts = [tuple(map(int, m.groups())) for m in re.finditer(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', z)]
            if len(pts) == 8: T.append(np.array(pts, float))
    tief = max(p[:, 1].max() for p in T)
    wmax = []
    for p in T:
        d = np.diff(p, axis=0); d /= np.linalg.norm(d, axis=1)[:, None]
        wmax.append(max(np.degrees(np.arccos(np.clip((d[j] * d[j + 1]).sum(), -1, 1))) for j in range(6)))
    wmax = np.array(wmax)
    print(f, 'Takte', len(T), 'tiefstes y', int(tief), 'Gelenk max %.1f Median des Takt-Max %.1f, Takte >30 Grad %d, >60 %d' % (wmax.max(), np.median(wmax), (wmax > 30).sum(), (wmax > 60).sum()))
# Biegeebene: Takte, in denen die Biegeachse zweier aufeinanderfolgender Gelenke (Betrag > 3 Grad) das Vorzeichen wechselt
for f in sys.argv[1:]:
    n_w, n = 0, 0
    for z in open(f, errors='replace'):
        if 'Schwanz B y' not in z: continue
        p = np.array([tuple(map(int, m.groups())) for m in re.finditer(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', z)], float)
        if len(p) != 8: continue
        d = np.diff(p, axis=0); d /= np.linalg.norm(d, axis=1)[:, None]
        ns = [np.cross(d[j], d[j + 1]) for j in range(6)]
        ns = [x for x in ns if np.linalg.norm(x) > np.sin(np.radians(3))]
        n += 1
        if any((a @ b) < 0 for a, b in zip(ns, ns[1:])): n_w += 1
    print(f, 'Takte mit Vorzeichenwechsel der Biegeebene', n_w, 'von', n)
