# Ein gemeinsamer UV-Massstab fuer Mesh 11 unten + Mesh 12 Hand (mit / ohne Kastenfinger) in die Grau-Zone N
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, v8_gemein as V, kopf_orig_bau as KB, riegel_handfarbe as RF
SP = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e'
N = np.load(SP + '/N_grau.npy')
blob = open(V.ASSET, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
def faces(m):
    for F, U in (('tf', 'tuv'), ('qf', 'quv')):
        for i, f in enumerate(m[F]):
            yield f, list(m[U][i])
def tr(uv, s, u0, v0, tu, tv):
    uv = list(uv); n = 3 if len(uv) == 9 else 4
    for j in range(n):
        uv[3*j] = int(round(s * (uv[3*j] - u0) + tu)); uv[3*j+1] = int(round(s * (uv[3*j+1] - v0) + tv))
    return uv
g11 = [uv for f, uv in faces(ms[11]) if max(uv[3*j+1] for j in range(3 if len(uv) == 9 else 4)) > 56]
g12h = [uv for f, uv in faces(ms[12]) if RF.krallen_klasse(f) == 'hand']
g12k = [uv for f, uv in faces(ms[12]) if RF.krallen_klasse(f) == 'kastenfinger']
for name, gr in (('M11u+M12 Hand', g11 + g12h), ('M11u+M12 Hand+Kasten', g11 + g12h + g12k)):
    us = [uv[3*j] for uv in gr for j in range(3 if len(uv) == 9 else 4)]; vs = [uv[3*j+1] for uv in gr for j in range(3 if len(uv) == 9 else 4)]
    u0, v0 = min(us), min(vs)
    for s100 in range(100, 40, -1):
        s = s100 / 100.0
        ok = []
        for tu in range(0, 40):
            for tv in range(0, 57):
                M = np.zeros(N.shape, bool); bad = False
                for uv in gr:
                    u2 = tr(uv, s, u0, v0, tu, tv)
                    M |= KB.raster(u2, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128, N.shape)
                if not (M & ~N).any():
                    ok.append((tu, tv))
        if ok:
            print(name, 'Ursprung', (u0, v0), 'groesstes s', s, 'Schuebe', ok)
            break
