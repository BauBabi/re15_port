#!/usr/bin/env python3
"""FORMBLATT2.md aus Vorlage + Daten: python bericht.py (liest gerippe.json, lauf/*.json, formblatt2_vorlage.md)."""
import json, math, os, re, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
F = os.path.normpath(os.path.join(HIER, '..'))
L = os.path.join(HIER, 'lauf')


def lese(n):
    return json.load(open(os.path.join(L, n), encoding='utf-8'))


def t_teile():
    m = lese('teile_mass.json')
    z = lese('teile_zeilen.json')
    aus = ['| Teil | Bild | Schnitte (Lage: von-bis) | Mittellinie, je zweiter Stuetzpunkt (x,y) Breite |', '|---|---|---|---|']
    for n, e in m.items():
        s = z[n]['schnitte']
        sch = ('Zeile ' if z[n]['art'] == 'zeilen' else 'Spalte ') + ', '.join('%d: %d-%d' % (a, b, c) for a, b, c, _, _ in s[::2])
        ml = ' '.join('(%d,%d) %d' % (a, b, c) for a, b, c in e['mittellinie'][::2])
        aus.append('| %s | %d | %s | %s |' % (n, e['bild'], sch, ml))
    return '\n'.join(aus)


def t_knochen(g):
    aus = ['| Nr | Knochen | Eltern | Versatz | Laenge | Dicke a (Anfang/Mitte/Ende) | Dicke b | Bild 1 | Bild 2 |', '|---|---|---|---|---|---|---|---|---|']
    for k in g['knochen']:
        d = k['dicke']
        v = k['versatz']
        aus.append('| %d | %s | %s | %d, %d, %d | %d | %d/%d/%d | %d/%d/%d | %s | %s |' % (k['index'], k['name'], k['eltern'], v[0], v[1], v[2], k['laenge'],
                   d[0]['a'], d[1]['a'], d[2]['a'], d[0]['b'], d[1]['b'], d[2]['b'], k['teil_bild1'] or '-', k['teil_bild2'] or '-'))
    return '\n'.join(aus)


def t_rest(g):
    aus = ['| Bild | Teil | IoU sichtbar | Teil gedeckt | Figur deckt Teil | Ueberstand | Kante Median px | Kante 90 % px |', '|---|---|---|---|---|---|---|---|']
    for b in ('bild1', 'bild2'):
        for n, r in g['restfehler'][b].items():
            if n.startswith('_'):
                continue
            aus.append('| %s | %s | %.2f | %.2f | %.2f | %.2f | %.1f | %.1f |' % (b[-1], n, r['iou'], r['decke'], r['decke_alle'], r['ueber'], r['kante_med'], r['kante_p90']))
    return '\n'.join(aus)


def t_profile():
    p = lese('profile_q.json')
    namen = {'arm': 'Arm (Ober- + Unterarm)', 'bein': 'Bein (Ober- + Unterschenkel)', 'rumpf': 'Rumpf (Huefte bis Brust)', 'schwanz': 'Schwanz gesamt', 'tiefe1': 'Huefte Tiefe Bild 1',
             'tiefe2': 'Huefte Tiefe Bild 2', 'gier1': 'Gier Bild 1 (Grad)', 'gier2': 'Gier Bild 2 (Grad)', 'kopfx2': 'Kopfmitte x Bild 2', 'kopfy2': 'Kopfmitte y Bild 2'}
    aus = ['| Groesse | Wert: Bildanteil der Kosten |', '|---|---|']
    for k, n in namen.items():
        if k in p:
            aus.append('| %s | %s |' % (n, ', '.join('%d: %.2f' % (w, b) for w, b in zip(p[k]['werte'], p[k]['bild']))))
    return '\n'.join(aus)


def t_pose(nr):
    z = lese('posen_lesen.json')[str(nr)]
    aus = ['| Gelenk | Bild | Hoehe | Tiefe |', '|---|---|---|---|']
    for n, e in z.items():
        aus.append('| %s | (%d,%d) | %d | %d |' % (n, e['bild'][0], e['bild'][1], e['hoehe'], e['tiefe']))
    return '\n'.join(aus)


def t_merkmale():
    m = lese('merkmale_ende.json')
    gr = {}
    aus = ['| Merkmal | Bild | Punkt | Knochen | Nutzer-Teil | Hoehe | vor Huefte | links(+)/rechts(-) der Mitte | Abstand zum Modell px |', '|---|---|---|---|---|---|---|---|---|']
    for e in m:
        aus.append('| %s | %d | (%d,%d) | %s | %s | %d | %d | %d | %.0f |' % (e['name'], e['bild'], e['punkt'][0], e['punkt'][1], e['knochen'], e['teil'], e['hoehe'], e['vorn'], e['links'], e['weg_px']))
    return '\n'.join(aus)


def t_pruef():
    p = lese('pruefpunkte_ende.json')
    aus = ['| Punkt | Modell |', '|---|---|']
    for a, b in p:
        aus.append('| %s | %s |' % (a, b))
    return '\n'.join(aus)


def main():
    g = json.load(open(os.path.join(F, 'gerippe.json'), encoding='utf-8'))
    t = open(os.path.join(HIER, 'formblatt2_vorlage.md'), encoding='utf-8').read()
    ers = {'{{TEILE}}': t_teile(), '{{KNOCHEN}}': t_knochen(g), '{{REST}}': t_rest(g), '{{PROFILE}}': t_profile(), '{{POSE1}}': t_pose(1), '{{POSE2}}': t_pose(2),
           '{{MERKMALE}}': t_merkmale(), '{{PRUEF}}': t_pruef()}
    for nr in (1, 2):
        p = g['posen']['bild%d' % nr]
        ers['{{LAGE%d}}' % nr] = '(%d, %d, %d)' % tuple(p['lage'])
        ers['{{GIER%d}}' % nr] = '%d (%s Grad)' % (p['gier'], str(p['gier_grad']).replace('.', ','))
        ers['{{WURZEL%d}}' % nr] = '%d' % -p['wurzel'][1]
    for k, v in ers.items():
        t = t.replace(k, v)
    open(os.path.join(F, 'FORMBLATT2.md'), 'w', encoding='utf-8').write(t)
    print('FORMBLATT2.md', len(t), 'Zeichen', 'Reste:', re.findall(r'\{\{.*?\}\}', t))


if __name__ == '__main__':
    main()
