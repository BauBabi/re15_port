"""Stufe 3: Kontaktblaetter aller Clips der Riesenform fuer den Nutzer (Offline-Buehne, Textur, Gouraud ohne Raumlicht).

    python s3_kontakt.py [--aus DIR]      Vorgabe: stufe3/kontakt/

Je Clip ein Blatt: Zeilen = Ansichten (rechte Seite, schraeg vorn links, schraeg hinten rechts), Spalten = Bilder des Clips.
Die Figur laeuft um ihre Spur weiter (Kamera folgt, Bodenraster steht in der Welt), so sieht man Gleiten der Fuesse.
Marken unter jedem Bild: T = Trefferbild (EDD Bit 16), S<n> = Ton-Id n (Bit 27).
"""
import os, sys, json, math, argparse
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_blatt as B      # noqa: E402

ASSET = os.path.join(HIER, '..', 'asset')
NAMEN = {0: 'Rennen (Bild 11 = Pose Bild 1)', 2: 'Stehen/Atmen (Bild 0 = Ruhe J, Kiefer zu)', 3: 'Hieb rechts, Treffer 26-30',
         4: 'Hieb links, Treffer 14-17', 5: 'Gehen', 8: 'Zucken (Tick ab Bild 5)', 9: 'Nachhieb', 10: 'Drohen/Auftritt (Bild 45-52 = Pose Bild 2)',
         11: 'Flucht, Ende 5080', 12: 'Sturz aus dem Zug, Ende 5090 (Bild 0 = Pose Bild 2)', 13: 'Taumeln (Rueckstoss, Schleife, Spur -80 je Bild)',
         26: 'Biss, Treffer 16-30',
         27: 'Griff (packt in Bild 8)'}
ANSICHTEN = [0, 1536, 3584]
SPALTEN = {0: 18, 2: 10, 3: 12, 4: 12, 5: 12, 8: 10, 9: 11, 10: 14, 11: 12, 12: 12, 13: 14, 26: 12, 27: 13}


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=os.path.join(HIER, '..', 'kontakt'))
    a.add_argument('--nur', default='')
    g = a.parse_args()
    emd = open(os.path.join(ASSET, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim = open(os.path.join(ASSET, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    tab = {c['clip']: c for c in json.load(open(os.path.join(ASSET, 'clips.json'), encoding='utf-8'))}
    nur = [int(x) for x in g.nur.split(',')] if g.nur else sorted(NAMEN)
    for c in nur:
        n = tab[c]['bilder']
        spalten = SPALTEN.get(c, 12)
        schritt = max(1, int(math.ceil((n - 1) / float(spalten - 1))))
        titel = 'Clip %d: %s   %d Bilder (Spalten jedes %d. Bild)   Herkunft: %s   Ansichten: rechts / schraeg vorn links / schraeg hinten rechts' % (
            c, NAMEN[c], n, schritt, tab[c]['quelle'])
        aus = os.path.join(g.aus, 'clip_%02d_%s.png' % (c, tab[c]['name']))
        B.clip_blatt(emd, tim, c, ANSICHTEN, schritt, aus, titel, max_spalten=spalten + 1, kamera_folgt=c not in (11, 12, 13))
        print(aus)


if __name__ == '__main__':
    main()
