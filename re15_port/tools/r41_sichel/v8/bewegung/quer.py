"""Querschnitt der Schwanz-Meshes 16-22 des Spiel-Assets: Hauptachse (x-z) und Flaechen je CLUT-Zeile."""
import sys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/tools/r41_sichel/v2')
import b2
mo = b2.modell(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD')
for b in range(16, 23):
    m = mo.meshes[b]
    V = np.array([v[:3] for v in m.tv] + [v[:3] for v in m.qv], float)
    xz = V[:, [0, 2]]
    r = np.hypot(xz[:, 0], xz[:, 1])
    sel = r > 20
    C = np.cov(xz[sel].T)
    w, e = np.linalg.eigh(C)
    ax = e[:, 1]
    zeilen = {}
    for verts, faces, uvs in ((m.tv, m.tf, m.tuv), (m.qv, m.qf, m.quv)):
        P = np.array([v[:3] for v in verts], float)
        for f, uv in zip(faces, uvs):
            ie = f[1::2]
            z = (uv[2] >> 6) & 0x1FF
            c = P[list(ie)].mean(0)
            zeilen.setdefault(z, []).append((c[0], c[2]))
    txt = []
    for z, cs in sorted(zeilen.items()):
        cs = np.array(cs); mc = cs.mean(0); txt.append('%d:n%d (%.0f,%.0f) proj %.0f' % (z, len(cs), mc[0], mc[1], mc @ ax))
    print(b, 'n', len(V), 'achse (%.3f,%.3f) sd %.0f/%.0f' % (ax[0], ax[1], np.sqrt(w[1]), np.sqrt(w[0])), 'winkel %.1f' % np.degrees(np.arctan2(ax[1], ax[0])), '|', '; '.join(txt))
