"""Zahlen-Pruefer V4 r2: liegen geaenderte Texel in UV-Flaechen der Koerpermeshes 0-8/10-15 (Ref2)?"""
import sys, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
from pz2_g1 import secs, md1, tim, R, S
a, b = open(R, 'rb').read(), open(S + 'EM30_V4.EMD', 'rb').read()
A, B = secs(a), secs(b); ta, tb = tim(A[8]), tim(B[8]); dm = (ta['pix'] != tb['pix'])
mb = md1(B[7])
belegt = np.zeros_like(dm, np.uint8)
for i in [k for k in range(16) if k != 9]:
    for q in mb[i]['quv_raw']:
        pg = q[5] & 0xf
        pts = np.array([[q[0], q[1]], [q[3], q[4]], [q[9], q[10]], [q[6], q[7]]], np.int32); pts[:, 0] += pg * 128
        cv2.fillPoly(belegt, [pts], 1); cv2.polylines(belegt, [pts], True, 1)
    for t in mb[i]['tuv_raw']:
        pg = t[5] & 0xf
        pts = np.array([[t[0], t[1]], [t[3], t[4]], [t[6], t[7]]], np.int32); pts[:, 0] += pg * 128
        cv2.fillPoly(belegt, [pts], 1); cv2.polylines(belegt, [pts], True, 1)
print('Diff-Texel gesamt', int(dm.sum()), 'davon in Koerper-UV', int((dm & (belegt > 0)).sum()))
ys, xs = np.nonzero(dm & (belegt > 0))
if len(xs): print('bbox', xs.min(), ys.min(), xs.max(), ys.max())
# Kopf/Schwanz-Flaechen
for nm, (u0, u1, v0, v1) in dict(kopf_vorn=(0, 47, 160, 223), kopf_seite=(56, 111, 160, 223), schwanz=(256 + 64, 256 + 127, 0, 255)).items():
    sub = dm[v0:v1 + 1, u0:u1 + 1]; print(nm, 'diff', int(sub.sum()), 'von', sub.size)
rest = dm.copy(); rest[160:224, 0:112] = False; rest[:, 320:384] = False
print('Diff ausserhalb Kopf-/Schwanzflaechen', int(rest.sum()))
