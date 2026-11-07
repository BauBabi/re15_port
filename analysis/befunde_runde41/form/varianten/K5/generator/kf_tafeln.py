#!/usr/bin/env python3
"""K5c Farbe (Runde 1 bis 3, Versuch 2): Nutzer-Tafeln 01-08 nach MESSBLATT_FARBE_V1 Abschnitt 7 (roh, kein Gamma auf irgendeine Kachel; Szenenlicht neben dem
Screenshot; Familienkachel mit gleichem Renderer und Licht; Zahlen L*, C*, L*>85 in der Beschriftung).

    python kf_tafeln.py <neu-Ordner> <aus-Ordner> [01 .. 07]     (Name der Variante: Umgebung KF_NAME, Standard 'K5c neu (Farbe Runde 2)')

01_szene_bild1.png  Bild 1 Original | K4 heute | K5c neu: Zeile 1 Spiellicht roh (form_pruefung.Kandidat.rendern 'mess'), Zeile 2 durch
                    form_video.video_filter (entzerren=False, rauschen=True) wie die Abfilmung
02_szene_bild2.png  dasselbe fuer Bild 2 (ROOM5090 Cut 11)
03_familie.png      EM30_HANGAR | RE2-G5 | RE2-G4 | K5c neu | K4 heute, render.blick (Licht 0,55 + 0,45 max(0, N.L), weiss), Umkugel 0,46, 3 Winkel
04_atlas.png        EM30 | K5c neu | K5c heute, je 2 Texel/Bildpunkt (gleiche Texelskala), darunter 8 Palettenfelder (k-Mittel Lab) mit Lab und Anteil
05_rundum_neu.png   K5c neu in Pose Bild 1 (Clip 0) und Ruhe/Stehen (Clip 2), 6 Winkel, Licht wie 03; EM30 als Vergleichskachel je Zeile
08_binnenzeichnung.png  (Versuch 2) Flanke / Schulter / Ruecken nah, 3-fach: RE2-G4 | EM30 | r3 | Variante, render.blick (Licht wie 03), roh
04_atlas.png (Versuch 2) zusaetzlich Zeile 2: Kanten (weiss) und Richtungsfeld (Farbton = Kantenrichtung, Helligkeit = Kohaerenz) nach
                    farbe_lesung/N2_skripte/n2_mass.bild_atlas fuer RE2-G4 | Variante
06_kopf_nah.png     Kopf/Kiefer: Zeile 1 Bild-2-Ausschnitt x 200-360, y 290-420, 3-fach: Original | Original T | K5c neu Kette | K5c heute Kette;
                    Zeile 2 Kopf+Kiefer (Mesh 21-23) neu, K5c heute und RE2-G4-Kopf (Mesh 26) unter dem Licht von 03, 3 Winkel"""
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
FL = os.path.normpath(os.path.join(HIER, '..', 'farbe_lesung'))
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
sys.path.insert(0, os.path.join(FL, 'B_skripte'))
import fl  # noqa: E402
import render as R  # noqa: E402
import k5blatt_hilfe as H  # noqa: E402
import p_zonen as PZ  # noqa: E402
import p_lade as PL  # noqa: E402

K5 = os.path.normpath(os.path.join(HIER, '..'))
K4 = os.path.join(K5, 'K4_heute')
K5C = os.path.join(K5, 'K5c')
GRUND = 92      # Hintergrund der render.blick-Kacheln (render.py)
NEU = os.environ.get('KF_NAME', 'K5c neu (Farbe Runde 2)')
LICHT_K = json.load(open(os.path.normpath(os.path.join(K5, '..', '..', 'pruefung', 'licht.json')), encoding='utf-8'))   # form_pruefung.licht_fuer


class _Tex:
    """form_buehne.Textur-Ersatz fuer RE2-Modelle aus emd_ansichtsblatt (px, lut mit Alpha, clut_y)."""

    def __init__(self, mo):
        self.px = mo.tex
        self.h, self.w = mo.tex.shape
        self.clut_y = mo.clut_y
        a = (mo.pal.sum(2) > 0) * 255
        self.lut = np.concatenate([mo.pal.astype(np.uint8), a[..., None].astype(np.uint8)], -1)


def re2_mo(kind):
    mo = PL.re2(kind)
    mo.T = _Tex(mo)
    return mo


def masse_text(img, msk):
    m = R.masse(img, msk)
    return 'L* %.1f  C* %.1f  L*>85 %.2f%%  %d px' % (m['L'], m['C'], m['hell85'], m['px'])


def kachel(img, titel, unter=None, zeile2=None):
    b = img[:, :, ::-1].copy() if img.shape[2] == 3 else img
    H.schrift(b, titel, 6, 18, 0.45)
    if zeile2:
        H.schrift(b, zeile2, 6, 36, 0.4, (200, 230, 255))
    if unter:
        H.schrift(b, unter, 6, b.shape[0] - 8, 0.4, (230, 230, 120))
    return b


def blick_teile(mo, kf, az, el, gross, teile=None):
    """wie render.blick, aber nur die Meshes in 'teile' (Nahansicht Kopf); Licht und Raster identisch."""
    if teile is None:
        return R.blick(mo, kf, az, el, gross, None)
    alt = mo.meshes
    leer = type(alt[0])
    try:
        neu = []
        for b, m in enumerate(alt):
            if b in teile:
                neu.append(m)
            else:
                c = leer.__new__(leer)
                c.__dict__.update(m.__dict__)
                c.tv, c.tn, c.tf, c.tuv, c.qv, c.qn, c.qf, c.quv = [], [], [], [], [], [], [], []
                neu.append(c)
        mo.meshes = neu
        return R.blick(mo, kf, az, el, gross, None)
    finally:
        mo.meshes = alt


def szene(n, neu, aus):
    x0, y0, x1, y1 = (150, 130, 470, 480) if n == 1 else (110, 180, 510, 480)
    o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
    zeilen = []
    for art in ('sauber', 'video'):
        ks = [kachel(o[y0:y1, x0:x1], 'Bild %d Original (Video-Standbild)' % n, None, 'roh, kein Gamma')]
        for nm, d in (('K4 heute (Spiel-Asset)', K4), (NEU, neu)):
            k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
            s, alb, m, kk = k.rendern(n)
            if art == 'video':
                s = np.asarray(PZ.FV.video_filter(s, bild=n, entzerren=False, rauschen=True), np.uint8)
            kk_ = '/'.join(('%.2f' % v).replace('.', ',') for v in LICHT_K['bild%d' % n]['k'])
            t2 = ('Disc-Licht %s x k (%s), roh' % ('5080 Cut 2' if n == 1 else '5090 Cut 11', kk_)) if art == 'sauber' else \
                'dasselbe + Videofilter (form_video), roh'
            ks.append(kachel(s[y0:y1, x0:x1], nm, None, t2))
        zeilen.append(H.reihe([cv2.resize(k, (k.shape[1] * 2, k.shape[0] * 2), interpolation=cv2.INTER_NEAREST) for k in ks]))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('%02d Bild %d: Original | K4 heute | %s. Z.1 Disc-Licht x Kanalfaktor k (pruefung/licht.json, beide gleich), '
                       'Z.2 + Videofilter. Kein Gamma. Z.1 ist heller als das echte Spielfenster (Tafel 07), Sicht-Pruefer r2 3.' % (n, n, NEU), t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def familie(neu, aus):
    modelle = [('EM30_HANGAR (Spiel, Clip 0)', fl.lade_em30(), 0), ('RE2-G5 (Typ 0x36, Clip 0)', re2_mo(0x36), 0),
               ('RE2-G4 (Typ 0x34, Clip 0)', re2_mo(0x34), 0), (NEU + ' (Clip 2)', fl.lade_var(neu), 2),
               ('K4 heute (Clip 2)', fl.lade_var(K4), 2)]
    zeilen = []
    for wt, az, el in R.WINKEL:
        ks = []
        for nm, mo, c in modelle:
            img, msk = R.blick(mo, R.kf(mo, c), az, el, 380, None)
            ks.append(kachel(img, nm, masse_text(img, msk), '%s, Licht 0,55+0,45 N.L, roh' % wt))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('03 Familie: gleicher Renderer, gleiches weisses Licht, Umkugel 0,46, roh, kein Gamma. Masse nur Figurpixel; px = Figurgroesse '
                       '(Vierbeiner ~halb so viele Pixel wie G4).', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def palette_feld(rgb, breite):
    L = PL.lab(rgb)
    cv2.setRNGSeed(3)
    _, lb, c = cv2.kmeans(L.astype(np.float32), 8, None, (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.05), 6, cv2.KMEANS_PP_CENTERS)
    a = np.bincount(lb.ravel(), minlength=8) / len(lb)
    o = np.argsort(c[:, 0])
    fe = np.full((8 * 26, breite, 3), 24, np.uint8)
    for i, j in enumerate(o):
        col = rgb[lb.ravel() == j].mean(0)
        fe[i * 26 + 2:i * 26 + 24, 4:44] = col[::-1]
        H.schrift(fe, 'Lab (%.1f/%.1f/%.1f)  %.1f %%' % (c[j, 0], c[j, 1], c[j, 2], 100 * a[j]), 50, i * 26 + 18, 0.42)
    return fe


def atlas(neu, aus):
    from p_messen import atlas as p_atlas
    ks = []
    for nm, mo in (('EM30_HANGAR (Spiel)', PL.em30()), (NEU, PL.var(neu)), ('K5c heute', PL.var(K5C))):
        per, _ = PL.sammeln(mo)
        rgb, mid = p_atlas(mo, per)
        im = rgb.copy()
        im[mid < 0] = 40
        im = cv2.resize(im[:, :, ::-1].astype(np.uint8), None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        u = mid >= 0
        L = PL.lab(rgb[u])
        C = np.hypot(L[:, 1], L[:, 2])
        H.schrift(im, nm, 6, 18, 0.5)
        H.schrift(im, 'genutzte Texel: Lab (%.1f/%.1f/%.1f), C* %.1f' % (L[:, 0].mean(), L[:, 1].mean(), L[:, 2].mean(), C.mean()), 6, im.shape[0] - 8, 0.42,
                  (230, 230, 120))
        ks.append(H.stapel([im, palette_feld(rgb[u], im.shape[1])]))
    t = H.reihe(ks)
    # Versuch 2: Kanten-/Richtungsbild (n2_mass.bild_atlas, 3-fach, hier auf 1-fach verkleinert): G4-Koerper | Variante-Koerper
    sys.path.insert(0, os.path.join(FL, 'N2_skripte'))
    import n2_mass as N
    Z = N.zonen()
    zb = []
    for nm, mo, zf in (('RE2-G4 Koerper', re2_mo(0x34), Z['g4']), (NEU + ' Koerper (x < 384)', fl.lade_var(neu), Z['k5'])):
        tmp = os.path.join(os.path.dirname(aus), '_n2_atlas_tmp.png')
        N.atlas_mass(mo, zf, nm, bild=tmp)
        b = cv2.imread(tmp)
        os.remove(tmp)
        b = cv2.resize(b, None, fx=1 / 3, fy=1 / 3, interpolation=cv2.INTER_AREA)
        b = cv2.resize(b, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        H.schrift(b, nm + ': Atlas | Kanten (Canny 6/12 L*) weiss, Rotflaechen-Umriss gelb | Richtung (Farbton) x Kohaerenz (Helligkeit)', 6, 18, 0.5)
        zb.append(b)
    breite = max(t.shape[1], max(b.shape[1] for b in zb))

    def auf(b):
        o = np.zeros((b.shape[0], breite, 3), np.uint8)
        o[:, :b.shape[1]] = b
        return o
    t = H.stapel([auf(t)] + [auf(b) for b in zb])
    kopf = H.kopfzeile('04 Atlas in gleicher Texelskala (2 Bildpunkte je Texel), roh. Darunter 8 Palettenfelder (k-Mittel in Lab ueber alle '
                       'genutzten Texel) mit Lab und Anteil; unten Kanten- und Richtungsbild (n2_mass) RE2-G4 und Variante.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


WINKEL6 = (('vorn', 90, 10), ('3/4 vorn', 45, 15), ('Seite', 0, 8), ('3/4 hinten', -135, 20), ('hinten', -90, 10), ('oben', 0, 70))


def rundum(neu, aus):
    em = fl.lade_em30()
    mo = fl.lade_var(neu)
    zeilen = []
    for nm, c in (('Pose Bild 1 (Clip 0)', 0), ('Ruhe/Stehen (Clip 2)', 2)):
        ks = []
        for wt, az, el in WINKEL6:
            img, msk = R.blick(mo, R.kf(mo, c), az, el, 300, None)
            ks.append(kachel(img, '%s, %s' % (NEU, wt), masse_text(img, msk), nm))
        img, msk = R.blick(em, R.kf(em, 0), 45, 15, 300, None)
        ks.append(kachel(img, 'EM30_HANGAR, 3/4 vorn', masse_text(img, msk), 'Vergleich, gleiches Licht'))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('05 ' + NEU + ' rundum, Licht wie 03 (0,55 + 0,45 N.L, weiss), roh, kein Gamma; rechts EM30 als Vergleich.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def kopf_nah(neu, aus):
    x0, y0, x1, y1 = 200, 290, 360, 420
    o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild2.png'), cv2.COLOR_BGR2RGB)
    T, _c = PZ.rueck(o, 2)
    ks = [kachel(o[y0:y1, x0:x1], 'Original Bild 2', None, 'Video, roh'), kachel(np.clip(T, 0, 255).astype(np.uint8)[y0:y1, x0:x1], 'Original T',
                                                                                    None, 'zurueckgerechnet (p_zonen.rueck)')]
    for nm, d in ((NEU, neu), ('K5c heute', K5C)):
        k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
        s, alb, m, kk = k.rendern(2)
        v = np.asarray(PZ.FV.video_filter(s, bild=2, entzerren=False, rauschen=True), np.uint8)
        Tv, _ = PZ.rueck(v, 2)
        ks.append(kachel(np.clip(Tv, 0, 255).astype(np.uint8)[y0:y1, x0:x1], nm, None, 'Kette T (wie Original T)'))
    z1 = H.reihe([cv2.resize(k, (k.shape[1] * 3, k.shape[0] * 3), interpolation=cv2.INTER_NEAREST) for k in ks])
    z2 = []
    g4 = re2_mo(0x34)
    for wt, az, el in (('3/4 vorn', 60, 10), ('Seite', 0, 5), ('unten vorn', 80, -30)):
        ks = []
        for nm, mo, c, teile in ((NEU + ' Kopf+Kiefer', fl.lade_var(neu), 1, (21, 22, 23)), ('K5c heute Kopf+Kiefer', fl.lade_var(K5C), 1, (21, 22, 23)),
                                 ('RE2-G4 Kopf (Mesh 26)', g4, 0, (26,))):
            img, msk = blick_teile(mo, R.kf(mo, c), az, el, 300, teile)
            ks.append(kachel(img, nm, masse_text(img, msk), '%s, Licht wie 03, roh' % wt))
        z2.append(H.reihe(ks))
    t = H.stapel([z1] + z2)
    kopf = H.kopfzeile('06 Kopf/Kiefer nah. Zeile 1: Bild 2 x 200-360, y 290-420, 3-fach. Zeilen 2-4: Kopf+Kiefer (K5c Clip 1 = Pose Bild 2, Maul offen) '
                       'neben dem RE2-G4-Kopf, Licht wie 03, roh.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def _ausschnitt(img, msk, art):
    """Nahausschnitt aus einem render.blick-Bild: Figurrahmen, davon Flanke = Mitte, Schulter = vorderes oberes Viertel, Ruecken = Mitte (Blick von oben).
    Feste Anteile des Figurrahmens fuer alle Modelle gleich (gleiche Behandlung, Messblatt V1 7.1)."""
    ys, xs = np.nonzero(msk)
    y0, y1, x0, x1 = ys.min(), ys.max(), xs.min(), xs.max()
    hh, ww = y1 - y0, x1 - x0
    s = int(min(hh, ww) * 0.5)
    if art == 'Schulter':
        cx, cy = x0 + int(0.7 * ww), y0 + int(0.4 * hh)
    else:
        cx, cy = x0 + ww // 2, y0 + int(0.45 * hh)
    a, b = max(cy - s // 2, 0), max(cx - s // 2, 0)
    return img[a:a + s, b:b + s]


def binnenzeichnung(neu, aus, r3=None):
    """08 (Versuch 2): Flanke (Seite), Schulter (3/4 vorn), Ruecken (von oben) nah, 3-fach Ausschnitt auf 360 px, je Zeile RE2-G4 | EM30 | r3 | Variante,
    render.blick mit dem Licht von 03 (0,55 + 0,45 N.L, weiss), roh, kein Gamma; gleiche Ausschnittregel fuer alle."""
    r3 = r3 or os.path.join(K5, 'farbe', 'K5c_farbe_r3')
    modelle = [('RE2-G4 (Typ 0x34, Clip 0)', re2_mo(0x34), 0), ('EM30_HANGAR (Clip 0)', fl.lade_em30(), 0),
               ('r3 (Farbe Runde 3, Clip 2)', fl.lade_var(r3), 2), (NEU + ' (Clip 2)', fl.lade_var(neu), 2)]
    zeilen = []
    for art, az, el in (('Flanke', 0, 8), ('Schulter', 45, 15), ('Ruecken', 0, 70)):
        ks = []
        for nm, mo, c in modelle:
            img, msk = R.blick(mo, R.kf(mo, c), az, el, 760, None)
            a = _ausschnitt(img, msk, art)
            a = cv2.resize(a, (360, 360), interpolation=cv2.INTER_NEAREST)
            ks.append(kachel(a, nm, None, '%s nah, Licht wie 03, roh' % art))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('08 Binnenzeichnung nah: Flanke | Schulter | Ruecken, je RE2-G4 | EM30 | r3 | ' + NEU + '. Gleicher Renderer und gleiches Licht '
                       '(0,55 + 0,45 N.L), gleiche Ausschnittregel (Figurrahmen), roh, kein Gamma.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


SPIEL = (('spiel_pose1_nachstellung_r5080.png', 1, 'ROOM5080 Schnitt 1, Nachstellung Bild 1 (Clip 0)'),
         ('spiel_pose1_r5080_s1.png', 1, 'ROOM5080 Schnitt 1, Lage Bild 1 (Clip 0)'),
         ('spiel_ruhe_r5080_s1.png', 1, 'ROOM5080 Schnitt 1, Ruhe (Clip 2)'),
         ('spiel_pose2_r5090_s11.png', 2, 'ROOM5090 Schnitt 11, Pose Bild 2 (Clip 1)'),
         ('spiel_ruhe_r5090_s11.png', 2, 'ROOM5090 Schnitt 11, Ruhe (Clip 2)'))


def spiel(spd, aus):
    """07: je Zeile Original-Standbild (Bild 1 bzw. 2) | K4 heute im Spiel | Variante im Spiel. Spielbilder aus kf_spiel.sh (gdigrab), unbearbeitet."""
    zeilen = []
    for f, n, txt in SPIEL:
        a = os.path.join(spd, 'heute_' + f)
        b = os.path.join(spd, f)
        if not (os.path.exists(a) and os.path.exists(b)):
            continue
        o = cv2.imread(PZ.FORM + '/bild%d.png' % n)
        ks = [o, cv2.imread(a), cv2.imread(b)]
        ks = [cv2.resize(k, (640, 480), interpolation=cv2.INTER_NEAREST) if k.shape[:2] != (480, 640) else k.copy() for k in ks]
        for k, t in zip(ks, ('Original Bild %d (Video-Standbild)' % n, 'K4 heute im Spiel: ' + txt, NEU + ' im Spiel: ' + txt)):
            H.schrift(k, t, 6, 18, 0.45)
            H.schrift(k, 'echtes Fenster (gdigrab), roh, kein Gamma', 6, 36, 0.4, (200, 230, 255))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('07 Spiel: eigene exe-Kopie, Kandidat ueber RE15_RE2_ASSET_ROOT (re15_port/tools/form_spiel.sh), echtes Fenster, kein AUTOSHOT, '
                       'kein SOFTWARE_RENDER. Links das Original-Standbild derselben Szene.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


if __name__ == '__main__':
    neu, ausd = sys.argv[1], sys.argv[2]
    os.makedirs(ausd, exist_ok=True)
    nur = sys.argv[3:] or ['01', '02', '03', '04', '05', '06', '08']
    if '07' in nur:
        spiel(os.path.join(ausd, 'spiel'), os.path.join(ausd, '07_spiel.png'))
    if '01' in nur:
        szene(1, neu, os.path.join(ausd, '01_szene_bild1.png'))
    if '02' in nur:
        szene(2, neu, os.path.join(ausd, '02_szene_bild2.png'))
    if '03' in nur:
        familie(neu, os.path.join(ausd, '03_familie.png'))
    if '04' in nur:
        atlas(neu, os.path.join(ausd, '04_atlas.png'))
    if '05' in nur:
        rundum(neu, os.path.join(ausd, '05_rundum_neu.png'))
    if '06' in nur:
        kopf_nah(neu, os.path.join(ausd, '06_kopf_nah.png'))
    if '08' in nur:
        binnenzeichnung(neu, os.path.join(ausd, '08_binnenzeichnung.png'))
    print('TAFELN-FERTIG', ausd)
