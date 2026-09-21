"""Schreibt die Maskenfilter (320x240 Byte, 1 = Texel behalten) fuer die Gegenmodelle."""
import os, sys
import numpy as np
from PIL import Image
from scipy import ndimage as nd

W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"
R_ = "C:/workspace/git/reAi_v2/"
LAG = {4: {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)},
       5: {'05_01': (53, 167), '05_02': (102, 119), '05_03': (114, 97)}}

for cut in (4, 5):
    lasso = np.zeros((240, 320), bool)
    for n, (x0, y0) in LAG[cut].items():
        a = np.asarray(Image.open(R_ + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
        h, w = a.shape
        lasso[y0:y0 + h, x0:x0 + w] |= (a > 110)
    lasso.astype(np.uint8).tofile(W_ + "filter_lasso_c%d.bin" % cut)
    img = np.asarray(Image.open(R_ + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C%d.png' % cut).convert('RGB'), np.uint8)
    bg = img[:, :640][0::2, 0::2]
    dunkel = bg.astype(int).sum(2) < 45
    saum = nd.distance_transform_edt(~lasso) <= 7
    (lasso | (dunkel & saum)).astype(np.uint8).tofile(W_ + "filter_saum7_c%d.bin" % cut)
    print("cut %d: Lasso %d, Lasso+Saum7 %d" % (cut, lasso.sum(), (lasso | (dunkel & saum)).sum()))
