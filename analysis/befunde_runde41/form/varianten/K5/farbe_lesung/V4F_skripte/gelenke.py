"""Gelenkkoerper (k_naht.kugel: Ellipsoid 8 x 5, 24 Vierecke + 16 Dreiecke, 34 Ecken am Stueck) je Mesh erkennen und benennen.
Rueckgabe: {(mesh, viereck, fi): name}. Name = Gelenk laut k_naht.GELENKE/MUFFEN (Mitte = versatz eines Kindes -> Gelenk, Mitte ~ 0 -> Muffe)."""
import numpy as np
import v4f
import form_emd as FE

GELENKE = {8: 'Huefte l', 9: 'Huefte r', 11: 'Schulter l (Arm l - Brust)', 16: 'Schulter r (Arm r - Brust)', 21: 'Hals - Brust', 1: 'Schwanz - Becken',
           22: 'Kopf - Hals', 7: 'Wirbel - Huefte', 10: 'Brust - Wirbel', 24: 'Knie l', 26: 'Knie r', 12: 'Ellbogen l', 17: 'Ellbogen r', 25: 'Fuss l',
           27: 'Fuss r', 13: 'Hand l', 18: 'Hand r', 2: 'Schwanz 1-2', 3: 'Schwanz 2-3', 4: 'Schwanz 3-4', 5: 'Schwanz 4-5', 6: 'Schwanz 5-6',
           23: 'Kiefer - Kopf'}
MUFFEN = {8: 'Muffe Huefte l', 9: 'Muffe Huefte r', 11: 'Muffe Schulter l', 16: 'Muffe Schulter r', 21: 'Muffe Hals - Brust', 22: 'Muffe Kopf - Hals',
          1: 'Muffe Schwanz - Becken', 13: 'Muffe Hand l', 18: 'Muffe Hand r'}


def gelenk_flaechen(ordner):
    f = FE.lese_emd(open(ordner + '/FORM.EMD', 'rb').read())
    kn = f['knochen']
    out = {}
    namen = {}
    for mi, m in enumerate(f['meshes']):
        V = len(m['ecken'])
        use = {}
        for art in ('dreiecke', 'vierecke'):
            for i, fc in enumerate(m[art]):
                for e in fc['e']:
                    use.setdefault(e, set()).add((art, i))
        s = 0
        while s + 34 <= V:
            fs = set().union(*[use.get(e, set()) for e in range(s, s + 34)])
            ok = all(all(s <= e < s + 34 for e in m[a][i]['e']) for a, i in fs)
            nq = sum(1 for a, i in fs if a == 'vierecke')
            if ok and nq == 24 and len(fs) - nq == 16:
                mitte = np.array(m['ecken'][s:s + 34], float).mean(0)
                kinder = [c for c, k in enumerate(kn) if k['eltern'] == mi and c != mi]
                best, bd = None, 1e18
                for c in kinder:
                    d = np.linalg.norm(mitte - np.array(kn[c]['versatz'], float))
                    if d < bd:
                        best, bd = c, d
                d0 = np.linalg.norm(mitte)
                if mi in MUFFEN and d0 < bd:
                    nm = MUFFEN[mi]
                else:
                    nm = GELENKE.get(best, 'Gelenk %d-%s' % (mi, best))
                for a, i in fs:
                    out[(mi, 1 if a == 'vierecke' else 0, i)] = nm
                namen[nm] = mi
                s += 34
            else:
                s += 1
    return out, namen


if __name__ == '__main__':
    import collections
    g, n = gelenk_flaechen(v4f.ST['Asset K5c'])
    print(len(g), len(n))
    print(collections.Counter(g.values()))
