#!/usr/bin/env python3
"""Rohbau C, Abgabe: python rc_abgabe.py TAG. Kopiert Blaetter des Laufs lauf/TAG in den Ordner und schreibt gerippe_c.json (Knochen und Posen, wie sie im EMD stehen)."""
import json
import os
import shutil
import sys

from rc_kern import HIER
import form_emd as FE

tag = sys.argv[1]
d = os.path.join(HIER, 'lauf', tag)
for n in ('vergleich_bild1.png', 'vergleich_bild2.png', 'teile_bild1.png', 'teile_bild2.png', 'rundum.png', 'rundum_hell.png'):
    shutil.copy(os.path.join(d, n), os.path.join(HIER, n))
emd = open(os.path.join(HIER, 'FORM.EMD'), 'rb').read()
f = FE.lese_emd(emd)
aus = dict(knochen=[dict(index=i, eltern=k['eltern'], versatz=k['versatz']) for i, k in enumerate(f['knochen'])],
           clips=[dict(name=n, kf=c['bilder'][0]['kf']) for n, c in zip(('pose1', 'pose2', 'ruhe'), f['clips'])],
           keyframes=[dict(wurzel=k['wurzel'], winkel=k['winkel']) for k in f['pool']])
json.dump(aus, open(os.path.join(HIER, 'gerippe_c.json'), 'w'), indent=0)
print('Abgabe aus', d, 'Knochen', len(aus['knochen']), 'Keyframes', len(aus['keyframes']))
