# Laufrichtung der Marmorierung: Strukturtensor (L*) im Streifen A u 44-83 v 30-88 und am EM30-Oberarm (Seite 0, Zeile 480, u 2-75 v 0-140)
import sys, numpy as np
from scipy import ndimage as nd
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
import birkin_formen as BF
from P_farbe import lab
mo = BF.modell_aus_blob(open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD','rb').read())
def richtung(row, u0, v0, u1, v1, page):
    L = lab(mo.pal[row][mo.tex[v0:v1+1, page*128+u0:page*128+u1+1]].astype(float))[..., 0]
    L = nd.gaussian_filter(L, 1.0)
    gy, gx = np.gradient(L)
    Jxx, Jyy, Jxy = nd.gaussian_filter(gx*gx, 3), nd.gaussian_filter(gy*gy, 3), nd.gaussian_filter(gx*gy, 3)
    # Kantenrichtung (Linien) senkrecht zum mittleren Gradienten: Anteil der Gradientenenergie in v (gy) -> Linien laufen laengs u
    return dict(anteil_gradient_v=round(float(Jyy.sum() / (Jxx.sum() + Jyy.sum())), 2))
print('Streifen A (Seite 2, Zeile 482) u 44-83 v 30-88:', richtung(2, 44, 30, 83, 88, 2))
print('Oberarm EM30 (Seite 0, Zeile 480) u 2-75 v 0-140:', richtung(0, 2, 0, 75, 140, 0))
