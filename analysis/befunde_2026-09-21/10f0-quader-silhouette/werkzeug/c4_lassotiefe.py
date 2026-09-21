import os, sys, struct
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image
import geom, msk

R_ = "C:/workspace/git/reAi_v2/"
W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"
fig = np.fromfile(W_ + "fig_marke.bin", np.int32).reshape(240, 320)
gez = fig > 0
bk = (1023 * fig.astype(np.int64)) >> 16
masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
lasso = np.zeros((240, 320), bool)
for n, (x0, y0) in {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)}.items():
    a = np.asarray(Image.open(R_ + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
    h, w = a.shape
    lasso[y0:y0 + h, x0:x0 + w] |= (a > 110)
tl = np.full((240, 320), 1 << 30, np.int64); tl[lasso] = tief[lasso]
vl = gez & (tl < bk)
vd = gez & (tief < bk) & ~vl
print("durch Lasso verdeckt %d: Maskentiefe %d..%d, Figur-Bucket %d..%d"
      % (vl.sum(), tl[vl].min(), tl[vl].max(), bk[vl].min(), bk[vl].max()))
print("durch Tiefschwarz     %d: Maskentiefe %d..%d, Figur-Bucket %d..%d"
      % (vd.sum(), tief[vd].min(), tief[vd].max(), bk[vd].min(), bk[vd].max()))

# Der Stuhl, der wirklich vor ihm steht: Quader (-1600,12200,1500,1500,-1950)
rdt = open(W_ + "re15_port/shared_assets/PSX/STAGE1/ROOM10F0.RDT", "rb").read()
cam = struct.unpack_from("<I", rdt, 0x24)[0]
Rm, t, H = geom.cut_view(rdt, cam, 4)
for (nm, X, Z) in [("x-1600 z12200", -1600, 12200), ("x1550 z11650", 1550, 11650),
                   ("x-1800 z8200", -1800, 8200)]:
    vz, reg = geom.quader_tiefe(Rm, t, H, X, X + 1500, Z, Z + 1500, -1950)
    v = np.asarray(vz, float)
    g = np.isfinite(v) & (v > 0) & np.asarray(reg, bool)
    if g.any():
        print("  Quader %-14s Kamera-z %.0f..%.0f -> Tiefe %d..%d, Silhouette %d Punkte"
              % (nm, v[g].min(), v[g].max(), msk.bucket(v[g].min()), msk.bucket(v[g].max()), int(g.sum())))
print("  Figur an der Marke: Kamera-z %d..%d -> Bucket %d..%d"
      % (fig[gez].min(), fig[gez].max(), bk[gez].min(), bk[gez].max()))
