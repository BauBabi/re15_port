"""Bauchseite der V8-Schwanzglieder je Asset: CLUT-Haeufigkeit (alte Regel), Eckenschwerpunkt, Keyframe-Beugung (neue Regel).
    python bauchachse_vergleich.py <emd> [<emd> ...]"""
import sys, math, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_gruenblut/re15_port/tools/r41_sichel/v2')
import b2, birkin_buehne as BB
for name in sys.argv[1:]:   # z. B. EM30_HANGAR_66db2244.EMD und das Spiel-Asset
    mo = b2.modell(name)
    xz, faces = [], []
    for b in range(17, 22):
        m = mo.meshes[b]
        for verts, fs, uvs in ((m.tv, m.tf, m.tuv), (m.qv, m.qf, m.quv)):
            P = np.array([v[:3] for v in verts], float)
            xz += [(v[0], v[2]) for v in P]
            for f, uv in zip(fs, uvs):
                faces.append((b, P[list(f[1::2])].mean(0), uv[2], uv))
    xz = np.array(xz); C = np.cov(xz.T, bias=True)
    th = 0.5 * math.atan2(2 * C[0, 1], C[0, 0] - C[1, 1]); ax = np.array([math.cos(th), math.sin(th)])
    # 1 CLUT-Haeufigkeit
    cl = {}
    for b, c, k, uv in faces: cl.setdefault(k, []).append(c)
    print(name, 'Achse +-(%.4f,%.4f)' % tuple(ax))
    for k, cs in sorted(cl.items(), key=lambda t: -len(t[1])):
        cs = np.array(cs); print('   CLUT %d (Zeile %d): %d Flaechen, Mitte entlang Achse %.0f' % (k, (k >> 6) & 0x1FF, len(cs), (cs[:, [0, 2]] @ ax).mean()))
    # 2 Eckenschwerpunkt (ohne Achs-Ecken)
    r = np.hypot(xz[:, 0], xz[:, 1]); sel = r > 50
    print('   Eckenschwerpunkt entlang Achse %.1f (Ecken %d)' % ((xz[sel] @ ax).mean(), sel.sum()))
    # 3 Krummung der Keyframes: Beugerichtung im Knochenraum des Vorglieds
    acc = np.zeros(2); n = 0; pos = neg = 0
    nkf = len(mo.frames)
    kfs = sorted(set(int(f) & 0xFFF for f in mo.frames))
    for kf in kfs:
        R, T = mo.pose(kf)
        for j in range(17, 23):
            Ra = R[j - 1].astype(float) / 4096.0; Rb = R[j].astype(float) / 4096.0
            a = Ra[:, 1] / np.linalg.norm(Ra[:, 1]); bb = Rb[:, 1] / np.linalg.norm(Rb[:, 1])
            v = bb - (bb @ a) * a
            loc = Ra.T @ v          # im Raum des Vorglieds
            w = np.array([loc[0], loc[2]])
            acc += w; n += 1
            s = w @ ax
            if s > 1e-3: pos += 1
            elif s < -1e-3: neg += 1
    print('   Keyframe-Beugung: Summe entlang Achse %.2f (Winkel %.1f Grad), Gelenke + %d / - %d, Keyframes %d' % (
        acc @ ax, math.degrees(math.atan2(acc[1], acc[0])), pos, neg, len(kfs)))
