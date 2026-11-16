import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import riegel_v8 as RV
for p in sys.argv[1:]:
    blob = open(p, 'rb').read()
    mo, A = RV.render(blob)
    bone = A['bone'][:480]
    r1 = RV.s1(bone); r2, r3, om = RV.s2_s3(mo, bone)
    c = RV.s2c_r5(mo)
    print(p, 'S1', r1['mittel'], r1['max'], r1['ok'], '| S2 a', r2['a'], 'b', r2['b'], 'c', r2['c'], 'c7', c['max_px'], [v['huckel_px'] for v in c['varianten']], '| S3e', r3['e_K'], 'S3d', r3['d_sattel_kopf'])
