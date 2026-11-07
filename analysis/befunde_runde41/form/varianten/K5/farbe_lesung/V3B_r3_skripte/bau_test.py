import sys, os, time
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import kf_g4 as KG
t = time.time()
b = KG.bau(sys.argv[1])
for nm, t_ in b['sonder'].items():
    print(nm, {k: v for k, v in t_.items() if k != 'flaechen'})
print('zeit', round(time.time() - t, 1))
