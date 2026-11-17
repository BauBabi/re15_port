import sys, os, json, collections
sys.dont_write_bytecode = True
G = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, G)
import numpy as np
import kf_k5g3 as K3, kf_k5g2_tex as TX, kf_k5e as E
FE = E.FE
var = sys.argv[1]
A = FE.lese_emd(open(os.path.join(var, 'BAHNSTEIG_FORM.EMD'), 'rb').read())
T = FE.tim_lesen(open(os.path.join(var, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
cl = [[int(x) for x in z] for z in T['cluts']]
nam = json.load(open(os.path.join(var, 'bau.json'), encoding='utf-8'))['textur']['zeilen']
labs = [TX.lab_zeile(z) for z in cl]
z = K3.abtasten(A, T['pixel'], int(T['clut_y']))
c = collections.Counter(); w = collections.Counter(); teile = collections.Counter()
for f, zi, x, y, i in z:
    L = labs[zi][i, 0]
    if L > 85:
        c[nam[zi]] += 1; w[(nam[zi], i, hex(cl[zi][i]), round(float(L), 1))] += 1; teile[f[0]] += 1
print('Zugriffe', len(z), 'L>85:', sum(c.values()))
print(c.most_common())
for k, v in sorted(w.items(), key=lambda t: -t[1])[:40]: print(k, v)
print('teile', teile.most_common())
