import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
from PIL import Image, ImageDraw
import msk

R_ = "C:/workspace/git/reAi_v2/"
W_ = "C:/workspace/git/reAi_v2/.claude/worktrees/wf_b4972d66-6b5-2/"
OUT = W_ + "analysis/befunde_2026-09-21/10f0-quader-silhouette/"
os.makedirs(OUT, exist_ok=True)

LAG = {4: {'04_01': (88, 84), '04_02': (68, 109), '04_03': (59, 218)},
       5: {'05_01': (53, 167), '05_02': (102, 119), '05_03': (114, 97)}}

def bg_of(cut):
    img = np.asarray(Image.open(R_ + 'analysis/befunde_2026-09-21/pri-runde19/zensus-kaesten/ausgeliefert_ROOM10F0_C%d.png' % cut).convert('RGB'), np.uint8)
    return img[:, :640][0::2, 0::2].copy()

def lasso_of(cut):
    m = np.zeros((240, 320), bool)
    for n, (x0, y0) in LAG[cut].items():
        a = np.asarray(Image.open(R_ + 'pri/STAGE1/10F0/%s.png' % n).convert('RGBA'))[:, :, 3]
        h, w = a.shape
        m[y0:y0 + h, x0:x0 + w] |= (a > 110)
    return m

def sp(arr, name, k=2, crop=None):
    a = arr[crop[1]:crop[3], crop[0]:crop[2]] if crop else arr
    Image.fromarray(a).resize((a.shape[1] * k, a.shape[0] * k), Image.NEAREST).save(OUT + name)

# 01 — der Abzug des Nutzers, Ausschnitt um Leon
shot = Image.open(R_ + 're15_port/build/platform/pc/befund_10F0_F335_marke1.bmp').convert('RGB')
shot.crop((66 * 3, 45 * 3, 130 * 3, 130 * 3)).resize((64 * 9, 85 * 9), Image.NEAREST).save(OUT + '01_marke_nutzer.png')

bg4 = bg_of(4); las4 = lasso_of(4)
d4 = bg4.astype(int).sum(2) < 45
masks, gc, mdecl, draw, tief4, wer, px, clut = msk.tiefenkarte('ROOM10F0', 4)
deck4 = tief4 < (1 << 30)

# 02 — reiner Hintergrund des Blocks
sp(bg4, '02_hintergrund_block.png', 10, (66, 70, 126, 135))

# 03 — die ausgelieferte Deckung ueber dem Block
ov = bg4.copy(); ov[deck4] = [255, 0, 255]
sp(ov, '03_maskendeckung_block.png', 10, (66, 70, 126, 135))

# 04 — Herkunft ueber den ganzen Winkel
ov = bg4.copy()
ov[deck4 & las4] = [0, 255, 0]
ov[deck4 & ~las4 & d4] = [255, 0, 0]
ov[deck4 & ~las4 & ~d4] = [255, 255, 0]
sp(ov, '04_herkunft_c4.png', 2)
sp(ov, '05_herkunft_block.png', 10, (66, 70, 126, 135))

# 06/07 — Figur an der Marke: Herkunft der Verdeckung / Gegenmodell
fig = np.fromfile(W_ + "fig_marke.bin", np.int32).reshape(240, 320)
gez = fig > 0
bk = (1023 * fig.astype(np.int64)) >> 16
tl = np.full((240, 320), 1 << 30, np.int64); tl[las4] = tief4[las4]
verd = gez & (tief4 < bk); verd_l = gez & (tl < bk); verd_d = verd & ~verd_l
ov = bg4.copy(); ov[gez] = [90, 90, 255]; ov[verd_l] = [255, 140, 0]; ov[verd_d] = [255, 0, 0]
sp(ov, '06_marke_herkunft.png', 9, (66, 45, 130, 130))
ov = bg4.copy(); ov[gez] = [90, 90, 255]; ov[verd_l] = [255, 140, 0]
sp(ov, '07_marke_gegenmodell_lasso.png', 9, (66, 45, 130, 130))

# 08 — Cut 5
bg5 = bg_of(5); las5 = lasso_of(5); d5 = bg5.astype(int).sum(2) < 45
m5, _, _, _, tief5, _, _, _ = msk.tiefenkarte('ROOM10F0', 5)
deck5 = tief5 < (1 << 30)
ov = bg5.copy()
ov[deck5 & las5] = [0, 255, 0]
ov[deck5 & ~las5 & d5] = [255, 0, 0]
sp(ov, '08_herkunft_c5.png', 2)

# 09 — die NACHFRAGE: was soll der Nutzer freistellen?
ov = bg4.copy()
ov[las4] = (ov[las4] * 0.35 + np.array([0, 255, 0]) * 0.65).astype(np.uint8)
ov[deck4 & ~las4] = (ov[deck4 & ~las4] * 0.45 + np.array([255, 0, 0]) * 0.55).astype(np.uint8)
im = Image.fromarray(ov).resize((960, 720), Image.NEAREST)
dr = ImageDraw.Draw(im)
dr.rectangle([76 * 3, 80 * 3, 116 * 3, 125 * 3], outline=(255, 255, 0), width=3)
im.save(OUT + '09_nachfrage_c4.png')
im2 = Image.fromarray(ov[70:135, 66:126]).resize((60 * 12, 65 * 12), Image.NEAREST)
im2.save(OUT + '10_nachfrage_zoom.png')
print("geschrieben nach", OUT)
for f in sorted(os.listdir(OUT)):
    print("  ", f)
