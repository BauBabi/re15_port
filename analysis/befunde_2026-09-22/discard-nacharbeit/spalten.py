#!/usr/bin/env python3
"""spalten.py — misst die Auswahl-Zeile im ABGEZOGENEN Bild nach.

Liest die PPM-Abzuege (echter Renderpfad, RE15_FRAMEDUMP vor SDL_RenderPresent) und
gibt aus, in welchen SPALTEN in der Zeile 196..203 heller Text steht. Damit ist die
Behauptung "Yes bei 174, No bei 244, Zeile 196, Cursor bei 160" am BILD geprueft und
nicht nur am Quelltext.

Aufruf: python spalten.py <bild.ppm> [...]
"""
import sys

def lade(p):
    d = open(p, "rb").read()
    # P6\n<w> <h>\n255\n
    teile = []
    i = 0
    while len(teile) < 4:
        while d[i:i+1].isspace(): i += 1
        j = i
        while not d[j:j+1].isspace(): j += 1
        teile.append(d[i:j]); i = j
    i += 1
    w, h = int(teile[1]), int(teile[2])
    return w, h, d[i:]

for p in sys.argv[1:]:
    w, h, px = lade(p)
    # Zeilenband der Auswahl: 196..203 (Glyphenhoehe der .msg-Schrift)
    spalten = []
    for x in range(w):
        hell = 0
        for y in range(196, 204):
            o = (y * w + x) * 3
            r, g, b = px[o], px[o+1], px[o+2]
            if r > 150 and g > 150 and b > 150: hell += 1
        spalten.append(hell)
    # zusammenhaengende Bloecke
    bloecke, start = [], None
    for x in range(w):
        if spalten[x] and start is None: start = x
        elif not spalten[x] and start is not None:
            if x - start >= 2: bloecke.append((start, x - 1))
            start = None
    if start is not None: bloecke.append((start, w - 1))
    # nur die rechte Haelfte (dort steht Yes/No; links steht der Fragetext)
    rechts = [b for b in bloecke if b[0] >= 150]
    print("%s  Bloecke in Zeile 196..203 ab Spalte 150: %s"
          % (p.split("/")[-1], ", ".join("%d..%d" % b for b in rechts) or "KEINE"))
