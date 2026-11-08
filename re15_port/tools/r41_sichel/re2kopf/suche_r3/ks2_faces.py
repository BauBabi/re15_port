import sys, json
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB
emd = sys.argv[1]; box = json.loads(sys.argv[2]); P = G.posen(); mo = b2.modell(emd)
prot = json.load(open(emd + '.json')) if len(sys.argv) < 4 else json.load(open(sys.argv[3]))
z = prot['zonen']
img, ids = G.zeichne(mo, P['85']); k = ids == 9
x0, y0, x1, y1 = box
win = np.zeros_like(k); win[y0:y1 + 1, x0:x1 + 1] = True
# Vorderkante: rechtestes Kopfpixel je Zeile
fr = np.zeros_like(k)
for y in range(480):
    xs = np.nonzero(k[y])[0]
    if len(xs): fr[y, xs.max() - 1:xs.max() + 1] = True
res = []
m9 = mo.meshes[9]
for kind, n in (('T', len(m9.tf)), ('Q', len(m9.qf))):
    for i in range(n):
        mm = KM.teil(mo, [i] if kind == 'T' else [], [i] if kind == 'Q' else [])
        _i, idf = G.zeichne(mm, P['85'], licht=False)
        f = (idf == 9) & k
        if f.any():
            zone = 'maul' if (kind == 'T' and i in z['maul_T']) else ('dunkel' if i in z['dunkel_' + kind] else 'grau')
            res.append((kind + str(i), zone, int((f & win).sum()), int((f & fr).sum()), int(f.sum())))
print('Flaeche Zone Fenster Vorderkante gesamt (sichtbar, Pose 85)')
for r in sorted(res, key=lambda r: -(r[2] * 100 + r[3])):
    if r[2] or r[3]: print(r)
