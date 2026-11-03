import numpy as np
exec(open('suche_liegen.py').read().split("print('RE2-Lage'")[0])
res = []
for dx in range(-1200, 1201, 50):
    for dz in range(-1200, 1201, 50):
        Q = (P[0] + dx, P[1] + dz)
        dw, df = pruef(Q, g0)
        if dw >= 0:
            res.append((int(np.hypot(dx, dz)), dx, dz, int(dw), int(df)))
res.sort(); print(res[:6])
# Feuer-Abstand Mitte der Wolke: Wurzel-Abstand zu Feuern
