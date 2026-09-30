"""Runde 34 Nacht, Spur F — Belegbogen aus Framedumps der echten exe (reine Auswertung).

Setzt ausgewaehlte Framedumps (PPM, 960x720 bei RE15_WINDOW_SCALE=3) verkleinert auf 320x240 mit
Bildnummer zu einem Bogen zusammen, damit die Abnahme am ARTEFAKT ansehbar und klein ablegbar ist.

    python r34n_f_bogen.py <laufverzeichnis> <ausgabe.png> <spalten> <bild>[:<titel>] ...

Beispiel:
    python r34n_f_bogen.py build/r34n_f/soll1110_nein F_belege/soll_1110_nein.png 4 460:Seite1 540:Seite2
"""
import os
import sys

from PIL import Image, ImageDraw


def main():
    if len(sys.argv) < 5:
        print(__doc__)
        return 2
    lauf, aus, spalten = sys.argv[1], sys.argv[2], int(sys.argv[3])
    felder = []
    for arg in sys.argv[4:]:
        nr, _, titel = arg.partition(":")
        felder.append((int(nr), titel))
    w, h, kopf = 320, 240, 14
    zeilen = (len(felder) + spalten - 1) // spalten
    bogen = Image.new("RGB", (spalten * w, zeilen * (h + kopf)), (40, 40, 40))
    zeichner = ImageDraw.Draw(bogen)
    for i, (nr, titel) in enumerate(felder):
        p = os.path.join(lauf, "f_%06d.ppm" % nr)
        x, y = (i % spalten) * w, (i // spalten) * (h + kopf)
        zeichner.text((x + 3, y + 1), ("F%d %s" % (nr, titel)).strip(), fill=(255, 255, 0))
        if not os.path.exists(p):
            zeichner.text((x + 3, y + kopf + 3), "fehlt", fill=(255, 0, 0))
            continue
        bild = Image.open(p).convert("RGB").resize((w, h), Image.BILINEAR)
        bogen.paste(bild, (x, y + kopf))
    os.makedirs(os.path.dirname(os.path.abspath(aus)), exist_ok=True)
    # Belegbilder klein halten (VERTRAG/Auftrag): 256-Farben-Palette, Schrift bleibt lesbar.
    bogen.convert("P", palette=Image.ADAPTIVE, colors=256).save(aus, optimize=True)
    print("bogen:", aus, bogen.size)
    return 0


if __name__ == "__main__":
    sys.exit(main())
