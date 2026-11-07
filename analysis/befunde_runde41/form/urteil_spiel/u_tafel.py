# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Tafeln aus der Spielreihe. Zeilen = Lage, Spalten = A, B, C."""
import os
import numpy as np
from PIL import Image, ImageDraw

AUS = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/urteil_spiel'
GANZ = ('abgabe_pose1_r5080', 'abgabe_ruhe_r5080', 'abgabe_pose2_r5090', 'ruhe_g1166_r5090')
NAH = ('ruhe_g2145', 'ruhe_g3172', 'hieb_g3172', 'biss_g3172', 'schwanzhieb_g1100')


def tafel(zeilen, ziel, box=None, s=1, gamma=None):
    w = (box[2] - box[0]) * s if box else 640
    h = (box[3] - box[1]) * s if box else 480
    b = Image.new('RGB', (3 * w + 40, len(zeilen) * (h + 24) + 10), (18, 20, 26))
    d = ImageDraw.Draw(b)
    for y, r in enumerate(zeilen):
        for x, k in enumerate('ABC'):
            p = '%s/spiel/%s_%s.png' % (AUS, k, r)
            xx, yy = 10 + x * (w + 10), 5 + y * (h + 24)
            d.text((xx, yy), '%s %s' % (k, r), fill=(230, 150, 70))
            if not os.path.exists(p):
                continue
            a = Image.open(p).convert('RGB')
            if box:
                a = a.crop(box).resize((w, h), Image.NEAREST)
            if gamma:
                q = np.array(a).astype(float) / 255
                a = Image.fromarray((np.clip(q ** gamma, 0, 1) * 255).astype(np.uint8))
            b.paste(a, (xx, yy + 14))
    b.save(ziel)


tafel(GANZ, AUS + '/spiel_tafel_ganz.png')
tafel(NAH, AUS + '/spiel_tafel_nah.png', box=(200, 90, 500, 290), s=2)
tafel(NAH, AUS + '/spiel_tafel_nah_hell.png', box=(200, 90, 500, 290), s=2, gamma=0.6)
