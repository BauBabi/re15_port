#!/usr/bin/env python3
"""K5d: Helligkeit der hellen Detailflecken an der Szenenkette auf das Niveau der Vorlage stellen (Auftrag: die Fotos geben Orte, Farbrichtung
und Helligkeitsniveau vor; Texturen nicht Texel fuer Texel eichen - hier wird je Fleck nur EIN Ziel-L* der Albedo verschoben, die
Binnenzeichnung bleibt, Farbe bleibt die Ziel-Farbe aus dem Messblatt).

    python kf_k5d_eichen.py <par.json> <arbeits-Ordner> [--runden 4]

Je Runde: kf_k5d.bauen -> Kette T (riegel_k5d.Stand, 3 Saaten) -> je Fleckgruppe mit "eichen": Ketten-T-L* auf den Fleckpixeln gegen das
Vorlagen-T-L* (Ziel = 100 %, Riegelfenster 80..125 %). Neues Ziel-L* = alt + (Vorlage - Kette) / VERSTAERKUNG (gemessen: Kette/Albedo
1,3..1,8 an F04/F08, Lauf v9), begrenzt auf 12..85. Schreibt das Ergebnis in par.json zurueck und protokolliert jede Runde.
"""
import argparse
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5d as B  # noqa: E402
import riegel_k5d as R  # noqa: E402

VERSTAERKUNG = 1.5


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('par')
    ap.add_argument('arbeit')
    ap.add_argument('--runden', type=int, default=4)
    a = ap.parse_args()
    vor = R.Vorlage()
    protokoll = []
    for r in range(a.runden):
        par = json.load(open(a.par, encoding='utf-8'))
        aus = os.path.join(a.arbeit, 'eich%d' % r)
        B.bauen(json.loads(json.dumps(par)), aus)
        R.v4f._KC.clear()
        st = R.Stand(aus)
        zeile = {}
        fertig = True
        for g in par['flecken']:
            if not g.get('eichen'):
                continue
            n = g['bild']
            m = np.zeros((480, 640), bool)
            for fid in g['flecken']:
                m |= R.FLECKEN[fid]
            m &= ~vor.d[n]['clip']
            k_ = float(st.d[n]['LT'][m][:, 0].mean())
            o_ = float(vor.d[n]['LT'][m][:, 0].mean())
            z = par['ziele'][g['ziel']]
            alt = float(z['lab'][0])
            q = k_ / o_
            neu = float(np.clip(alt + (o_ - k_) / VERSTAERKUNG, 12.0, 85.0))
            if not 0.9 <= q <= 1.12:
                fertig = False
                z['lab'][0] = round(neu, 1)
            zeile[g['ziel']] = dict(kette=round(k_, 1), vorlage=round(o_, 1), quote=round(q, 3), albedo_L_alt=alt, albedo_L_neu=z['lab'][0])
        print('Runde', r, json.dumps(zeile, ensure_ascii=False), flush=True)
        protokoll.append(dict(runde=r, flecken=zeile))
        json.dump(par, open(a.par, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        if fertig:
            break
    json.dump(protokoll, open(os.path.join(a.arbeit, 'eichen_protokoll.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)


if __name__ == '__main__':
    main()
