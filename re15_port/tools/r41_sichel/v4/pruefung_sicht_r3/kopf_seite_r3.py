"""Pruefung V4 r3 Sicht: Kopf seitlich (Pose 85) - Gradient (Struktur) und Dunkelanteil im Videoraum, Original Vorlage 2 roh
(Kopfbox 412-467 x 190-263 und enge Box 440-474 x 205-255) gegen Id 9 von V4 r3 / V4 r2; Rumpf-Vergleich wie vorn (Kopf/Rumpf)."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R = sys.argv[1]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def grad(Lm): return np.hypot(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0), cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1))
o2 = L(R + '/vorlage2_entbl.png'); lo = G.lum(o2); go = grad(lo); out = {}
def bx(b): m = np.zeros((480, 640), bool); m[b[1]:b[3]+1, b[0]:b[2]+1] = True; return m
for nm, b in (('orig_kopfbox', (412, 190, 467, 263)), ('orig_eng', (440, 205, 474, 255)), ('orig_buckel_rumpf', (372, 186, 414, 260))):
    m = bx(b); out[nm] = dict(grad=round(float(go[m].mean()), 1), glatt_lt8=round(float((go[m] < 8).mean()), 3), lum_std=round(float(lo[m].std()), 1))
for nm in ('V4', 'V4r2'):
    V = G.videoraum(L(R + '/%s_85.png' % nm)); ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]; lv = G.lum(V); g = grad(lv)
    for i, k in ((9, 'Id9_Kopf'), (8, 'Id8_Rumpf')):
        m = cv2.erode((ids == i).astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
        out['%s_%s' % (nm, k)] = dict(grad=round(float(g[m].mean()), 1), glatt_lt8=round(float((g[m] < 8).mean()), 3), lum_std=round(float(lv[m].std()), 1), n=int(m.sum()))
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
