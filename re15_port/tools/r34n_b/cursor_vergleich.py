#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht, BAU): ist der Hebetisch-Cursor in ROOM1150 PIXELGLEICH mit dem Cursor,
den der Port in ROOM11F0 zeichnet ("unser Cursor", Nutzer)?

Fussabdruck = Punkte, die sich zwischen einem Bild MIT und einem Bild OHNE Cursor unterscheiden
(je Raum dieselbe Kamera): 11F0 = m2 F480 (Cursor am Start) gegen m2 F700 (Cursor weggefahren);
1150 = Bild mit Cursor am Start gegen Bild vor dem Halt (Cut 4, Kuppel zu). Verglichen werden
Lage UND Farbe jedes Punkts im Fenster um den Start (160,119), 960x720-Framedumps.

Aufruf: cursor_vergleich.py <11f0_mit.ppm> <11f0_ohne.ppm> <1150_mit.ppm> <1150_ohne.ppm> [--bild out.png]
"""
import sys
from PIL import Image

X0, X1, Y0, Y1 = 430, 540, 310, 400      # 960er-Fenster um den Start (160,119)*3


def fussabdruck(mit, ohne):
    a = Image.open(mit).convert("RGB").load()
    b = Image.open(ohne).convert("RGB").load()
    return {(x, y): a[x, y] for y in range(Y0, Y1) for x in range(X0, X1) if a[x, y] != b[x, y]}


def main():
    f11 = fussabdruck(sys.argv[1], sys.argv[2])
    f50 = fussabdruck(sys.argv[3], sys.argv[4])
    s11, s50 = set(f11), set(f50)
    ge = s11 & s50
    farb = sum(1 for p in ge if f11[p] == f50[p])
    print("Fussabdruck 11F0: %d  1150: %d  gemeinsam: %d  nur 11F0: %d  nur 1150: %d" % (
        len(s11), len(s50), len(ge), len(s11 - s50), len(s50 - s11)))
    print("farbgleich in der Schnittmenge: %d von %d" % (farb, len(ge)))
    if "--bild" in sys.argv:
        out = sys.argv[sys.argv.index("--bild") + 1]
        a = Image.open(sys.argv[1]).convert("RGB").crop((X0, Y0, X1, Y1)).resize((330, 270), Image.NEAREST)
        b = Image.open(sys.argv[3]).convert("RGB").crop((X0, Y0, X1, Y1)).resize((330, 270), Image.NEAREST)
        c = Image.new("RGB", (670, 270), (255, 0, 255))
        c.paste(a, (0, 0))
        c.paste(b, (340, 0))
        c.save(out)
        print("Bild:", out)


if __name__ == "__main__":
    main()
