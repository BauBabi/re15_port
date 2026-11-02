"""Nachbesserung 1: Belegblaetter aus dem echten Fenster (Laeufe von s3_n1_lauf.sh unter re15_port/build_n1a/spiel/).

    python s3_n1_belegblatt.py textur      -> stufe3/nachbesserung1/textur_vorher_nachher.png
    python s3_n1_belegblatt.py kopf        -> stufe3/nachbesserung1/kopf_frontal_spiel.png

Laeufe: tex_vor_* = Asset vor dem Texturfix (K3 mit Mosaik-Gelenkkoerpern), tex_nach_* = Endstand (Hautstueck je Gelenkkoerper),
5080 Schnitt 4 (RE15_BB_POSE -23500,-18200, Clip 2 Bild 0 = Ruhe, Gier 0/1024/2048/3072/3584), 5090 Schnitt 11 (Lage von Bild 2
941,-23776, Clip 10 Bild 45 = Pose Bild 2, Gier 3874/802/1826/0/512). Je Lauf das letzte Bild (Pose steht fest).
"""
import os, sys, glob
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S  # noqa: E402

SPIEL = os.path.join(S.WURZEL, 're15_port', 'build_n1a', 'spiel')
N1 = os.path.join(HIER, '..', 'nachbesserung1')


def letztes(name):
    b = sorted(glob.glob(os.path.join(SPIEL, name, 'bild', 'b_*.png')))
    return Image.open(b[-1]).convert('RGB') if b else None


def blatt(aus, titel, zeilen, box=None, skala=1.0):
    tafeln = []
    for text, namen in zeilen:
        ims = [letztes(n) for n in namen]
        if any(i is None for i in ims):
            print('fehlt', namen)
            continue
        if box:
            ims = [i.crop(box) for i in ims]
        if skala != 1.0:
            ims = [i.resize((int(i.size[0] * skala), int(i.size[1] * skala)), Image.NEAREST) for i in ims]
        tafeln.append((text, ims))
    w = max(sum(i.size[0] for i in ims) + 8 * (len(ims) - 1) for _t, ims in tafeln)
    h = 30 + sum(ims[0].size[1] + 24 for _t, ims in tafeln)
    out = Image.new('RGB', (w, h), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((6, 8), titel, fill=(240, 200, 120))
    y = 30
    for text, ims in tafeln:
        d.text((6, y + 4), text, fill=(220, 220, 220))
        x = 0
        for i in ims:
            out.paste(i, (x, y + 22))
            x += i.size[0] + 8
        y += ims[0].size[1] + 24
    out.save(aus)
    print(aus, out.size)


def main():
    was = sys.argv[1]
    if was == 'textur':
        zeilen = []
        for raum, gier, wort in (('5080', 0, 'seitlich (rechte Seite)'), ('5080', 2048, 'seitlich (linke Seite)'), ('5080', 1024, 'hinten'),
                                 ('5080', 3072, 'vorn (nah)'), ('5090', 3874, 'vorn, Pose/Lage von Bild 2'), ('5090', 802, 'seitlich'),
                                 ('5090', 1826, 'hinten')):
            zeilen.append(('%s %s (Gier %d): links vorher (K3, Gelenkkoerper-Mosaik), rechts nachher (je Gelenkkoerper ein Hautstueck, Ellbogen r setzt die Haut fort)' % (raum, wort, gier),
                           ['tex_vor_%s_g%d' % (raum, gier), 'tex_nach_%s_g%d' % (raum, gier)]))
        blatt(os.path.join(N1, 'textur_vorher_nachher.png'),
              'Nachbesserung 1, Texturbefund: echtes Fenster (eigene exe-Kopie, gdigrab), 5080 Schnitt 4 Ruhe / 5090 Schnitt 11 Pose Bild 2. '
              'Texel ausserhalb TIM 35 -> 0, Mosaik der 31 Gelenkkoerper -> zusammenhaengende Haut', zeilen)
    else:
        zeilen = []
        # 5080 Gier 3584 (3/4) zeigt in Schnitt 4 den Kopf nicht (ausserhalb des Bildes), der Vorher-Lauf 5090 Gier 512 blieb schwarz
        # (Schnitt nicht gesetzt); dafuer die Seitenansichten mit Kopf im Bild
        for raum, gier, wort in (('5080', 3072, 'frontal'), ('5080', 2048, 'seitlich'), ('5090', 0, 'frontal'),
                                 ('5090', 3874, 'Lage/Pose von Bild 2, 3/4 von vorn'), ('5090', 802, 'seitlich')):
            zeilen.append(('%s %s (Gier %d), Spielkamera (5080 Schnitt 4, 5090 Schnitt 11): links vor dem Texturfix, rechts Endstand' % (raum, wort, gier),
                           ['tex_vor_%s_g%d' % (raum, gier), 'tex_nach_%s_g%d' % (raum, gier)]))
        blatt(os.path.join(N1, 'kopf_frontal_spiel.png'),
              'Kopf K3 aus der Spielkamera, echtes Fenster (Klaerung "Schildkroete statt Alligator"): Geometrie unveraendert, nur Gelenkkoerper-Textur neu',
              zeilen)


if __name__ == '__main__':
    main()
