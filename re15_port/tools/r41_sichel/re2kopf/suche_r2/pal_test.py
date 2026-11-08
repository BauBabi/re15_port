"""Palettentransformation der G2-Zeile: L* x k (a*, b* fest), 5-Bit-Quantisierung; Kopf-Median im RDT-Licht je k (Pose 135 / 85) am
Runde-1-EMD (nur LUT-Zeile 486 ersetzt, Texel unveraendert)."""
import os, sys, json
import numpy as np
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB
from skimage.color import rgb2lab, lab2rgb
P = G.posen()

def zeile_transform(clut15, k, g=1.0, cap=None):
    """15-Bit-CLUT-Zeile -> RGB (x8 wie BB.modell) -> Lab -> L*' = k x min(L*, cap), a*/b* x g -> RGB -> 5 Bit. 0 bleibt 0; nichts wird neu 0."""
    c = np.asarray(clut15).astype(np.int64)
    rgb = np.stack([(c & 31) * 8, ((c >> 5) & 31) * 8, ((c >> 10) & 31) * 8], -1) / 255.0
    lab = rgb2lab(rgb.reshape(-1, 1, 3)).reshape(-1, 3)
    L = lab[:, 0] if cap is None else np.minimum(lab[:, 0], cap)
    lab2 = np.stack([np.clip(L * k, 0, 100), lab[:, 1] * g, lab[:, 2] * g], 1)
    rgb2 = np.clip(lab2rgb(lab2.reshape(-1, 1, 3)).reshape(-1, 3), 0, 1)
    q = np.clip(np.rint(rgb2 * 255 / 8.0), 0, 31).astype(np.int64)
    w = q[:, 0] | (q[:, 1] << 5) | (q[:, 2] << 10) | (c & 0x8000)
    w = np.where(c == 0, 0, np.where(w == 0, 1, w))
    return w

if __name__ == '__main__':
    emd = os.path.join(KB.SP, 'sfv4', 'emd', 'EM30_V4re2.EMD')
    mo = b2.modell(emd)
    import struct
    z = KB.ZEILE_G2 - mo.clut_y
    lut0 = mo.lut.copy()
    # 15-Bit-Zeile aus dem EMD
    import emd_ansichtsblatt as EA, birkin_formen as BF
    blob = open(emd, 'rb').read(); d = EA.emd_dir(blob)
    _cx, _cy, _cw, ch, c, _e = BF._clut(blob[d[8]:])
    row = c[z]
    for k in (1.0, 1.4, 1.6, 1.8, 2.0, 2.2):
        w = zeile_transform(row, k)
        lut = lut0.copy()
        lut[z, :, 0] = (w & 31) << 3; lut[z, :, 1] = ((w >> 5) & 31) << 3; lut[z, :, 2] = ((w >> 10) & 31) << 3; lut[z, :, 3] = (w != 0) * 255
        mo.lut = lut
        out = []
        for h in ('135', '85'):
            il, ids = G.zeichne(mo, P[h])
            Ll = G.lab(il.astype(float))[..., 0]
            k9 = ids == 9; r8 = ids == 8
            out.append('%s Kopf p50 %.1f Rumpf %.1f Verh %.2f' % (h, np.median(Ll[k9]), np.median(Ll[r8]), Ll[k9].mean() / Ll[r8].mean()))
        lab = rgb2lab((np.stack([(w & 31) * 8, ((w >> 5) & 31) * 8, ((w >> 10) & 31) * 8], -1) / 255.0).reshape(-1, 1, 3)).reshape(-1, 3)
        print('k %.1f: %s | Palette L* p50 %.1f, Eintraege gesaettigt (L*>=99) %d' % (k, ' | '.join(out), np.median(lab[row != 0, 0]), int((lab[:, 0] >= 99).sum())))
    mo.lut = lut0
