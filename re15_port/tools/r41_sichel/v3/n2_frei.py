# SF V3 Runde 2: freie Texelflaechen im EM30-Atlas (welche Texel nutzt irgendein Mesh) - fuer Kopf-Seitenbild und Schulterauge.
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import birkin_formen as BF
from machart_v3 import _tris
blob = open(sys.argv[1], 'rb').read()
mo = BF.modell_aus_blob(blob)
print('tex', mo.tex.shape, 'pal', mo.pal.shape)
belegt = np.zeros(mo.tex.shape, int) - 1
for b, m in enumerate(mo.meshes):
    for P, U, clut, page in _tris(m):
        u0, u1 = int(U[:, 0].min()), int(U[:, 0].max()); v0, v1 = int(U[:, 1].min()), int(U[:, 1].max())
        pb = 128 * (page & 0xF)
        belegt[v0:v1 + 1, pb + u0:pb + u1 + 1] = b
for b in sorted(set(belegt.ravel()) - {-1}):
    ys, xs = np.nonzero(belegt == b)
    print('mesh', b, 'u', xs.min(), xs.max(), 'v', ys.min(), ys.max())
frei = belegt < 0
for pg in range(mo.tex.shape[1] // 128):
    f = frei[:, pg * 128:(pg + 1) * 128]
    rows = np.nonzero(f.all(1))[0]
    print('page', pg, 'freie volle Zeilen', (rows.min(), rows.max(), len(rows)) if len(rows) else None)
    # freie Spaltenbloecke ab v 156
    sub = f[156:]
    cols = np.nonzero(sub.all(0))[0]
    print('   ab v156 freie Spalten', cols.min() if len(cols) else None, cols.max() if len(cols) else None, len(cols))
