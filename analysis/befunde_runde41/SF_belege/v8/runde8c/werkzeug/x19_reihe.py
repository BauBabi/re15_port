"""Runde 8c Messung (keine Eichung): Bauchfarbe je Bild F255-F320 (jedes 5.) im echten Spielbild, Lab-Abstand zur Video-Mitte (Pruefer gruenblut,
RGB (98,5, 58,5, 59,5) = Mitte von (89-108, 51-66, 52-67)); Zusammenfassung Median / Min / Max ueber die Bilder mit >= 500 Bauch-Bildpunkten."""
import os, sys, colorsys
import numpy as np
from PIL import Image
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import v4_gemein as G
bild = lambda l, F: np.asarray(Image.open(os.path.join(l, 'fd', 'f%06d.ppm' % F)).convert('RGB')).astype(float)
ziel = G.lab(np.array([[[98.5, 58.5, 59.5]]]))[0, 0]
bez, mb = sys.argv[1:3]
for lauf in sys.argv[3:]:
    rows = []
    for F in range(255, 321, 5):
        B = bild(bez, F)
        m = np.abs(bild(mb, F) - B).sum(-1) > 24
        if m.sum() < 500:
            continue
        med = np.median(bild(lauf, F)[m], axis=0)
        _h, s, v = colorsys.rgb_to_hsv(*(med / 255.0))
        lab = G.lab(med[None, None])[0, 0]
        rows.append((F, s, v, lab[0], np.hypot(lab[1], lab[2]), np.degrees(np.arctan2(lab[2], lab[1])), float(np.linalg.norm(lab - ziel))))
    a = np.array([r[1:] for r in rows])
    f = lambda k: '%.2f [%.2f-%.2f]' % (np.median(a[:, k]), a[:, k].min(), a[:, k].max())
    g = lambda k: '%.1f [%.1f-%.1f]' % (np.median(a[:, k]), a[:, k].min(), a[:, k].max())
    print('%-14s %d Bilder: S %s V %s L* %s C* %s h %s dE zur Video-Mitte %s' % (os.path.basename(lauf.rstrip('/')), len(rows), f(0), f(1), g(2), g(3), g(4), g(5)))
