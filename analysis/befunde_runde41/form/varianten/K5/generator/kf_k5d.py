#!/usr/bin/env python3
"""Feinschliff K5d (Nutzer 2026-11-08): Variante auf Basis des Spiel-Assets K5c (16 CLUT-Zeilen), Spiel-Asset bleibt unangetastet.

    python kf_k5d.py <par.json> <aus-Ordner>

Eingaben (fest):
  stufe3/basis_k5c/FORM.EMD/.TIM (3 Posen-Clips, 16 Zeilen; sha256 a2e87fb8... / 7187ff09...)
  re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD (28 Clips, sha256 1ab56076...; MD1 byte-gleich Basis16, FARBE.md E.3)
  feinschliff/messung_v4f_flecken.npz (Fleckmasken der Vorlage, Messblatt V4F 1) - nur ORT der Flecken, keine Farbe.
Ausgaben in <aus>: FORM.EMD/FORM.TIM/form.json (3-Clip-Basis fuer die Kette), BAHNSTEIG_FORM.EMD/.TIM (28 Clips, EDD/EMR/Morph
byte-gleich Asset, nur MD1 neu), bau.json (Bericht: Inseln, Eintraege, Krallen-Ecken, Faktoren).

Verfahren (Messblatt V4F 4 "Wege je Nutzerpunkt"):
 f  Helligkeit: je CLUT-Zeile z ein Faktor f_z = min(g, F_z) auf alle Eintraege (RGB linear in 8 bit, dann 5 bit), F_z = Bedarf je Zeile
    (Messblatt V4F 3.2, groesster Wert mit/ohne Filter ueber Bild 1/2); Leuchtstellen-Zeilen 5 (F1), 6 (Maul-Band) und Zaehne 7 bleiben.
 g  Krallen: Zeile 1 gehoert allein den Krallen (Messblatt 2.1) -> alle Eintraege auf die Handfarbe (Albedo-Mittel der Hand-Meshes nach f,
    a* <= KRALLE_A_MAX). Ueberzaehlige Krallen (Messblatt 2.1: 29/20/19 rechts, 28 links ohne Spitze im Nutzerumriss) werden in die Hand
    eingezogen: alle Ecken auf ZUG x (Ecke - Wurzel) + Wurzel, Wurzel = Mittel der KRALLE_WURZEL_N hand-naechsten Ecken (kleinstes x im
    Krallenraum). Flaechen und Ecken bleiben (keine Loecher, keine entarteten Dreiecke fuer den Software-Renderer).
 a-e Neue Stellen (Schwanzspitze, Gelenke, Lippe, Buckel, Flecken D1-D6): Die Flaechen bekommen eine eigene Texelinsel im freien TIM-Bereich
    (Kopie ihrer Texel, je Zeile und Zusammenhangskomponente, 1 Texel Saum), Indizes auf freie Eintraege ihrer Zeile mit neuer Farbe.
    "ganz": alle Texel der Flaechen; "Fleck": nur die Texel, die im Kennungs-Render von K5c unter der Vorlagen-Fleckmaske liegen.
    Farbe je Ziel: Lab = Ziel + s_L (L - mittel) bzw. s_C (a/b - mittel) - die Binnenzeichnung (Hell/Dunkel) bleibt, Mittel = Ziel.
Geometrie (ausser den Krallen), Knochen, Clips byte-gleich; Zeilen bleiben 16 (Port-Texturhoehe 4096).
"""
import collections
import hashlib
import json
import os
import shutil
import struct
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
FORM = os.path.normpath(os.path.join(K5, '..', '..'))
WT = os.path.normpath(os.path.join(FORM, '..', '..', '..'))
sys.path.insert(0, os.path.join(WT, 're15_port', 'tools'))
sys.path.insert(0, os.path.join(K5, 'farbe_lesung', 'V4F_skripte'))
import form_emd as FE  # noqa: E402
import v4f  # noqa: E402
import gelenke as G  # noqa: E402

BASIS = os.path.join(FORM, 'stufe3', 'basis_k5c')
ASSET = os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM')
FLECKEN = os.path.join(K5, 'feinschliff', 'messung_v4f_flecken.npz')
LIPPE_PKL = os.path.join(K5, 'feinschliff', 'K5d', 'lippe_k5c.pkl')   # kf_k5d_lippe.py an K5c
LEUCHT_ZEILEN = (5, 6, 7)          # F1, Maul-Band, Zaehne (FARBE.md E.1 / Messblatt V4F 4)
Z_KRALLE = 1                        # Messblatt V4F 2.1: Zeile 1 nur Krallen (75/75 Eintraege, 375/375 Texel exklusiv)


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


# ---------------------------------------------------------------- Farbe
def wort_rgb(w):
    w = np.asarray(w, np.int64)
    return np.stack([w & 31, (w >> 5) & 31, (w >> 10) & 31], -1).astype(float) * 255.0 / 31.0


def rgb_wort(rgb, alt):
    """8-bit RGB -> 15 bit, STP-Bit des alten Worts bleibt; 0x0000 (transparent) wird opakes Schwarz 0x8000 (form_emd.clut_wort)."""
    c = np.clip(np.round(np.asarray(rgb, float) * 31.0 / 255.0), 0, 31).astype(int)
    w = int(c[0]) | (int(c[1]) << 5) | (int(c[2]) << 10) | (int(alt) & 0x8000)
    return w if w else 0x8000


def lab(rgb):
    return v4f.lab(np.asarray(rgb, float).reshape(-1, 3)).reshape(-1, 3)


_KOMBI = np.array([[i >> 2 & 1, i >> 1 & 1, i & 1] for i in range(8)])


def lab_wort(ziel_lab, alt, a_max=None, richtung=0):
    """Lab-Ziel -> 15-bit-Wort: von den 8 Abrundungs-/Aufrundungs-Kombinationen je Kanal die mit kleinstem dE76 zum Ziel
    (bei a_max nur Kombinationen mit a* <= a_max, falls vorhanden). richtung -1 (Ader): nur Kombinationen mit L* und a* <= Ziel,
    +1 (Haut): L* und a* >= Ziel, falls vorhanden - die 15-Bit-Rundung verschiebt so dL*/da* Ader - Haut nie zur Ader hin (RV6).
    STP-Bit des alten Worts bleibt."""
    t = np.asarray(ziel_lab, float)
    rgb = rgb_von_lab(t)[0] * 31.0 / 255.0
    k = np.clip(np.floor(rgb)[None, :] + _KOMBI, 0, 31)
    L = lab(k * 255.0 / 31.0)
    d = np.linalg.norm(L - t[None, :], axis=1)
    if richtung:
        ok = (richtung * (L[:, 0] - t[0]) >= -1e-6) & (richtung * (L[:, 1] - t[1]) >= -1e-6)
        if ok.any():
            d[~ok] += 1e6
    if a_max is not None and (L[:, 1] <= a_max).any():
        d[L[:, 1] > a_max] = 1e9
    c = k[int(np.argmin(d))].astype(int)
    w = int(c[0]) | (int(c[1]) << 5) | (int(c[2]) << 10) | (int(alt) & 0x8000)
    return w if w else 0x8000


def rgb_von_lab(L):
    return v4f.rgb_von_lab(np.asarray(L, float).reshape(-1, 3)).reshape(-1, 3)


def ader_klassen(nz):
    """Ader-Eintraege je 16-Zeilen-Index wie riegel_farbe_v3.rv6: G4-Rotklasse (kf_g4.r4_eintraege w > 0) der Basiszeile laut
    farbe/K5c_farbe_v3/bau.json (17-Zeilen-Zaehlung; 16 -> 17: z + 1 fuer z >= 2, FARBE.md E.1). Zeilen F1/Band/Zaehne ohne Klasse."""
    sys.path.insert(0, HIER)
    import kf_g4 as KG
    basis = {0: 0, 1: 1, 2: 2, 3: 0, 4: 1, 5: 2}
    b = json.load(open(os.path.join(K5, 'farbe', 'K5c_farbe_v3', 'bau.json'), encoding='utf-8'))
    basis.update({int(k): int(x) for k, x in b.get('zeilen', {}).get('basis', {}).items()})
    _g4, g4tim, _q = KG.g4_modell()
    _px, cl_g4 = KG.g4_cluts(g4tim)
    w = {p: KG.r4_eintraege(cl_g4[p])[3] for p in range(3)}
    out = {}
    for z in range(nz):
        z17 = z if z < 2 else z + 1
        if z17 in (KG.Z_F1, KG.Z_BAND, KG.Z_ZAHN) or z17 not in basis:
            continue
        out[z] = w[basis[z17]] > 0
    return out


# ---------------------------------------------------------------- Flaechen
def flaechen(form, clut_y):
    """[(key=(mesh, viereck, fi), f-dict, zeile)] in der Reihenfolge von v4f/gelenke."""
    out = []
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                out.append(((mi, vi, fi), f, ((int(f['clut']) >> 6) & 0x1FF) - clut_y))
    return out


def texel_poly(f):
    po = (int(f['page']) & 0xF) * 128
    uv = f['uv']
    if len(uv) == 4:
        pts = [uv[0], uv[1], uv[3], uv[2]]
    else:
        pts = uv
    return np.array([[p[0] + po, p[1]] for p in pts], np.int32)


def texel_maske(f, shape):
    m = np.zeros(shape, np.uint8)
    p = texel_poly(f)
    cv2.fillPoly(m, [p], 1)
    cv2.polylines(m, [p], True, 1)
    return m.astype(bool)


# ---------------------------------------------------------------- Bau
def bauen(par, aus):
    emd_b = open(os.path.join(BASIS, 'FORM.EMD'), 'rb').read()
    tim_b = open(os.path.join(BASIS, 'FORM.TIM'), 'rb').read()
    form = FE.lese_emd(emd_b)
    T = FE.tim_lesen(tim_b)
    px0 = T['pixel'].copy()
    px = T['pixel'].copy()
    cl0 = [list(z) for z in T['cluts']]
    H, W = px.shape
    nz = len(cl0)
    FL = flaechen(form, T['clut_y'])
    pos = {k: i for i, (k, _f, _z) in enumerate(FL)}
    bericht = dict(par=par)

    # --- belegte Texel und benutzte Eintraege je Zeile (wie referenzen.py: fillPoly + polylines)
    tmask = [texel_maske(f, (H, W)) for _k, f, _z in FL]
    belegt = np.zeros((H, W), bool)
    benutzt = np.zeros((nz, 256), bool)
    for (k, f, z), m in zip(FL, tmask):
        belegt |= m
        benutzt[z, np.unique(px0[m])] = True
    frei_eintr = {z: [e for e in range(256) if not benutzt[z, e]] for z in range(nz)}
    bericht['texel_belegt_vorher'] = int(belegt.sum())

    # --- f Helligkeit: je Zeile Lab-Skalierung L*, a*, b* x c_z (gleicher Faktor fuer Haut und Adern einer Zeile -> Vorzeichen von dL*/da*
    # zwischen Ader und Haut bleiben, RV6). a*/b* nur mit 1 + chroma_anteil x (c_z - 1): heller, aber nicht bunter (der Nutzer liest die
    # Figur als grau/blau; volle Skalierung machte den Kopf magenta, Tafel f3 Lauf v10). c_z = min(g, Kappe_z); Kappe_z = L*-Verhaeltnis, das der lineare Zeilenbedarf F_z (Messblatt 3.2)
    # bei L* 15 (Mittel der Figur-Albedo) ergibt. Zeilen 5/6/7 bleiben.
    g = float(par['helligkeit']['g_lab'])
    Fz = {int(k): float(v) for k, v in par['helligkeit']['bedarf_je_zeile'].items()}
    rgb15 = rgb_von_lab([15.0, 0.0, 0.0])[0]
    ader = ader_klassen(nz)
    fz = {}
    cl = [list(z) for z in cl0]
    for z in range(nz):
        if z in LEUCHT_ZEILEN:
            fz[z] = 1.0
            continue
        kappe = float(lab(np.clip(rgb15 * Fz.get(z, 99.0), 0, 255))[0, 0]) / 15.0 if z in Fz else 99.0
        fz[z] = round(min(float(par['helligkeit'].get('g_lab_zeile', {}).get(str(z), g)), kappe), 4)
        for e in range(256):
            if not benutzt[z, e]:
                continue
            L_ = lab(wort_rgb(cl0[z][e]))[0]
            r_ = 0 if z not in ader else (-1 if ader[z][e] else 1)
            kc = 1.0 + float(par['helligkeit'].get('chroma_anteil', 1.0)) * (fz[z] - 1.0)
            cl[z][e] = lab_wort(L_ * np.array([fz[z], kc, kc]), cl0[z][e], richtung=r_)
    bericht['faktor_je_zeile'] = fz

    # --- g Krallen Farbe (Zeile 1 nur Krallen): Handfarbe nach f, a* begrenzt
    kp = par['krallen']
    hand_lab = []
    hand_w = []
    for (k, f, z), m in zip(FL, tmask):
        if k[0] in kp['hand_meshes']:
            idx, c = np.unique(px0[m], return_counts=True)
            hand_lab.append(lab(wort_rgb([cl[z][i] for i in idx])))
            hand_w.append(c)
    hl = np.vstack(hand_lab)
    hw = np.concatenate(hand_w).astype(float)
    hand_mittel = (hl * hw[:, None]).sum(0) / hw.sum()
    ziel = hand_mittel.copy()
    ziel[0] += float(kp.get('dL', 0.0))
    amax = float(kp['a_max'])
    if ziel[1] > amax - 1.5:
        s = (amax - 1.5) / ziel[1]
        ziel[1:] *= s
    e1 = [e for e in range(256) if benutzt[Z_KRALLE, e]]
    w1 = np.zeros(len(e1))
    for (k, f, z), m in zip(FL, tmask):
        if z == Z_KRALLE:
            idx, c = np.unique(px0[m], return_counts=True)
            for i, cc in zip(idx, c):
                w1[e1.index(int(i))] += cc
    L1 = lab(wort_rgb([cl[Z_KRALLE][e] for e in e1]))
    m1 = (L1 * w1[:, None]).sum(0) / w1.sum()
    neu = np.column_stack([ziel[0] + kp['s_L'] * (L1[:, 0] - m1[0]), ziel[1] + kp['s_C'] * (L1[:, 1] - m1[1]),
                           ziel[2] + kp['s_C'] * (L1[:, 2] - m1[2])])
    neu[:, 1] = np.minimum(neu[:, 1], amax - 0.5)
    neu[:, 0] = np.clip(neu[:, 0], 0, 100)
    for e, c in zip(e1, neu):
        cl[Z_KRALLE][e] = lab_wort(c, cl0[Z_KRALLE][e], a_max=amax - 0.5)
    bericht['krallen_farbe'] = dict(hand_albedo_lab=[round(float(x), 2) for x in hand_mittel], ziel_lab=[round(float(x), 2) for x in ziel],
                                    eintraege=len(e1), a_max=amax)

    # --- g Krallen Geometrie: einziehen
    kr = {}
    for mi in kp['einziehen']:
        ecken = np.array(form['meshes'][mi]['ecken'], float)
        o = np.argsort(ecken[:, 0])[:kp['wurzel_n']]
        w = ecken[o].mean(0)
        neue = np.round(w + kp['zug'] * (ecken - w)).astype(int)
        kr[mi] = dict(ecken=len(ecken), wurzel=[round(float(x), 1) for x in w],
                      flaechen=len(form['meshes'][mi]['dreiecke']) + len(form['meshes'][mi]['vierecke']))
        form['meshes'][mi]['ecken'] = [[int(a), int(b), int(c)] for a, b, c in neue]
    bericht['krallen_eingezogen'] = kr

    # --- Ziele der Stellen
    ziele = par['ziele']
    gel, _n = G.gelenk_flaechen(BASIS)
    gel_name = collections.defaultdict(list)
    for key, nm in gel.items():
        gel_name[nm].append(key)

    # Lippe: Kopfhaut-Ziel aus den nicht roten Texeln der Zeile 14 (Kopf/Kiefer), nach f
    if 'lippe' in ziele and ziele['lippe'].get('lab') == 'kopfhaut':
        acc, ww = [], []
        for (k, f, z), m in zip(FL, tmask):
            if z == 14 and k[0] in (22, 23):
                idx, c = np.unique(px0[m], return_counts=True)
                L_ = lab(wort_rgb([cl[z][i] for i in idx]))
                ok = ~((L_[:, 1] > 15) & (np.hypot(L_[:, 1], L_[:, 2]) > 15))     # ohne Rotklasse (Messblatt V4F 2.4: a* > 15, C* > 15)
                acc.append(L_[ok])
                ww.append(c[ok])
        a_ = np.vstack(acc)
        w_ = np.concatenate(ww).astype(float)
        ziele['lippe'] = dict(ziele['lippe'], lab=[round(float(x), 2) for x in (a_ * w_[:, None]).sum(0) / w_.sum()])
    bericht['ziele'] = ziele

    # Zuweisung: je Flaeche (Index in FL) -> {'ganz': ziel} oder {'fleck': {ziel: Texelmaske}}
    ganz = {}
    K_cache = {}
    for gr in par['ganz']:
        keys = []
        for nm in gr.get('gelenke', []):
            if nm not in gel_name:
                raise SystemExit('Gelenk %s unbekannt' % nm)
            keys += gel_name[nm]
        for mi in gr.get('meshes', []):
            keys += [k for k, _f, _z in FL if k[0] == mi]
        if gr.get('lippe'):
            # Lippensaum mit roter Albedo (Messblatt V4F 2.4, lippe_flaechen: 111 von 140 Flaechen); das Maul-Band (Zeile 6) bleibt
            # Leuchtstelle (E30), soweit es in Bild 2 am Unterkiefer (Mesh 23) sichtbar ist (>= band_min_px): dort liegt das Fleischband
            # der Vorlage. Band-Flaechen am Oberkiefer (Mesh 22; Vorlage Oberkiefer dunkel, Messblatt 2.4) und in Bild 2 unsichtbare
            # Band-Reste werden grau-blau.
            import pickle
            lp = pickle.load(open(LIPPE_PKL if gr['lippe'] == 'LIPPE_PKL' else gr['lippe'], 'rb'))
            lk = lp['keys']
            if 2 not in K_cache:
                _s, K_cache[2] = v4f.szene_id(BASIS, 2)
            K2 = K_cache[2]
            sicht = collections.Counter()
            for y, x in zip(*np.nonzero(K2['maske'])):
                sicht[(int(K2['mesh'][y, x]), int(K2['flaeche'][y, x] >= 100000), int(K2['flaeche'][y, x] % 100000))] += 1
            band_bleibt = [k for k, _f, z in FL if z == 6 and k[0] == 23 and sicht[k] >= gr['band_min_px']]
            bericht['band_bleibt'] = dict(flaechen=len(band_bleibt), px_bild2=sum(sicht[k] for k in band_bleibt),
                                          px_bild2_band_gesamt=sum(sicht[k] for k, _f, z in FL if z == 6))
            for i in lp['lc']:
                keys.append(tuple(lk[i]))
            keys += [k for k, _f, z in FL if z == 6 and k not in band_bleibt]
            keys = [k for k in keys if k not in band_bleibt]
        for k in keys:
            i = pos[tuple(k)]
            if FL[i][2] in (5, 7):
                continue
            ganz.setdefault(i, gr['ziel'])
    bericht['ganz_flaechen'] = collections.Counter(ganz.values())

    fleck = collections.defaultdict(dict)      # i -> ziel -> Texelmaske
    fm = np.load(FLECKEN)
    # Leuchtstelle schuetzen (ausser_flecken): Flaechen, die dort sichtbar sind, malen ihre dort sichtbaren Texel nicht; in einer Insel gilt
    # die Sperre fuer alle Mitglieder (gemeinsame Texel). Andere Flaechen mit denselben Texeln bekommen ihre eigene Kopie (Insel).
    sperr_f = {}
    for gr in par['flecken']:
        n = gr['bild']
        if n not in K_cache:
            _s, K_cache[n] = v4f.szene_id(BASIS, n)
        K = K_cache[n]
        sp = np.zeros((480, 640), bool)
        for fid in gr['flecken']:
            sp |= fm[fid]
        if gr.get('dilat', 0):
            d = gr['dilat']
            sp = cv2.dilate(sp.astype(np.uint8), np.ones((2 * d + 1, 2 * d + 1), np.uint8)) > 0
        aus_m = np.zeros((480, 640), bool)
        for fid in gr.get('ausser_flecken', []):
            aus_m |= fm[fid]
        aus_m = cv2.dilate(aus_m.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
        sp &= ~aus_m & K['maske']
        for y, x in zip(*np.nonzero(aus_m & K['maske'])):
            key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
            sperr_f.setdefault(pos[key], np.zeros((H, W), bool))[K['v'][y, x], K['u'][y, x]] = True
        for y, x in zip(*np.nonzero(sp)):
            key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
            if key[0] not in gr['meshes']:
                continue
            i = pos[key]
            if FL[i][2] in (5, 6, 7) and gr.get('ziel') != 'ruecken_glanz' or FL[i][2] in (6, 7):
                continue
            if i in ganz and ganz[i] == 'lippe':
                continue
            m = fleck[i].setdefault(gr['ziel'], np.zeros((H, W), bool))
            m[K['v'][y, x], K['u'][y, x]] = True
    sperr_f = {i: cv2.dilate(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) for i, m in sperr_f.items()}
    for i in list(fleck):
        for zn, m in fleck[i].items():
            m2 = cv2.dilate(m.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
            m2 = cv2.morphologyEx(m2.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8)) > 0
            fleck[i][zn] = m2 & tmask[i]
            if i in sperr_f:
                fleck[i][zn] &= ~sperr_f[i]
    bericht['fleck_flaechen'] = {zn: sum(1 for i in fleck if zn in fleck[i]) for zn in {z for d in fleck.values() for z in d}}

    # --- Inseln je Zeile und Komponente
    alle = sorted(set(ganz) | set(fleck))
    # Inseln getrennt je Zeile UND Klasse (ganz-Ziel bzw. "fleck"): Gelenkkoerper kopieren die UV der Elternflaeche (k_naht.kugel), teilen also
    # Texel mit Gliedflaechen; eine gemeinsame Insel wuerde die Fleckfarbe mit der Gelenkfarbe ueberschreiben.
    je_zeile = collections.defaultdict(list)
    for i in alle:
        je_zeile[(FL[i][2], ganz.get(i, 'fleck'))].append(i)
    frei = ~cv2.dilate(belegt.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)   # 1 Texel Abstand zu allem Belegten
    zuordnung = {}          # (zeile, ziel) -> {alt: neu}
    stufen = {}             # (zeile, ziel) -> {Stufe: neu}
    zaehl = collections.defaultdict(collections.Counter)
    NB = int(par.get('stufen', 20))
    L_zeile = {z: lab(wort_rgb(cl[z]))[:, 0] for z in range(nz)}
    L_rng = {z: (float(L_zeile[z][benutzt[z]].min()), float(L_zeile[z][benutzt[z]].max())) if benutzt[z].any() else (0.0, 100.0)
             for z in range(nz)}
    inseln = []
    for z, klasse in sorted(je_zeile):
        idxs = je_zeile[(z, klasse)]
        U = np.zeros((H, W), np.uint8)
        for i in idxs:
            U |= tmask[i]
        U = cv2.dilate(U, np.ones((3, 3), np.uint8))
        nl, labl, st, _c = cv2.connectedComponentsWithStats(U, connectivity=8)
        for c in range(1, nl):
            x0, y0, w, h = st[c, 0], st[c, 1], st[c, 2], st[c, 3]
            C = (labl[y0:y0 + h, x0:x0 + w] == c)
            mitglieder = [i for i in idxs if (labl[tmask[i]] == c).any()]
            # Maltabelle der Komponente: Ziel je Texel - erst "ganz" (ganze Flaeche, z. B. Gelenkkoerper), darueber die Flecken der Vorlage
            # (genauerer Ort, z. B. der helle Fleck auf dem Gelenk); sonst alter Index
            ziel_t = np.full((h, w), '', object)
            for i in mitglieder:
                if i in ganz:
                    mm = cv2.dilate(tmask[i].astype(np.uint8), np.ones((3, 3), np.uint8))[y0:y0 + h, x0:x0 + w].astype(bool) & C
                    ziel_t[mm] = ganz[i]
            for i in mitglieder:
                if i in fleck:
                    for zn, m in fleck[i].items():
                        mm = m[y0:y0 + h, x0:x0 + w] & C
                        ziel_t[mm] = zn
            for i in mitglieder:
                if i in sperr_f:
                    ziel_t[sperr_f[i][y0:y0 + h, x0:x0 + w] & C] = ''
            # Platz suchen: Komponente ohne Ueberlappung mit belegt/vergeben, Seitenbedingung je Flaeche
            res = cv2.matchTemplate((~frei).astype(np.float32), C.astype(np.float32), cv2.TM_CCORR)
            kand = np.argwhere(res < 0.5)
            ok = None
            for (ny, nx) in kand:
                dx, dy = nx - x0, ny - y0
                seiten = {}
                for i in mitglieder:
                    p = texel_poly(FL[i][1])
                    xa, xb = p[:, 0].min() + dx, p[:, 0].max() + dx
                    s = None
                    for pp in (xa // 128, xa // 128 - 1):
                        if 0 <= pp <= 3 and pp * 128 <= xa and xb <= pp * 128 + 255:
                            s = pp
                            break
                    if s is None:
                        break
                    seiten[i] = s
                else:
                    ok = (dx, dy, seiten)
                    break
            if ok is None:
                raise SystemExit('kein Platz fuer Insel Zeile %d Komponente %d (%dx%d)' % (z, c, w, h))
            dx, dy, seiten = ok
            # Eintraege und Kopie
            alt = px0[y0:y0 + h, x0:x0 + w]
            neu_b = alt.copy()
            for zn in sorted({v for v in ziel_t[C].tolist() if v}):
                tm = C & (ziel_t == zn)
                key = (z, zn)
                zu = zuordnung.setdefault(key, {})
                zb = stufen.setdefault(key, {})
                for a_ in np.unique(alt[tm]):
                    a_ = int(a_)
                    if a_ not in zu:
                        # Stufen: alte Eintraege gleicher Helligkeit (L* nach f, NB Stufen ueber den Zeilenumfang) teilen einen neuen Eintrag
                        b_ = int(np.clip((L_zeile[z][a_] - L_rng[z][0]) / max(L_rng[z][1] - L_rng[z][0], 1e-6) * NB, 0, NB - 1))
                        if b_ not in zb:
                            if not frei_eintr[z]:
                                raise SystemExit('Zeile %d: keine freien Eintraege mehr' % z)
                            zb[b_] = frei_eintr[z].pop(0)
                        zu[a_] = zb[b_]
                neu_b[tm] = np.vectorize(lambda a_: zu[int(a_)])(alt[tm]) if tm.any() else neu_b[tm]
                ua, ca = np.unique(alt[tm], return_counts=True)
                for a_, c_ in zip(ua, ca):
                    zaehl[key][int(a_)] += int(c_)      # Texel je alter Index in den bemalten Inselbereichen (Gewicht der Farbe)
            ziel_px = px[y0 + dy:y0 + dy + h, x0 + dx:x0 + dx + w]
            ziel_px[C] = neu_b[C]
            frei[y0 + dy:y0 + dy + h, x0 + dx:x0 + dx + w][cv2.dilate(C.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)] = False
            for i in mitglieder:
                f = FL[i][1]
                po_alt = (int(f['page']) & 0xF) * 128
                s = seiten[i]
                f['uv'] = [[u + po_alt + dx - s * 128, v + dy] for u, v in f['uv']]
                f['page'] = (int(f['page']) & ~0xF) | s
                assert all(0 <= u <= 255 and 0 <= v <= 255 for u, v in f['uv'])
            inseln.append(dict(zeile=z, klasse=klasse, komponente=c, groesse=[int(w), int(h)], von=[int(x0), int(y0)], nach=[int(x0 + dx), int(y0 + dy)],
                               flaechen=len(mitglieder), texel=int(C.sum()),
                               ziele=dict(collections.Counter(v for v in ziel_t[C].tolist() if v))))
    bericht['inseln'] = inseln

    # --- Farben der neuen Eintraege
    neue_eintraege = {}
    for (z, zn), zu in zuordnung.items():
        zp = ziele[zn]
        # je neuer Eintrag: Mittel (Lab) der alten Eintraege, die ihn speisen, gewichtet mit ihrer Texelzahl in den Inseln
        ne = sorted(set(zu.values()))
        L_ = np.zeros((len(ne), 3))
        w_ = np.zeros(len(ne))
        for j, e in enumerate(ne):
            olds = [a_ for a_ in zu if zu[a_] == e]
            La = lab(wort_rgb([cl[z][a_] for a_ in olds]))
            wa = np.array([max(1, zaehl[(z, zn)][a_]) for a_ in olds], float)
            L_[j] = (La * wa[:, None]).sum(0) / wa.sum()
            w_[j] = wa.sum()
        mitt = (L_ * w_[:, None]).sum(0) / w_.sum()
        t = np.array(zp['lab'], float)
        neu = np.column_stack([t[0] + zp['s_L'] * (L_[:, 0] - mitt[0]), t[1] + zp['s_C'] * (L_[:, 1] - mitt[1]),
                               t[2] + zp['s_C'] * (L_[:, 2] - mitt[2])])
        hell = np.zeros(len(ne), bool)
        if 'zweiton' in zp:
            zt = zp['zweiton']
            o = np.argsort(-L_[:, 0])
            kum = np.cumsum(w_[o]) / w_.sum()
            hell = np.zeros(len(ne), bool)
            summe = 0.0
            for j in o:                       # hellste alte Stufen zuerst, solange der Anteil das Ziel nicht ueberschreitet (+ 5 Punkte)
                if summe + w_[j] / w_.sum() <= zt['anteil'] + 0.05:
                    hell[j] = True
                    summe += w_[j] / w_.sum()
            t2 = np.array(zt['lab'], float)
            if hell.any():
                mh = (L_[hell] * w_[hell, None]).sum(0) / w_[hell].sum()
                neu[hell] = np.column_stack([t2[0] + zp['s_L'] * (L_[hell, 0] - mh[0]), t2[1] + zp['s_C'] * (L_[hell, 1] - mh[1]),
                                             t2[2] + zp['s_C'] * (L_[hell, 2] - mh[2])])
        if 'a_max' in zp:
            neu[:, 1] = np.minimum(neu[:, 1], zp['a_max'])
        neu[:, 0] = np.clip(neu[:, 0], 0, 100)
        for e, c in zip(ne, neu):
            cl[z][e] = lab_wort(c, 0, a_max=zp.get('a_max'))
        neue_eintraege['%d|%s' % (z, zn)] = dict(eintraege=len(ne), alte_indizes=len(zu), alt_mittel_lab=[round(float(x), 1) for x in mitt],
                                                 neu_lab=[[round(float(x), 1) for x in r] for r in neu], gewicht=[int(x) for x in w_],
                                                 zweiton_hell=[bool(x) for x in hell] if 'zweiton' in zp else None)
    bericht['neue_eintraege'] = neue_eintraege
    bericht['freie_eintraege_nachher'] = {z: len(v) for z, v in frei_eintr.items()}

    # --- schreiben
    os.makedirs(aus, exist_ok=True)
    emd = FE.schreibe_emd(form)
    tim = FE.tim_schreiben(px, cl, T['clut_x'], T['clut_y'], T['bild_x'], T['bild_y'])
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(emd)
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(tim)
    fj = json.load(open(os.path.join(BASIS, 'form.json'), encoding='utf-8'))
    fj['name'] = os.path.basename(os.path.normpath(aus))
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    # Asset: 28 Clips, nur MD1 neu (Sektion am Ende vor dem Verzeichnis, Laenge gleich)
    a = open(ASSET + '.EMD', 'rb').read()
    dir_off, d = FE.verzeichnis(a)
    md1_alt = a[d[7]:dir_off]
    md1_neu = FE.md1_schreiben(form)
    if len(md1_neu) != len(md1_alt) or emd_b[FE.verzeichnis(emd_b)[1][7]:FE.verzeichnis(emd_b)[0]] != md1_alt:
        raise SystemExit('MD1 des Assets passt nicht zur Basis16')
    a_neu = a[:d[7]] + md1_neu + a[dir_off:]
    open(os.path.join(aus, 'BAHNSTEIG_FORM.EMD'), 'wb').write(a_neu)
    open(os.path.join(aus, 'BAHNSTEIG_FORM.TIM'), 'wb').write(tim)
    bericht['sha256'] = {n: sha(os.path.join(aus, n)) for n in ('FORM.EMD', 'FORM.TIM', 'BAHNSTEIG_FORM.EMD', 'BAHNSTEIG_FORM.TIM')}
    bericht['eingang_sha256'] = dict(basis_emd=sha(os.path.join(BASIS, 'FORM.EMD')), basis_tim=sha(os.path.join(BASIS, 'FORM.TIM')),
                                     asset_emd=sha(ASSET + '.EMD'), asset_tim=sha(ASSET + '.TIM'))
    bericht['texel_neu'] = int((px != px0).sum())
    bericht['zeilen'] = len(cl)
    json.dump(bericht, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False,
              default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    return bericht


if __name__ == '__main__':
    par = json.load(open(sys.argv[1], encoding='utf-8'))
    b = bauen(par, sys.argv[2])
    print(json.dumps({k: b[k] for k in ('faktor_je_zeile', 'krallen_farbe', 'krallen_eingezogen', 'neue_eintraege', 'freie_eintraege_nachher',
                                        'texel_neu', 'sha256')}, indent=1, default=str))
    print('Inseln', len(b['inseln']))
