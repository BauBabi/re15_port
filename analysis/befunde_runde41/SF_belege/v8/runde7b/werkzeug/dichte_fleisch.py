# Dichte der Original-Flaechen, die das Fleischfeld (Zeile 482, Seite 2, v 92-125) benutzen: gesamt und je Texelrichtung (u / v)
import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
sys.path.insert(0, '.')
import numpy as np, adern_r4 as A4
from dichte import tris
MO, TO = A4.original()
rows = []
for b, m in enumerate(MO):
    for key, p, t, uv in tris(m):
        if ((uv[2] >> 6) & 0x1FF) != 482:
            continue
        vm = t[:, 1].mean()
        if not (92 <= vm <= 125):
            continue
        e1, e2 = p[1] - p[0], p[2] - p[0]
        A = 0.5 * np.linalg.norm(np.cross(e1, e2))
        if A < 1:
            continue
        x = e1 / np.linalg.norm(e1); n = np.cross(e1, e2); y = np.cross(n / np.linalg.norm(n), x)
        Wm = np.array([[e1 @ x, e2 @ x], [e1 @ y, e2 @ y]])
        T = np.array([t[1] - t[0], t[2] - t[0]]).T
        At = 0.5 * abs(np.linalg.det(T))
        if At < 0.5:
            continue
        J = T @ np.linalg.inv(Wm)       # Texel je Einheit; Zeile 0 = du, Zeile 1 = dv
        # Dichte in Texelrichtung v: |dv/dWelt| = Norm der Zeile 1 (Texel v je Einheit in der staerksten Richtung)
        rows.append((b, key, A, np.sqrt(At / A), np.linalg.norm(J[0]), np.linalg.norm(J[1])))
r = np.array([x[2:] for x in rows])
w = r[:, 0] / r[:, 0].sum()
print('Original-Flaechen im Fleischfeld v 92-125: n', len(rows), 'Meshes', sorted(set(x[0] for x in rows)))
print('Dichte sqrt(Texel/Flaeche) Median %.4f gew %.4f p10 %.4f p90 %.4f' % (np.median(r[:, 1]), (w * r[:, 1]).sum(), np.percentile(r[:, 1], 10), np.percentile(r[:, 1], 90)))
print('Texel u je Einheit Median %.4f, Texel v je Einheit Median %.4f (gew %.4f / %.4f)' % (np.median(r[:, 2]), np.median(r[:, 3]), (w * r[:, 2]).sum(), (w * r[:, 3]).sum()))
