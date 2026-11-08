import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import kopf_re2_bau as KB
p = json.loads(json.dumps(KB.PARAM_R3))
for k, v in json.loads(sys.argv[2]).items():
    if k == 'form': p['form'].update(v)
    else: p['pal'][k].update(v)
neu, prot, _ = KB.bauen(p); open(sys.argv[1], 'wb').write(neu)
json.dump(prot, open(sys.argv[1] + '.json', 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
