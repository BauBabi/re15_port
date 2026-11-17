#!/usr/bin/env python3
"""Liest die Schlusszeilen "[tri-liste] ENDE ..." (G3-Zaehler, render_pc.c) aus den debug.log der Spiellaeufe und schreibt
K5/k5g/K5g3/geo/messung/spiel_dreiecke.json (Eingang von riegel_k5g3.py R9).

  python kf_k5g3_spielzahl.py <ordner-mit-laeufen> [<ordner> ...]   (je Unterordner mit debug.log ein Lauf; Name = Unterordner)
"""
import hashlib
import json
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
ZIEL = os.path.join(os.path.dirname(HIER), 'k5g', 'K5g3', 'geo', 'messung', 'spiel_dreiecke.json')
RX = re.compile(r'\[tri-liste\] ENDE hoechststand=(\d+) \(F(\d+)\) verworfen=(\d+) bilder_mit_verlust=(\d+) grenze=(\d+)')


def main(ordner):
    laeufe = []
    for o in ordner:
        for n in sorted(os.listdir(o)):
            p = os.path.join(o, n, 'debug.log')
            if not os.path.isfile(p):
                continue
            t = open(p, encoding='utf-8', errors='replace').read()
            m = RX.search(t)
            if not m:
                continue
            raum = re.search(r'EXIT_AT: Bild (\d+) in Raum ([0-9a-f]+)', t)
            laeufe.append(dict(lauf=n, quelle=p.replace(chr(92), '/'), raum=raum.group(2) if raum else '?', bilder=int(raum.group(1)) if raum else -1,
                               hoechststand=int(m.group(1)), bild=int(m.group(2)), verworfen=int(m.group(3)), bilder_mit_verlust=int(m.group(4)),
                               grenze=int(m.group(5)), form_geladen='Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips' in t,
                               ueberlauf_zeilen=t.count('[tri-liste] UEBERLAUF')))
    # Die Laeufe nehmen das Spiel-Asset (shared_assets/RE2, seit dem R9-Einbau == K5/k5g/K5g3/BAHNSTEIG_FORM.EMD); dessen sha gehoert in die
    # Messung (riegel_k5g3.py R9 nimmt Geometrie-EMD oder Asset). Seit der Stufe zaehne_vorn (16.11.2026) ist das Asset != geo/BAHNSTEIG_FORM.EMD.
    emd = os.path.join(os.path.dirname(ZIEL), '..', '..', 'BAHNSTEIG_FORM.EMD')
    d = dict(quelle='G3-Zaehler render_pc.c (Schlusszeile [tri-liste] ENDE aus re15_testhaken_ende), Laeufe von '
                    're15_port/tests/integration/test_r41_form_dreiecke.cmake bzw. generator/kf_k5g3_spiel.sh',
             emd_sha256=hashlib.sha256(open(emd, 'rb').read()).hexdigest(), laeufe=laeufe)
    os.makedirs(os.path.dirname(ZIEL), exist_ok=True)
    json.dump(d, open(ZIEL, 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    for l in laeufe:
        print('%-12s Raum %s  hoechststand %5d (F%d)  verworfen %d  Grenze %d' % (l['lauf'], l['raum'], l['hoechststand'], l['bild'], l['verworfen'], l['grenze']))
    print('->', ZIEL.replace(chr(92), '/'))


if __name__ == '__main__':
    main(sys.argv[1:])
