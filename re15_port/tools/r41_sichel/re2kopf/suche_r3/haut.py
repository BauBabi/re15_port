import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, ref2tex_v4 as RT, riegel_ref2tex as RR, kopf_re2_bau as KB, b2
A = RT.atlas(G.REF2)
print(A.keys())
body = [b for b in range(16) if b != 9]
mk = RR._flaechen_maske(A['ms'], body, A['tex'].shape)
Ls = []; labs = []
for z, m in mk.items():
    zi = z - 480
    idx = A['tex'][m]
    lab = KB.lab_pal(KB.clut15_rgb(A['clut'][zi]))[idx]
    labs.append(lab[idx != 0])
lab = np.vstack(labs)
C = np.hypot(lab[:, 1], lab[:, 2])
print('Koerper-Texel', len(lab), 'L p10/50/90', np.percentile(lab[:, 0], [10, 50, 90]).round(1), 'a/b p50', np.median(lab[:, 1:], 0).round(1), 'C p50', np.median(C).round(1))
for cmax in (12, 18, 25):
    s = C <= cmax
    print(' C<=%d Anteil %.2f L p50 %.1f a/b %s' % (cmax, s.mean(), np.median(lab[s, 0]), np.median(lab[s, 1:], 0).round(1)))
# render: Rumpf median L in Pose 135
mo = b2.modell(G.REF2); P = G.posen()
for h in ('135', '85'):
    img, ids = G.zeichne(mo, P[h]); L = G.lab(img.astype(float))
    for b in (8, 9):
        k = ids == b
        print(h, 'Id', b, 'L p50 %.1f' % np.median(L[..., 0][k]), 'ab', np.median(L[..., 1:][k], 0).round(1))
