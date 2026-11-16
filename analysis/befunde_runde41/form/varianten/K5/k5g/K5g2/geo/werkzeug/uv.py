# Umrissverlust nach Teil an den N Ansichten mit dem groessten Verlust (K5e_f1 gegen MULDE)
import sys, json, collections
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
a, b, n = sys.argv[1], sys.argv[2], int(sys.argv[3])
A = {(r['ansicht'], r['clip'], r['bild']): r['alle']['figur'] for r in json.load(open('lauf_k5e_f1.json'))['erg']}
B = {(r['ansicht'], r['clip'], r['bild']): r['alle']['figur'] for r in json.load(open('lauf_mulde.json'))['erg']}
top = sorted(A, key=lambda k: -(A[k] - B[k]))[:n]
wa = MM.modell(a); wb = MM.modell(b); s = collections.Counter(); gr = {}
for (name, c, bi) in top:
    ma = MM.raster(a, name, MM.bild_index(wa['cb'], c, bi), wa['alle'])[1][0]
    mb = MM.raster(b, name, MM.bild_index(wb['cb'], c, bi), wb['alle'])[1][0]
    weg = (ma >= 0) & (mb < 0)
    for t, z in collections.Counter(ma[weg].tolist()).items():
        s[t] += z
        if z > gr.get(t, (0,))[0]: gr[t] = (z, name, c, bi)
print('Umrissverlust nach Teil an den', n, 'Ansichten mit dem groessten Verlust:', dict(s), 'Summe', sum(s.values()))
print('groesste je Teil:', gr)
