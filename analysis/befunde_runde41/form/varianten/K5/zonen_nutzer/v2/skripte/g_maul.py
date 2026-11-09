"""Runde 3 (Pruefer r2 Nr. 11): "geoeffnetes Maul" nach LAGE, nicht nach der Farbliste.
Die material.json-Liste "Maulinneres" ist eine FARBLISTE (Flaechen, deren heutige Texel auf Texturzeile 0 rot sind), keine Lageliste:
8 ihrer Flaechen (22|1|1, 8, 9, 194, 201, 209, 210, 217) liegen aussen an der Kopf-Unterseite und sind in jeder Pose gross sichtbar
(die roten Hals-Streifen), die uebrigen sind fast oder ganz verdeckt.
Regel (Lage): Kopf 22 + Unterkiefer 23 allein, 84 Richtungen (Gier 0-330 in 30er-Schritten x Hoehe -75/-45/-20/0/20/45/75),
v4f.rundum_bild 300 x 300, sichtbare Pixel je Flaeche. Posen: Clip 12 Bild 0 (= Bild 2, Maul offen), Clip 2 Bild 0 und Clip 0 Bild 0
(Maul fast zu). "Von vorn" = Gier 60-120, Hoehe <= 20.
  Maulinneres = Flaeche, die mit offenem Maul von vorn >= 30 px zeigt UND in Clip 2 und Clip 0 jeweils < 30 % ihrer Clip-12-Sichtbarkeit
  (sie wird erst durch das Oeffnen sichtbar). Zaehne (material.json "Zahn") bleiben Zone 41.
Ausgabe: ARBEIT/maul_lage.json (Sichtbarkeit je Flaeche, Mengen)."""
import json, sys
import numpy as np
from g_basis import *

ka = kand(ASSET)
POSEN = {'12': (12, 0), '2': (2, 0), '0': (0, 0)}


def sicht():
    res = {}
    for name, (clip, bild) in POSEN.items():
        vis, vorn = {}, {}
        for az in range(0, 360, 30):
            for el in (-75, -45, -20, 0, 20, 45, 75):
                _, K = v4f.rundum_bild(ASSET, clip, bild, az, el, 300, grund=255, teile=(22, 23))
                m = K['maske']; key = (K['mesh'].astype(np.int64) * 1000000 + K['flaeche'])[m]
                u, c = np.unique(key, return_counts=True)
                for kk, n in zip(u.tolist(), c.tolist()):
                    f = fl_schluessel(kk // 1000000, kk % 1000000)
                    vis[f] = vis.get(f, 0) + n
                    if 60 <= az <= 120 and el <= 20:
                        vorn[f] = vorn.get(f, 0) + n
        res[name] = dict(alle=vis, vorn=vorn)
    return res


def main(ziel):
    ml = material_listen()
    S = sicht()
    alle = [f for f in alle_flaechen(ka.mo) if f.startswith(('22|', '23|'))]
    g = lambda p, k, f: S[p][k].get(f, 0)
    innen = sorted(f for f in alle if g('12', 'vorn', f) >= 30 and g('2', 'alle', f) < 0.3 * g('12', 'alle', f)
                   and g('0', 'alle', f) < 0.3 * g('12', 'alle', f) and f not in ml['Zahn'])
    liste = sorted(ml['Maulinneres'])
    aussen_gross = sorted(f for f in liste if min(g(p, 'alle', f) for p in POSEN) >= 1000)
    out = dict(regel=__doc__.strip(), maulinneres_lage=innen,
               farbliste_maulinneres=dict(flaechen=liste, aussen_gross_sichtbar=aussen_gross,
                                          davon_im_maulinneren_nach_lage=sorted(set(liste) & set(innen))),
               sichtbarkeit={f: {p: [g(p, 'alle', f), g(p, 'vorn', f)] for p in POSEN} for f in sorted(set(innen) | set(liste))})
    json.dump(out, open(ziel, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('Maulinneres nach Lage', len(innen), innen)
    print('Farbliste davon aussen gross sichtbar', aussen_gross)
    print('Farbliste im Maulinneren', out['farbliste_maulinneres']['davon_im_maulinneren_nach_lage'])


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/maul_lage.json')
