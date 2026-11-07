"""Pruefung V4 r5 Sicht: Kopf vorn - Textur (Lum-Gradient) Kopf gegen Rumpf im Original (Boxen) und in V4 (Ids,
Videoraum = gleiche Unschaerfe wie das Video) - ist der Kopf so fleckig wie der Koerper oder eine glatte Kuppel?"""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2 = sys.argv[1]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def lum(a): return a @ np.array([0.299, 0.587, 0.114])
def grad(Lm): return np.hypot(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0), cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1))
def box(b): m = np.zeros((480, 640), bool); m[b[1]:b[3]+1, b[0]:b[2]+1] = True; return m
def er(m, n=2): return cv2.erode(m.astype(np.uint8), np.ones((2*n+1, 2*n+1), np.uint8)).astype(bool)
o1 = L(R2 + '/vorlage1_entbl.png'); go = grad(lum(o1)); out = {}
out['orig'] = dict(kopf=round(float(go[er(box((368, 139, 416, 191)))].mean()), 1), rumpf=round(float(go[er(box((352, 196, 448, 292)))].mean()), 1), lum_std_kopf=round(float(lum(o1)[er(box((368, 139, 416, 191)))].std()), 1), lum_std_rumpf=round(float(lum(o1)[er(box((352, 196, 448, 292)))].std()), 1))
for nm in ('V4', 'REF2', 'V4r4'):
    v = L(R2 + '/%s_135.png' % nm); V = G.videoraum(v); ids = np.load(R2 + '/%s_135_ids.npy' % nm)[:480]; g = grad(lum(V))
    out[nm + '_videoraum'] = dict(kopf=round(float(g[er(ids == 9)].mean()), 1), rumpf=round(float(g[er(ids == 8)].mean()), 1), lum_std_kopf=round(float(lum(V)[er(ids == 9)].std()), 1), lum_std_rumpf=round(float(lum(V)[er(ids == 8)].std()), 1))
for k, v in out.items(): v['verh_kopf_rumpf_grad'] = round(v['kopf'] / v['rumpf'], 2)
print(json.dumps(out, indent=1)); json.dump(out, open(sys.argv[2] + '/PS5_kopf_textur_vorn.json', 'w'), indent=1)
