import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import unterseite_r5 as U5, v8_gemein as V
par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json'))); par.update(json.loads(sys.argv[2]))
blob = open(sys.argv[1], 'rb').read(); v7 = open(V.SP + '/sfv8/emd/EM30_V7.EMD', 'rb').read()
print('a/b', U5.s15_flaechen(blob, v7, par))
print('c', U5.s15_bild(blob))
print('d', {k: (v['sprung_modell'], v['sprung_vorlage'], v['modell']) for k, v in U5.s15_ruhe(blob)['ansichten'].items()})
print('e', U5.s15_grenze(blob))
print('g', U5.s15_reifen(blob, par))
print('kanten', U5.kanten_mass(blob))
