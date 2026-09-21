import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np
import msk

for cut in (4, 5):
    masks, gc, mdecl, draw, tief, wer, px, clut = msk.tiefenkarte('ROOM10F0', cut)
    def patch(m):
        s = px[m['srcY']:m['srcY'] + m['h'], m['srcX']:m['srcX'] + m['w']]
        p = np.zeros((m['h'], m['w']), np.uint8); p[:s.shape[0], :s.shape[1]] = s
        return p
    lage = {}
    doppel_lage, doppel_texel, tex_doppelt = 0, 0, 0
    for m in masks:
        k = (m['dstX'], m['dstY'], m['w'], m['h'], m['depth'])
        if k in lage:
            doppel_lage += 1
            a, b = patch(lage[k]) != 0, patch(m) != 0
            if np.array_equal(a, b):
                doppel_texel += 1
                tex_doppelt += int(b.sum())
        else:
            lage[k] = m
    einz = len(lage)
    flaeche = sum(m['w'] * m['h'] for m in masks)
    print("Cut %d: %d Rechtecke, %d verschiedene Lagen, %d Doppel (Lage+Groesse+Tiefe gleich), "
          "davon %d auch TEXELGLEICH (%d Texel doppelt gezeichnet)"
          % (cut, len(masks), einz, doppel_lage, doppel_texel, tex_doppelt))
    print("        Atlasflaeche %d Texel, davon opak %d; Kapazitaet RE15_PRI_MAX_MASKS_PER_CUT=105"
          % (flaeche, int((tief < (1 << 30)).sum())))
