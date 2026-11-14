"""Runde 8d Messung (keine Eichung): wirken die Gliedfugen im Spielbild als Streifen? Je Bild F255-F320 (jedes 5.) und Klasse (Masken je Glied,
x21_maske_glied.py): Fugen-Bildpunkte = Bildpunkte eines Glieds in <= 3 px Abstand zu einem Nachbarglied derselben Klasse; Mitte = Bildpunkte
>= 8 px von jedem Nachbarglied. Streifen = L*(Fugen) - L*(Mitte) (Median ueber alle Glieder des Bilds; + = heller Streifen, - = dunkler);
dazu je Gelenk der Median ueber die Bilder.   python x23_band.py <bezug> <maskenordner> <lauf> [<lauf> ...]"""
import os, sys
import numpy as np
from PIL import Image
from scipy import ndimage
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import v4_gemein as G

bild = lambda l, F: np.asarray(Image.open(os.path.join(l, 'fd', 'f%06d.ppm' % F)).convert('RGB')).astype(float)
Lmed = lambda px: float(G.lab(np.median(px, axis=0)[None, None])[0, 0][0])
bez, mo = sys.argv[1:3]
FR = range(255, 321, 5)
M = {}
for F in FR:
    B = bild(bez, F)
    for kl, pre in (('grau', 'MG'), ('bauch', 'MB')):
        for g in range(16, 23):
            M[(F, kl, g)] = np.abs(bild(os.path.join(mo, 'lauf_%s_%d' % (pre, g)), F) - B).sum(-1) > 24
for lauf in sys.argv[3:]:
    print('== %s' % os.path.basename(lauf.rstrip('/')))
    for kl in ('bauch', 'grau'):
        je_bild, je_gel = [], {}
        for F in FR:
            P = bild(lauf, F)
            fuge = np.zeros(P.shape[:2], bool); mitte = np.zeros(P.shape[:2], bool)
            for g in range(16, 23):
                m = M[(F, kl, g)]
                if m.sum() < 15:
                    continue
                nb = np.zeros_like(m)
                for h in (g - 1, g + 1):
                    if 16 <= h <= 22:
                        nb |= M[(F, kl, h)]
                d = ndimage.distance_transform_edt(~nb) if nb.any() else np.full(m.shape, 99.0)
                fuge |= m & (d <= 3); mitte |= m & (d >= 8)
            for j in range(1, 7):
                a, b = M[(F, kl, 15 + j)], M[(F, kl, 16 + j)]
                if a.sum() < 15 or b.sum() < 15:
                    continue
                fj = (a & (ndimage.distance_transform_edt(~b) <= 3)) | (b & (ndimage.distance_transform_edt(~a) <= 3))
                if fj.sum() >= 15 and mitte.sum() >= 30:
                    je_gel.setdefault(j, []).append(Lmed(P[fj]) - Lmed(P[mitte]))
            if fuge.sum() >= 30 and mitte.sum() >= 30:
                je_bild.append(Lmed(P[fuge]) - Lmed(P[mitte]))
        if je_bild:
            print('  %-5s Streifen L*(Fugen) - L*(Mitte): Median %+.1f [%+.1f .. %+.1f] ueber %d Bilder; je Gelenk %s' % (
                kl, np.median(je_bild), min(je_bild), max(je_bild), len(je_bild),
                ' '.join('%d:%+.1f(%d)' % (j, np.median(v), len(v)) for j, v in sorted(je_gel.items()))))
