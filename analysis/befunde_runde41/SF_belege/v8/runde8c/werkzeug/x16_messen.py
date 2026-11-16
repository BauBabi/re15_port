"""Runde 8c Messung (keine Eichung): Bauch / Ring / ganzer Schwanz im echten Spielbild (F265/F267/F307 = h22/h24/h64) je Probe-Lauf.
Masken: Bildpunkte, in denen sich ein Masken-Lauf (x10_maske.py: gewaehlte Flaechen einfarbig fast weiss) vom Bezugs-Lauf unterscheidet.
python x16_messen.py <bezug> <maske_bauch> <maske_ring> <maske_alle> <lauf> [<lauf> ...]   Werte: RGB-Median, HSV S / V des Medians, Lab."""
import os, sys, colorsys
import numpy as np
from PIL import Image
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import v4_gemein as G
FR = tuple(int(x) for x in os.environ['FR'].split(',')) if os.environ.get('FR') else (265, 267, 307)
bild = lambda l, F: np.asarray(Image.open(os.path.join(l, 'fd', 'f%06d.ppm' % F)).convert('RGB')).astype(float)


def wert(px):
    med = np.median(px, axis=0)
    _h, s, v = colorsys.rgb_to_hsv(*(med / 255.0))
    lab = G.lab(med[None, None])[0, 0]
    return '(%d,%d,%d) S %.2f V %.2f L* %.1f C* %.1f' % (med[0], med[1], med[2], s, v, lab[0], np.hypot(lab[1], lab[2]))


bez, mb, mr, ma = sys.argv[1:5]
for lauf in sys.argv[5:]:
    print('== %s' % os.path.basename(lauf.rstrip('/')))
    for F in FR:
        B = bild(bez, F)
        m = {'Bauch': np.abs(bild(mb, F) - B).sum(-1) > 24, 'Ring': np.abs(bild(mr, F) - B).sum(-1) > 24, 'Schwanz': np.abs(bild(ma, F) - B).sum(-1) > 24}
        P = bild(lauf, F)
        print('  F%d ' % F + ' | '.join('%s %d px %s' % (k, int(v.sum()), wert(P[v]) if v.sum() > 20 else '-') for k, v in m.items()))
