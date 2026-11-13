import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'); sys.path.insert(0, '.')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB
from dichte import bericht
for p in sys.argv[1:]:
    ms = KB.teile(open(p, 'rb').read())[2]
    print('==', p)
    rows = bericht(ms, list(V.SCHWANZ), lambda b, k, z: z == 495, 'Schwanz Zeile 495')
    for b in V.SCHWANZ:
        bericht(ms, [b], lambda b, k, z: z == 495, '  Mesh %d 495' % b)
    ref = 0.0714
    d = np.array([r[5] for r in rows]); st = np.array([r[6] for r in rows]); A = np.array([r[3] for r in rows])
    print('  Anteil Flaeche mit Dichte 0,5-2 x 0,0714: %.1f %%; Streckung <= 2: %.1f %%; Dreiecke ausserhalb Dichte %d, Streckung > 2 %d (von %d)' % (
        100 * A[(d >= 0.5 * ref) & (d <= 2 * ref)].sum() / A.sum(), 100 * A[st <= 2].sum() / A.sum(), int(((d < 0.5 * ref) | (d > 2 * ref)).sum()), int((st > 2).sum()), len(d)))
