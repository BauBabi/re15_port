# Versuch Unterkiefer: Teil 23 (und wahlweise 22) NUR teilen, kein Verschieben. python tj.py
import sys, json, collections, copy
import numpy as np
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde as M
import kf_k5g2_mulde_mass as MM
G = M.G; U = M.U
emd0, form0 = M.grundform()
tab = json.load(open(M.TABELLE))
def basis():
    f = copy.deepcopy(form0)
    for t, n in tab['teile'].items():
        f['meshes'][int(t)] = copy.deepcopy(n)
    return f
def alle(f, t):
    return set(M.flaechen_liste(f['meshes'][t]))
var = {}
f = basis(); U.unterteilen(f, {23: alle(f, 23)}); var['j1'] = copy.deepcopy(f)
U.unterteilen(f, {23: alle(f, 23)}); var['j2'] = copy.deepcopy(f)
f = basis(); U.unterteilen(f, {23: alle(f, 23), 22: alle(f, 22)}); var['jk1'] = copy.deepcopy(f)
f = basis(); U.unterteilen(f, {22: alle(f, 22)}); var['k1'] = copy.deepcopy(f)
namen = []
for k, f in var.items():
    open(k + '.EMD', 'wb').write(G.emd_mit_md1(emd0, f)); namen.append(k + '.EMD')
    print(k, {t: len(f['meshes'][t]['dreiecke']) + 2 * len(f['meshes'][t]['vierecke']) for t in (22, 23)}, flush=True)
PR = ('23>22', '22>23', '23>16', '16>23', '23>21', '21>23', '22>16', '16>22', '17>22', '22>17', '23>10', '10>23')
ref = json.load(open('lauf_k5e_f1.json'))['erg']
n = int(sys.argv[1]) if len(sys.argv) > 1 else 60
top = sorted(ref, key=lambda r: -(r['alle']['paare'].get('23>22', 0) + r['alle']['paare'].get('22>23', 0)))[:n]
print('K5e_f1 an den', n, 'Ansichten:', {p: sum(r['alle']['paare'].get(p, 0) for r in top) for p in PR})
MU = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/k5g/K5g2/geo/MULDE.EMD'
for emd in [MU] + namen:
    w = MM.modell(emd); s = collections.Counter(); kopf = 0; dr = 0
    for r in top:
        i = MM.bild_index(w['cb'], r['clip'], r['bild'])
        x = MM.ansicht(emd, r['ansicht'], i)
        for p in PR:
            s[p] += x['alle']['paare'].get(p, 0)
        kopf += sum(v for p, v in x['alle']['paare'].items() if set(p.split('>')) & {'22', '23'}); dr = max(dr, x['dreiecke'])
    print(emd.split('/')[-1], dict(s), 'Summe mit 22/23', kopf, 'Dreiecke max', dr, flush=True)
