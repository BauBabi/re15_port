"""Je Lauf: Bilder 'zu' (Tuer zu, Cut 3, Figur bei (3807,-942)) und 'auf' (Tuer offen, gleiche Lage) per Ereignis 'Tuer klappt auf' ausgerichtet."""
import numpy as np, bilder, sys
def t_auf(name):
    for t, s in bilder.ereignisse(name):
        if 'Tuer klappt auf' in s: return t
for name in sys.argv[1:]:
    ta = t_auf(name)
    want = {'zu': [ta - 6.0, ta - 5.5, ta - 5.0], 'auf': [ta + 12.5, ta + 13.0, ta + 13.5]}
    got = {}
    for i, t, b in bilder.bilder(name):
        for k, ts in want.items():
            for j, tw in enumerate(ts):
                if abs(t - round(tw * 4) / 4) < 1e-6:
                    got[(k, j)] = b.copy()
    for k in want:
        arr = [got[(k, j)] for j in range(3)]
        # Kastenbereich (logisch x 160..270, y 15..225) in allen drei Bildern gleich?
        reg = [a[45:675, 480:810].astype(int) for a in arr]
        print(name, k, 't=%.2f' % (round(want[k][2] * 4) / 4), 'Kastenbereich max. Abweichung zwischen 3 Bildern:', max(np.abs(reg[0] - reg[1]).max(), np.abs(reg[0] - reg[2]).max()))
        np.save('k_%s_%s.npy' % (name, k), arr[2])
