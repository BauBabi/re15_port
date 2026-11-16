import os, sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np
import v8_gemein as V, kopf_orig_bau as KB, adern_v8 as AV, adern_r4 as A4
from PIL import Image, ImageDraw
MO, TO = A4.original()
z = 482
c = TO['clut'][z - 480].astype(np.int64)[TO['pix']]
im = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1).astype(np.uint8)
nz = A4.nutzung(None, z)
crop = im[0:176, 256:256 + 48].copy()
m = nz[0:176, 256:304]
crop2 = crop.copy(); crop2[~m] = (crop2[~m] * 0.3).astype(np.uint8)
S = 8
a = Image.fromarray(np.concatenate([crop, np.zeros((176, 4, 3), np.uint8), crop2], 1)).resize((100 * S, 176 * S), Image.NEAREST)
d = ImageDraw.Draw(a)
for v in range(0, 176, 8):
    d.text((2, v * S), str(v), fill=(255, 255, 0))
for u in range(0, 48, 8):
    d.text((u * S, 2), str(u), fill=(0, 255, 255))
a.save('orig_p2_zoom.png')
# Lab L* / a* je Zeile v (u 4-36) fuer Zeile 482
L = AV.lab15(TO['clut'][z - 480])
Lp = L[TO['pix'][:, 256:304]]
for v in range(80, 176, 2):
    sel = m[v, :] if m[v, :].any() else np.ones(48, bool)
    print(v, 'genutzt u', np.nonzero(m[v])[0].min() if m[v].any() else -1, np.nonzero(m[v])[0].max() if m[v].any() else -1,
          'L* %.1f sd %.1f a* %.1f' % (Lp[v, 4:37, 0].mean(), Lp[v, 4:37, 0].std(), Lp[v, 4:37, 1].mean()))
