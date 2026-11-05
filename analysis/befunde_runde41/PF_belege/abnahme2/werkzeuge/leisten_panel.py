import numpy as np, json
from PIL import Image
import bilder
from leisten import leiste, stapel, OUT
up = lambda a, f: np.repeat(np.repeat(a, f, 0), f, 1)
P = {(f, s, z): np.load('p_p_%s_%s_%s.npy' % (s, f, z)) for f in ('leon', 'elza') for s in ('vor', 'nach') for z in ('vor_loesung', 'nach_loesung')}
# 1) ganzes Panel (Fenster 960x720 -> hier logisch x2)
reihen = []
for f in ('leon', 'elza'):
    reihen.append(leiste([up(P[(f, 'vor', 'vor_loesung')], 2), up(P[(f, 'nach', 'vor_loesung')], 2), up(P[(f, 'vor', 'nach_loesung')], 2), up(P[(f, 'nach', 'nach_loesung')], 2)],
                         ['vor NB1 (A 100, vor Loesung)', 'nach NB1 (A 100, vor Loesung)', 'vor NB1 (A 80, geloest)', 'nach NB1 (A 80, geloest)'],
                         '%s: Panel 2030 im Fenster (gdigrab, logisches Bild 320x240 = Mitte jedes 3x3-Blocks, hier 2-fach nearest)' % f.capitalize()))
stapel(reihen).save(OUT + 'panel_vor_nach_leon_elza.png')
# 2) Schriftzuege 6-fach, Leon (Elza bildgleich)
TEXTE = [('A 100', (48, 130, 86, 144)), ('80 links (rot)', (88, 69, 108, 83)), ('BRIDGE', (212, 50, 268, 64)), ('SET 80 (PRESSURE 80)', (214, 68, 264, 82)),
         ('B 50', (133, 130, 165, 144)), ('C 30', (177, 130, 209, 144)), ('CAUTION / HIGH PRESSURE', (120, 182, 232, 206))]
reihen = []
for name, (x0, y0, x1, y1) in TEXTE:
    a = P[('leon', 'vor', 'vor_loesung')][y0:y1, x0:x1]; b = P[('leon', 'nach', 'vor_loesung')][y0:y1, x0:x1]
    reihen.append(leiste([up(a, 6), up(b, 6)], ['vor NB1', 'nach NB1'], name))
stapel(reihen).save(OUT + 'panel_schrift_vor_nach_zoom6.png')
# 3) Instrument + Drehscheiben 5-fach (vor der Loesung)
x0, y0, x1, y1 = 100, 64, 256, 178
stapel([leiste([up(P[(f, 'vor', 'vor_loesung')][y0:y1, x0:x1], 5), up(P[(f, 'nach', 'vor_loesung')][y0:y1, x0:x1], 5)], ['vor NB1', 'nach NB1'],
               '%s: Zeiger-Instrument und Drehscheiben, A 100, 5-fach' % f.capitalize()) for f in ('leon', 'elza')]).save(OUT + 'panel_scheiben_vor_nach_zoom5.png')
print('ok')
