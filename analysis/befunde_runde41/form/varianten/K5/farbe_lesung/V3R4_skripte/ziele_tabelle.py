"""Kompakte Zieltabelle (Abschnitt 5 des Nachtrags) aus ziele.json."""
import json
Z = json.load(open('ziele.json', encoding='utf-8'))
def f(x): return '-' if x is None else ('%.1f' % x).replace('.', ',')
rows = ['| Bild / Region (Bloecke) | RV1 Rot eng % | RV2 Farbton a\*-Klasse Grad | RV3 dL Rot - Haut | RV4 groesste Rotflaeche eng % (Obergrenze) | RV5 C\* Rotklasse (Info) |',
        '|---|---|---|---|---|---|']
for k, e in Z.items():
    if not k.endswith('|T'):
        continue
    n, rn, _ = k.split('|')
    cells = []
    for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p', 'C_rot'):
        x = e.get(m)
        if not x or not x.get('ziel'):
            cells.append('kein Ziel (Rundlauf ohne Rotklasse)'); continue
        lo, hi = x['ziel']
        fen = ('<= %s' % f(hi)) if m == 'gross_eng_p' else ('%s..%s' % (f(lo), f(hi)))
        if x['V3'] is None:
            u = 'V3 ohne Rotklasse'
        else:
            u = 'V3 %s %s' % (f(x['V3']), 'GRUEN' if x['ok'] else '**ROT**')
        if not x['bindend']:
            u += ' (Info)'
        cells.append('O %s, %s; %s' % (f(x['O']), fen, u))
    rows.append('| %s %s (%d) | %s |' % (n.replace('bild', 'Bild '), rn, e['rot_eng'].get('bloecke', 0), ' | '.join(cells)))
open('ziele_tabelle.md', 'w', encoding='utf-8').write('\n'.join(rows))
print('\n'.join(rows))
