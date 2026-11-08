"""Pruefung V4 r2 Sicht (Wiederholung): Unterarm (B)-links (Id 14) gegen die dunkle Region im Vorlagebild 1 (Box 298-325 x 262-340,
2 px erodiert); dazu Box-auf-Port (dieselbe Box auf der V4-Buehne) und die Lage von Id 14 in V4."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2 = sys.argv[1]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
o1 = L(R2 + '/vorlage1_entbl.png'); v4 = L(R2 + '/V4_135.png'); V = G.videoraum(v4); ids = np.load(R2 + '/V4_135_ids.npy')[:480]
ref = L(R2 + '/REF2_135.png'); Vr = G.videoraum(ref); idr = np.load(R2 + '/REF2_135_ids.npy')[:480]
def er(m): return cv2.erode(m.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
b = np.zeros((480, 640), bool); b[264:339, 300:324] = True
def st(img, m): lab = G.lab(img[m]); return dict(n=int(m.sum()), lab=G.r1(lab.mean(0)), L_p10_50_90=G.r1(np.percentile(lab[:, 0], [10, 50, 90])), hell30=round(float((lab[:, 0] > 30).mean()), 3))
o = st(o1, b); out = {'orig_box_298-325x262-340': o}
for nm, img, ii in (('V4', V, ids), ('REF2', Vr, idr)):
    m = er(ii == 14); s = st(img, m); s['dE00'] = round(G.de00(o['lab'], s['lab']), 1); ys, xs = np.nonzero(ii == 14); s['id14_box'] = [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]; out[nm + '_Id14'] = s
    s2 = st(img, b & (ii >= 0)); s2['dE00'] = round(G.de00(o['lab'], s2['lab']), 1); out[nm + '_box_auf_port'] = s2
print(json.dumps(out, indent=1)); json.dump(out, open(sys.argv[2] + '/PS2_unterarm_links.json', 'w'), indent=1)
