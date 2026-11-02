#!/usr/bin/env python3
"""Variante K, Folgen fuer Stufe 3 (gemessen, nicht umgesetzt): wie liegt der neue Kopf in den 28 Clips der Stufe 3 (Kopfwinkel des alten Kopfes)?

    python k_s3_kopf.py AUS.json "J=stufe3/asset/BAHNSTEIG_FORM.EMD" "K3=../_lauf/K3_s3.EMD" ...

Je Clip und Bild (Aktorraum, Y unten, Vorderseite +X):
 - boden: tiefster Punkt von Kopf+Kiefer (Knochen 22, 23) gegen den tiefsten Punkt der ganzen Figur (Boden der Pose); > 0 = Kopf steckt im Boden
 - schnauze_x: vorderste Ecke von Kopf/Kiefer; laenge: Ausdehnung der ganzen Figur in X (Kollision, Schatten, Kamera)
 - biss: Abstand der Kieferspitze (vorderste Ecke von 23) zum Bisspunkt des Ticks, Teil 10 + (1400, 0, 0) im Teilraum, Radius 1500
   (birkin_bahnsteig.c bb_biss: bb_nah(e, pl, BB_TEIL_RUMPF, BB_L(1400), 0, BB_L(1500)))
Kein RE-Wert."""
import json
import sys

import numpy as np

import k_kern as KK
import k_loecher as KL

BISS_PUNKT = np.array([1400.0, 0, 0])
BISS_RADIUS = 1500.0


def messen(emd):
    fig = KL.Figur(emd)
    clips = json.load(open(KK.os.path.join(KK.FORM, 'stufe3', 'asset', 'clips.json'), encoding='utf-8'))
    out = {}
    for c in range(len(fig.clips)):
        zeilen = []
        for b in range(fig.clips[c][1]):
            R, T = fig.welt(fig.kf(c, b))
            alle = np.vstack([V @ R[i].T + T[i] for i, (V, F) in enumerate(fig.faces) if len(V)])
            kopf = np.vstack([fig.faces[i][0] @ R[i].T + T[i] for i in (22, 23)])
            kiefer = fig.faces[23][0] @ R[23].T + T[23]
            spitze = kiefer[np.argmax(kiefer[:, 0])]
            bp = R[10] @ BISS_PUNKT + T[10]
            zeilen.append(dict(bild=b, boden=float(kopf[:, 1].max() - alle[:, 1].max()), kopf_unten=float(kopf[:, 1].max()), figur_unten=float(alle[:, 1].max()),
                               schnauze_x=float(kopf[:, 0].max()), laenge=float(alle[:, 0].max() - alle[:, 0].min()),
                               biss=float(np.linalg.norm(spitze - bp)), spitze=spitze.round().tolist(), bisspunkt=bp.round().tolist()))
        nm = clips[c]['name'] if c < len(clips) else str(c)
        z = zeilen
        out[c] = dict(name=nm, bilder=len(z), kopf_im_boden_bilder=sum(1 for x in z if x['boden'] > 30), kopf_tiefer_als_fuesse_max=round(max(x['boden'] for x in z)),
                      schnauze_x_max=round(max(x['schnauze_x'] for x in z)), laenge_max=round(max(x['laenge'] for x in z)),
                      biss_min=round(min(x['biss'] for x in z)), biss_max=round(max(x['biss'] for x in z)))
    return out


if __name__ == '__main__':
    R = {}
    for sp in sys.argv[2:]:
        nm, p = sp.split('=', 1)
        R[nm] = messen(p)
        print('==', nm)
        for c, v in R[nm].items():
            if v['bilder'] > 1 or c in (1,):
                print('  %2d %-12s %3d Bilder: Kopf unter Fussebene in %3d Bildern (max %+5d), Schnauze x max %5d, Laenge max %5d, Kieferspitze-Bisspunkt %4d..%4d'
                      % (c, v['name'], v['bilder'], v['kopf_im_boden_bilder'], v['kopf_tiefer_als_fuesse_max'], v['schnauze_x_max'], v['laenge_max'], v['biss_min'], v['biss_max']))
    json.dump(R, open(sys.argv[1], 'w', encoding='utf-8'), indent=1)
