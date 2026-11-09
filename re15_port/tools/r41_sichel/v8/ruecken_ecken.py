"""SF V8 Runde 2: einzelne Ecken von Mesh 8 auf einen Bildpunkt in PV setzen -> ruecken_v8.json (Messblatt V8 6b).

    python ruecken_ecken.py KETTE_JSON RUECKEN_START AUS ECKE:X:Y[,ECKE:X:Y...] [--basis EMD_V7]

Anlass (Pruefung V8 r1: S3 rot, Huegel H 3,0 bei x 423 statt 5-10 bei x 428-445, Sattel Ruecken -> Kopf 2,3): die Koordinatensuche
(ruecken_suche.py --riegel) blieb an einer Stufe 187 -> 192 bei x 429/430 haengen (Lauf sfv8/r2 rs2/rs3). Hier werden die
Silhouetten-Ecken am Huegel gezielt gesetzt: kleinste Verschiebung im Knochenraum 8, deren Bild in PV (Spielkamera 50A0/12) genau den
Zielpunkt trifft (Jacobi der Projektion, 2 x 3, Pseudoinverse). Die Zielpunkte sind PORT-WAHL und stehen mit ihrer Herleitung im Dossier
(Abschnitt "V8 Runde 2"): Kuppel = obere Huelle der Vorlage-Oberkontur x 400-447 (m13b_oben.json), Huegel "Little red hill from the back"
mit groesster Hoehe ueber der Sehne x 405 -> 447 bei x ~435, glatter Anschluss an den Kopf (Mesh 9 bleibt bytegleich, Nutzer "jetzt schon
gut"; "smooth straight to head without further hills....").
Kein RE-Wert.
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V        # noqa: E402
import v8_bau as VB          # noqa: E402
import kopf_orig_bau as KB   # noqa: E402
import ruecken_v8 as RV8     # noqa: E402
import riegel_v8 as RG       # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('kette'); ap.add_argument('start'); ap.add_argument('aus'); ap.add_argument('ziele')
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    a = ap.parse_args()
    kette, ru = json.load(open(a.kette)), json.load(open(a.start))
    ziele = [(int(z.split(':')[0]), float(z.split(':')[1]), float(z.split(':')[2])) for z in a.ziele.split(',')]
    blob, _ = VB.bauen(a.basis, 's', kette=kette)
    d, _o, ms, tim = KB.teile(blob)
    RV8.ruecken(ms, ru, {})
    blob2 = KB.packen(blob, d, ms, tim)
    _d2, _o2, ms2, _t2 = KB.teile(blob2)
    mo = V.modell(blob2)
    R, T = V.pose_r_t(mo)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    P = np.array([v[:3] for v in ms2[8]['v']], float)
    neu = dict(ru['ecken'])
    prot = []
    for i, zx, zy in ziele:
        q = V.proj(V.PV, P[i] @ R8.T + T8)[0]
        J = np.zeros((2, 3))
        for k in range(3):
            e = np.zeros(3); e[k] = 5.0
            J[:, k] = (V.proj(V.PV, (P[i] + e) @ R8.T + T8)[0] - V.proj(V.PV, (P[i] - e) @ R8.T + T8)[0]) / 10.0
        dP = J.T @ np.linalg.solve(J @ J.T, np.array([zx - q[0], zy - q[1]]))
        alt = neu.get(str(i), [0, 0, 0])
        neu[str(i)] = [int(round(alt[k] + dP[k])) for k in range(3)]
        prot.append(dict(ecke=i, bild_vorher=[round(float(c), 1) for c in q], ziel=[zx, zy], verschiebung=[int(round(c)) for c in dP]))
        print('Ecke %d: Bild (%.1f, %.1f) -> (%.1f, %.1f), Verschiebung %s' % (i, q[0], q[1], zx, zy, np.round(dP).astype(int)))
    out = dict(ru)
    out['ecken'] = {k: neu[k] for k in sorted(neu, key=int)}
    out['quelle'] = '%s; ruecken_ecken.py (Kette %s): %s' % (ru.get('quelle', ''), os.path.basename(a.kette), a.ziele)
    out['ecken_ziele'] = (ru.get('ecken_ziele') or []) + prot
    b2, _ = VB.bauen(a.basis, 'sr', kette=kette, ruecken=out)
    mo2 = V.modell(b2)
    r = V.b2.umriss_schnell(mo2, V.PV, knochen=list(range(23)))
    r2, r3, _om = RG.s2_s3(mo2, np.where(r, 0, -1))
    out['riegel'] = dict(S2=r2, S3=r3)
    json.dump(out, open(a.aus, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('S2', r2); print('S3', r3)


if __name__ == '__main__':
    main()
