#!/usr/bin/env python3
"""Variante K: Tabellen fuer K.md aus den Messdateien (form_pruefung bericht.json, mass_kiefer_krallen.json, loecher_stufe3.json, vorher_J_stufe3.json).
    python k_bericht.py > ../_lauf/tabellen.md"""
import json
import os
from collections import Counter

import k_kern as KK

K = KK.K_DIR
VAR = {'J': os.path.join(KK.VAR, 'J_dicker'), 'K1': os.path.join(K, 'K1_lang'), 'K2': os.path.join(K, 'K2_breit'), 'K3': os.path.join(K, 'K3_fit')}
HAUPT = ('Bein l - Huefte', 'Bein r - Huefte', 'Arm l - Brust', 'Arm r - Brust', 'Hals - Brust', 'Schwanz - Becken', 'Kopf - Hals', 'Brust - Wirbel')


def f(x, n=3):
    return ('%.*f' % (n, x)).replace('.', ',') if isinstance(x, (int, float)) else str(x)


def pruef(d):
    b = json.load(open(os.path.join(d, 'pruefung', 'bericht.json'), encoding='utf-8'))
    out = {}
    for n in (1, 2):
        B = b['bild%d' % n]
        pk = [p for p in B['punkte'] if p.get('zaehlt', True)]
        fl = [p for p in B['flaechen'] if p.get('zaehlt', True)]
        ze = [p for p in B['zeichnung'] if p.get('zaehlt', True)]
        vb = B.get('verbote', [])
        out[n] = dict(iou=B['umriss']['iou'], punkte='%d/%d' % (sum(p['ok'] for p in pk), len(pk)), nein=[p['id'] for p in pk if not p['ok']],
                      flaechen='%d/%d' % (sum(p['ok'] for p in fl), len(fl)), zeichnung='%d/%d' % (sum(p['ok'] for p in ze), len(ze)),
                      zeichnung_nein=[p['id'] for p in ze if not p['ok']], verbote='%d/%d' % (sum(p['ok'] for p in vb), len(vb)), mae=B.get('mae_weiss'))
    out['flaechen_zahl'] = b['kennzahlen'].get('flaechen') if isinstance(b.get('kennzahlen'), dict) else None
    return out


def je_naht(p):
    R = json.load(open(p, encoding='utf-8'))
    c, jc = Counter(), {}
    for cl in R['clips']:
        jc[cl['clip']] = (cl['naht'], cl['naht_px'], cl.get('je_naht', {}).get('Kiefer - Kopf', 0))
        for k, v in cl['je_naht'].items():
            c[k] += v
    return c, jc, R['summe']


def main():
    P = {n: pruef(d) for n, d in VAR.items()}
    M = json.load(open(os.path.join(K, 'mass_kiefer_krallen.json'), encoding='utf-8'))
    print('| | Flaechen | Bild 1 IoU | Bild 1 Punkte | Bild 1 Boden/Kerbe | Bild 2 IoU | Bild 2 Punkte | Bild 2 Boden/Verd. | Zeichnung 1 / 2 | Verbote 2 |')
    print('|---|---|---|---|---|---|---|---|---|---|')
    for n, p in P.items():
        print('| %s | %s | %s | %s%s | %s | %s | %s | %s | %s / %s%s | %s |' % (
            n, p['flaechen_zahl'], f(p[1]['iou']), p[1]['punkte'], (' (' + ', '.join(p[1]['nein']) + ')') if p[1]['nein'] else '', p[1]['flaechen'],
            f(p[2]['iou']), p[2]['punkte'], p[2]['flaechen'], p[1]['zeichnung'], p[2]['zeichnung'],
            (' (Bild 2: ' + ', '.join(p[2]['zeichnung_nein']) + ')') if p[2]['zeichnung_nein'] else '', p[2]['verbote']))
    print()
    print('| | IoU Oberkiefer | ohne Rumpf | Schwerpunkt px / Achse Grad | IoU Unterkiefer | Schwerpunkt px / Achse Grad | Arm r ohne Unterkiefer | Krallen rechts (Paare/Soll, mittl. px) | Krallen links |')
    print('|---|---|---|---|---|---|---|---|---|')
    for n, m in M.items():
        o, u = m['oberkiefer'], m['unterkiefer']
        kr, kl = m['krallen_rechts'], m['krallen_links']
        print('| %s | %s | %s | %s / %s | %s | %s / %s | %s | %d/%d, %s | %d/%d, %s |' % (
            n, f(o['iou']), f(o['iou_ohne_rumpf']), f(o['lage']['schwerpunkt_abstand_px'], 1), f(o['lage']['achse_abweichung_grad'], 1), f(u['iou']),
            f(u['lage']['schwerpunkt_abstand_px'], 1), f(u['lage']['achse_abweichung_grad'], 1), f(m['rechter_arm_ohne_unterkiefer']['iou']),
            kr['paare'], kr['spitzen_soll'], f(kr['abstand_mittel_px'], 1), kl['paare'], kl['spitzen_soll'], f(kl['abstand_mittel_px'], 1)))
    print()
    v, vc, vs = je_naht(os.path.join(K, 'loecher', 'vorher_J_stufe3.json'))
    N = {n: je_naht(os.path.join(VAR[n], 'loecher_stufe3.json')) for n in ('K1', 'K2', 'K3')}
    clips = {c['clip']: c['name'] for c in json.load(open(os.path.join(KK.FORM, 'stufe3', 'asset', 'clips.json'), encoding='utf-8'))}
    print('Nahtloecher je Clip (Zahl / px; in Klammern davon Mundwinkel Kiefer - Kopf, px):')
    print()
    print('| Clip | J vorher | K1 | K2 | K3 |')
    print('|---|---|---|---|---|')
    for c in sorted(vc):
        print('| %d %s | %d / %d (%d) | %s |' % (c, clips.get(c, ''), vc[c][0], vc[c][1], vc[c][2],
                                         ' | '.join('%d / %d (%d)' % N[n][1][c] for n in ('K1', 'K2', 'K3'))))
    print('| Summe | %d / %d (%d) | %s |' % (vs['naht'], vs['naht_px'], v.get('Kiefer - Kopf', 0),
                                         ' | '.join('%d / %d (%d)' % (N[n][2]['naht'], N[n][2]['naht_px'], N[n][0].get('Kiefer - Kopf', 0)) for n in ('K1', 'K2', 'K3'))))
    print()
    print('Nahtloecher je Gelenk ueber alle 28 Clips, px:')
    print()
    alle = sorted(set(v) | set().union(*[set(N[n][0]) for n in N]), key=lambda k: -v.get(k, 0))
    print('| Gelenk | J vorher | K1 | K2 | K3 |')
    print('|---|---|---|---|---|')
    for k in alle:
        print('| %s%s | %d | %s |' % (k, ' (Hauptnaht)' if k in HAUPT else '', v.get(k, 0), ' | '.join(str(N[n][0].get(k, 0)) for n in ('K1', 'K2', 'K3'))))
    s_v = sum(x for k, x in v.items() if k != 'Kiefer - Kopf')
    print('| Summe ohne Mundwinkel | %d | %s |' % (s_v, ' | '.join(str(sum(x for k, x in N[n][0].items() if k != 'Kiefer - Kopf')) for n in ('K1', 'K2', 'K3'))))
    print('| Summe Hauptnaehte | %d | %s |' % (sum(v.get(k, 0) for k in HAUPT), ' | '.join(str(sum(N[n][0].get(k, 0) for k in HAUPT)) for n in ('K1', 'K2', 'K3'))))


if __name__ == '__main__':
    main()
