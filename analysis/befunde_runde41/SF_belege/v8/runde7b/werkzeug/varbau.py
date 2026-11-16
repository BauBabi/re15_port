# Runde 7b: Variante bauen = v8_bau.bauen(V7, 'srwaunhb') mit einer eigenen unterseite-json (Spiel-Asset unberuehrt)
# python varbau.py NAME '{json-zusatz}'
import sys, os, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V, v8_bau as VB, unterseite_r5 as U5
par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json')))
par.update(json.loads(sys.argv[2]) if len(sys.argv) > 2 else {})
neu, prot = VB.bauen(V.SP + '/sfv8/emd/EM30_V7.EMD', 'srwaunhb', unterseite=par)
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'emd', sys.argv[1] + '.EMD')
open(p, 'wb').write(neu)
print(sys.argv[1], prot['emd'], prot['tim'], prot['unterseite']['flaechen'], prot['unterseite']['deckel_unten'])
