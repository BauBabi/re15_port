import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB, riegel_re2kopf as RG
P = G.posen()
emd = sys.argv[1]; prot = json.load(open(emd + '.json'))
mo = b2.modell(emd); M = KM.messen(mo, P, bilder=True); RG.hell_unten(mo, M)
Z = RG.tr2_tr3(mo, M) + RG.kopf_r2(mo, M, P, prot['zonen'])
z2, i2 = RG.tv2(emd, mo, P, prot)
for z in Z + z2:
    k = z['mass'].split()
    if any(t in z['mass'] for t in ('K10', 'K4', 'K7', 'S2', 'KS2', 'K5', 'K6', 'K9e', 'K1k', 'T-R3', 'T-V2')):
        print('  ', 'OK ' if z['ok'] else 'ROT', z['mass'][:28].ljust(28), z['ist'][:200])
