import sys
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v6kopf')
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools')
import kopf_orig_bau as KB
M = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7c/m/'
names = sys.argv[1].split(',')
ecken = [int(x) for x in sys.argv[2].split(',')]
ms = {n: KB.teile(open(M + n + '.EMD', 'rb').read())[2][8] for n in names}
for n in names:
    print(n, 'Ecken', {e: ms[n]['v'][e][:3] for e in ecken})
ref = ms[names[-1]]
for kind, F, U in (('t', 'tf', 'tuv'), ('q', 'qf', 'quv')):
    for i, f in enumerate(ref[F]):
        if set(f[1::2]) & set(ecken):
            print('%s%d ecken %s' % (kind, i, f[1::2]))
            for n in names:
                if i < len(ms[n][U]):
                    uv = ms[n][U][i]; k = 3 if kind == 't' else 4
                    print('   %-4s %s zeile %d' % (n, [(uv[3*j], uv[3*j+1]) for j in range(k)], (uv[2] >> 6) & 0x1ff), 'ecken', ms[n][F][i][1::2])
