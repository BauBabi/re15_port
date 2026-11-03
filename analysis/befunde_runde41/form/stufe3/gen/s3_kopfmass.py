"""Nachbesserung 1, D: Masse des K3-Kopfs fuer den Tick (nur messen, kein RE-Wert).

    python s3_kopfmass.py [EMD] [--json aus.json]

- Kieferspitze: vorderste Ecke des Kiefer-Meshes (Knochen 23) im Raum von Knochen 23 (fest) und je Bild in Clip 26 (Biss) im Aktorraum und
  im Raum von Teil 10 (BB_TEIL_RUMPF), dazu der heutige Bisspunkt des Ticks (Teil 10 + (1400, 0, 0), birkin_bahnsteig.c bb_biss) und die
  Mitte der Zahnreihe (Kiefer-Mesh x-Mitte der Zaehne).
- Ausdehnung je Clip im Aktorraum (Gier 0, Vorderseite +X, Y unten): vorn (max x), hinten (min x), seitlich (max |z|), Schnauze vorn.
"""
import os, sys, json, argparse, math
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402

TICK = (0, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 26, 27)


def main():
    a = argparse.ArgumentParser()
    a.add_argument('emd', nargs='?', default=os.path.join(HIER, '..', 'asset', 'BAHNSTEIG_FORM.EMD'))
    a.add_argument('--json')
    g = a.parse_args()
    f = FE.lese_emd(open(g.emd, 'rb').read())
    r = S.Rig()
    V23 = r.ecken[23]
    i_tip = int(np.argmax(V23[:, 0]))
    tip = V23[i_tip]
    # Zahnreihe: Ecken des Kiefers oberhalb des Kinnbodens (y kleiner als Mitte), x-Mitte der vorderen Haelfte
    zahn_mitte = np.array([float(np.median(V23[V23[:, 0] > 0.5 * V23[:, 0].max(), 0])), float(tip[1]), 0.0])
    out = dict(kiefer_spitze_knochen23=tip.round().tolist(), kiefer_x_bereich=[float(V23[:, 0].min()), float(V23[:, 0].max())],
               kiefer_mitte_vorn_knochen23=zahn_mitte.round().tolist(), biss=[], clips={})
    c = f['clips'][26]['bilder']
    for b in range(len(c)):
        k = f['pool'][c[b]['kf']]
        R, T = r.fk(r.aus_keyframe(k))
        spitze = R[23] @ tip + T[23]
        mitte = R[23] @ zahn_mitte + T[23]
        alt = R[10] @ np.array([1400.0, 0, 0]) + T[10]
        im10 = R[10].T @ (spitze - T[10])
        out['biss'].append(dict(bild=b, treffer=bool(c[b]['wort'] & FE.BILD_TREFFER), spitze=spitze.round().tolist(), zahnmitte=mitte.round().tolist(),
                                spitze_in_teil10=im10.round().tolist(), bisspunkt_alt=alt.round().tolist(),
                                spitze_minus_alt_xz=round(float(math.hypot(*(spitze - alt)[[0, 2]])), 0)))
    for ci in TICK:
        cl = f['clips'][ci]['bilder']
        vorn, hinten, seit, schn = -1e9, 1e9, 0, -1e9
        for b in cl:
            k = f['pool'][b['kf']]
            R, T = r.fk(r.aus_keyframe(k))
            P = r.punkte(r.aus_keyframe(k), R, T)
            A = np.vstack([x for x in P if len(x)])
            vorn, hinten, seit = max(vorn, A[:, 0].max()), min(hinten, A[:, 0].min()), max(seit, np.abs(A[:, 2]).max())
            schn = max(schn, max(P[22][:, 0].max(), P[23][:, 0].max()))
        out['clips'][ci] = dict(bilder=len(cl), vorn=round(vorn), hinten=round(hinten), seitlich=round(seit), schnauze_vorn=round(schn))
    print('Kieferspitze im Raum von Knochen 23:', out['kiefer_spitze_knochen23'], ' Kiefer x', out['kiefer_x_bereich'],
          ' Mitte vordere Zahnreihe', out['kiefer_mitte_vorn_knochen23'])
    print('Biss Clip 26 (Aktorraum, Wurzel = Huefte):')
    for e in out['biss']:
        if 12 <= e['bild'] <= 32:
            print('  Bild %2d %s Spitze %s  in Teil 10 %s  Zahnmitte %s  alter Bisspunkt %s  Abstand %d' % (
                e['bild'], 'T' if e['treffer'] else ' ', e['spitze'], e['spitze_in_teil10'], e['zahnmitte'], e['bisspunkt_alt'], e['spitze_minus_alt_xz']))
    print('Ausdehnung je Clip (Aktorraum): vorn / hinten / seitlich / Schnauze vorn')
    for ci, v in out['clips'].items():
        print('  Clip %2d: %5d %6d %5d %5d' % (ci, v['vorn'], v['hinten'], v['seitlich'], v['schnauze_vorn']))
    if g.json:
        json.dump(out, open(g.json, 'w', encoding='utf-8'), indent=1)


if __name__ == '__main__':
    main()
