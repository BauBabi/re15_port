"""Runde 4 (Pruefer r3 Nr. 5): "geoeffnetes Maul" und Oberlippe nach LAGE, mit WIRKLICH geschlossenem Maul als Vergleich.
Runde 3 nahm Clip 0 Bild 0 als "fast zu"; dort ist das Maul aber offen (Kieferwinkel unten gemessen, Gaumen 22|1|30 zeigt dort
ein Vielfaches von Clip 5). Jetzt: "zu" = Clip 5/0, 7/0 und 2/0 (Kieferwinkel je Pose gemessen und ausgegeben, Drehung Unterkiefer
23 gegen Kopf 22 wie streifen/skripte/r3_lage.py), "offen" = Clip 12 Bild 0 (= Bild 2).
Die material.json-Listen "Maulinneres" und "Lippensaum" sind FARBLISTEN (heutige Texturfarbe), keine Lagelisten.
Sichtbarkeit: Kopf 22 + Unterkiefer 23 allein, 84 Richtungen (Gier 0-330 in 30er-Schritten x Hoehe -75/-45/-20/0/20/45/75),
v4f.rundum_bild 300 x 300, sichtbare Pixel je Flaeche. "Von vorn" = Gier 60-120, Hoehe <= 20.
  Maulinneres = Flaeche, die mit offenem Maul von vorn >= 30 px zeigt UND in JEDER Zu-Pose < 30 % ihrer Offen-Sichtbarkeit
  (sie wird erst durch das Oeffnen sichtbar). Zaehne (material.json "Zahn", eigene Stuecke) bleiben Zone 41.
  Oberlippe (Zone 42, Modellgliederung) = Flaeche von Kopf 22, nicht Maulinneres, nicht Zahn, die eine Kante (gleiche Ecken im
  Knochenraum) mit einer Maulinneres- oder Zahnflaeche von Kopf 22 teilt UND auch bei geschlossenem Maul sichtbar ist (in jeder
  Zu-Pose >= 30 % der Offen-Sichtbarkeit und > 0 px), also der aeussere Rand der Maulöffnung.
Ausgabe: ARBEIT/maul_lage.json (Kieferwinkel, Sichtbarkeit je Flaeche, Mengen)."""
import json, sys
from collections import defaultdict
import numpy as np
from g_basis import *

ka = kand(ASSET)
OFFEN = '12'
ZU = ('5', '7', '2')
VERGLEICH_R3 = '0'  # Runde-3-"zu"; nur zum Vergleich gemessen
POSEN = {'12': (12, 0), '5': (5, 0), '7': (7, 0), '2': (2, 0), '0': (0, 0)}


def kieferwinkel(clip, bild):
    R, _ = ka.mo.pose(ka.kf(clip, bild))
    M = (R[22].astype(float).T @ R[23].astype(float)) / 4096.0 / 4096.0
    return float(np.degrees(np.arccos(np.clip((np.trace(M) - 1) / 2, -1, 1))))


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


def ecken(f):
    b, q, i = map(int, f.split('|')); mm = ka.mo.meshes[b]
    g = (mm.qf if q else mm.tf)[i]; vv = mm.qv if q else mm.tv
    L = [tuple(int(c) for c in vv[x][:3]) for x in g[1::2]]
    return [L[0], L[1], L[3], L[2]] if q else L


def kanten(f):
    L = ecken(f)
    return {frozenset((L[j], L[(j + 1) % len(L)])) for j in range(len(L))}


def main(ziel):
    ml = material_listen()
    S = sicht()
    alle = [f for f in alle_flaechen(ka.mo) if f.startswith(('22|', '23|'))]
    g = lambda p, k, f: S[p][k].get(f, 0)
    zu_anteil = lambda f: max(g(p, 'alle', f) for p in ZU) / max(g(OFFEN, 'alle', f), 1)
    innen = sorted(f for f in alle if g(OFFEN, 'vorn', f) >= 30 and all(g(p, 'alle', f) < 0.3 * g(OFFEN, 'alle', f) for p in ZU)
                   and f not in ml['Zahn'])
    # Oberlippe nach Lage: Rand der Maul-Oeffnung am Kopf 22
    rand_von = {f for f in alle if f.startswith('22|') and (f in innen or f in ml['Zahn'])}
    km = defaultdict(set)
    for f in alle:
        if f.startswith('22|'):
            for e in kanten(f):
                km[e].add(f)
    nachbar = set()
    for f in rand_von:
        for e in kanten(f):
            nachbar |= km[e]
    lippe = sorted(f for f in nachbar - rand_von
                   if all(g(p, 'alle', f) > 0 and g(p, 'alle', f) >= 0.3 * g(OFFEN, 'alle', f) for p in ZU))
    lippe_nicht_aussen = sorted(f for f in nachbar - rand_von if f not in lippe)
    # Runde-3-Regel zum Vergleich (Clip 2 und Clip 0 als "zu")
    r3 = sorted(f for f in alle if g(OFFEN, 'vorn', f) >= 30 and g('2', 'alle', f) < 0.3 * g(OFFEN, 'alle', f)
                and g('0', 'alle', f) < 0.3 * g(OFFEN, 'alle', f) and f not in ml['Zahn'])
    liste = sorted(ml['Maulinneres'])
    aussen_gross = sorted(f for f in liste if min(g(p, 'alle', f) for p in POSEN) >= 1000)
    kw = {p: round(kieferwinkel(*POSEN[p]), 2) for p in POSEN}
    grenz = sorted(f for f in alle if f not in innen and f not in ml['Zahn'] and g(OFFEN, 'alle', f) >= 200
                   and 0.3 <= zu_anteil(f) < 0.45)
    out = dict(regel=__doc__.strip(), kieferwinkel_grad=kw, offen=OFFEN, zu=list(ZU),
               maulinneres_lage=innen, oberlippe_lage=lippe,
               oberlippe_kandidaten_nicht_aussen=lippe_nicht_aussen,
               grenzfaelle_zu_anteil_30_bis_45=[dict(flaeche=f, offen=g(OFFEN, 'alle', f), zu_anteil=round(zu_anteil(f), 3)) for f in grenz],
               aenderung_gegen_runde3=dict(neu=sorted(set(innen) - set(r3)), heraus=sorted(set(r3) - set(innen))),
               farbliste_lippensaum_22=dict(flaechen=sorted(f for f in ml['Lippensaum'] if f.startswith('22|')),
                                           davon_maulinneres_lage=sorted(f for f in ml['Lippensaum'] if f in innen),
                                           davon_oberlippe_lage=sorted(f for f in ml['Lippensaum'] if f in lippe)),
               farbliste_maulinneres=dict(flaechen=liste, aussen_gross_sichtbar=aussen_gross,
                                          davon_im_maulinneren_nach_lage=sorted(set(liste) & set(innen))),
               sichtbarkeit={f: {p: [g(p, 'alle', f), g(p, 'vorn', f)] for p in POSEN}
                             for f in sorted(set(innen) | set(liste) | set(lippe) | set(r3) | set(lippe_nicht_aussen) | set(grenz))})
    json.dump(out, open(ziel, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('Kieferwinkel', kw)
    print('Maulinneres nach Lage', len(innen), innen)
    print('gegen Runde 3', out['aenderung_gegen_runde3'])
    print('Oberlippe nach Lage', len(lippe), lippe)
    print('Rand, aber nicht aussen', lippe_nicht_aussen)
    print('Grenzfaelle', out['grenzfaelle_zu_anteil_30_bis_45'])
    print('Farbliste aussen gross', aussen_gross)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/maul_lage.json')
