"""Markdown-Tabellen fuer MESSBLATT_FARBE_V3_NACHTRAG_ROT.md aus mess.json / ziele.json (keine Handabschrift)."""
import json
import numpy as np

E = json.load(open('mess.json', encoding='utf-8'))
Z = json.load(open('ziele.json', encoding='utf-8'))
S = E['saaten']
REG = E['regionen'] + ['Figur']


def f(x, k=1):
    if x is None:
        return '-'
    return ('%.*f' % (k, x)).replace('.', ',')


def mittel(z, pre, m):
    v = [z['%s%d' % (pre, s)][m] for s in S if z['%s%d' % (pre, s)][m] is not None]
    return float(np.mean(v)) if v else None


aus = []
for raum, titel in (('T', 'Kette, Texelraum T = Umkehr / Figurlicht (p_zonen.rueck)'), ('V', 'roh in der Kette (Videobild V)')):
    for n in ('1', '2'):
        B = E['bilder'][n]
        aus.append('\n**Bild %s, %s.** Schwelle a\\* >= %s (Neutral-Probe p99). Spalten je Mass: Vorlage O | Rundlauf R (3 Saaten) | V3 r3 (3 Saaten).\n'
                   % (n, titel, f(B['A0'][raum], 0)))
        aus.append('| Region | px (Bloecke) | Rot eng % | Farbton a\\*-Klasse Grad | C\\* Rotklasse | dL Rot - Haut (L\\*) | groesste Rotflaeche eng % | a\\*-Klasse % (rot + mauve) |')
        aus.append('|---|---|---|---|---|---|---|---|')
        for rn in REG:
            z = B['zonen'][rn]
            if 'O_T' not in z:
                aus.append('| %s | %d | nicht sichtbar (< %d px) | | | | | |' % (rn, z['px'], E['min_px']))
                continue
            o = z['O_' + raum]
            cells = []
            for m in ('rot_eng', 'h_rot', 'C_rot', 'dL', 'gross_eng_p', 'rot'):
                cells.append('%s / %s / %s' % (f(o[m]), f(mittel(z, 'R_' + raum, m)), f(mittel(z, 'V3_' + raum, m))))
            aus.append('| %s | %d (%d) | %s |' % (rn, o['px'], z['bloecke'], ' | '.join(cells)))

aus.append('\n**Neutral (Tafel-03-Licht 0,55 + 0,45 N.L, 3 Winkel zusammen), Texel = Licht herausgerechnet, Schwelle a\\* >= %s.** '
           'Spalten je Mass: V3 r3 | RE2-G4.\n' % f(E['neutral']['A0'], 0))
aus.append('| Region | px V3 / G4 | Rot eng % | Farbton a\\*-Klasse Grad | C\\* Rotklasse | dL Rot - Haut (L\\*) | groesste Rotflaeche eng % | a\\*-Klasse % |')
aus.append('|---|---|---|---|---|---|---|---|')
for rn in REG:
    v, g = E['neutral']['V3'][rn], E['neutral']['G4'][rn]
    vt, gt = v.get('texel'), g.get('texel')
    def p(d, m):
        return f(d[m]) if d else '-'
    cells = ['%s / %s' % (p(vt, m), p(gt, m)) for m in ('rot_eng', 'h_rot', 'C_rot', 'dL', 'gross_eng_p', 'rot')]
    aus.append('| %s | %s / %s | %s |' % (rn, vt['px'] if vt else v['px'], gt['px'] if gt else g['px'], ' | '.join(cells)))

# Leuchtstellen (Info)
aus.append('\n**Leuchtstellen (unveraendert, nur Info):** ')
for n in ('1', '2'):
    L = E['bilder'][n]['leuchtstelle']
    aus.append('Bild %s %s: O_T Lab %s, V3 Lab %s; ' % (n, 'F1' if n == '1' else 'Band', L['O_T']['lab'], L['V3_T15']['lab']))

# Ziele
aus.append('\n\n| Bild / Region | Mass | Ziel (O) | Toleranz | Fenster | V3 r3 | Urteil V3 r3 |')
aus.append('|---|---|---|---|---|---|---|')
NAME = dict(rot_eng='Rot eng %', h_rot='Farbton Grad', dL='dL Rot-Haut', gross_eng_p='groesste Rotflaeche %', C_rot='C* Rotklasse')
for key, e in Z.items():
    if not key.endswith('|T'):
        continue
    for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p', 'C_rot'):
        x = e.get(m)
        if not x or x.get('ziel') is None:
            continue
        aus.append('| %s | %s | %s | %s | %s..%s | %s | %s%s |' % (key[:-2].replace('bild', 'Bild ').replace('|', ' / '), NAME[m], f(x['O']), f(x['tol']),
                   f(x['ziel'][0]), f(x['ziel'][1]), f(x['V3']), 'GRUEN' if x['ok'] else 'ROT', '' if x['bindend'] else ' (Info)'))
open('tabellen.md', 'w', encoding='utf-8').write('\n'.join(aus))
print('\n'.join(aus))
