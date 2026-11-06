"""SF V4 (Bauer): Schwanzgelenke und -durchmesser an die Riegel T1-T6 nachstellen (Messblatt V4 Abschnitt 4 / 7).

    python form_suche_v4.py [--runden 6]   -> druckt je Runde T1-T6 und die nachgestellten SCHWANZ_V4 gelenke/durchmesser

Verfahren: Bau mit den aktuellen Werten (birkin_formen.baue, Variante V4), Render Pose "85" (Ids), Messung riegel_v4.schwanz_mass_h.
Korrektur im Bild (Einheit H, Bezug = geplanter Beginn): Fehler des gemessenen Beginns (T1) -> Gelenke 0-1 voll, Gelenk 2 halb;
Biegung (T2) -> Gelenk 3; Knick (T3) -> Gelenk 5; Ende (T4, inkl. Dorn) -> Gelenk 7 (Gelenk 4/6 auf halbem Weg mitgezogen).
Durchmesser: f 0,7 -> Gelenk 2, f 0,5 -> Gelenk 3, f 0,3 -> Gelenk 4 (Verhaeltnis Ziel / gemessen); der Knick bleibt die Taille.
Die Werte werden danach von Hand in SCHWANZ_V4 uebernommen (Quelle: Ausgabe dieses Laufs, Dossier V4).
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import riegel_v4 as RV

ZIEL = dict(T1=(0.15, 0.767), T2=(-0.245, 0.036), T3=(-0.265, 0.257), T4=(-0.245, 0.397))
ZB = {0.7: 0.14, 0.5: 0.125, 0.3: 0.065}


def messen(sw):
    BF.V4_FARBEN = os.path.join(G.SFV4, "nicht_da.npz")
    BF.SCHWANZ_V4.update(sw)
    BF.BAU_INFO.clear()
    blob = BF.baue(BF.lade_em30(), BF._variante_v4())
    info = BF.BAU_INFO['schwanz_v2']
    mo = b2.modell(blob)
    img, ids, iv = G.zeichne(mo, G.posen()['85'], voll=True)
    ok, so, H = G.h_port(iv)
    return RV.schwanz_mass_h(img, ids, ok, so), info


def main():
    ap = argparse.ArgumentParser(); ap.add_argument('--runden', type=int, default=6)
    a = ap.parse_args()
    sw = dict(gelenke=[list(x) for x in BF.SCHWANZ_V4['gelenke']], durchmesser=list(BF.SCHWANZ_V4['durchmesser']))
    # Gelenk-Korrektur in geplanten Koordinaten: der Plan-Beginn (SCHWANZ_V4 beginn) und der gemessene Beginn fallen bis auf wenige px
    # zusammen; die Fehler unten werden gegen den gemessenen Beginn gerechnet und direkt auf die Plan-Gelenke addiert.
    for r in range(a.runden):
        s, info = messen(dict(gelenke=[tuple(x) for x in sw['gelenke']], durchmesser=sw['durchmesser']))
        H = s['H']
        print('Runde %d: T1 %.3f/%.3f T2 %.3f/%.3f (%.0f Grad) T3 %.3f/%.3f T4 %.3f/%.3f | f0,1 %.3f f0,3 %.3f f0,5 %.3f f0,7 %.3f | '
              'Taille %.3f Ende B %.3f L %.3f %.0f Grad' % (r, s['T1'][0], s['T1'][1], *s['T2'], s['drehung'], *s['T3'], *s['T4'],
                                                          s['T5'][0.1], s['T5'][0.3], s['T5'][0.5], s['T5'][0.7], s['taille'],
                                                          s['end_breite'], s['end_laenge'], s['end_richtung']), flush=True)
        print('   gelenke', [tuple(round(v, 3) for v in x) for x in sw['gelenke']], 'durchmesser', [round(v, 3) for v in sw['durchmesser']])
        # Bezug = GEMESSENER Beginn (T2-T4 sind relativ zum Beginn definiert); die Wurzel (Gelenke 0-1) bleibt, weil der sichtbare Beginn
        # an der Rueckenkontur der Figur liegt (siehe SCHWANZ_V4 beginn); Gelenk 2 = Mitte zwischen Gelenk 1 und der Biegung
        bx, by = s['beginn']
        J = np.array(sw['gelenke'], float)
        def fehler(pkt, ziel):
            ist = np.array([(pkt[0] - bx) / H, (pkt[1] - by) / H])
            return np.array(ziel) - ist
        J[3] += fehler(s['biegung'], ZIEL['T2'])
        J[5] += fehler(s['knick'], ZIEL['T3'])
        J[7] += fehler(s['ende'], ZIEL['T4'])
        J[2] = 0.5 * (J[1] + J[3])
        J[4] = 0.5 * (J[3] + J[5]) + np.array([-0.012, 0.0])
        J[6] = 0.5 * (J[5] + J[7])
        sw['gelenke'] = [list(x) for x in J]
        d = sw['durchmesser']
        for f, j in ((0.7, 2), (0.5, 3), (0.3, 4)):
            if s['T5'][f] > 1e-3:
                d[j] = float(np.clip(d[j] * (ZB[f] / s['T5'][f]) ** 0.7, 0.02, 0.22))
        sw['durchmesser'] = d
    print(json.dumps(sw))


if __name__ == '__main__':
    main()
