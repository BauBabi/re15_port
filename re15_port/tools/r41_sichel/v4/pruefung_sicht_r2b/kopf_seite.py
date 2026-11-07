"""Pruefung V4 r2 Sicht (Wiederholung): Kopf seitlich - Dunkelanteile, Flaechigkeit (Gradient) und Form (Spaltenhoehen) im
Original (Vorlage 2 roh, Kopfbox 412-467 x 190-263 des Pruefers r1 und engere Box um Auge/Haare 440-474 x 205-255) gegen V4 Id 9 Pose 85."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
R2 = sys.argv[1]
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
o2 = L(R2 + '/vorlage2_entbl.png'); v4 = L(R2 + '/V4_85.png'); V = G.videoraum(v4); ids = np.load(R2 + '/V4_85_ids.npy')[:480]
lo = G.lab(o2)[..., 0]; lv = G.lab(V)[..., 0]; ls = G.lab(v4)[..., 0]
def grad(Lm): return np.hypot(cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 1, 0), cv2.Sobel(Lm.astype(np.float32), cv2.CV_32F, 0, 1))
out = {}
for nm, bx in (('orig_box412-467x190-263', (412, 190, 467, 263)), ('orig_box440-474x205-255', (440, 205, 474, 255))):
    m = np.zeros((480, 640), bool); m[bx[1]:bx[3]+1, bx[0]:bx[2]+1] = True
    out[nm] = dict(L_lt8=round(float((lo[m] < 8).mean()), 3), L_lt12=round(float((lo[m] < 12).mean()), 3), L_min=round(float(lo[m].min()), 1), L_p5=round(float(np.percentile(lo[m], 5)), 1), grad_mittel=round(float(grad(lo)[m].mean()), 1))
m = cv2.erode((ids == 9).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
out['V4_Id9_videoraum'] = dict(L_lt8=round(float((lv[m] < 8).mean()), 3), L_lt12=round(float((lv[m] < 12).mean()), 3), L_min=round(float(lv[m].min()), 1), L_p5=round(float(np.percentile(lv[m], 5)), 1), grad_mittel=round(float(grad(lv)[m].mean()), 1))
out['V4_Id9_schirm'] = dict(L_lt8=round(float((ls[m] < 8).mean()), 3), grad_mittel=round(float(grad(ls)[m].mean()), 1), grad_lt4_anteil=round(float((grad(ls)[m] < 4).mean()), 3))
mr = cv2.erode((ids == 8).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
out['V4_Id8_Rumpf_schirm'] = dict(grad_mittel=round(float(grad(ls)[mr].mean()), 1), grad_lt4_anteil=round(float((grad(ls)[mr] < 4).mean()), 3))
print(json.dumps(out, indent=1)); json.dump(out, open(sys.argv[2] + '/PS2_kopf_seite.json', 'w'), indent=1)
