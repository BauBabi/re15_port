"""Runde 8d Messung (keine Eichung): Farbwechsel an den Gliedfugen im echten Spielbild.
Masken je Glied und Klasse (x21_maske_glied.py: Flaechen des Meshes N mit Klasse grau / bauch fast weiss; Differenz zum Bezugslauf > 24).
Je Bild F255-F320 (jedes 5.) und Gelenk j (Glied 15+j | 16+j) und Klasse:
  Kante  = Lab-Abstand der Mediane der Bildpunkte beider Glieder in <= 3 px Abstand zum anderen Glied (direkt an der Fuge);
  innen  = Lab-Abstand im selben Glied zwischen 0-3 px und 6-12 px von der Fuge (Verlauf innerhalb eines Glieds, Vergleich);
  Stufe  = Abstand der Glied-Mediane (ganzes Glied).
Nur Gelenke mit >= 15 Bildpunkten je Seite.
    python x22_fugen.py <bezug> <maskenordner> <lauf> [<lauf> ...]"""
import os, sys
import numpy as np
from PIL import Image
from scipy import ndimage
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import v4_gemein as G

bild = lambda l, F: np.asarray(Image.open(os.path.join(l, 'fd', 'f%06d.ppm' % F)).convert('RGB')).astype(float)
lab = lambda px: G.lab(np.median(px, axis=0)[None, None])[0, 0]
bez, mo = sys.argv[1:3]
FR = range(255, 321, 5)
masken = {}
for F in FR:
    B = bild(bez, F)
    for kl, pre in (('grau', 'MG'), ('bauch', 'MB')):
        for g in range(16, 23):
            masken[(F, kl, g)] = np.abs(bild(os.path.join(mo, 'lauf_%s_%d' % (pre, g)), F) - B).sum(-1) > 24
for lauf in sys.argv[3:]:
    print('== %s' % os.path.basename(lauf.rstrip('/')))
    erg = {}
    for F in FR:
        P = bild(lauf, F)
        for kl in ('grau', 'bauch'):
            for j in range(1, 7):
                a, b = masken[(F, kl, 15 + j)], masken[(F, kl, 16 + j)]
                if a.sum() < 15 or b.sum() < 15:
                    continue
                da, db = ndimage.distance_transform_edt(~b), ndimage.distance_transform_edt(~a)   # Abstand zum anderen Glied
                an, bn = a & (da <= 3), b & (db <= 3)
                af = a & (da >= 6) & (da <= 12)
                if an.sum() < 15 or bn.sum() < 15:
                    continue
                kante = float(np.linalg.norm(lab(P[an]) - lab(P[bn])))
                innen = float(np.linalg.norm(lab(P[an]) - lab(P[af]))) if af.sum() >= 15 else np.nan
                stufe = float(np.linalg.norm(lab(P[a]) - lab(P[b])))
                erg.setdefault((kl, j), []).append((F, kante, innen, stufe, lab(P[an])[0], lab(P[bn])[0]))
    for (kl, j), r in sorted(erg.items()):
        a = np.array([x[1:] for x in r])
        print('  %-5s Gelenk %d (%d|%d): %2d Bilder  Kante dE Median %.1f Max %.1f | innen %.1f | Stufe %.1f | L* an der Fuge %.1f | %.1f'
              % (kl, j, 15 + j, 16 + j, len(r), np.median(a[:, 0]), a[:, 0].max(), np.nanmedian(a[:, 1]), np.median(a[:, 2]),
                 np.median(a[:, 3]), np.median(a[:, 4])))
