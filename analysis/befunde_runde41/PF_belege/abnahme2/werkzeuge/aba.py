import numpy as np, sys, bilder, panel_inc
lauf, welche = sys.argv[1], sys.argv[2]
G = panel_inc.grund('panel_%s.inc' % welche).astype(np.int16)
ev = bilder.ereignisse(lauf, ('[panel2030]',))
t0 = next(t for t, s in ev if 'offen:' in s); t1 = next(t for t, s in ev if 'GELOEST: A' in s)
seq = []
for i, t, b in bilder.bilder(lauf):
    if t0 + 0.2 < t < t1 - 0.1:
        l = bilder.logisch(b)
        if (np.abs(l.astype(np.int16) - G).max(-1) == 0).mean() > 0.6:
            seq.append(l.copy())
# aufeinanderfolgende gleiche Bilder (Aufnahme schneller als Spiel) zusammenfassen
u = [seq[0]]
for l in seq[1:]:
    if not np.array_equal(l, u[-1]): u.append(l)
aba = 0; px = 0
for k in range(1, len(u) - 1):
    d1 = (u[k] != u[k - 1]).any(-1); same = (u[k - 1] == u[k + 1]).all(-1)
    m = d1 & same
    if m.any(): aba += 1; px += int(m.sum())
fen = [(50, 58, 90, 120), (90, 58, 100, 120), (136, 58, 162, 120), (180, 58, 206, 120)]
print(lauf, 'Panel-Bilder', len(seq), 'verschiedene Spielbilder', len(u), '| A-B-A im ganzen Panel: Bilder %d, Texel %d' % (aba, px))
