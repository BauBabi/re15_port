import numpy as np, sys, bilder, panel_inc
lauf, welche = sys.argv[1], sys.argv[2]
G = panel_inc.grund('panel_%s.inc' % welche).astype(np.int16)
ev = bilder.ereignisse(lauf, ('[panel2030]',))
t0 = next(t for t, s in ev if 'offen:' in s); t1 = next(t for t, s in ev if 'GELOEST: A' in s)
fuell = [t for t, s in ev if 'umgefuellt' in s]
seq = []
for i, t, b in bilder.bilder(lauf):
    if t0 + 0.2 < t < t1 - 0.1:
        l = bilder.logisch(b)
        if (np.abs(l.astype(np.int16) - G).max(-1) == 0).mean() > 0.6:
            seq.append((t, l.copy()))
u = [seq[0]]
for t, l in seq[1:]:
    if not np.array_equal(l, u[-1][1]): u.append((t, l))
Z = (slice(70, 106), slice(107, 143)); Bo = (slice(218, 232), slice(150, 170))
for name, R in (('Zeiger', Z), ('unten x150..169 y218..231', Bo)):
    zs = []
    for k in range(1, len(u) - 1):
        a, b, c = u[k - 1][1][R], u[k][1][R], u[k + 1][1][R]
        if ((a != b).any(-1) & (a == c).all(-1)).any(): zs.append(round(u[k][0], 2))
    print(lauf, name, 'A-B-A-Texel in Bildern bei', zs[:30], '| Fuellbeginn', fuell)
