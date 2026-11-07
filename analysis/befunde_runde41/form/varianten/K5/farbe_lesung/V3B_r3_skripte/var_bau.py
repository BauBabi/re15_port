"""Bau mit geaenderter Zuordnung (Experiment). python var_bau.py <aus> <teil>=<g4meshes>:<felder>[:ohne_inseln] ..."""
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import kf_g4 as KG
aus = sys.argv[1]
neu = []
for nm, zm, qm, art, op in KG.ZUORDNUNG:
    for a in sys.argv[2:]:
        t, spec = a.split('=')
        if t == nm:
            q, f = spec.split(':')[:2]
            qm = [int(x) for x in q.split(',')]
            op = dict(op); op['nur'] = f.split(',')
            op.pop('ohne_inseln', None)
            if len(spec.split(':')) > 2:
                op['ohne_inseln'] = {qm[0]: [int(x) for x in spec.split(':')[2].split(',')]}
    neu.append((nm, zm, qm, art, op))
KG.ZUORDNUNG = neu
b = KG.bau(aus)
for nm in b['teile']:
    if any(a.startswith(nm + '=') for a in sys.argv[2:]):
        print(nm, {k: v for k, v in b['teile'][nm].items() if k not in ('k5c',)})
