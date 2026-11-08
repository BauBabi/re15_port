#!/usr/bin/env python3
"""Farbe Versuch 3 (G4-Uebertragung): Nutzer-Tafeln 01-08 nach MESSBLATT_FARBE_V3 6 (Darstellungsregel: nie ein Modell aufhellen, ohne die
Vergleichsmodelle gleich zu behandeln; Familienkachel roh unter gleichem Licht; Szenenlicht-Kacheln neben dem Original; Nahkacheln der Feuerstellen
Bild 1 x 270-410 / y 200-300 4-fach, Bild 2 x 150-360 / y 290-425 3-fach; Knochen-/Herkunftskarten nur als beschriftete Falschfarbe).

    python kf_tafeln_v3.py <V3-Ordner> <aus-Ordner> [01 .. 08]

Gemeinsamer Code mit Versuch 2: kf_tafeln.py (szene, familie, rundum, spiel). Neu: Nahkacheln in 01/02, Atlas G4 | V3 mit Herkunftskarte (04),
Kopf nah unter neutralem, Bild-1- und Bild-2-Licht neben dem G4-Kopf (06), Binnenzeichnung G4 | EM30 | Versuch 2 | V3 (08)."""
import os
import sys

import cv2
import numpy as np

os.environ.setdefault('KF_NAME', 'K5c V3 (G4-Uebertragung)')
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_tafeln as KT  # noqa: E402
import kf_g4 as KG  # noqa: E402
import riegel_farbe_v3 as RV  # noqa: E402

H, R, PZ, fl = KT.H, KT.R, KT.PZ, KT.fl
NEU = KT.NEU
K5 = KT.K5
V2 = os.path.join(K5, 'farbe', 'K5c_farbe_v2')
NAH = {1: (270, 200, 410, 300, 4), 2: (150, 290, 360, 425, 3)}       # Messblatt V3 6.3


def nah(img, n):
    x0, y0, x1, y1, f = NAH[n]
    a = img[y0:y1, x0:x1]
    return cv2.resize(a, (a.shape[1] * f, a.shape[0] * f), interpolation=cv2.INTER_NEAREST)


def szene(n, neu, aus):
    KT.szene(n, neu, aus)
    oben = cv2.imread(aus)
    o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
    ks = [KT.kachel(nah(o, n), 'Original Bild %d, Nahkachel' % n, None, 'Video, roh')]
    for nm, d in (('K4 heute', KT.K4), (NEU, neu)):
        k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
        s, alb, m, kk = k.rendern(n)
        v = np.asarray(PZ.FV.video_filter(s, bild=n, entzerren=False, rauschen=True, saat=RV.SAAT), np.uint8)
        ks.append(KT.kachel(nah(v, n), nm + ', Nahkachel', None, 'Spiellicht + Videofilter (Saat 15), roh'))
    x0, y0, x1, y1, f = NAH[n]
    z = H.reihe(ks)
    kopf = H.kopfzeile('Nahkachel der Feuerstelle (Messblatt V3 6.3): x %d-%d, y %d-%d, %d-fach. %s' % (
        x0, x1, y0, y1, f, 'Bild 1: Stelle F1 auf dem Becken (Knochen 0)' if n == 1 else 'Bild 2: Fleischband am Unterkiefer'), z.shape[1])
    breite = max(oben.shape[1], z.shape[1])

    def auf(b):
        out = np.zeros((b.shape[0], breite, 3), np.uint8)
        out[:, :b.shape[1]] = b
        return out
    cv2.imwrite(aus, H.stapel([auf(oben), auf(kopf), auf(z)]))


TEIL_FARBE = {   # Falschfarbe je K5c-Teil (nur Karte, kein Modellbild)
    'Rumpf': (200, 60, 60), 'Schwanz': (60, 160, 220), 'Hals': (230, 200, 40), 'Hinterbein l (W)': (80, 200, 80), 'Hinterbein r (W)': (40, 120, 40),
    'Vorderbein l (W)': (200, 120, 220), 'Vorderbein r (W)': (120, 60, 160), 'Klauen': (255, 160, 60), 'Kopf + Kiefer aussen': (240, 240, 240),
    'Maulinneres F2': (255, 80, 160), 'F1-Stelle': (255, 0, 255), 'Maul-Band': (255, 120, 0), 'Zaehne F8': (0, 255, 255), 'Augen F10': (0, 0, 255),
}


def _teil(nm):
    for k in ('Hinterbein l (W)', 'Hinterbein r (W)', 'Vorderbein l (W)', 'Vorderbein r (W)'):
        if nm.startswith(k):
            return k
    return 'Klauen' if nm.startswith('Klauen') else nm


def atlas(neu, aus):
    import json
    bau = json.load(open(os.path.join(neu, 'bau.json'), encoding='utf-8'))
    v = RV.Var(neu)
    g4, g4tim, _q = KG.g4_modell()
    G = KG.G4Quelle(g4)
    px, cl = KG.g4_cluts(g4tim)
    g = np.full((256, 512, 3), 40, np.uint8)
    for p in range(3):
        sl = slice(p * 128, (p + 1) * 128)
        g[:, sl] = np.round(KG.wort_rgb(np.array(cl[p])[px[:, sl]])).astype(np.uint8)
    g[:, :384][~G.global_maske] = 40
    a = np.full((256, 512, 3), 40, np.uint8)
    b1 = np.full((256, 512, 3), 40, np.uint8)
    herk = np.full((256, 512, 3), 40, np.uint8)
    for key, mi, z, ecken, a3 in v.flaechen:
        xy = v.texel(ecken)
        if not len(xy):
            continue
        xs, ys = np.clip(xy[:, 0], 0, 511), np.clip(xy[:, 1], 0, 255)
        col = np.round(v.rgb_zeilen[z][v.px[ys, xs]]).astype(np.uint8)
        if z in (3, 4, 5):
            b1[ys, xs] = col
        else:
            a[ys, xs] = col
        nm = bau['flaechen_herkunft']['%d|%s|%d' % key][0]
        herk[ys, xs] = TEIL_FARBE.get(_teil(nm), (128, 128, 128))
    ks = []
    for img, t1, t2 in ((g, 'RE2-G4 Atlas (Seiten 0-2, CLUT 0-2), genutzte Texel', 'Quelle CDEMD0.EMS Typ 0x34, TIM @0x65e800'),
                        (a, NEU + ': Texel mit G4-Zeilen 0-2 und Sonderfeld-Zeilen 6-8', 'Seite 3 = Sonderfelder F1-Stelle (oben), Maul-Band'),
                        (b1, NEU + ': Texel mit Bild-1-Zeilen 3-5', 'Aderrot im Farbton des Bahnsteig-Rots'),
                        (herk, 'Herkunftskarte (Falschfarbe je K5c-Teil, kein Modellbild)', 'Legende rechts')):
        im = cv2.resize(img[:, :, ::-1], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
        H.schrift(im, t1, 6, 18, 0.5)
        H.schrift(im, t2, 6, im.shape[0] - 8, 0.42, (230, 230, 120))
        ks.append(im)
    leg = np.full((512, 300, 3), 24, np.uint8)
    for i, (nm, c) in enumerate(TEIL_FARBE.items()):
        leg[10 + i * 32:34 + i * 32, 8:40] = c[::-1]
        H.schrift(leg, nm, 48, 28 + i * 32, 0.45)
    t = H.stapel([H.reihe([ks[0], ks[1]]), H.reihe([ks[2], ks[3], leg])])
    kopf = H.kopfzeile('04 Atlas, 2 Bildpunkte je Texel, roh: RE2-G4 | V3 (G4-Zeilen + Sonderfelder) / V3 (Bild-1-Zeilen) | Herkunft je Teil. '
                       'Seiten 0-2 der V3-TIM sind die G4-Indizes bytegleich (Riegel H1).', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def kopf_nah(neu, aus):
    zeilen = []
    for n in (2, 1):
        o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        T, _c = PZ.rueck(o, n)
        ks = [KT.kachel(nah(o, n), 'Original Bild %d' % n, None, 'Video, roh'),
              KT.kachel(nah(np.clip(T, 0, 255).astype(np.uint8), n), 'Original T', None, 'zurueckgerechnet (p_zonen.rueck)')]
        for nm, d in ((NEU, neu), ('K4 heute', KT.K4)):
            k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
            s, alb, m, kk = k.rendern(n)
            vv = np.asarray(PZ.FV.video_filter(s, bild=n, entzerren=False, rauschen=True, saat=RV.SAAT), np.uint8)
            Tv, _ = PZ.rueck(vv, n)
            ks.append(KT.kachel(nah(np.clip(Tv, 0, 255).astype(np.uint8), n), nm, None, 'Kette T (wie Original T), Bild-%d-Licht' % n))
        zeilen.append(H.reihe(ks))
    g4 = KT.re2_mo(0x34)
    for wt, az, el in (('3/4 vorn', 60, 10), ('Seite', 0, 5), ('unten vorn', 80, -30)):
        ks = []
        for nm, mo, c, teile in ((NEU + ' Kopf+Kiefer', fl.lade_var(neu), 1, (21, 22, 23)), ('K4 heute Kopf+Kiefer', fl.lade_var(KT.K4), 1, (21, 22, 23)),
                                 ('RE2-G4 Kopf (Mesh 26)', g4, 0, (26,)), ('RE2-G4 Mesh 16 = Quelle F18', g4, 0, (16,))):
            img, msk = KT.blick_teile(mo, R.kf(mo, c), az, el, 300, teile)
            ks.append(KT.kachel(img, nm, KT.masse_text(img, msk), '%s, neutral (0,55 + 0,45 N.L), roh' % wt))
        zeilen.append(H.reihe(ks))
    breite = max(z.shape[1] for z in zeilen)

    def auf(b):
        out = np.zeros((b.shape[0], breite, 3), np.uint8)
        out[:, :b.shape[1]] = b
        return out
    t = H.stapel([auf(z) for z in zeilen])
    kopf = H.kopfzeile('06 Gesicht/Maul nah. Z.1 Bild 2 (x 150-360, y 290-425, 3-fach): Maul-Band lachsrot, Zaehne hell. Z.2 Bild 1 (x 270-410, '
                       'y 200-300, 4-fach): Stelle F1 rosa-mauve auf dem Becken. Z.3-5 Kopf+Kiefer (Clip 1, Maul offen) neben dem RE2-G4-Kopf und der Hautquelle G4 16 (F18), neutral, roh.',
                       t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


R3 = os.path.join(K5, 'farbe', 'K5c_farbe_v3r3')
RV_DIR = os.path.join(K5, 'farbe_lesung', 'V3R4_skripte')
RV_REGIONEN = ['Ruecken', 'Flanke', 'Brust/Bauch', 'Vorderbeine', 'Hinterbeine', 'Hals', 'Kopf', 'Schwanz']   # riegel_rot.REGIONEN
RV_BILD = {1: ['Schwanz', 'Ruecken', 'Flanke', 'Vorderbeine', 'Hinterbeine', 'Figur'],
           2: ['Ruecken', 'Brust/Bauch', 'Vorderbeine', 'Vorderbeine ohne Krallen', 'Hals', 'Kopf', 'Figur']}   # bindende RV1-Zeilen (Nachtrag 5)
# Runde 5: Bild 2 "Vorderbeine" nur Info, bindend "Vorderbeine ohne Krallen" (riegel_rot.py, Maske V3R5_skripte/krallen_bild2.npz)
R5_DIR = os.path.join(K5, 'farbe_lesung', 'V3R5_skripte')
NEU_TITEL = os.environ.get('KF_RUNDE_NEU', 'Runde 5 (neu)')


def rot_vorlage(neu, aus, rv_neu=None):
    """09 Rot wie die Vorlage: je Bild und Region Vorlage | Runde 3 | neu unter Szenenlicht + Videofilter (Saat 15), roh, gleicher Ausschnitt;
    Region als duenne Linie (Annotation, nicht im Bild verrechnet). Unterschrift: RV1 Rot eng % (Mittel 3 Saaten, riegel_rot.py) und RV2 Farbton."""
    import json as _j
    M = np.load(os.path.join(RV_DIR, 'rot_masken.npz'))
    Z = _j.load(open(os.path.join(RV_DIR, 'ziele.json'), encoding='utf-8'))
    Z['bild2|Vorderbeine ohne Krallen|T'] = _j.load(open(os.path.join(R5_DIR, 'ziel_ohne_krallen.json'), encoding='utf-8'))['ziel']
    kz = np.load(os.path.join(R5_DIR, 'krallen_bild2.npz'))
    kralle2 = np.unpackbits(kz['kralle'])[:480 * 640].reshape(480, 640).astype(bool)
    w3 = _j.load(open(os.path.join(R5_DIR, 'gegen_v3r3_r5.json'), encoding='utf-8'))   # Runde 3 mit dem Runde-5-riegel_rot (neue Region)
    wn = _j.load(open(rv_neu or os.path.join(K5, 'farbe', 'riegel_rot_K5c_farbe_v3.json'), encoding='utf-8'))
    bloecke = []
    for n in (1, 2):
        o = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        vids = []
        for d in (R3, neu):
            k = PZ.P.Kandidat(os.path.join(d, 'form.json'))
            sb, alb, m, kk = k.rendern(n)
            vids.append(np.asarray(PZ.FV.video_filter(sb, bild=n, entzerren=False, rauschen=True, saat=RV.SAAT), np.uint8))
        basis = np.unpackbits(M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool)
        reg = M['reg%d' % n].astype(int)
        for rn in RV_BILD[n]:
            if rn == 'Vorderbeine ohne Krallen':
                rm = basis & (reg == RV_REGIONEN.index('Vorderbeine')) & (~kralle2 if n == 2 else True)
            else:
                rm = basis & ((reg == RV_REGIONEN.index(rn)) if rn != 'Figur' else (reg >= 0))
            ys, xs = np.where(rm)
            y0, y1 = max(0, ys.min() - 6), min(480, ys.max() + 7)
            x0, x1 = max(0, xs.min() - 6), min(640, xs.max() + 7)
            f = max(1, int(220 // (y1 - y0)))
            cont, _h = cv2.findContours(rm[y0:y1, x0:x1].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            zr = Z['bild%d|%s|T' % (n, rn)]
            O, (lo, hi) = zr['rot_eng']['O'], zr['rot_eng']['ziel']
            hO = zr['h_rot']['O'] if zr.get('h_rot') else None
            key = 'Bild %d %s ' % (n, rn)
            ks = []
            for nm, img, quelle in (('Vorlage Bild %d' % n, o, None), ('Runde 3', vids[0], w3), (NEU_TITEL, vids[1], wn)):
                a = cv2.resize(img[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST).copy()
                cs = [c * f + f // 2 for c in cont]
                cv2.drawContours(a, cs, -1, (0, 200, 255), 1)
                if quelle is None:
                    unter = 'Rot eng %.1f %% (Fenster %.1f..%.1f), Farbton %s' % (O, lo, hi, '%.1f Grad' % hO if hO is not None else '-')
                else:
                    r1 = quelle.get(key + 'RV1 Rot eng %')
                    r2 = quelle.get(key + 'RV2 Farbton a*-Klasse Grad')
                    unter = 'Rot eng %.1f %% %s%s, Farbton %s' % (r1['wert'], 'GRUEN' if r1['ok'] else 'ROT', '' if r1.get('bindend', True) else ' (Info)',
                                                                 ('%.1f Grad %s' % (r2['wert'], 'GRUEN' if r2['ok'] else 'ROT')) if r2 else '-')
                if a.shape[1] < 340:                       # schmale Regionen: Kachel rechts mit Grund auffuellen (Schrift lesbar), Bild unveraendert
                    a = np.concatenate([a, np.full((a.shape[0], 340 - a.shape[1], 3), 24, np.uint8)], 1)
                ks.append(KT.kachel(a, '%s, %s' % (nm, rn), unter.replace('.', ','), 'Video roh' if quelle is None else 'Spiellicht + Videofilter Saat 15, roh'))
            bloecke.append(H.reihe(ks))
    t = H.stapel(bloecke)
    kopf = H.kopfzeile('09 Rot wie die Vorlage (MESSBLATT_FARBE_V3_NACHTRAG_ROT 5): je Region Vorlage | Runde 3 | ' + NEU_TITEL + ', gleicher Ausschnitt, '
                       'gleiches Licht und Filter, roh, kein Gamma. Linie = Messregion (rot_masken.npz). Zahlen: RV1/RV2, Kette T, Mittel 3 Saaten.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


def binnenzeichnung(neu, aus):
    modelle = [('RE2-G4 (Typ 0x34, Clip 0)', KT.re2_mo(0x34), 0, 'G4'), ('EM30_HANGAR (Clip 0)', fl.lade_em30(), 0, 'EM30'),
               ('Versuch 2 Runde 2 (Clip 2)', fl.lade_var(V2), 2, 'K5c'), (NEU + ' (Clip 2)', fl.lade_var(neu), 2, 'K5c')]
    zeilen = []
    for art, az, el in (('Flanke', 0, 8), ('Schulter', 45, 15), ('Ruecken', 0, 70)):
        ks = []
        for nm, mo, c, tag in modelle:
            img, msk = R.blick(mo, R.kf(mo, c), az, el, 760, None)
            a = KT._ausschnitt(img, msk, art, tag)
            a = cv2.resize(a, (360, 360), interpolation=cv2.INTER_NEAREST)
            ks.append(KT.kachel(a, nm, None, '%s nah, Licht wie 03, roh' % art))
        zeilen.append(H.reihe(ks))
    t = H.stapel(zeilen)
    kopf = H.kopfzeile('08 Binnenzeichnung nah: Flanke | Schulter | Ruecken, je RE2-G4 | EM30 | Versuch 2 Runde 2 | ' + NEU + '. Gleicher Renderer, '
                       'gleiches Licht (0,55 + 0,45 N.L), Ausschnitt je Modell auf dieselbe Anatomie, roh, kein Gamma.', t.shape[1])
    cv2.imwrite(aus, H.stapel([kopf, t]))


if __name__ == '__main__':
    neu, ausd = sys.argv[1], sys.argv[2]
    os.makedirs(ausd, exist_ok=True)
    nur = sys.argv[3:] or ['01', '02', '03', '04', '05', '06', '08', '09']
    if '07' in nur:
        lg = os.path.join(ausd, 'spiel_lauf_v3.log')
        wege = sorted(set(z.split(':', 1)[1].split('(')[0].strip() for z in open(lg, encoding='utf-8', errors='replace') if z.startswith('Verfahren:'))) if os.path.exists(lg) else []
        KT.spiel(os.path.join(ausd, 'spiel_v3'), os.path.join(ausd, '07_spiel.png'), ' / '.join(wege) or 'Weg unbekannt')
    if '01' in nur:
        szene(1, neu, os.path.join(ausd, '01_szene_bild1.png'))
    if '02' in nur:
        szene(2, neu, os.path.join(ausd, '02_szene_bild2.png'))
    if '03' in nur:
        KT.familie(neu, os.path.join(ausd, '03_familie.png'))
    if '04' in nur:
        atlas(neu, os.path.join(ausd, '04_atlas.png'))
    if '05' in nur:
        KT.rundum(neu, os.path.join(ausd, '05_rundum_neu.png'))
    if '06' in nur:
        kopf_nah(neu, os.path.join(ausd, '06_kopf_nah.png'))
    if '08' in nur:
        binnenzeichnung(neu, os.path.join(ausd, '08_binnenzeichnung.png'))
    if '09' in nur:
        rot_vorlage(neu, os.path.join(ausd, '09_rot_vorlage.png'))
    print('TAFELN-FERTIG', ausd)
