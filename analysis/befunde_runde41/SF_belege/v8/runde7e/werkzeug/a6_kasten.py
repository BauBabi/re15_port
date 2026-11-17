# Lage des Kastenfingers und der Handteile entlang der Finger-Achse (Anteil der Handlaenge), gegen S6 der Vorlage
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, riegel_handfarbe as RF, hand_mass as HM, v4_gemein as G
P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
plain = sys.argv[1]; hf = sys.argv[2]
for f, b in RF.PAARE_F:
    a = RF.ppm(plain, f); h = RF.ppm(hf, f)
    k = RF.klassen(h, a)
    bb = RF.bereiche_modell(k)
    W = np.array(bb['W']); T = np.array(bb['T']); ax = (T - W) / np.linalg.norm(T - W); Lh = np.linalg.norm(T - W)
    v = HM.vorlage(b)
    out = []
    for n in ('hand', 'kastenfinger', 'daumenkralle', 'finger'):
        yy, xx = np.nonzero(k[n])
        t = ((xx - W[0]) * ax[0] + (yy - W[1]) * ax[1]) / Lh
        lab = G.lab(a[k[n]].astype(float))
        out.append('%s %d px t p10/50/90 %.2f %.2f %.2f rot %.2f L %.1f' % (n, len(t), *np.percentile(t, [10, 50, 90]), (lab[:, 1] >= 10).mean(), np.median(lab[:, 0])))
    print('F%d S6 Modell %.3f Vorlage birkin%d %.3f | %s' % (f, bb['s6_px'] / bb['hand_px'], b, v['bis_krallenwurzel'] / v['hand_laenge'], ' | '.join(out)))
