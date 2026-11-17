# Gegenprobe: Mulde 10/21 aus der Tabelle, aber Kopf 22 = K5e_f1 (Hinterkopf nicht zurueckgenommen)
import sys, json, copy, collections
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde as M
import kf_k5g2_mulde_mass as MM
emd0, form0 = M.grundform(); tab = json.load(open(M.TABELLE))
f = copy.deepcopy(form0)
for t in ('10', '21'):
    f['meshes'][int(t)] = tab['teile'][t]
open('k22.EMD', 'wb').write(M.G.emd_mit_md1(emd0, f))
MU = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5g/K5g2/geo/MULDE.EMD'
A = {(r['ansicht'], r['clip'], r['bild']): r for r in json.load(open('lauf_k5e_f1.json'))['erg']}
B = {(r['ansicht'], r['clip'], r['bild']): r for r in json.load(open('lauf_mulde.json'))['erg']}
n = int(sys.argv[1])
top1 = sorted(A, key=lambda k: -(A[k]['alle']['figur'] - B[k]['alle']['figur']))[:n]
top2 = sorted(A, key=lambda k: -sum(A[k]['ab4']['streif']))[:n]
w = MM.modell('k22.EMD')
for nm, top in (('groesster Umrissverlust', top1), ('meiste ab4-Streifen in K5e_f1', top2)):
    z = collections.Counter()
    for k in top:
        x = MM.ansicht('k22.EMD', k[0], MM.bild_index(w['cb'], k[1], k[2]))
        z['k22 ab4'] += sum(x['ab4']['streif']); z['k22 ab5'] += sum(x['ab5']['streif']); z['k22 Figur'] += x['alle']['figur']
        z['k22 ab4 > 20'] += sum(x['ab4']['streif']) > 20
        z['MULDE ab4'] += sum(B[k]['ab4']['streif']); z['MULDE ab5'] += sum(B[k]['ab5']['streif']); z['MULDE Figur'] += B[k]['alle']['figur']
        z['K5e_f1 ab4'] += sum(A[k]['ab4']['streif']); z['K5e_f1 Figur'] += A[k]['alle']['figur']
    print(n, 'Ansichten,', nm, ':', dict(z), flush=True)
