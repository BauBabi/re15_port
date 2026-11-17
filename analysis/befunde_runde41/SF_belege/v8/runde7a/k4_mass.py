"""7a K4: Armhaltung - Varianten-Tabelle (offline, echte Szenenposen Kamera Cut 12).
python k4_mass.py NAME EMD  -> k4_NAME.json"""
import os, sys, json
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import v8_gemein as V
import riegel_hand as RH
import riegel_v8 as RV
import r3_gemein as R3
import luecke_r5 as L
import ruecken_r5 as RR5
sys.path.insert(0, os.path.join(V.R41, 'v7hand'))
import f_rast as FR, f_analyse as FA
REF = open(os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'), 'rb').read()
SP = R3.szene_posen()
KOERPER = (0, 1, 2, 4, 5, 7, 8)          # Becken, Beine, Bauch, Rumpf (ohne Arme, Sichel, Schwanz)


def gesicht(mo, mo7, frames=(311, 312, 313, 314)):
    out = {}
    for f in frames:
        p = SP[f]
        A, A7 = L.zeichne(mo, p, L.VARIANTEN[0]), L.zeichne(mo7, p, L.VARIANTEN[0])
        ges = (A7['bone'][:480] == 9) & (A7['lok'][:480][..., 0] >= RR5.GESICHT_X)
        b = A['bone'][:480]
        ver = ges & (b != 9) & (b >= 0)
        out[f] = dict(px=int(ver.sum()), knochen={int(x): int((b[ver] == x).sum()) for x in np.unique(b[ver])})
    return out


def lage(mo, frames):
    """je Bild: Winkel Ellbogen->Handgelenk gegen die Senkrechte (Grad, + = Hand nach Bild-rechts), Abstand Handgelenk und Krallenspitze
    zum naechsten Koerper-Bildpunkt (Knochen 0,1,2,4,5,7,8) in px, Hand-Bildpunkte, die neben (nicht vor) dem Koerper liegen."""
    out = {}
    for f in frames:
        p = SP[f]
        A = FR.zeichne(mo, p)
        bone = A['bone'][:480]
        k = np.isin(bone, KOERPER)
        ky, kx = np.nonzero(k)
        E = np.asarray(FA.welt(mo, p, 11, np.zeros((1, 3)))[0], float)[:2]
        Wp = np.asarray(FA.welt(mo, p, 12, np.zeros((1, 3)))[0], float)[:2]
        P12 = np.array([x[:3] for x in mo.meshes[12].tv], float)
        pp = np.asarray(FA.welt(mo, p, 12, P12), float)[:, :2]
        T = pp[29]
        d = lambda q: float(np.min(np.hypot(kx - q[0], ky - q[1]))) if len(kx) else -1
        v = Wp - E
        wink = float(np.degrees(np.arctan2(v[0], v[1])))
        # Hand (Knochen 11+12) frei gezeichnet: Anteil der Bildpunkte ausserhalb der Koerper-Silhouette (Zeilenspanne je Zeile)
        Ah = FR.zeichne(mo, p, knochen=(11, 12))
        hm = Ah['bone'][:480] >= 0
        hy, hx = np.nonzero(hm)
        aus = 0
        for y, x in zip(hy, hx):
            row = np.nonzero(k[y])[0]
            if not len(row) or x < row.min() or x > row.max():
                aus += 1
        out[f] = dict(winkel=round(wink, 1), abst_W=round(d(Wp), 1), abst_T=round(d(T), 1), hand_px=int(hm.sum()), neben_px=int(aus),
                      E=E.round(1).tolist(), W=Wp.round(1).tolist(), T=T.round(1).tolist())
    return out


def boden(mo):
    """alle Keyframes: Hand-Ecken (Mesh 12) unter der tiefsten Ecke der Fuesse/Unterschenkel (Meshes 2, 3, 5, 6)."""
    s = mo.skel
    P12 = np.array([v[:3] for v in mo.meshes[12].tv], float)
    FU = {b: np.array([v[:3] for v in mo.meshes[b].tv], float) for b in (2, 3, 5, 6) if mo.meshes[b].tv}
    liste = []
    erst = RH._keyframes(mo)
    for kf, (c, b) in sorted(erst.items()):
        R, T = mo.pose(kf)
        w = lambda bb, P: P @ (R[bb].astype(float) / 4096).T + T[bb].astype(float)
        sohle = max(float(w(bb, P)[:, 1].max()) for bb, P in FU.items())
        tief = float(w(12, P12)[:, 1].max()) - sohle
        if tief > 0:
            liste.append((c, b, kf, round(tief, 1)))
    return dict(kf=len(liste), max=max([x[3] for x in liste], default=0), liste=sorted(liste, key=lambda x: -x[3])[:8])


def messen(blob):
    mo, mo7 = V.modell(blob), V.modell(REF)
    out = dict(sha=V.sha(blob))
    t = RH.s20(blob, REF)
    out['S20'] = dict(ok=t['ok'], rot=t['teile_rot'], a=t['a'] if 'a' in t else None, b=t['b']['winkel'], c=t['c'])
    g = RH._dd(blob)
    out['g'] = dict(zusammen=g['zusammen_kf'], krallen=g['krallen_kf'], hand=g['hand_kf'], szene=[list(x) for x in RH._szene(g['liste'])],
                    je_clip=RH._je_clip({(e['clip'], e['bild']) for e in g['liste']}), tiefste=g['tiefste'][:3])
    s9 = RV.s9(blob, REF)
    out['S9'] = s9
    m, A = RV.render(blob)
    r2, r3, _om = RV.s2_s3(m, A['bone'][:480])
    out['S3'] = dict(e_K=r3['e_K'], d=r3['d_sattel_kopf'], ok=r3['ok'])
    out['gesicht'] = gesicht(mo, mo7)
    out['lage'] = lage(mo, list(range(311, 325)))
    out['boden'] = boden(mo)
    return out


if __name__ == '__main__':
    nm, emd = sys.argv[1], sys.argv[2]
    r = messen(open(emd, 'rb').read())
    json.dump(r, open('k4_%s.json' % nm, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print(nm, r['sha'][:8], 'S20', r['S20']['ok'], 'g', r['g']['zusammen'], r['g']['krallen'], 'szene', len(r['g']['szene']),
          'S9', [(a['ansicht'], a['ueber_v7_px'], a['neue_ecken']) for a in r['S9']['ansichten']], 'S3e', r['S3']['e_K'],
          'gesicht', {f: v['px'] for f, v in r['gesicht'].items()}, 'boden', r['boden']['kf'], r['boden']['max'])
    print('lage', {f: (v['winkel'], v['abst_W'], v['abst_T'], v['neben_px']) for f, v in r['lage'].items()})
