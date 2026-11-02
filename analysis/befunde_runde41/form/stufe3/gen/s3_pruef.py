"""Stufe 3: Pruefung des Assets BAHNSTEIG_FORM.EMD (Zahlen, keine Bilder).

    python s3_pruef.py [EMD] [--json aus.json]

Je Clip: Bildzahl, tiefster Punkt je Bild (Boden y = 0; + = unter dem Boden), Gleiten stehender Glieder in der Welt (Aktor
laeuft um die Spur weiter, wie bb_schieben), Sprung zwischen Folgebildern (groesste Winkelaenderung eines Knochens, kuerzester
Bogen), Spur-Tempo je Bild. Dazu: die drei Posen von J (Ruhe, Bild 1, Bild 2) muessen als Keyframe bytegleich im Asset liegen.
"""
import os, sys, json, math, argparse
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402

# Clips, deren Spur der Tick auf die Lage schiebt (bb_schieben): Rennen, Hieb, Gehen/Weichen, Nachhieb, Drohen, Biss, Flucht, Sturz.
# Nicht: Zucken (BB_ZUCKEN nur bb_anim), Griff (Opfer-Spur), Stehen.
BEWEGT = {0, 3, 4, 5, 9, 10, 11, 12, 26}
GLIED_ENDE = {'LH': 25, 'RH': 27, 'LV': 13, 'RV': 18}


def lade(pfad):
    f = FE.lese_emd(open(pfad, 'rb').read())
    return f


def pruefen(pfad):
    f = lade(pfad)
    r = S.Rig()
    J = r.form
    _R, _T = r.fk(r.ruhe)
    global NEUTRAL_Y
    NEUTRAL_Y = {g: float(_T[k][1]) for g, k in GLIED_ENDE.items()}
    erg = dict(datei=os.path.basename(pfad), clips=[])
    # Posen von J bytegleich?
    def kf_bytes(form, k):
        n = len(form['knochen'])
        k = dict(k, spur=[0, 0, 0])            # die Spur traegt keine Pose (bb_spur), verglichen werden Wurzel + Winkel
        return FE.keyframe_schreiben(k, n, FE.keyframe_groesse(n))
    j_posen = {'bild1': J['pool'][J['clips'][0]['bilder'][0]['kf']], 'bild2': J['pool'][J['clips'][1]['bilder'][0]['kf']],
               'ruhe': J['pool'][J['clips'][2]['bilder'][0]['kf']]}
    fund = {k: [] for k in j_posen}
    for ci, c in enumerate(f['clips']):
        for bi, b in enumerate(c['bilder']):
            kb = kf_bytes(f, f['pool'][b['kf']])
            for k, v in j_posen.items():
                if kb == kf_bytes(J, v):
                    fund[k].append((ci, bi))
    erg['j_posen'] = fund
    for ci, c in enumerate(f['clips']):
        n = len(c['bilder'])
        tief, spur, sprung = [], [], []
        enden = []
        prev = None
        for bi, b in enumerate(c['bilder']):
            k = f['pool'][b['kf']]
            p = r.aus_keyframe(k)
            R, T = r.fk(p)
            P = r.punkte(p, R, T)
            tief.append(round(max(float(x[:, 1].max()) for x in P if len(x)), 1))
            spur.append(k['spur'][0])
            # tiefster Punkt je Glied (Fuss/Hand samt Kindern)
            e = {}
            for g, kn in GLIED_ENDE.items():
                ks = [kn] + [j for j in range(32) if r.eltern[j] == kn]
                e[g] = (T[kn].copy(), max(float(P[j][:, 1].max()) for j in ks if len(P[j])))
            enden.append(e)
            if prev is not None:
                # wahre Drehung je Knochen zwischen Folgebildern (Weltdrehung, Grad), nicht der Euler-Abstand
                d = 0.0
                for j in range(32):
                    m = prev[j].T @ R[j]
                    d = max(d, math.degrees(math.acos(max(-1.0, min(1.0, (np.trace(m) - 1) / 2)))))
                sprung.append(round(d, 1))
            prev = R
        # Gleiten: Glied steht (tiefster Punkt des Glieds > -40) in zwei Folgebildern -> Weltversatz in x/z
        A = spur if ci in BEWEGT else [0] * n
        gleit = 0.0
        for bi in range(1, n):
            for g in GLIED_ENDE:
                (t0, y0), (t1, y1) = enden[bi - 1][g], enden[bi][g]
                ny = NEUTRAL_Y[g]
                if abs(t0[1] - ny) < 3 and abs(t1[1] - ny) < 3:
                    w0 = t0 + np.array([A[bi - 1], 0, 0])
                    w1 = t1 + np.array([A[bi], 0, 0])
                    gleit = max(gleit, float(np.linalg.norm((w1 - w0)[[0, 2]])))
        tempo = [spur[0]] + [spur[i] - spur[i - 1] for i in range(1, n)]
        erg['clips'].append(dict(clip=ci, bilder=n, tief_max=max(tief), tief_min=min(tief), sprung_max=max(sprung) if sprung else 0,
                                 gleiten_max=round(gleit, 1), tempo_min=min(tempo), tempo_max=max(tempo),
                                 treffer=[i for i, b in enumerate(c['bilder']) if b['wort'] & FE.BILD_TREFFER],
                                 ton=[[i, b['wort'] >> 28] for i, b in enumerate(c['bilder']) if b['wort'] & FE.BILD_TON],
                                 tief=tief))
    return erg


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('emd', nargs='?', default=os.path.join(HIER, '..', 'asset', 'BAHNSTEIG_FORM.EMD'))
    a.add_argument('--json')
    g = a.parse_args()
    e = pruefen(g.emd)
    print('Posen von J im Asset (Clip, Bild):', e['j_posen'])
    print('%4s %6s %9s %9s %9s %8s %13s' % ('Clip', 'Bilder', 'tief_max', 'tief_min', 'Dreh/Bild', 'Gleiten', 'Tempo'))
    for c in e['clips']:
        if c['bilder'] > 1:
            print('%4d %6d %9.1f %9.1f %8.1fG %8.1f %6d..%-6d' % (c['clip'], c['bilder'], c['tief_max'], c['tief_min'], c['sprung_max'],
                                                          c['gleiten_max'], c['tempo_min'], c['tempo_max']))
    if g.json:
        json.dump(e, open(g.json, 'w', encoding='utf-8'), indent=1)
