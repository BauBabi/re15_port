# Texeldichte je Flaeche (Texel je Einheit = sqrt(Texelflaeche / Weltflaeche)), Streckung (Verhaeltnis der Hauptachsen der Abbildung)
import os, sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB, adern_v8 as AV, adern_r4 as A4, unterseite_r5 as U5

def tris(m):
    P = np.array([v[:3] for v in m['v']], float)
    for i, (f, uv) in enumerate(zip(m['tf'], m['tuv'])):
        yield 'd%d' % i, P[[f[1], f[3], f[5]]], np.array([[uv[0], uv[1]], [uv[3], uv[4]], [uv[6], uv[7]]], float), uv
    for i, (f, uv) in enumerate(zip(m['qf'], m['quv'])):
        p = P[[f[1], f[3], f[5], f[7]]]
        t = np.array([[uv[0], uv[1]], [uv[3], uv[4]], [uv[6], uv[7]], [uv[9], uv[10]]], float)
        yield 'v%d' % i, p[[0, 1, 3]], t[[0, 1, 3]], uv
        yield 'v%d' % i, p[[0, 3, 2]], t[[0, 3, 2]], uv

def abb(p, t):
    """-> (Weltflaeche, Texelflaeche, Dichte, Streckung) einer Dreiecksabbildung Welt->Texel"""
    e1, e2 = p[1] - p[0], p[2] - p[0]
    A = 0.5 * np.linalg.norm(np.cross(e1, e2))
    if A < 1e-6:
        return None
    x = e1 / np.linalg.norm(e1); n = np.cross(e1, e2); y = np.cross(n / np.linalg.norm(n), x)
    W = np.array([[e1 @ x, e2 @ x], [e1 @ y, e2 @ y]])
    T = np.array([t[1] - t[0], t[2] - t[0]]).T
    try:
        J = T @ np.linalg.inv(W)          # Texel je Einheit (2x2)
    except np.linalg.LinAlgError:
        return None
    s = np.linalg.svd(J, compute_uv=False)
    At = 0.5 * abs(np.linalg.det(T))
    return A, At, float(np.sqrt(At / A)), float(s[0] / max(s[1], 1e-9)), s

def bericht(ms, meshes, filt=None, name=''):
    rows = []
    for b in meshes:
        for key, p, t, uv in tris(ms[b]):
            z = (uv[2] >> 6) & 0x1FF
            if filt and not filt(b, key, z):
                continue
            r = abb(p, t)
            if r is None or r[1] < 0.5:
                continue
            rows.append((b, key, z) + r)
    if not rows:
        print(name, 'leer'); return rows
    A = np.array([r[3] for r in rows]); d = np.array([r[5] for r in rows]); st = np.array([r[6] for r in rows])
    w = A / A.sum()
    print('%-34s n %4d  Dichte Median %.4f (gew. %.4f, p10 %.4f p90 %.4f)  Streckung Median %.2f p90 %.2f max %.2f' %
          (name, len(rows), np.median(d), (w * d).sum(), np.percentile(d, 10), np.percentile(d, 90), np.median(st), np.percentile(st, 90), st.max()))
    return rows

if __name__ == '__main__':
    MO, TO = A4.original()
    bericht(MO, [11, 12, 14, 15], lambda b, k, z: z == 482, 'Original 11/12/14/15 Zeile 482')
    bericht(MO, [14, 15], lambda b, k, z: z == 482, 'Original 14/15 Zeile 482')
    bericht(MO, list(range(len(MO))), None, 'Original alle')
    for nm, pfad in (('V7', V.SP + '/sfv8/emd/EM30_V7.EMD'), ('vor7b', V.ASSET)):
        blob = open(pfad, 'rb').read()
        _d, _o, ms, tim = KB.teile(blob)
        bericht(ms, [8], None, nm + ' Mesh 8')
        bericht(ms, [1, 2, 3, 4, 5, 6], None, nm + ' Beine 1-6')
        bericht(ms, [13, 14, 15], None, nm + ' rechter Arm 13-15')
        bericht(ms, list(V.SCHWANZ), lambda b, k, z: z == 487, nm + ' Schwanz Zeile 487')
        bericht(ms, list(V.SCHWANZ), lambda b, k, z: z != 487, nm + ' Schwanz Unterseite (!=487)')
        for b in V.SCHWANZ:
            bericht(ms, [b], lambda b, k, z: z != 487, nm + '   Mesh %d unten' % b)
