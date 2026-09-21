"""Welche HERKUNFT hat jeder verdeckte Figurpunkt an der Marke?
   - Lasso      : der verdeckende Texel gehoert zur Freistellung des Nutzers
   - Tiefschwarz: er kam erst durch die Regel Kanalsumme<45 (raum.py:327/354)
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image
from scipy import ndimage as nd
import msk

SC = os.path.dirname(os.path.abspath(__file__)) + "/"
R_ = "C:/workspace/git/reAi_v2/"
W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"

fig = np.fromfile(W_ + "fig_marke.bin", np.int32).reshape(240, 320)
gez = fig > 0
bk = (1023 * fig.astype(np.int64)) >> 16
masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
img = np.asarray(Image.open(R_ + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C4.png').convert('RGB'), np.uint8)
bg = img[:, :640][0::2, 0::2]
lasso = np.zeros((240, 320), bool)
for n, (x0, y0) in {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)}.items():
    a = np.asarray(Image.open(R_ + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
    h, w = a.shape
    lasso[y0:y0 + h, x0:x0 + w] |= (a > 110)
dunkel = bg.astype(int).sum(2) < 45

verd = gez & (tief < bk)
tl = np.full((240, 320), 1 << 30, np.int64); tl[lasso] = tief[lasso]
verd_l = gez & (tl < bk)
verd_d = verd & ~verd_l
print("verdeckt gesamt %d | durch Lasso-Texel %d | erst durch Tiefschwarz %d"
      % (verd.sum(), verd_l.sum(), verd_d.sum()))
ys, xs = np.nonzero(verd_d)
if len(ys):
    print("  Tiefschwarz-Verdeckung bbox x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
    dist = nd.distance_transform_edt(~lasso)
    dd = dist[verd_d]
    print("  Abstand des verdeckenden Texels zur Freistellung: min %.0f Median %.0f max %.0f"
          % (dd.min(), np.median(dd), dd.max()))

ov = bg.copy()
ov[gez] = [90, 90, 255]
ov[verd_l] = [255, 130, 0]
ov[verd_d] = [255, 0, 0]
c = ov[45:130, 66:130]
Image.fromarray(c).resize((c.shape[1] * 9, c.shape[0] * 9), Image.NEAREST).save(SC + 'marke_herkunft.png')

# Zeilenprofil
print()
print(" Zeile | gezeichnet | durch Lasso | durch Tiefschwarz")
for y in range(55, 120):
    a = int(gez[y].sum())
    if not a: continue
    print("  %4d | %10d | %11d | %d" % (y, a, int(verd_l[y].sum()), int(verd_d[y].sum())))
