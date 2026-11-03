"""Nachbesserung 2, Textur-Spur, Hilfe: sichtbare Pixel bestimmter Flaechen (z. B. Gelenkkoerper-Flaechen, deren UV am Feldrand
geklemmt ist) in den Spielansichten.

    python s3_n2_t_sicht.py SPIEL_DIR BASIS_DIR       (BASIS_DIR/textur_n2.json: ueber_feld_flaechen)
"""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_n2_textur as T  # noqa: E402


def main():
    spiel, basis = sys.argv[1:3]
    ber = json.load(open(os.path.join(basis, 'textur_n2.json'), encoding='utf-8'))
    ids = set(b * 100000 + a * 50000 + i for b, a, i in ber.get('ueber_feld_flaechen', []))
    emd = open(os.path.join(spiel, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    tim = open(os.path.join(spiel, 'BAHNSTEIG_FORM.TIM'), 'rb').read()
    ges = 0
    for an in T.ANSICHTEN:
        _r, _a, kenn, _t = T.ansicht(emd, tim, *an, hintergrund=False)
        n = int(np.isin(kenn, list(ids)).sum())
        ges += n
        print(an[0], an[5], 'sichtbar', n, 'von Figur', int((kenn >= 0).sum()))
    print('Flaechen', len(ids), 'sichtbar gesamt', ges)


if __name__ == '__main__':
    main()
