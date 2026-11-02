#!/usr/bin/env python3
"""Tabellen fuer FORMBLATT2 aus gerippe.json + lauf/teile_mass.json + lauf/vergleich.json + lauf/merkmale_*.json. Schreibt lauf/tabellen.md."""
import json, math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
F = os.path.normpath(os.path.join(HIER, '..'))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.normpath(os.path.join(F, '..', '..', '..', 're15_port', 'tools')))
import form_kamera as FK   # noqa: E402

aus = []


def teile():
    m = json.load(open(os.path.join(HIER, 'lauf', 'teile_mass.json'), encoding='utf-8'))
    aus.append('| Teil | Bild | Laenge px | Mittellinie (x,y) Breite px | Engstellen | Wuelste |\n|---|---|---|---|---|---|')
    for n, e in m.items():
        ml = e['mittellinie']
        s = ' '.join('(%d,%d) %d' % (a, b, c) for a, b, c in ml[::2])
        eng = ' '.join('(%d,%d) %d' % (a, b, c) for a, b, c in e['engstellen']) or '-'
        wu = ' '.join('(%d,%d) %d' % (a, b, c) for a, b, c in e['wuelste']) or '-'
        aus.append('| %s | %d | %d | %s | %s | %s |' % (n, e['bild'], e['laenge_px'], s, eng, wu))
    aus.append('')


def knochen():
    g = json.load(open(os.path.join(F, 'gerippe.json'), encoding='utf-8'))
    aus.append('| Nr | Knochen | Eltern | Versatz (x,y,z) | Laenge | Dicke a Anfang/Mitte/Ende | Dicke b | Teil Bild 1 | Teil Bild 2 |\n|---|---|---|---|---|---|---|---|---|')
    for k in g['knochen']:
        d = k['dicke']
        aus.append('| %d | %s | %s | %s | %d | %d/%d/%d | %d/%d/%d | %s | %s |' % (
            k['index'], k['name'], k['eltern'], tuple(k['versatz']), k['laenge'], d[0]['a'], d[1]['a'], d[2]['a'], d[0]['b'], d[1]['b'], d[2]['b'],
            k['teil_bild1'] or '-', k['teil_bild2'] or '-'))
    aus.append('')
    aus.append('| Bild | Teil | IoU sichtbar | Deckung Teil | Deckung Figur | Ueberstand | Kante Median px | Kante p90 px |\n|---|---|---|---|---|---|---|---|')
    for b in ('bild1', 'bild2'):
        for n, r in g['restfehler'][b].items():
            if n.startswith('_'):
                continue
            aus.append('| %s | %s | %.2f | %.2f | %.2f | %.2f | %.1f | %.1f |' % (b[-1], n, r['iou'], r['decke'], r['decke_alle'], r['ueber'], r['kante_med'], r['kante_p90']))
    aus.append('')
    aus.append('Ganzzahl-Probe: %s' % g['ganzzahl_probe'])
    aus.append('')


def main():
    teile()
    if os.path.exists(os.path.join(F, 'gerippe.json')):
        knochen()
    open(os.path.join(HIER, 'lauf', 'tabellen.md'), 'w', encoding='utf-8').write('\n'.join(aus))
    print('\n'.join(aus)[:1500])


if __name__ == '__main__':
    main()
