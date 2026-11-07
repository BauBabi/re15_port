# -*- coding: utf-8 -*-
"""Tafeln aus der Spielreihe: Zeilen = Lage, Spalten = A, B, C. Zusaetzlich 3fach-Ausschnitte um die Figur in ROOM5080."""
import os
import numpy as np
from PIL import Image, ImageDraw

AUS = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/bewertung_spiel'
REIHEN = ('r5080_ruhe_g2145', 'r5080_ruhe_g3172', 'r5090_ruhe_g1166', 'r5080_biss_g3172', 'r5080_seitenhieb_g3172')


def tafel(zeilen, ziel, box=None, s=1):
    w = (box[2] - box[0]) * s if box else 640
    h = (box[3] - box[1]) * s if box else 480
    b = Image.new('RGB', (3 * w + 40, len(zeilen) * (h + 24) + 10), (18, 20, 26))
    d = ImageDraw.Draw(b)
    for y, r in enumerate(zeilen):
        for x, k in enumerate('ABC'):
            p = '%s/spiel_%s_%s.png' % (AUS, k, r)
            xx, yy = 10 + x * (w + 10), 5 + y * (h + 24)
            d.text((xx, yy), '%s %s' % (k, r), fill=(230, 150, 70))
            if not os.path.exists(p):
                continue
            a = Image.open(p).convert('RGB')
            if box:
                a = a.crop(box).resize((w, h), Image.NEAREST)
            b.paste(a, (xx, yy + 14))
    b.save(ziel)


tafel(REIHEN[:3], AUS + '/spiel_tafel_ruhe.png')
tafel(REIHEN, AUS + '/spiel_tafel_nah.png', box=(240, 90, 460, 290), s=3)
