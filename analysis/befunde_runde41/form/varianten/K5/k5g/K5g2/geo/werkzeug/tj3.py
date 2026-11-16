import sys, json, collections
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
PR = ('23>22', '22>23', '23>16', '16>23')
ref = json.load(open('lauf_mulde.json'))['erg']
n = int(sys.argv[1])
top = sorted(ref, key=lambda r: -(r['alle']['paare'].get('23>22', 0) + r['alle']['paare'].get('22>23', 0)))[:n]
print('MULDE an den', n, 'Ansichten:', {p: sum(r['alle']['paare'].get(p, 0) for r in top) for p in PR})
for emd in sys.argv[2:]:
    w = MM.modell(emd); s = collections.Counter(); dr = 0
    for r in top:
        i = MM.bild_index(w['cb'], r['clip'], r['bild'])
        x = MM.ansicht(emd, r['ansicht'], i)
        for p in PR:
            s[p] += x['alle']['paare'].get(p, 0)
        dr = max(dr, x['dreiecke'])
    print(emd, dict(s), 'Dreiecke max', dr, flush=True)
