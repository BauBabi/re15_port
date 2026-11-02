"""Nachbesserung 1, B: Loecher an Achsel und Huefte schliessen (Nutzerbefund "Loecher zwischen Bein und Oberkoerper", Abnahme 1 M2).

    python s3_n1_naht.py punkte LOCH.json [--aus punkte.json]        Lochstrahlen -> Punkte im Raum des Elternknochens
    python s3_n1_naht.py bauen KOERPER.json --aus DIR                   K3_fit + Zusatzkoerper -> DIR/FORM.EMD/.TIM/form.json
    python s3_n1_naht.py pruefen KOERPER.json LOCH.json                 Zusatzkoerper gegen Bild 1/2 (neue Pixel) und gegen die Lochposen

Weg: Das EMD kennt keine Gewichte, Mesh i haengt starr an Knochen i. Ein Loch an einer Naht ist ein Sichtstrahl, der zwischen Eltern- und
Kindmesh hindurch auf den Hintergrund faellt. Je Lochpixel (k_loecher, gleiche Ansichten, orthografisch) der Punkt des Strahls, der dem
Drehpunkt am naechsten liegt, im Raum des Elternknochens: dort muss Koerper sein. Ein Zusatzkoerper (Ellipsoid wie k_naht.kugel, UV/CLUT der
naechsten Elternflaeche) deckt diese Punkte. Er gilt nur, wenn er in den Posen der Screenshots (Bild 1: Clip 0, Bild 2: Clip 1 von K3) hoechstens
k_naht.NEU_SCHWARZ_MAX neue Figurpixel auf Ziel-Schwarz und NEU_AUSSEN_MAX ausserhalb Ziel-Weiss setzt (wie die Abstimmung von K) - also dort,
wo die Screenshots ihn nicht sehen. Kein RE-Wert: Geometrie fuer ein neues Modell.
"""
import os, sys, json, copy, argparse, math, shutil
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
KGEN = os.path.join(S.FORM_DIR, 'varianten', 'K_alligator', 'generator')
sys.path.insert(0, KGEN)
import k_kern as KK         # noqa: E402
import k_loecher as KL      # noqa: E402
import k_naht as KN         # noqa: E402
FE = KK.FE

ZIEL = {'Bein l - Huefte': (8, 0), 'Bein r - Huefte': (9, 0), 'Arm l - Brust': (11, 10), 'Arm r - Brust': (16, 10)}
ASSET = os.path.join(HIER, '..', 'asset', 'BAHNSTEIG_FORM.EMD')
K3 = os.path.join(S.FORM_DIR, 'varianten', 'K_alligator', 'K3_fit')


def figur_mit(form_meshes, emd_pfad=ASSET):
    """Asset (Clips, Gerippe) mit anderen Meshes, als k_loecher.Figur."""
    s3 = FE.lese_emd(open(emd_pfad, 'rb').read())
    s3['meshes'] = form_meshes
    return KL.Figur(FE.schreibe_emd(s3, pruefen=False))


def lochfaelle(js, naehte=tuple(ZIEL)):
    R = json.load(open(js, encoding='utf-8'))
    out = []
    for c in R['clips']:
        for b in c['bilder']:
            if any(l['naht'] in naehte for l in b['loecher']):
                out.append((c['clip'], b['bild'], b['ansicht']))
    return out


def lochpunkte(fig, faelle, naehte=tuple(ZIEL)):
    V = {a[0]: a for a in KL.ANSICHTEN}
    P = {n: [] for n in naehte}
    px = {n: 0 for n in naehte}
    for c, b, an in faelle:
        _, az, el = V[an]
        A = KL.blick(az, el)
        kf = fig.kf(c, b)
        idb, piv, mitte = KL.rendern(fig, kf, A)
        R, T = fig.welt(kf)
        for l in KL.loecher(idb, piv):
            if l['naht'] not in naehte:
                continue
            kind, eltern = ZIEL[l['naht']]
            x, y, w, h = l['box']
            sub = idb[y:y + h, x:x + w] < 0
            ys, xs = np.nonzero(sub)
            g = KL.GROSS
            for yy, xx in zip(ys + y, xs + x):
                q = np.array([(xx + 0.5 - g / 2) / KL.MASSSTAB, (yy + 0.5 - g / 2) / KL.MASSSTAB, 0.0])
                basis = mitte + A.T @ q
                vor = A[2]
                t = float((T[kind] - basis) @ vor)
                X = basis + t * vor
                P[l['naht']].append(dict(eltern=(R[eltern].T @ (X - T[eltern])).tolist(), kind=(R[kind].T @ (X - T[kind])).tolist(),
                                         fall=[c, b, an]))
            px[l['naht']] += l['px']
    return P, px


def knochen_von(k):
    kind, eltern = ZIEL[k['naht']]
    return kind if k.get('am') == 'kind' else eltern


def koerper_form(koerper):
    """K3_fit-Form + Zusatzkoerper. koerper = Liste dict(naht, mitte (Elternraum), achse, rq, rn)."""
    form = FE.lese_emd(open(os.path.join(K3, 'FORM.EMD'), 'rb').read())
    q = copy.deepcopy(form)
    n = 0
    for k in koerper:
        kn = knochen_von(k)
        n += KN.kugel(form, kn, k['mitte'], k['achse'], k['rq'], k['rn'], q['meshes'][kn])
    FE.orientiere(form)
    return form, n


def bild_neu(form_alt, form_neu):
    """Neue Figurpixel je Screenshot-Pose (K3: Bild 1 = Clip 0, Bild 2 = Clip 1) gegen ziel2_bild*.png: (auf Schwarz, ausserhalb Weiss)."""
    import cv2
    tim = open(os.path.join(K3, 'FORM.TIM'), 'rb').read()
    fj = json.load(open(os.path.join(K3, 'form.json'), encoding='utf-8'))
    out = {}
    for nb in (1, 2):
        s = fj['bild%d' % nb]
        kam = KK.kamera(nb)
        z = cv2.imread(os.path.join(KK.FORM, 'ziel2_bild%d.png' % nb), 0)
        idb = []
        for f in (form_alt, form_neu):
            mo = FE.modell(FE.schreibe_emd(f, pruefen=False), tim)
            R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
            Rw, Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
            _t, i = KK.raster(kam, *KK.tris_welt(f, Rw, Tw))
            idb.append(i)
        neu = (idb[1] >= 0) & (idb[0] < 0)
        out[nb] = (int((neu & (z == 0)).sum()), int((neu & (z != 255)).sum()))
    return out


class BildProbe:
    """Grundraster der K3-Form in Bild 1 und Bild 2; neue Pixel eines Zusatzkoerpers (nur seine Dreiecke gegen den Tiefenpuffer)."""

    def __init__(self, form=None):
        import cv2
        self.form = form or FE.lese_emd(open(os.path.join(K3, 'FORM.EMD'), 'rb').read())
        tim = open(os.path.join(K3, 'FORM.TIM'), 'rb').read()
        fj = json.load(open(os.path.join(K3, 'form.json'), encoding='utf-8'))
        mo = FE.modell(FE.schreibe_emd(self.form, pruefen=False), tim)
        self.B = {}
        for nb in (1, 2):
            s = fj['bild%d' % nb]
            kam = KK.kamera(nb)
            R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
            Rw, Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
            tief, idb = KK.raster(kam, *KK.tris_welt(self.form, Rw, Tw))
            z = cv2.imread(os.path.join(KK.FORM, 'ziel2_bild%d.png' % nb), 0)
            self.B[nb] = dict(kam=kam, Rw=Rw, Tw=Tw, tief=tief, idb=idb, z=z)

    def neu(self, koerper):
        out = {}
        for nb, b in self.B.items():
            tief, idb = b['tief'].copy(), b['idb'].copy()
            for k in koerper:
                kn = knochen_von(k)
                tri = KN.ellipsoid_dreiecke(k['mitte'], k['achse'], k['rq'], k['rn'])
                w = tri.reshape(-1, 3) @ b['Rw'][kn].T + b['Tw'][kn]
                tief, idb = KK.raster(b['kam'], w.reshape(-1, 3, 3), np.full(len(tri), 99), tief, idb)
            neu = (idb >= 0) & (b['idb'] < 0)
            out[nb] = (int((neu & (b['z'] == 0)).sum()), int((neu & (b['z'] != 255)).sum()))
        return out


def deckung(k, punkte):
    """Anteil der Lochpunkte im Ellipsoid (Kugel: rq = rn)."""
    P = np.array(punkte, float)
    if not len(P):
        return 1.0
    u, w, n = KN._basis(k['achse'])
    d = P - np.array(k['mitte'], float)
    v = (d @ u) ** 2 / k['rq'] ** 2 + (d @ w) ** 2 / k['rq'] ** 2 + (d @ n) ** 2 / k['rn'] ** 2
    return float((v <= 1.0).mean())


def suchen(punkte_js, aus, runden=3):
    """Gierig: je Runde und Naht die Kugel (am Eltern- oder am Kindknochen, Mitte zwischen der Mitte noch offener Lochpunkte und dem
    Drehpunkt, Radius 150-520) mit der groessten Deckung der noch offenen Lochpunkte, bei Gleichstand die kleinste; sie muss zusammen mit
    allen schon gewaehlten in Bild 1 und Bild 2 die Grenzen von k_naht halten (wo die Screenshots sie sehen, ist sie verboten)."""
    D = json.load(open(punkte_js, encoding='utf-8'))
    probe = BildProbe()
    form = probe.form
    gewaehlt = []
    for runde in range(runden):
        for nm, pts in D['punkte'].items():
            kind, eltern = ZIEL[nm]
            offen = [p for p in pts if not any(k['naht'] == nm and deckung(k, [p[k.get('am', 'eltern')]]) > 0 for k in gewaehlt)]
            if not offen:
                continue
            best = None
            for am in ('eltern', 'kind'):
                P = np.array([p[am] for p in offen], float)
                piv = np.array(form['knochen'][kind]['versatz'], float) if am == 'eltern' else np.zeros(3)
                m = P.mean(0)
                for zu in (0.0, 0.2, 0.4, 0.6):
                    c = m + (piv - m) * zu
                    for r in (150, 180, 220, 260, 300, 350, 400, 460, 520):
                        k = dict(naht=nm, am=am, mitte=[round(float(x)) for x in c], achse=[0, 1.0, 0], rq=float(r), rn=float(r))
                        dk = deckung(k, P)
                        if dk <= 0 or (best is not None and (dk < best[0] or (dk == best[0] and r >= best[1]['rq']))):
                            continue
                        nb = probe.neu(gewaehlt + [k])
                        if all(v[0] <= KN.NEU_SCHWARZ_MAX and v[1] <= KN.NEU_AUSSEN_MAX for v in nb.values()):
                            best = (dk, k, nb, len(offen))
            if best:
                print('Runde %d %-16s offen %3d Punkte, Deckung %.2f  Kugel am %s Mitte %s r %d  Bild 1/2 neu %s' % (
                    runde, nm, best[3], best[0], best[1]['am'], best[1]['mitte'], best[1]['rq'], best[2]), flush=True)
                gewaehlt.append(dict(best[1], deckung=round(best[0], 3), runde=runde))
            else:
                print('Runde %d %-16s offen %3d Punkte: keine Kugel haelt Bild 1/2' % (runde, nm, len(offen)), flush=True)
    json.dump(dict(koerper=gewaehlt, quelle=punkte_js, grenzen=[KN.NEU_SCHWARZ_MAX, KN.NEU_AUSSEN_MAX], bild=probe.neu(gewaehlt)),
              open(aus, 'w', encoding='utf-8'), indent=1)
    return gewaehlt


def main():
    a = argparse.ArgumentParser()
    a.add_argument('was', choices=('punkte', 'bauen', 'pruefen', 'suchen'))
    a.add_argument('datei')
    a.add_argument('loch', nargs='?')
    a.add_argument('--aus')
    g = a.parse_args()
    if g.was == 'punkte':
        fig = KL.Figur(ASSET)
        fa = lochfaelle(g.datei)
        P, px = lochpunkte(fig, fa)
        for n, v in P.items():
            if not v:
                continue
            v = np.array([x['eltern'] for x in v])
            kind, eltern = ZIEL[n]
            piv = np.array(fig.form['knochen'][kind]['versatz'], float)
            print('%-16s %4d Lochpixel, Punkte im Raum von %d: Mitte %s, Streuung %s, Drehpunkt %s' % (
                n, px[n], eltern, v.mean(0).round().tolist(), v.std(0).round().tolist(), piv.round().tolist()))
        if g.aus:
            json.dump(dict(faelle=fa, punkte=P, px=px), open(g.aus, 'w', encoding='utf-8'))
    elif g.was == 'suchen':
        suchen(g.datei, g.aus)
    elif g.was == 'bauen':
        kp = json.load(open(g.datei, encoding='utf-8'))
        form, n = koerper_form(kp['koerper'])
        os.makedirs(g.aus, exist_ok=True)
        open(os.path.join(g.aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form))
        shutil.copy(os.path.join(K3, 'FORM.TIM'), os.path.join(g.aus, 'FORM.TIM'))
        fj = json.load(open(os.path.join(K3, 'form.json'), encoding='utf-8'))
        fj['name'] = os.path.basename(os.path.normpath(g.aus))
        json.dump(fj, open(os.path.join(g.aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
        shutil.copy(g.datei, os.path.join(g.aus, 'zusatzkoerper.json'))
        print('%s: %d Zusatzkoerper, %d Flaechen dazu' % (g.aus, len(kp['koerper']), n))
    else:
        kp = json.load(open(g.datei, encoding='utf-8'))
        alt = FE.lese_emd(open(os.path.join(K3, 'FORM.EMD'), 'rb').read())
        neu, n = koerper_form(kp['koerper'])
        print('Bild 1/2 neue Pixel (auf Schwarz, ausserhalb Weiss):', bild_neu(alt, neu), 'Grenze', KN.NEU_SCHWARZ_MAX, KN.NEU_AUSSEN_MAX)
        if g.loch:
            fa = lochfaelle(g.loch)
            for nm, form in (('vorher', alt), ('nachher', neu)):
                fig = figur_mit(form['meshes'])
                _P, px = lochpunkte(fig, fa)
                print(nm, px)


if __name__ == '__main__':
    main()


BEULE_POSEN = ((2, 0), (5, 20), (5, 60), (3, 28), (4, 35), (10, 100), (0, 7), (9, 20), (27, 40), (26, 20))
BEULE_ANSICHTEN = ('vorn', 'hinten', 'links', 'rechts', 'oben', '3/4 vorn rechts oben', '3/4 hinten links oben')


def bewerten(koerper, loch_js, naehte=None):
    """(Lochpixel je Naht auf den Lochfaellen von loch_js, neue Umrisspixel (Beulen) auf BEULE_POSEN x BEULE_ANSICHTEN) fuer K3 + koerper."""
    alt = FE.lese_emd(open(os.path.join(K3, 'FORM.EMD'), 'rb').read())
    neu, _n = koerper_form(koerper)
    fn = figur_mit(neu['meshes'])
    fa = lochfaelle(loch_js, naehte or tuple(ZIEL))
    _P, px = lochpunkte(fn, fa, naehte or tuple(ZIEL))
    fo = figur_mit(alt['meshes'])
    V = {a[0]: a for a in KL.ANSICHTEN}
    beule = 0
    for c, b in BEULE_POSEN:
        for an in BEULE_ANSICHTEN:
            _, az, el = V[an]
            A = KL.blick(az, el)
            ia, _, m = KL.rendern(fo, fo.kf(c, b), A)
            ib, _, _ = KL.rendern(fn, fn.kf(c, b), A, mitte=m)
            beule += int(((ib >= 0) & (ia < 0)).sum())
    return px, beule
