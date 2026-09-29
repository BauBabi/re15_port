#!/usr/bin/env python3
"""Runde 33 / Thema K: Kontaktbogen der Abnahme-Framedumps (echte exe, RE15_FRAMEDUMP) und
Kachel-Mittelwerte der Zielkachel (156,76) 48x40 -> x3 im 960x720-Bild (468,228) 144x120.
Eingang build/r33_karte_abnahme/*.ppm, Ausgang analysis/befunde_runde33/karte_belege/."""
import os, glob
from PIL import Image, ImageDraw
import numpy as np
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, '..', '..', '..'))
IN = os.path.join(REPO, 'build', 'r33_karte_abnahme')
OUT = os.path.join(REPO, 'analysis', 'befunde_runde33', 'karte_belege')
BILDER = [
    ('vorher_a_002250', 'VORHER F2250: RUNTER -> bleibt 3F'),
    ('lauf_a_001900',   'Hinweis F1900 (Runde 30)'),
    ('lauf_a_002200',   'nachher F2200: Karte 3F'),
    ('lauf_a_002250',   'F2250 RUNTER -> 2F, Umriss'),
    ('lauf_a_002270',   'F2270 2F, rote Phase'),
    ('lauf_c_000260',   'geladen (Platz 3) F260: 2F rot'),
    ('lauf_d_000300',   'im Funkraum F300: stetig AKTUELL'),
    ('lauf_e_000350',   'zurueck in 1150 F350: 2F BESUCHT'),
]
W, H = 480, 360
bogen = Image.new('RGB', (W * 4, (H + 24) * 2), (20, 20, 20))
d = ImageDraw.Draw(bogen)
for i, (n, t) in enumerate(BILDER):
    p = os.path.join(IN, n + '.ppm')
    if not os.path.exists(p): continue
    im = Image.open(p).convert('RGB')
    a = np.asarray(im).astype(int)[228 + 6:228 + 114, 468 + 6:468 + 138].reshape(-1, 3).mean(0)
    print('%-18s Kachel-Mittel (%5.1f %5.1f %5.1f)  %s' % (n, a[0], a[1], a[2], t))
    x, y = (i % 4) * W, (i // 4) * (H + 24)
    bogen.paste(im.resize((W, H)), (x, y + 24))
    d.text((x + 6, y + 6), t, fill=(255, 255, 0))
os.makedirs(OUT, exist_ok=True)
bogen.save(os.path.join(OUT, 'abnahme_kontaktbogen.jpg'), quality=82)
for n in ('lauf_a_002270', 'lauf_a_002250'):
    Image.open(os.path.join(IN, n + '.ppm')).convert('RGB').save(os.path.join(OUT, n + '.jpg'), quality=85)
print('->', OUT)
