#!/usr/bin/env python3
"""Alle Laeufe zusammenlegen: Kosten, Restfehler, Streuung der Masse. Schreibt lauf/vergleich.json.

    python vergleich.py lauf/b*.pt --schwelle 1.15
"""
import glob, json, math, os, sys
import numpy as np, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402
import ziele as ZI    # noqa: E402
import ansicht as AN  # noqa: E402

GER = FT.GER


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    schwelle = 1.2
    if '--schwelle' in sys.argv:
        schwelle = float(sys.argv[sys.argv.index('--schwelle') + 1])
    args = [a for a in args if a != str(schwelle)]
    dateien = []
    for a in args:
        dateien += glob.glob(os.path.join(HIER, a))
    ger = RG.Gerippe(GER, FT.DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    _, m = ZI.lade()
    zeilen = []
    for pf in sorted(dateien):
        d = torch.load(pf, weights_only=False)
        kosten = d['kosten'].numpy()
        z = d['zustand']
        for i in range(d['B']):
            zeilen.append((float(kosten[i]), os.path.basename(pf), i, d['meta'].get('opt', {}).get('hand2')))
    zeilen.sort()
    print('Starts', len(zeilen), 'beste', zeilen[0][:3])
    for q in zeilen[:30]:
        print('  %.2f %s %d %s' % q)
    gute = [z for z in zeilen if z[0] <= zeilen[0][0] * schwelle]
    print('Innerhalb Faktor', schwelle, ':', len(gute))
    werte = []
    for kost, datei, i, hand2 in gute[:40]:
        par, ids, d = FT.lade(os.path.join(HIER, 'lauf', datei), wahl=[i])
        with torch.no_grad():
            lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
        L, V, D = FT.als_dict(lae, ver, a3, b3, 0)
        e = dict(kosten=kost, datei=datei, i=i, hand2=hand2, L=L, V=V, D=D)
        for q in range(2):
            T = st[q]['T'][0].cpu().numpy()
            R = st[q]['R'][0].cpu().numpy()
            w = T[0]
            bi = bilder[q]
            e['p%d' % (q + 1)] = dict(wurzel=[float(x) for x in w], gier=math.degrees(float(par.gier[q][0])) % 360,
                                      tiefe=float((w - bi.kam['auge']) @ bi.kam['R'][2]),
                                      nick=math.degrees(float(par.ang[q][0, 0, 2])),
                                      kopf=[float(x) for x in T[RG.IDX['kopf']]],
                                      hand_r=[float(x) for x in T[RG.IDX['hand_r']]], hand_l=[float(x) for x in T[RG.IDX['hand_l']]],
                                      fuss_r=[float(x) for x in T[RG.IDX['fuss_r']]], fuss_l=[float(x) for x in T[RG.IDX['fuss_l']]],
                                      top=float((st[q]['c'][0, :, 1] - st[q]['ry'][0]).min()))
        erg, _, _ = AN.tabelle(par, ger, bilder, 0, m)
        e['rest'] = {str(k): v for k, v in erg.items()}
        werte.append(e)
    json.dump(werte, open(os.path.join(HIER, 'lauf', 'vergleich.json'), 'w', encoding='utf-8'), indent=0)

    print('Tabelle gute Starts: Kosten Datei Start Hand2 | Gier1 Gier2 Tiefe1 Tiefe2 Nick1 Nick2 | Kopf Bild2 (x,y) Hoehe | Hand_r Hoehe Bild2 | Fuesse Bild2 Hoehe')
    for e in werte:
        p1, p2 = e['p1'], e['p2']
        kam = bilder[1].kam
        k = FT.FK.projiziere(kam, np.array(p2['kopf']))
        print('%6.2f %-8s %d %-5s | %4.0f %4.0f %5.0f %5.0f %4.0f %4.0f | (%3.0f,%3.0f) %5.0f | %5.0f | %5.0f %5.0f' % (
            e['kosten'], e['datei'][:-3], e['i'], e['hand2'], p1['gier'], p2['gier'], p1['tiefe'], p2['tiefe'], p1['nick'], p2['nick'], k[0], k[1], -p2['kopf'][1],
            -p2['hand_r'][1], -p2['fuss_l'][1], -p2['fuss_r'][1]))

    def spanne(f):
        x = np.array([f(e) for e in werte])
        return '%8.0f %8.0f %8.0f  rel %4.0f%%' % (x.min(), np.median(x), x.max(), 100 * (x.max() - x.min()) / max(1, abs(np.median(x))))
    print('Spanne ueber', len(werte), 'gute Starts (min, Median, max)')
    for k in RG.L_KEYS:
        print(' L %-16s' % k, spanne(lambda e: e['L'][k]))
    print(' L %-16s' % 'schwanz_summe', spanne(lambda e: sum(e['L']['schwanz%d' % q] for q in range(1, 7))))
    print(' L %-16s' % 'arm_summe', spanne(lambda e: e['L']['oberarm'] + e['L']['unterarm']))
    print(' L %-16s' % 'bein_summe', spanne(lambda e: e['L']['oberschenkel'] + e['L']['unterschenkel']))
    print(' L %-16s' % 'rumpf_summe', spanne(lambda e: e['L']['huefte'] + e['L']['wirbel'] + e['L']['brust'] + e['V']['huefte_hinten']))
    for k in RG.V_KEYS:
        print(' V %-16s' % k, spanne(lambda e: e['V'][k] * (100 if k == 'klaue_z' else 1)))
    for k in RG.D_KEYS:
        print(' D %-16s' % k, 'a0', spanne(lambda e: e['D'][k][0][0]), 'a2', spanne(lambda e: e['D'][k][0][2]), 'bm', spanne(lambda e: e['D'][k][1][1]))
    for p in ('p1', 'p2'):
        print(p, 'Gier', spanne(lambda e: e[p]['gier']), 'Tiefe', spanne(lambda e: e[p]['tiefe']), 'Nick', spanne(lambda e: e[p]['nick']), 'Scheitel', spanne(lambda e: -e[p]['top']))
    print('Restfehler (IoU je Teil), gute Starts:')
    for nr in ('1', '2'):
        for n in ZI.TEILE[int(nr)]:
            x = np.array([e['rest'][nr][n]['iou'] for e in werte])
            print('  ', nr, n, 'IoU %.2f %.2f %.2f' % (x.min(), np.median(x), x.max()))


if __name__ == '__main__':
    main()
