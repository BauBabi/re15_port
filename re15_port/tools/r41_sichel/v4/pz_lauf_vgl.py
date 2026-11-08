"""Zahlen-Pruefer: Rueckfaltung (Gelenkwinkel > 90 Grad) und Bodenabstand in den Laeufen V4 / V3r2 / R2n (Ref2) vergleichen."""
import re, numpy as np, sys
SC = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/'
def lies(p):
    K, B = {}, {}
    for l in open(p, encoding='latin-1'):
        m = re.match(r'\[hangar-takt\] h(-?\d+) Schwanz B y \S+ (.*)', l)
        if m: K[int(m.group(1))] = np.array(re.findall(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', m.group(2)), float)
        m = re.match(r'\[hangar-takt\] h(-?\d+) F(\d+) B clip (\d+)', l)
        if m: B[int(m.group(1))] = int(m.group(3))
    return K, B
def winkel(P):
    return [float(np.degrees(np.arccos(np.clip((P[i]-P[i-1]) @ (P[i+1]-P[i]) / np.linalg.norm(P[i]-P[i-1]) / np.linalg.norm(P[i+1]-P[i]), -1, 1)))) for i in range(1, len(P)-1)]
for nm, p in (('V4', SC + 'sfv4/r/V4/debug.log'), ('V3r2', SC + 'sfv3/r/V3r2/debug.log'), ('Ref2 (R2n)', SC + 'sfv3/r/R2n/debug.log')):
    try:
        K, B = lies(p)
    except FileNotFoundError:
        print(nm, 'fehlt'); continue
    if not K: print(nm, 'keine Kettenzeilen'); continue
    W = {h: winkel(K[h]) for h in K}
    f90 = [h for h in K if max(W[h]) > 90]
    gang = [h for h in K if B.get(h) == 1]
    print('%-10s Takte %d, Winkel>90: %d (Gang %d/%d), max Gelenkwinkel %.1f, tiefster Punkt y %d, Spitze y Gang min/max %s' % (
        nm, len(K), len(f90), len([h for h in f90 if h in gang]), len(gang), max(max(w) for w in W.values()),
        max(k[:, 1].max() for k in K.values()), [int(min(K[h][-1][1] for h in gang)), int(max(K[h][-1][1] for h in gang))] if gang else None))
