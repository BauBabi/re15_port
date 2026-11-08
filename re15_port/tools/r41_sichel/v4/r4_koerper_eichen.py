"""SF V4 Runde 4 (Bauer): Nachstellung der Koerperfarben an den Riegeln F6/F7/F8 und den FS-Boxen (Lab-Versatz je Region/Gruppe).

    python r4_koerper_eichen.py <basis.EMD> [--runden 3]  -> r4_koerper_korrektur.json + farben_koerper_v4.npz (r2_koerper_farben.py)

Anlass: mit den Kacheln (r2_koerper_farben.py 'oberkoerper', Pruefung V4 r3 Sicht 1.5) traf der Fit die Ziele je Kachel, die Messblatt-
Fenster aber nicht (Lauf r4 Koerper 6: F6 dL +5,2, F8 +4,5, Unterschenkel Rotanteil 0,30). Je Runde: Fit (r2_koerper_farben.main), Bau
(birkin_formen V4), Messung (riegel_v4.koerper_farben), Versatz += 0,8 x (Ziel - Ist) in Lab. Ziele: Messblatt V4 F6/F7/F8 (riegel_v4.ZIEL,
F7/F8 an der Fensterlage) und die Sicht-Boxen der FS-Zeilen (Mittel der Original-Box, 2 px erodiert, wie riegel_v4.koerper_farben).
Gruppen: F7 = Id 13 in den Kacheln, F8 = Id 10 in den Kacheln; oberarm_bl_unten,
oberarm_br_unten, beine = Regionen von r2_koerper_farben. Faktor 0,8 (gedaempft, damit geteilte Texel nicht ueberschwingen); PORT-WAHL.
"""
import os, sys, json, argparse, importlib
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import riegel_v4 as RV

AUS = os.path.join(HIER, 'r4_koerper_korrektur.json')
# V4 Runde 4: F6 wird NICHT nachgestellt. F6 misst alle roten Bildpunkte (R - G >= 15) von Id 8 gegen die roten Bildpunkte des Fensters
# 372-424 x 197-232 (16,7 / 17,0 / 6,1); im Original haben die roten Bildpunkte des ganzen Oberkoerpers (Polygon + Rumpfbox) 19,6 / 11,1 /
# 3,1 - wer F6 nach Wortlaut trifft, faerbt den ganzen Rumpf so rot wie die Brustmitte (Lauf r4 Koerper-Eichung 1: a* +11,5, Rot Mitte/
# aussen 1,81, Schultern a*>12 0,24). Nur L* wird verschoben (Lauf r4 Koerper-Eichung 2 mit Lab: F8 kippte nach b* -3,5).
GRUPPEN = dict(F7=('F7', None), F8=('F8', None),
               oberarm_bl_unten=('FS_oa_bl_u', (300, 213, 345, 250)), oberarm_br_unten=('FS_oa_br_u', (455, 228, 500, 262)),
               beine=('FS_beine', (356, 352, 392, 430)))
DAEMPFUNG = 0.8
KAPPE = 6.0       # hoechster Lab-Versatz je Kanal (PORT-WAHL; Lauf r4 Koerper-Eichung 1 brauchte bis 12 und verschob die Verteilung)


def ziel_box(box):
    from scipy import ndimage as nd
    o1 = G.vorlage(1)
    b_ = np.zeros((480, 640), bool); b_[box[1]:box[3] + 1, box[0]:box[2] + 1] = True
    return RV.lab1(o1[nd.binary_erosion(b_, iterations=2)].mean(0))


def main():
    ap = argparse.ArgumentParser(); ap.add_argument('basis'); ap.add_argument('--runden', type=int, default=3)
    ap.add_argument('--arbeit', default=os.path.join(G.SFV4, 'r4', 'koerper_eichen'))
    a = ap.parse_args()
    os.makedirs(a.arbeit, exist_ok=True)
    korr = {k: [0.0, 0.0, 0.0] for k in GRUPPEN}
    ziele = {k: (np.array(RV.ZIEL[v[0]], float) if v[1] is None else ziel_box(v[1])) for k, v in GRUPPEN.items()}
    prot = dict(ziele={k: [round(float(x), 2) for x in v] for k, v in ziele.items()}, runden=[])
    kd = os.path.join(a.arbeit, 'korrektur.json')
    for r in range(a.runden + 1):
        json.dump(dict(korrektur=korr), open(kd, 'w'))
        os.environ['RE15_R4_KORR'] = kd
        import r2_koerper_farben as RK
        importlib.reload(RK)
        sys.argv = ['r2_koerper_farben.py', '--emd', a.basis]
        RK.main()
        import r2_expt as X, birkin_formen as BF
        p = os.path.join(a.arbeit, 'runde%d.EMD' % r)
        open(p, 'wb').write(X.bauen(farben=BF.V4_FARBEN if os.path.exists(BF.V4_FARBEN) else None))
        mo = b2.modell(p)
        _Z, erg, _ = RV.koerper_farben(mo, G.posen())
        ist = {k: np.asarray(erg[v[0]][0], float) for k, v in GRUPPEN.items()}
        zeile = dict(runde=r, korrektur={k: [round(x, 2) for x in v] for k, v in korr.items()},
                     ist={k: [round(float(x), 2) for x in v] for k, v in ist.items()},
                     de00={k: round(G.de00(ist[k], ziele[k]), 2) for k in GRUPPEN})
        prot['runden'].append(zeile)
        print('Runde', r, json.dumps(zeile), flush=True)
        if r == a.runden:
            break
        for k in GRUPPEN:
            # Gruppen, deren Riegel mit Abstand erfuellt ist (dE00 <= 3, |dL| <= 2,5), bleiben stehen: F6 misst ALLE roten Bildpunkte von
            # Id 8 gegen das rote Fenster - weiteres Nachschieben macht mehr Bildpunkte rot und verschiebt die Rot-Verteilung (Lauf r4
            # Koerper-Eichung 1: a* +11,5, Rotanteil Rumpf 0,58 statt 0,31). Versatz je Kanal hoechstens +-KAPPE.
            if zeile['de00'][k] <= 3.0 and abs(ist[k][0] - ziele[k][0]) <= 2.5:
                continue
            korr[k] = [float(np.clip(korr[k][0] + DAEMPFUNG * (ziele[k][0] - ist[k][0]), -KAPPE, KAPPE)), 0.0, 0.0]
    # letzter Stand: die Korrektur, mit der farben_koerper_v4.npz zuletzt erzeugt wurde
    json.dump(dict(korrektur=prot['runden'][-1]['korrektur'], quelle='r4_koerper_eichen.py, Basis %s' % os.path.basename(a.basis),
                   protokoll=prot), open(AUS, 'w'), indent=1)
    print('geschrieben', AUS)


if __name__ == '__main__':
    main()
