"""Messung (keine Eichung): Farbe der Bauchflaeche im ECHTEN Spielbild (Framedumps 960x720) der Proben a / c; Bauch-Bildpunkte = Bildpunkte,
in denen sich a und c unterscheiden (gleiche Geometrie, nur der Bauch-Texel anders). Lab-Median. Angabe zur Richtung, kein Ziel."""
import os, sys
import numpy as np
from PIL import Image
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import v4_gemein as G
R8 = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8'
for F in (265, 267, 307, 313):
    ims = {k: np.asarray(Image.open(os.path.join(R8, d, 'fd', 'f%06d.ppm' % F)).convert('RGB')).astype(float) for k, d in (('a', 'probe_a'), ('b', 'szene'), ('c', 'probe_c'))}
    m = np.abs(ims['a'] - ims['c']).sum(-1) > 12
    mb = (np.abs(ims['a'] - ims['b']).sum(-1) > 12)
    out = []
    for k in ('a', 'b', 'c'):
        lab = G.lab(ims[k][m][None])[0]
        C = np.hypot(lab[:, 1], lab[:, 2]); h = np.degrees(np.arctan2(lab[:, 2], lab[:, 1]))
        out.append('%s L* %.1f C* %.1f h %.0f' % (k, np.median(lab[:, 0]), np.median(C), np.median(h)))
    print('F%d Bauch-Bildpunkte %d (Band-Bildpunkte a!=b %d): %s' % (F, int(m.sum()), int(mb.sum()), ' | '.join(out)))
