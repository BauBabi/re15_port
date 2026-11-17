# K2: Flaechen der Schwanzwurzel in F312-F324. Offline-ID (f_rast, szene_posen) gegen den echten Klassen-Marker-Framedump geprueft.
import sys, os, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
from PIL import Image
import v8_gemein as V, r3_gemein as R3, f_rast as FR, glaette_r7b as GL, unterseite_r5 as U5, kopf_orig_bau as KB
emd, dl, dm = sys.argv[1], sys.argv[2], sys.argv[3]
blob = open(emd, 'rb').read(); mo = V.modell(blob); ms = KB.teile(blob)[2]
par = json.load(open(os.path.join(U5.HIER, 'unterseite_v8.json')))
kl = U5.flaechen_klassen(ms, par)
klm = {b: {k: (c, [None if t is None else round(t) for t in th]) for k, c, th in kl[b]} for b in V.SCHWANZ}
sp = R3.szene_posen()
F0, F1 = int(sys.argv[4]), int(sys.argv[5])
for F in range(F0, F1 + 1):
    A = FR.zeichne(mo, sp[F], None, licht=False)
    bone = A['bone'][:480]; z = A['zeile'][:480]
    off_u = (bone >= 16) & (bone <= 22) & (z == 495); off_o = (bone >= 16) & (bone <= 22) & (z == 487)
    a = np.asarray(Image.open(os.path.join(dl, 'fd', 'f%06d.ppm' % F)).convert('RGB'))
    m = np.asarray(Image.open(os.path.join(dm, 'fd', 'f%06d.ppm' % F)).convert('RGB'))
    k = GL.klassen_bild(m, a)
    k = k[::3, ::3].repeat(2, 0).repeat(2, 1)[:480, :640] if False else np.array(Image.fromarray(k.astype(np.int16)).resize((640, 480), Image.NEAREST))
    re_u, re_o = k == 0, k == 1
    iou = lambda p, q: (p & q).sum() / max((p | q).sum(), 1)
    # Flaechen nach Pixeln (offline)
    cnt = {}
    ys, xs = np.nonzero((bone >= 16) & (bone <= 22))
    for y, x in zip(ys, xs):
        key = (int(bone[y, x]), str(A['flaeche'][y, x])); cnt[key] = cnt.get(key, 0) + 1
    top = sorted(cnt.items(), key=lambda t: -t[1])[:14]
    print('F%d echt unten %d oben %d | offline unten %d oben %d | IoU unten %.2f oben %.2f' % (F, re_u.sum(), re_o.sum(), off_u.sum(), off_o.sum(), iou(re_u, off_u), iou(re_o, off_o)))
    print('   ', ' '.join('%d:%s(%s,%s)=%d' % (b, f, klm[b][f][0], klm[b][f][1], n) for (b, f), n in top))
