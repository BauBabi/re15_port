#!/usr/bin/env python3
"""schaden_bauen.py - Runde 35 Spur M: das beschaedigte hintere Fenster ROOM1120 Cut 1.

KUNST = PORT-WAHL (Nutzer: "Es waere super, wenn du dann auch im Background das Hintere Fenster
etwas beschaedigen koenntest"). KEIN BSS-Patch: der Port legt die Aenderung als Pixel-Operationen
UEBER den laufenden Hintergrund (Software-Framebuffer, direkt nach dem Hintergrund, vor dem 3D-Pass
= unter Leon/Kraehe, wie die Generator-Lampen panel_lampen_pc.c).

Vorbild RE2 room1090: nach dem Ereignis wechselt RE2 auf Kamerabilder mit Glasscherben am Boden
(cam 2/3/4 -> 9/10/11, gleiche Kameralage, Differenz = Scherben unter den Fenstern; sub15 @0x0032/36
Set(14,4,0)/Set(15,5,0) im Splitter-Bild). Im Port: (1) die zwei Scheiben bekommen je ein
gezacktes Loch (dunkler, keine Spiegelung mehr) mit hellen Bruchkanten und Spruengen, (2) helle
Scherben-Tupfen auf dem Boden unter dem Fenster.

Geometrie (gemessen am dekodierten Hintergrund build/bg_ppm/ROOM11201.ppm, Leuchtdichte-Tabelle im
Dossier): Scheibe links x 183..192, rechts x 196..205, y 83..100 (Pfosten x 193..195, Bruestung
y 101..103); Bodenkante unter der Wand y ~128..132 (Rueckprojektion z 11200, y 0 -> Zeile 130..132).

Ausgabe: re15_port/engine/src/gen/fenster1120_schaden.inc  {x, y, art, a, r, g, b}
  art 0 = abdunkeln: px = px * a / 256
  art 1 = mischen:   px = px + (rgb - px) * a / 256
Aufruf: python schaden_bauen.py [--bild <ppm>] [--vorschau <png>]
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "..", "engine", "src", "gen", "fenster1120_schaden.inc")

SCHEIBEN = [(183, 83, 10, 18), (196, 83, 10, 18)]
# Lochumrisse relativ zur Scheibe (x 0..9, y 0..17) - PORT-WAHL, gezackt, je Scheibe anders.
LOCH = [
    [(2, 0), (4, 3), (6, 1), (8, 4), (7, 8), (9, 11), (7, 15), (5, 13), (3, 17), (1, 12), (2, 8), (0, 5)],
    [(1, 2), (3, 0), (5, 4), (8, 2), (9, 7), (6, 10), (8, 16), (4, 14), (2, 17), (1, 10), (3, 7)],
]
# Spruenge (Polylinien relativ zur Scheibe), vom Lochrand zu den Ecken.
SPRUNG = [
    [[(0, 5), (0, 1)], [(9, 11), (9, 16)], [(3, 17), (0, 17)]],
    [[(9, 7), (9, 0)], [(1, 10), (0, 15)], [(8, 16), (9, 17)]],
]
# Scherben auf dem Boden (Bildpunkte) unter dem Fenster - PORT-WAHL (RE2-Vorbild: Scherbenfeld der
# Kamerabilder 9/10/11).
SCHERBEN = [(181, 129), (185, 131), (188, 128), (190, 130), (193, 132), (196, 129), (199, 131),
            (201, 128), (204, 130), (207, 129), (186, 133), (198, 133), (203, 132)]

GLAS = (150, 165, 170)     # Bruchkante / Scherbe: kuehles helles Grau (Lampenlicht auf Glas)


def innen(poly, x, y):
    n = len(poly); c = False
    for i in range(n):
        x1, y1 = poly[i]; x2, y2 = poly[(i + 1) % n]
        if (y1 > y) != (y2 > y):
            xs = x1 + (y - y1) * (x2 - x1) / float(y2 - y1)
            if x < xs:
                c = not c
    return c


def linie(a, b):
    (x0, y0), (x1, y1) = a, b
    n = max(abs(x1 - x0), abs(y1 - y0), 1)
    return {(round(x0 + (x1 - x0) * k / n), round(y0 + (y1 - y0) * k / n)) for k in range(n + 1)}


def bauen():
    ops = {}
    for (sx, sy, w, h), poly, spr in zip(SCHEIBEN, LOCH, SPRUNG):
        loch = {(x, y) for y in range(h) for x in range(w) if innen(poly, x + 0.5, y + 0.5)}
        for (x, y) in loch:
            ops[(sx + x, sy + y)] = (0, 40, 0, 0, 0)          # Loch: Spiegelung weg (40/256)
        for y in range(h):
            for x in range(w):
                if (x, y) in loch:
                    continue
                nb = any((x + dx, y + dy) in loch for dx in (-1, 0, 1) for dy in (-1, 0, 1))
                if nb:
                    # Bruchkante, Staerke je Pixel gestreut (deterministisch), damit keine Umrisslinie
                    a = 70 + ((x * 7 + y * 13) % 5) * 22
                    ops[(sx + x, sy + y)] = (1, a, GLAS[0], GLAS[1], GLAS[2])
        for pl in spr:
            for i in range(len(pl) - 1):
                for (x, y) in linie(pl[i], pl[i + 1]):
                    if 0 <= x < w and 0 <= y < h and (x, y) not in loch:
                        ops.setdefault((sx + x, sy + y), (1, 90, GLAS[0], GLAS[1], GLAS[2]))
    for (x, y) in SCHERBEN:
        ops[(x, y)] = (1, 170, GLAS[0], GLAS[1], GLAS[2])
    return sorted((y, x, v) for (x, y), v in ops.items())


def main():
    ops = bauen()
    with open(OUT, "w", newline="\n") as f:
        f.write("/* gen/fenster1120_schaden.inc - erzeugt von re15_port/tools/r35_fenster/schaden_bauen.py\n"
                " * (Runde 35 Spur M). PORT-WAHL-Kunst: beschaedigtes hinteres Fenster ROOM1120 Cut 1.\n"
                " * {x, y, art (0 abdunkeln / 1 mischen), a/256, r, g, b} - NICHT VON HAND AENDERN. */\n")
        for y, x, (art, a, r, g, b) in ops:
            f.write("    { %3d, %3d, %d, %3d, %3d, %3d, %3d },\n" % (x, y, art, a, r, g, b))
    print("%d Operationen -> %s" % (len(ops), os.path.normpath(OUT)))
    if "--vorschau" in sys.argv:
        from PIL import Image
        bild = sys.argv[sys.argv.index("--bild") + 1]
        im = Image.open(bild).convert("RGB")
        px = im.load()
        for y, x, (art, a, r, g, b) in ops:
            o = px[x, y]
            if art == 0:
                px[x, y] = tuple(c * a // 256 for c in o)
            else:
                px[x, y] = tuple(c + (t - c) * a // 256 for c, t in zip(o, (r, g, b)))
        im.save(sys.argv[sys.argv.index("--vorschau") + 1])


if __name__ == "__main__":
    main()
