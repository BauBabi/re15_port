# Paare 22/23 gegen 16 an den Ansichten mit den meisten solchen Streifen in K5e_f1: python p16.py N EMD...
import sys, json, collections
GEN = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator'
sys.path.insert(0, GEN)
import kf_k5g2_mulde_mass as MM
PR = ('22>16', '16>22', '23>16', '16>23')
ref = json.load(open('lauf_k5e_f1.json'))['erg']
n = int(sys.argv[1])
top = sorted(ref, key=lambda r: -sum(r['alle']['paare'].get(p, 0) for p in PR))[:n]
print('K5e_f1 an den', n, 'Ansichten:', {p: sum(r['alle']['paare'].get(p, 0) for r in top) for p in PR}, 'ab5', sum(sum(r['ab5']['streif']) for r in top))
for emd in sys.argv[2:]:
    w = MM.modell(emd); s = collections.Counter(); ab5 = 0; dr = 0; gr = (0,)
    for r in top:
        i = MM.bild_index(w['cb'], r['clip'], r['bild'])
        x = MM.ansicht(emd, r['ansicht'], i)
        for p in PR:
            s[p] += x['alle']['paare'].get(p, 0)
        a = sum(x['ab5']['streif']); ab5 += a; dr = max(dr, x['dreiecke']); gr = max(gr, (a, r['ansicht'], r['clip'], r['bild']))
    print(emd.split('/')[-1], dict(s), 'ab5', ab5, 'groesste ab5', gr, 'Dreiecke max', dr, flush=True)
