"""SF V4 Runde 2 (Bauer): Schwanzgelenke an T1-T6 UND an die absoluten Nutzerpunkte (Pruefung V4 r1 Zahlen 4.1) nachstellen.

    python r2_form_suche.py [--runden 6] [--start JSON]

Ziel je Punkt (Bildpunkte der Kamera von Vorlage 2, Pose "85"): Mitte aus
  relativ  = Messblatt V4 Abschnitt 4/7 im Port-Mass (Beginn = Scheitel - 0,15 H, Sohle - 0,767 H; Biegung/Knick/Ende = Beginn +
             (-0,245, 0,036) / (-0,265, 0,257) / (-0,245, 0,397) H) und
  absolut  = Nutzer-Linienenden (Messblatt Abschnitt 1a): Beginn (357,243), Knick (290,308), Ende (295,343); Biegung ~(295,252)
             (Abschnitt 4 Zeile "Biegung", Leser A (295,250)).
Grund: Port-Figur und Video-Figur sind verschieden gross (H 264 gegen 253, Sohle 434 gegen 436); relativ und absolut liegen bis
0,044 H auseinander, die Mitte erfuellt beide Riegel (je +-0,03 H). Korrektur: Beginn -> Gelenk 1 (Ende von Id 16), Biegung ->
Gelenk 3, Knick -> Gelenk 5, Ende -> Gelenk 7; Gelenke 2/4/6 auf halbem Weg; Durchmesser f 0,7/0,5/0,3 -> Gelenke 2/3/4.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import r2_expt as X

NUTZER = dict(beginn=(357, 243), biegung=(295, 252), knick=(290, 308), ende=(295, 343))
REL = dict(biegung=(-0.245, 0.036), knick=(-0.265, 0.257), ende=(-0.245, 0.397))
ZB = {0.7: 0.14, 0.5: 0.125, 0.3: 0.065}


def ziele(s):
    H, sch, so = s['H'], s['scheitel'], s['sohle']
    b_rel = np.array([sch - 0.15 * H, so - 0.767 * H])
    z = dict(beginn=0.5 * (b_rel + np.array(NUTZER['beginn'])))
    for k, (dx, dy) in REL.items():
        z[k] = 0.5 * (b_rel + np.array([dx * H, dy * H]) + np.array(NUTZER[k]))
    return z


def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--runden', type=int, default=6); ap.add_argument('--start')
    ap.add_argument('--fest', default='')
    a = ap.parse_args()
    sw = json.loads(a.start)
    fest = set(int(x) for x in a.fest.split(',') if x)
    for r in range(a.runden):
        Z, s = X.schwanz_messen(X.bauen(sw), 'fs%d' % r)
        H = s['H']
        z = ziele(s)
        print('Runde %d:' % r, ' | '.join('%s %s' % (q['mass'].split()[0], q['ist']) for q in Z if q['mass'][:2] in ('T1', 'T2', 'T3', 'T4', 'TA', 'T5', 'T6')), flush=True)
        print('   gelenke', json.dumps([[round(v, 3) for v in x] for x in sw['gelenke']]), 'durchmesser', [round(v, 4) for v in sw['durchmesser']], flush=True)
        J = np.array(sw['gelenke'], float)
        f = {k: (z[k] - np.asarray(s[k], float)) / H for k in z}
        print('   Fehler (H)', {k: [round(float(v), 3) for v in f[k]] for k in f}, flush=True)
        for k, j in (('beginn', 1), ('biegung', 3), ('knick', 5), ('ende', 7)):
            if j not in fest:
                J[j] += f[k]
        if 2 not in fest: J[2] = 0.5 * (J[1] + J[3]) + np.array([0.0, 0.0])
        if 4 not in fest: J[4] = 0.5 * (J[3] + J[5])
        if 6 not in fest: J[6] = 0.5 * (J[5] + J[7])
        sw['gelenke'] = [list(map(float, x)) for x in J]
        d = sw['durchmesser']
        for ff, j in ((0.7, 2), (0.5, 3), (0.3, 4)):
            if s['T5'][ff] > 1e-3:
                d[j] = float(np.clip(d[j] * (ZB[ff] / s['T5'][ff]) ** 0.7, 0.02, 0.22))
        sw['durchmesser'] = d
    print(json.dumps(sw))


if __name__ == '__main__':
    main()
