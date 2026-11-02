#!/usr/bin/env python3
"""Variante K: Gelenkkoerper am Kiefergelenk (Mundwinkel) so klein, dass er den Blick auf Ober- und Unterkiefer in Bild 2 nicht verdeckt.

    python k_maulwinkel.py KOPF.json NAHT.json AUS_NAHT.json

Mit voller Groesse (Faktor 1,0 / 1,0) deckt er bei offenem Maul den Mundwinkel und dabei in Bild 2 einen Teil des Unterkiefers zu (K3: IoU Unterkiefer
0,804 -> 0,576). Gewaehlt wird die groesste Stufe (fq, fn) nach Volumen, bei der die IoU von Ober- und Unterkiefer (k_mass) je um hoechstens 0,01
sinkt gegenueber dem Bau ohne diesen Koerper. Das offene Maul selbst ist gewollt (E20) und kein Nahtloch. Kein RE-Wert."""
import json
import os
import sys

import k_bau as KB
import k_mass as KM
import k_naht as KN

STUFEN = [(fq, fn) for fq in (1.0, 0.8, 0.6, 0.5, 0.4, 0.3) for fn in (1.0, 0.8, 0.6, 0.4)]
SPIEL = 0.01


def main(kopf, naht, aus):
    g = json.load(open(naht, encoding='utf-8'))
    tmp = os.path.splitext(os.path.abspath(aus))[0] + '_maulwinkel'      # je Variante eigener Ordner (parallele Laeufe)
    ohne = dict(g)
    ohne['gelenke'] = {k: v for k, v in g['gelenke'].items() if k != 'Kiefer - Kopf'}
    json.dump(ohne, open(tmp + '_naht.json', 'w', encoding='utf-8'))
    KB.bauen(kopf, tmp, 'maulwinkel', tmp + '_naht.json', None, pruefen=False)
    b = KM.messen(tmp)
    basis = (b['oberkiefer']['iou'], b['unterkiefer']['iou'])
    prot, wahl = [], None
    for fq, fn in sorted(STUFEN, key=lambda t: -(t[0] * t[0] * t[1])):
        mit = dict(ohne)
        mit['gelenke'] = dict(ohne['gelenke'])
        mit['gelenke']['Kiefer - Kopf'] = [23, fq, fn, [1, 0, 0]]
        json.dump(mit, open(tmp + '_naht.json', 'w', encoding='utf-8'))
        KB.bauen(kopf, tmp, 'maulwinkel', tmp + '_naht.json', None, pruefen=False)
        m = KM.messen(tmp)
        d = (round(m['oberkiefer']['iou'] - basis[0], 3), round(m['unterkiefer']['iou'] - basis[1], 3))
        prot.append(dict(fq=fq, fn=fn, delta_ober=d[0], delta_unter=d[1]))
        print('  Kiefer - Kopf fq %.1f fn %.1f: Oberkiefer %+.3f Unterkiefer %+.3f' % (fq, fn, d[0], d[1]), flush=True)
        if min(d) >= -SPIEL:
            wahl = (fq, fn)
            break
    if wahl:
        g['gelenke']['Kiefer - Kopf'] = [23, wahl[0], wahl[1], [1, 0, 0]]
    else:
        g['gelenke'].pop('Kiefer - Kopf', None)
    g['maulwinkel'] = dict(basis_ober_unter=basis, protokoll=prot, wahl=wahl, spiel=SPIEL)
    json.dump(g, open(aus, 'w', encoding='utf-8'), indent=1)
    print('Mundwinkel:', wahl)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], sys.argv[3])
