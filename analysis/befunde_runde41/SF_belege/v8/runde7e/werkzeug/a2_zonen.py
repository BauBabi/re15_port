# Runde 7e Analyse 2: L*-Profil der Texelzeilen (Seite 2, Zeile 482) unter Mesh 11/12, Grau-Zone, Fussabdruecke der Gruppen
import os, sys
import numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import v8_gemein as V
import kopf_orig_bau as KB

blob = open(V.ASSET, 'rb').read()
d, _o, ms, tim = KB.teile(blob)
t = KB.tim_teile(tim)
pal = t['clut']
def lab_row(z):
    c = pal[z - 480].astype(int)
    rgb = np.stack([(c & 31), (c >> 5) & 31, (c >> 10) & 31], 1).astype(np.float32) / 31.0
    return cv2.cvtColor(rgb[None, :, ::-1].copy(), cv2.COLOR_BGR2Lab)[0]
L482 = lab_row(482); L490 = lab_row(490); L495 = lab_row(495)
pix = t['pix']
def faces(m):
    for kind, F, UV, n in (('d', m['tf'], m['tuv'], 3), ('v', m['qf'], m['quv'], 4)):
        for k, (f, uv) in enumerate(zip(F, UV)):
            yield '%s%d' % (kind, k), f, uv, n
def mask(uv, n):
    return KB.raster(uv, n, (uv[5] & 0xF) * 128, pix.shape)
# Mesh 11 Texelnutzung je Zeile v
use11 = np.zeros(pix.shape, bool)
for key, f, uv, n in faces(ms[11]):
    use11 |= mask(uv, n)
print('L*-Profil (Zeile 482) der von Mesh 11 benutzten Texel je v (Median, Anteil a*>=10 auf Zeile 495):')
for v in range(0, 90, 3):
    sel = use11[v:v+3]
    idx = pix[v:v+3][sel]
    if len(idx):
        print('  v %2d-%2d n %3d L482 %5.1f  L495 %5.1f a495 %5.1f  L490 %5.1f a490 %5.1f' % (v, v+2, len(idx), np.median(L482[idx, 0]),
              np.median(L495[idx, 0]), np.median(L495[idx, 1]), np.median(L490[idx, 0]), np.median(L490[idx, 1])))
# Gruppen
unten = []; oben = []
for key, f, uv, n in faces(ms[11]):
    vs = [uv[3*j+1] for j in range(n)]
    (unten if max(vs) > 56 else oben).append(key)
print('Mesh 11 oben (v <= 56):', oben)
print('Mesh 11 unten (v > 56):', unten)
N = np.zeros(pix.shape, bool)
for key, f, uv, n in faces(ms[11]):
    if key in oben:
        N |= mask(uv, n)
ys, xs = np.nonzero(N)
print('Grau-Zone N: %d Texel, x %d-%d (u %d-%d) v %d-%d' % (N.sum(), xs.min(), xs.max(), xs.min()-256, xs.max()-256, ys.min(), ys.max()))
idx = pix[N]
print('  N auf Zeile 482: L %.1f a %.1f b %.1f (Median), Anteil L<15 %.3f' % (np.median(L482[idx,0]), np.median(L482[idx,1]), np.median(L482[idx,2]), (L482[idx,0] < 15).mean()))
# Zeilen-Profil von N (u-Bereich je v)
for v in range(0, 60, 4):
    r = np.nonzero(N[v])[0]
    print('  N v %2d: u %s' % (v, '%d-%d (%d)' % (r.min()-256, r.max()-256, len(r)) if len(r) else '-'))
np.save(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e/N_grau.npy', N)
# Mesh 12 Gruppen
z = lambda uv: uv[2] >> 6
H = np.zeros(pix.shape, bool); Kasten = np.zeros(pix.shape, bool)
kasten_ecken = {32, 33, 34, 35, 36, 37, 38, 39}
for key, f, uv, n in faces(ms[12]):
    if z(uv) == 490:
        mk = mask(uv, n)
        if set(f[1::2]) & kasten_ecken:
            Kasten |= mk
        else:
            H |= mk
U = np.zeros(pix.shape, bool)
for key, f, uv, n in faces(ms[11]):
    if key in unten:
        U |= mask(uv, n)
for nm, M in (('Hand ohne Kastenfinger', H), ('Kastenfinger', Kasten), ('Mesh 11 unten', U)):
    ys, xs = np.nonzero(M)
    idx = pix[M]
    print('%s: %d Texel u %d-%d v %d-%d; Zeile 490 L %.1f a %.1f; Zeile 482 L %.1f a %.1f; a490>=10: %.3f; in N: %d' % (nm, M.sum(), xs.min()-256, xs.max()-256, ys.min(), ys.max(),
          np.median(L490[idx,0]), np.median(L490[idx,1]), np.median(L482[idx,0]), np.median(L482[idx,1]), (L490[idx,1] >= 10).mean(), (M & N).sum()))
