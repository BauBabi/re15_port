# Runde 7e2: Daumen-Flaechen von Mesh 12 (ohne Daumenkralle 491) - Lage und Capcom-UV-Texel auf den Zeilen 482/490/491/492/495
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, cv2, v8_gemein as V, kopf_orig_bau as KB, handfarbe_r7e as HF, hand_r6 as H6
alt = open(r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7e/emd/ALT_5c902d49.EMD', 'rb').read()
d, _o, ms, tim = KB.teile(alt); t = KB.tim_teile(tim)
m = ms[12]
P = np.array([v[:3] for v in m['v']], float)
def lab_row(z):
    c = t['clut'][z - 480].astype(int)
    rgb = np.stack([(c & 31), (c >> 5) & 31, (c >> 10) & 31], 1).astype(np.float32) / 31.0
    return cv2.cvtColor(rgb[None, :, ::-1].copy(), cv2.COLOR_BGR2Lab)[0]
LR = {z: lab_row(z) for z in (482, 490, 491, 492, 495)}
# Achse der Hand: Handgelenk (Ecken 1-4) -> Mitte der Krallen-Ringe A/B/C (Modellraum Knochen 12)
w = P[[1, 2, 3, 4]].mean(0); kr = np.mean([P[H6.KRALLEN[n][1]].mean(0) for n in 'ABC'], 0); spitze = max(P[[H6.KRALLEN[n][0] for n in 'ABC']] @ ((kr - w) / np.linalg.norm(kr - w)) - w @ ((kr - w) / np.linalg.norm(kr - w)))
ax = (kr - w) / np.linalg.norm(kr - w)
print('Achse Handgelenk -> Krallenringe, Laenge bis zur fernsten Krallenspitze %.0f' % spitze)
print('Ecken 13-16, 23-26 (Daumen?) und 18-22 (Daumenkralle): Anteil entlang der Achse, Abstand von der Achse')
for i in list(range(13, 17)) + list(range(18, 27)):
    q = P[i] - w
    print('  Ecke %d t %.2f quer %.0f' % (i, (q @ ax) / spitze, np.linalg.norm(q - (q @ ax) * ax)))
# Klauen-Texel (Flaechen auf 492) als "roter Texelbereich"
rot_tex = np.zeros((256, 384), bool)
for k, f, uv in HF._flaechen(m):
    if HF.klasse12(f) == 'finger':
        rot_tex |= KB.raster(uv, HF._n(uv), (uv[5] & 0xF) * 128)
print('Texel unter den 18 Fingerkrallen-Flaechen: %d' % rot_tex.sum())
print('Flaechen mit einer Ecke aus 13-16 / 23-26 (Daumen) - Capcom-UV (5c902d49 = EM30):')
for k, f, uv in HF._flaechen(m):
    e = set(f[1::2])
    if not (e & set(range(13, 17)) | e & set(range(23, 27))) or HF.klasse12(f) != 'hand':
        continue
    mk = KB.raster(uv, HF._n(uv), (uv[5] & 0xF) * 128)
    idx = t['pix'][mk]
    tt = [((P[j] - w) @ ax) / spitze for j in f[1::2]]
    s = ' '.join('%d: L %4.1f a %4.1f b %5.1f rot %.2f' % (z, np.median(LR[z][idx, 0]), np.median(LR[z][idx, 1]), np.median(LR[z][idx, 2]), (LR[z][idx, 1] >= 10).mean()) for z in (495, 482, 491, 492))
    print('  %-4s Ecken %s t %.2f-%.2f  Texel %3d, davon unter Krallen %3d | %s' % (k, sorted(e), min(tt), max(tt), mk.sum(), (mk & rot_tex).sum(), s))
