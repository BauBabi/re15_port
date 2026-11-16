import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import rl, ruhe_wunsch as W
mt = W.mat(); aus = {}
for d in sys.argv[2:]:
    A, _F = W.stand_albedo(d); mo = rl.fl.lade_var(d)
    e = W.wuensche(A, mo, 2, mt); aus[d[-14:]] = e
    print(d[-14:], json.dumps({k: {kk: vv for kk, vv in v.items() if kk != 'je_winkel'} for k, v in e.items()}))
json.dump(aus, open(sys.argv[1], 'w'), indent=1)
