"""Baufehler A (Zahlen-Pruefer V3 r3): F1-Fleischtexel (Herkunftsart 2, Zeile 6) mit rohem G4-Blutrot (a* > 30, Farbton > 10 Grad).
Aufruf aus generator/: python ../farbe_lesung/V3R4_bau/lauf2/baufehler_a.py   -> Runde 3: 11 von 162, Runde 4: 0 von 162."""
import numpy as np
import riegel_farbe_v3 as R
KG = R.KG
for d in ('../farbe/K5c_farbe_v3r3', '../farbe/K5c_farbe_v3'):
    v = R.Var(d)
    n = roh = 0
    seen = set()
    for key, mi, z, ecken, a3 in v.flaechen:
        if z != KG.Z_F1:
            continue
        for x, y in v.texel(ecken):
            if (x, y) in seen or v.hart[y, x] != 2:
                continue
            seen.add((x, y))
            n += 1
            L = R.lab(np.array([v.rgb_zeilen[z][v.px[y, x]]]))[0]
            if L[1] > 30 and np.degrees(np.arctan2(L[2], L[1])) > 10:
                roh += 1
    print(d, 'F1-Fleischtexel', n, 'mit G4-Blutrot:', roh)
