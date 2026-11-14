# Runde 7e Analyse 4: groesster gleichmaessiger Massstab s je Gruppe, mit dem die Gruppe (UV x s + Schub) ganz in der Grau-Zone N liegt
import os, sys
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import v8_gemein as V
import kopf_orig_bau as KB
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e'
N = np.load(SP + '/N_grau.npy')
blob = open(V.ASSET, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
def faces(m):
    for kind, F, UV, n in (('d', m['tf'], m['tuv'], 3), ('v', m['qf'], m['quv'], 4)):
        for k, (f, uv) in enumerate(zip(F, UV)):
            yield '%s%d' % (kind, k), f, list(uv), n
def tr(uv, n, s, u0, v0, tu, tv):
    uv = list(uv)
    for j in range(n):
        uv[3*j] = int(round(s * (uv[3*j] - u0) + tu)); uv[3*j+1] = int(round(s * (uv[3*j+1] - v0) + tv))
    return uv
def aussen(gr, s, u0, v0, tu, tv):
    M = np.zeros(N.shape, bool)
    for key, f, uv, n in gr:
        u2 = tr(uv, n, s, u0, v0, tu, tv)
        if min(u2[3*j] for j in range(n)) < 0 or min(u2[3*j+1] for j in range(n)) < 0:
            return 10**6, None
        M |= KB.raster(u2, n, (uv[5] & 0xF) * 128, N.shape)
    return int((M & ~N).sum()), M
kasten = {32, 33, 34, 35, 36, 37, 38, 39}
g11 = [x for x in faces(ms[11]) if max(x[2][3*j+1] for j in range(x[3])) > 56]
g12 = [x for x in faces(ms[12]) if (x[2][2] >> 6) == 490]
g12h = [x for x in g12 if not set(x[1][1::2]) & kasten]
g12k = [x for x in g12 if set(x[1][1::2]) & kasten]
for name, gr in (('M11 unten', g11), ('M12 alle 490', g12), ('M12 Hand ohne Kasten', g12h), ('M12 Kastenfinger', g12k)):
    us = [x[2][3*j] for x in gr for j in range(x[3])]; vs = [x[2][3*j+1] for x in gr for j in range(x[3])]
    u0, v0 = min(us), min(vs)
    best = None
    for s100 in range(100, 30, -2):
        s = s100 / 100.0
        ok = []
        for tu in range(0, 40):
            for tv in range(0, 57):
                a, M = aussen(gr, s, u0, v0, tu, tv)
                if a == 0:
                    ok.append((tu, tv))
        if ok:
            best = (s, ok)
            break
    print(name, len(gr), 'Flaechen, UV u %d-%d v %d-%d' % (u0, max(us), v0, max(vs)), '-> groesstes s', best[0] if best else None,
          'Schuebe', len(best[1]) if best else 0, best[1][:12] if best else '')
