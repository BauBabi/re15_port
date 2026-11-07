"""Messblatt-Nachtrag V2, Abschnitt 3, Versuche V6/V7 mit exakter Neubeleuchtung.
Lichtkarte = dieselben Dreiecke (form_buehne.dreiecke mit Spiellicht 'mess', Bild 1) mit weisser Textur gerastert (Alpha wie die echte
Textur), also farb je Pixel; sauber_neu = trunc(albedo_neu x farb / 255) wie form_buehne.rastern, Hintergrund unveraendert.
Gegenprobe: albedo_neu = albedo ergibt sauber bitgleich (wird geprueft und ausgegeben).
V6: Ruecken/Flanke, Anteil p der Pixel mit dem hoechsten Albedo-a* -> Zielrot L* (a* 26,5, b* 18,0 = Familienwunde kf_mal3, Farbton 34 Grad).
V7: zusaetzlich in allen Figurzonen (ausser den Rotpixeln) a*, b* der Albedo mal g ('ab') bzw. nur warm ('warm': a* mal g, b* mal g nur bei b* > 0).
Blau-Anteil = Albedo b* < -6 und C* > 8 (Klasse B3).
"""
import os, sys, json, copy
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import n2_bild1 as B
PZ = B.PZ
P = PZ.P
lab = B.lab


def lab2rgb(L):
    return np.clip(cv2.cvtColor(L.reshape(1, -1, 3).astype(np.float32), cv2.COLOR_LAB2RGB).reshape(-1, 3) * 255, 0, 255)


def lichtkarte(k, n):
    lage, gier, clip, bild = k.stand(n)
    kam = P.kamera(n)
    kf = k.kf(clip, bild)
    licht, _kk = P.licht_fuer(n, k.licht)
    weiss = copy.copy(k.tex)
    weiss.lut = k.tex.lut.copy()
    weiss.lut[..., :3] = 255
    ziel = np.zeros((P.HOEHE, P.BREITE, 3), np.uint8)
    # farb ohne uint8-Kappung: Textur 255 -> rgb = farb (geklemmt 0..255). Fuer exakte Nachbildung auch farb > 255 noetig: zweiter Lauf mit Textur 128.
    P.FB.rastern(P.FB.dreiecke(k.mo, kf, lage, gier, kam, licht), weiss, ziel, kam.abbildung)
    halb = copy.copy(k.tex)
    halb.lut = k.tex.lut.copy()
    halb.lut[..., :3] = 128
    z2 = np.zeros((P.HOEHE, P.BREITE, 3), np.uint8)
    P.FB.rastern(P.FB.dreiecke(k.mo, kf, lage, gier, kam, licht), halb, z2, kam.abbildung)
    farb = np.where(ziel >= 255, (z2.astype(np.float32) + 0.5) * 255.0 / 128.0, ziel.astype(np.float32) + 0.5)
    return farb


def neu(sauber, alb, farb, sel, alb_neu):
    s2 = sauber.copy()
    s2[sel] = np.clip(alb_neu[sel].astype(np.float32) * farb[sel] / 255.0, 0, 255).astype(np.uint8)
    return s2


def main(vdir, aus):
    k = P.Kandidat(vdir + '/form.json')
    sauber, alb, m, kk = k.rendern(1)
    farb = lichtkarte(k, 1)
    fig = m & (alb.max(2) > 0)
    zz = B.zonen1(fig)
    Zf = B.figur(zz)
    allefig = alb.max(2) > 0
    chk = neu(sauber, alb, farb, allefig, alb)
    abw = np.abs(chk.astype(int) - sauber.astype(int))[allefig]
    print('Gegenprobe Neubeleuchtung: max |d| %d, Anteil d>0 %.4f' % (abw.max(), (abw > 0).mean()))
    out = dict(gegenprobe=dict(max=int(abw.max()), anteil=float((abw > 0).mean())), v6=[], v7=[])
    zr = dict(zz)[B.RF]
    La = lab(alb[zr].astype(float))
    ordnung = np.argsort(-La[:, 1])
    ys, xs = np.nonzero(zr)

    def lauf(alb2):
        s2 = neu(sauber, alb, farb, allefig, alb2)
        vv = PZ.FV.video_filter(s2, bild=1, entzerren=False, rauschen=True)
        T2, _c = PZ.rueck(vv, 1)
        return B.stat(T2, zz)

    for Lz in (35, 45, 55):
        for p in (0.10, 0.15, 0.20):
            rotsel = np.zeros((480, 640), bool)
            n = int(p * zr.sum())
            rotsel[ys[ordnung[:n]], xs[ordnung[:n]]] = True
            alb2 = alb.astype(np.float32).copy()
            alb2[rotsel] = lab2rgb(np.array([[Lz, 26.5, 18.0]], float))[0]
            st = lauf(alb2)
            r = dict(L_ziel=Lz, anteil=p, ruecken_rot=st['a12_C15_L20'], figur_rot=st['figur_rot_p'], figur_C=st['figur_C'], figur_ab=st['figur_ab_streu'],
                     ruecken_lab=st['lab_mittel'])
            out['v6'].append(r)
            print('V6', r)
            sys.stdout.flush()
    rotsel = np.zeros((480, 640), bool)
    n = int(0.15 * zr.sum())
    rotsel[ys[ordnung[:n]], xs[ordnung[:n]]] = True
    for modus in ('ab', 'warm'):
        for g in (1.0, 1.5, 2.0, 2.5):
            alb2 = alb.astype(np.float32).copy()
            sel = Zf & ~rotsel
            Lq = lab(alb[sel].astype(float))
            if modus == 'ab':
                Lq[:, 1:] *= g
            else:
                Lq[:, 1] *= g
                Lq[:, 2] = np.where(Lq[:, 2] > 0, Lq[:, 2] * g, Lq[:, 2])
            alb2[sel] = lab2rgb(Lq)
            alb2[rotsel] = lab2rgb(np.array([[45, 26.5, 18.0]], float))[0]
            st = lauf(alb2)
            Lalb = lab(alb2[Zf])
            blau = 100 * float(((Lalb[:, 2] < -6) & (np.hypot(Lalb[:, 1], Lalb[:, 2]) > 8)).mean())
            r = dict(modus=modus, g=g, figur_C=st['figur_C'], figur_ab=st['figur_ab_streu'], figur_rot=st['figur_rot_p'], ruecken_rot=st['a12_C15_L20'],
                     albedo_C=round(float(np.hypot(Lalb[:, 1], Lalb[:, 2]).mean()), 1), albedo_blau_p=round(blau, 1),
                     zonen_C={a: v['C'] for a, v in st['zonen'].items()}, zonen_ab={a: v['ab'] for a, v in st['zonen'].items()})
            out['v7'].append(r)
            print('V7', r)
            sys.stdout.flush()
    json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else B.K5 + '/farbe/K5c_farbe_r3', sys.argv[2] if len(sys.argv) > 2 else os.path.join(B.FL, 'N2_bild1_v7.json'))
