"""Runde 4 (Pruefer r3 Nr. 6): Welche der 10 Flaechen von Zone 45 (Kopf-Unterseite mit heutiger Maul-Farbe) sind im GANZEN Modell
sichtbar? (g_maul.py misst Kopf 22 + Unterkiefer 23 allein; dort sind 8 gross sichtbar, aber der Hals verdeckt im ganzen Modell 2 davon.)
Ganzes Modell, Clips 0-9 je Bild 0, 8 Gier-Richtungen (0-315 in 45er-Schritten) x 4 Hoehen von unten (-20/-40/-60/-80 Grad),
= 320 Ansichten, v4f.rundum_bild 300 x 300. Je Flaeche: Zahl der Ansichten mit >= 1 px und Summe der px.
Ausgabe: ARBEIT/kopfunterseite.json"""
import json, sys
import numpy as np
from g_basis import *

ka = kand(ASSET)
ML = json.load(open(ARBEIT + '/maul_lage.json', encoding='utf-8'))
ZIEL = sorted(set(ML['farbliste_maulinneres']['flaechen']) - set(ML['maulinneres_lage']))
ZIEL = [f for f in ZIEL if f.startswith('22|')]


def main(ziel):
    ans, px = {f: 0 for f in ZIEL}, {f: 0 for f in ZIEL}
    n = 0
    for clip in range(10):
        for az in range(0, 360, 45):
            for el in (-20, -40, -60, -80):
                _, K = v4f.rundum_bild(ASSET, clip, 0, az, el, 300, grund=255)
                m = K['maske']; key = (K['mesh'].astype(np.int64) * 1000000 + K['flaeche'])[m]
                u, c = np.unique(key, return_counts=True)
                d = {fl_schluessel(kk // 1000000, kk % 1000000): nn for kk, nn in zip(u.tolist(), c.tolist())}
                for f in ZIEL:
                    if d.get(f, 0) > 0:
                        ans[f] += 1; px[f] += d[f]
                n += 1
    out = dict(regel=__doc__.strip(), ansichten=n, flaechen={f: dict(ansichten_sichtbar=ans[f], px=px[f]) for f in ZIEL},
               aussen_sichtbar=sorted(f for f in ZIEL if ans[f] >= n // 4),
               verdeckt=sorted(f for f in ZIEL if ans[f] < n // 4))
    json.dump(out, open(ziel, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    for f in ZIEL:
        print(f, ans[f], px[f])
    print('aussen sichtbar', out['aussen_sichtbar'], 'verdeckt', out['verdeckt'])


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/kopfunterseite.json')
