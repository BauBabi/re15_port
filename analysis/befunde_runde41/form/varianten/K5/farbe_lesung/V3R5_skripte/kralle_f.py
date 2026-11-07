"""Runde 5: Krallenzeile = G4-Zeile 1 (Runde-3-Klauenfarbe) mit L* unveraendert und Buntheit x f. Je f: Bild 2 Vorderbeine RV1/RV2/RV3
(alle, nur Kralle, ohne Kralle) durch die Kette (3 Saaten), Krallenmaske aus kralle_rv.kennung.
    python kralle_f.py f1 f2 ...
"""
import json, os, sys, shutil
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kralle_rv as KR  # noqa
R, KG, FE, PZ = KR.R, KR.KG, KR.FE, KR.PZ
K5 = KR.K5


MODUS = os.environ.get('KMODUS', 'C')   # C: Buntheit x f (L* bleibt); K: lineares sRGB x f (dunkler, Farbton bleibt)


def zeile_f(g4zeile, f):
    if MODUS == 'K':
        rgb8 = KG.wort_rgb(g4zeile).astype(float) / 255.0
        lin = np.where(rgb8 <= 0.04045, rgb8 / 12.92, ((rgb8 + 0.055) / 1.055) ** 2.4) * f
        srgb = np.where(lin <= 0.0031308, 12.92 * lin, 1.055 * lin ** (1 / 2.4) - 0.055)
        rgb = np.clip(np.round(srgb * 255), 0, 255)
        neu = list(g4zeile)
        for i in range(256):
            if g4zeile[i] == 0:
                continue
            w = KG.wort(rgb[i]) | (g4zeile[i] & 0x8000)
            neu[i] = w if w != 0 else 0x8000
        return neu
    L = KG.lab(KG.wort_rgb(g4zeile))
    rgb = KG.rgb_aus_lab(np.stack([L[:, 0], f * L[:, 1], f * L[:, 2]], 1))
    neu = list(g4zeile)
    for i in range(256):
        if g4zeile[i] == 0:
            continue
        w = KG.wort(rgb[i]) | (g4zeile[i] & 0x8000)
        neu[i] = w if w != 0 else 0x8000
    return neu


def variante(vdir, aus, f):
    os.makedirs(aus, exist_ok=True)
    emd = open(os.path.join(vdir, 'FORM.EMD'), 'rb').read()
    t = FE.tim_lesen(open(os.path.join(vdir, 'FORM.TIM'), 'rb').read())
    cl = [list(z) for z in t['cluts']]
    zk = len(cl)
    cl.append(zeile_f(cl[1], f))
    form = FE.lese_emd(emd)
    for mi in KR.KLAUEN:
        for teil in ('dreiecke', 'vierecke'):
            for fl in form['meshes'][mi][teil]:
                fl['clut'] = ((t['clut_y'] + zk) << 6) | 0
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form, pruefen=True))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(t['pixel'], cl, clut_x=t['clut_x'], clut_y=t['clut_y']))
    shutil.copyfile(os.path.join(vdir, 'form.json'), os.path.join(aus, 'form.json'))


def main():
    M = np.load(os.path.join(KR.RR, 'rot_masken.npz'))
    E = json.load(open(os.path.join(KR.RR, 'mess.json'), encoding='utf-8'))
    v5 = os.path.join(K5, 'farbe', 'K5c_farbe_v3')
    kr = KR.kennung(v5, os.path.join(HIER, 'f_id'))
    n = 2
    basis0 = np.unpackbits(M['basis2'])[:480 * 640].reshape(480, 640).astype(bool)
    reg = M['reg2'].astype(int)
    A0 = E['bilder']['2']['A0']['T']
    out = {}
    for f in [float(x) for x in sys.argv[1:]]:
        d = os.path.join(HIER, '%s_%.3f' % (MODUS, f))
        variante(v5, d, f)
        k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
        sauber, alb, m, _k = k.rendern(n)
        basis = basis0 & m & (alb.max(2) > 0)
        Ls = [R.lab(PZ.rueck(np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0])
              for s in R.SAATEN]
        rm = basis & (reg == R.REGIONEN.index('Vorderbeine'))
        fig = basis & (reg >= 0)
        out[f] = dict(alle=KR.messe(Ls, rm, A0), kralle=KR.messe(Ls, rm & kr[n], A0), ohne=KR.messe(Ls, rm & ~kr[n], A0),
                      figur=KR.messe(Ls, fig, A0))
        print(f, json.dumps(out[f]))
    json.dump(out, open(os.path.join(HIER, 'kralle_%s_%s.json' % (MODUS, '_'.join(sys.argv[1:]))), 'w'), indent=1)


if __name__ == '__main__':
    main()
