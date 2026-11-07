#!/usr/bin/env python3
"""Pruefer MESSUNG, Teil 7: Gegenproben. Was bleibt von den Zahlen, wenn der Trick weg ist?
 b_arm_c      B, linker Arm Pose 1 mit den Winkeln aus C (Arm steht frei am Boden)
 b_arm_ger    B, linker Arm Pose 1 mit den Winkeln aus gerippe.json
 a_sym, c_sym A, C: je Paar Links/Rechts gemitteltes Mesh (spiegelgleich), Skelett und Pose unveraendert
 c_hand_l     C, linke Hand = Spiegel der rechten (grosse Hand)
Schreibt je Probe EMD + form.json, Auswertung durch form_pruefung.pruefe_kandidat. Nur urteil_mass/."""
import copy, json, os, shutil, sys
import numpy as np
from scipy.optimize import linear_sum_assignment as lsa
from m1_skelett import F, W, G, NAME, lade
sys.path.insert(0, W + '/re15_port/tools')
import form_emd as FE
import form_pruefung as P

PAARE = {'a': [(8, 9), (24, 26), (25, 27), (11, 16), (12, 17), (13, 18), (14, 29), (15, 20), (28, 19), (30, 31)],     # A: l1 z+ <-> r3 z-, siehe m1: Index nicht gespiegelt
         'c': [(8, 9), (24, 26), (25, 27), (11, 16), (12, 17), (13, 18), (14, 19), (15, 20), (28, 29), (30, 31)]}
STRENG = None


def ecken(m):
    return np.array(m['ecken'], float)


def spiegel(v):
    return v * np.array([1, 1, -1.0])


def mitteln(f, l, r):
    ml, mr = f['meshes'][l], f['meshes'][r]
    vl, vr = ecken(ml), ecken(mr)
    if len(vl) != len(vr):
        return None
    mR = spiegel(vr)
    d = np.sqrt(((vl[:, None, :] - mR[None, :, :]) ** 2).sum(2))
    i, j = lsa(d)
    rest = d[i, j]
    neu = (vl[i] + mR[j]) / 2
    ml['ecken'] = [[int(round(c)) for c in p] for p in neu]             # i ist 0..n-1
    q = [None] * len(vr)
    for a, b in zip(i, j):
        q[b] = [int(round(c)) for c in spiegel(neu[a])]
    mr['ecken'] = q
    return float(np.median(rest)), float(rest.max())


def spiegel_mesh(f, ziel, quelle):
    """Mesh quelle gespiegelt auf Knochen ziel: Ecken, Normalen, Windung drehen."""
    m = copy.deepcopy(f['meshes'][quelle])
    m['ecken'] = [[c[0], c[1], -c[2]] for c in m['ecken']]
    m['normalen'] = [[c[0], c[1], -c[2]] for c in m['normalen']]
    f['meshes'][ziel] = m
    FE.orientiere(f)


def schreibe(f, x, name, aend=None):
    d = F + '/urteil_mass/probe_' + name
    os.makedirs(d, exist_ok=True)
    open(d + '/FORM.EMD', 'wb').write(FE.schreibe_emd(f, pruefen=False))
    j = json.load(open(F + '/urteil_mass/streng_%s/form.json' % x, encoding='utf-8'))
    j['emd'], j['tim'] = 'FORM.EMD', j['tim']
    j['name'] = name
    json.dump(j, open(d + '/form.json', 'w', encoding='utf-8'), indent=1)
    return d


def zeile(R, name):
    b1, b2 = R['bild1'], R['bild2']
    fl = {x['id']: x for x in b1['flaechen']}
    z = lambda l: '%d/%d' % (sum(1 for y in l if y['zaehlt'] and y['ok']), sum(1 for y in l if y['zaehlt']))    # noqa: E731
    T = R['teile']
    t = {}
    for n in ('bild1', 'bild2'):
        for q in T[n]['teile']:
            t[q['teil']] = q
    print('%-11s Umriss %.3f/%.3f Schwarz %d/%d L1 %d L2 %d SCH %d Punkte %s %s Flaechen %s %s | mIoU %.3f/%.3f | %s' % (
        name, b1['umriss']['iou'], b2['umriss']['iou'], b1['umriss']['schwarz_getroffen'], b2['umriss']['schwarz_getroffen'], fl['L1']['treffer'], fl['L2']['treffer'], fl['SCH']['treffer'],
        z(b1['punkte']), z(b2['punkte']), z(b1['flaechen']), z(b2['flaechen']), T['bild1']['iou_mittel'], T['bild2']['iou_mittel'],
        ' '.join('%s %.2f' % (k[:9], v['iou']) for k, v in t.items())))


def main():
    wahl = sys.argv[1:] or ['basis', 'b_arm_c', 'b_arm_ger', 'a_sym', 'c_sym', 'c_hand_l']
    for nm in wahl:
        if nm == 'basis':
            for x in 'abc':
                R, _ = P.pruefe_kandidat(F + '/urteil_mass/streng_%s/form.json' % x, F + '/urteil_mass/streng_%s/voll' % x)
                zeile(R, x + ' streng')
            continue
        if nm in ('b_arm_c', 'b_arm_ger'):
            _, f = lade('b')
            kf = f['clips'][0]['bilder'][0]['kf']
            if nm == 'b_arm_c':
                _, fc = lade('c')
                wc = fc['pool'][fc['clips'][0]['bilder'][0]['kf']]['winkel']
            else:
                wc = G['posen']['bild1']['winkel']
            for i in (11, 12, 13):
                f['pool'][kf]['winkel'][i] = list(wc[i])
            d = schreibe(f, 'b', nm)
        elif nm in ('a_sym', 'c_sym'):
            x = nm[0]
            _, f = lade(x)
            for l, r in PAARE[x]:
                e = mitteln(f, l, r)
                print('  Paar %s/%s: %s' % (NAME[l], NAME[r], 'Topologie verschieden, bleibt' if e is None else 'Zuordnung Median %.0f, max %.0f' % e))
            d = schreibe(f, x, nm)
        elif nm == 'c_hand_l':
            _, f = lade('c')
            for l, r in PAARE['c']:
                if l in (13, 14, 15, 28, 30):
                    spiegel_mesh(f, l, r)
            d = schreibe(f, 'c', nm)
        R, _ = P.pruefe_kandidat(d + '/form.json', d + '/lauf')
        zeile(R, nm)


if __name__ == '__main__':
    main()
