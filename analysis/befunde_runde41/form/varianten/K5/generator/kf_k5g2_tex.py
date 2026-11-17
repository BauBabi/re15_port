#!/usr/bin/env python3
"""K5g2 Textur-Stufe (E3 Grundlage v2 Zone fuer Zone, V1-V6) - Hilfen fuer kf_k5g2.py und riegel_k5g2.py.

Grundsaetze (Auftrag K5g Runde 1, Vorgaben V1-V6):
 - Basis: Original-RE2-G4-Rampen (CDEMD0.EMS Typ 0x34, TIM @0x65e800; kf_g4.g4_roh), Texel/UV wie Spiel-Asset K5e_f1 (= K5c), ausser den
   Flaechen der Fleck-Zonen, die eigene Texel-Kopien bekommen (V3).
 - Je Material EINE Transformation der G4-Rampe: L' = knie(f x L*) (multiplikativ, fuer alle Klassen einschliesslich der Adern), C' = k x C*
   (Buntheit im selben Verhaeltnis wie die G4-Eintraege), ein Farbton h je Material. Keine Eichung je Fleck/Texel am Video.
 - Flecken texelgenau: jede Fleck-Zone (zonen_v2.json `texel`) traegt ihr Material genau auf ihren Texeln und nur auf ihren Traegerflaechen
   (modell_zuordnung.liste). Jede Traegerflaeche bekommt eine eigene Kopie ihrer Texel (freier Texturplatz, UV/Seite umgelegt); in der Kopie
   tragen die Fleck-Texel neue Indizes, die nur in der Fleck-Zeile ihr Material haben. Innerhalb eines Flecks bleibt die Helligkeitsstruktur:
   Material-Tausch je Texel ueber die Rampe (Rang des G4-L* des Texels in der Zone -> gleicher Rang in der Materialrampe).
 - Zeilen: Grund-Zeilen (Haut je G4-Seite, Tausch je G4-Seite, Kralle, F1, Maul, Zahn) und Fleck-Zeilen (Kopie einer Grund-Zeile + Material
   an freien Indizes). Zeilen <= 16, Port-Texturhoehe = Zeilen x 256 <= 4096.
"""
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5e as E  # noqa: E402
import kf_g4 as KG  # noqa: E402

FE = E.FE
K5 = E.K5
ZONEN = os.path.join(K5, 'zonen_nutzer', 'v2', 'zonen_v2.json')
KRALLEN_TEILE = (14, 15, 19, 20, 28, 29, 30, 31)
HAENDE = (13, 18)   # Runde 2: Haende -> Rang-Tausch auf die Hautrampe (kf_k5g2.textur)
FLECK_ART = {'2': 'wunde', '3': 'muster', '4': 'muster', '9': 'muster', '11': 'muster', '12': 'rotfleisch', '14': 'weiss', '15': 'weiss',
             '16': 'braun', '18': 'weiss', '19': 'braun', '24': 'weiss', '27': 'fleisch', '29': 'auge'}
# Vorrang bei Texel-Konflikten zwischen Zonen auf derselben Traegerflaeche ("nach Lage"): 11 vor 12 (zonen_v2: "wo Ihr oranger Umriss
# liegt, gilt Ihr Umriss"); sonst gewinnt die kleinere (genauer gezeichnete) Zone.
VORRANG = {('11', '12'): '11'}


def knie(L, k):
    L = np.asarray(L, float)
    return np.where(L <= k, L, k + (100 - k) * np.tanh((L - k) / (100 - k)))


def schluessel(s):
    return tuple(int(x) for x in s.split('|'))


def lade_zonen():
    return json.load(open(ZONEN, encoding='utf-8'))


def flaeche_maske(f, shape):
    """Texel-Maske einer Flaeche (fillPoly + polylines wie kf_k5e.flaechen_idx), TIM-Koordinaten (x inkl. 128 x Seite)."""
    seite = int(f['page']) & 0xF
    po = seite * 128
    uv = f['uv']
    pts = [uv[0], uv[1], uv[3], uv[2]] if len(uv) == 4 else uv
    p = np.array([[a + po, b] for a, b in pts], np.int32)
    mk = np.zeros(shape, np.uint8)
    cv2.fillPoly(mk, [p], 1)
    cv2.polylines(mk, [p], True, 1)
    return mk.astype(bool)


def alle_flaechen(form):
    for mi, m in enumerate(form['meshes']):
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi, f in enumerate(m[art]):
                yield (mi, vi, fi), f


def gew_quantil(x, w, q):
    o = np.argsort(x, kind='stable')
    cs = np.cumsum(np.asarray(w, float)[o])
    return float(np.interp(q, (cs - 0.5 * np.asarray(w, float)[o]) / cs[-1], np.asarray(x, float)[o]))


def lab_zeile(zeile):
    L = KG.lab(KG.wort_rgb([int(x) for x in zeile]))
    return L


def ton(L):
    return np.degrees(np.arctan2(L[..., 2], L[..., 1]))


def buntheit(L):
    return np.hypot(L[..., 1], L[..., 2])


# ------------------------------------------------------------------ Material je Flaeche
def materialplan(form, X, Z):
    """{flaeche: (art, g4seite)} fuer alle Flaechen der neuen Geometrie.
    art: haut | tausch | hand | kralle | maul | zahn | f1 (hand/kralle werden in kf_k5g2.textur zu tausch). g4seite: G4-Seite der Rampe (Seite 0-2 direkt, Seite 3 ueber die G4-Quellzeile der
    K5c-Zeile, material.json; gespiegelte rechte Krallen: Seite der linken Partnerflaeche)."""
    F, mat, quell = X['F'], X['mat'], X['quell']
    ms = E.material_seite(F, mat, quell)
    zon = {z['nr']: z for z in Z['zonen']}

    def liste(nr):
        return set(schluessel(k) for k in zon[nr]['modell_zuordnung']['liste'])
    mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
    lippen = set(schluessel(k) for nm in ('Lippensaum', 'Maul_Band') for k in mj[nm]['liste'])
    maul, zahn = liste('30'), liste('41')
    zehen = liste('21') | liste('22')
    tausch = liste('31') | liste('40') | liste('42') | liste('45') | lippen | set(k for k, v in mat.items() if v in ('Auge', 'Maulinneres'))
    paare = {19: 14, 20: 15, 29: 28, 31: 30}
    plan = {}
    for k, _f in alle_flaechen(form):
        mi = k[0]
        quelle = (paare[mi], k[1], k[2]) if mi in paare else k
        seite = ms[quelle][1]
        if k in maul:
            art = 'maul'
        elif k in zahn:
            art = 'zahn'
        elif mi not in paare and mat.get(k) == 'F1':
            art = 'f1'
        elif mi in KRALLEN_TEILE or k in zehen:
            art = 'kralle'
        elif mi in HAENDE:
            art = 'hand'
        elif k in tausch:
            art = 'tausch'
        else:
            art = 'haut'
        plan[k] = (art, seite)
    return plan


# ------------------------------------------------------------------ Transformationen
def transform(Lab, f, k, h, kn, modus='L'):
    """EINE Transformation: L' = knie(f L) (multiplikativ fuer alle Klassen), Farbton h, Buntheit weich an den sRGB-Rand:
    modus 'L': C' = k x L' (gleiches Buntheit/Helligkeit-Verhaeltnis fuer alle Eintraege; Haut, Adern, Gelenk-Weiss, Krallen - die roten
               G4-Adern werden nicht roter als die Haut, Nutzer zu K5c "die uebertriebenen roten feuerrillen");
    modus 'C': C' = k x C* (Buntheit im Verhaeltnis der Quelleintraege; Fleisch, braeunliche Musterung)."""
    Lab = np.asarray(Lab, float).reshape(-1, 3)
    Ln = knie(Lab[:, 0] * f, kn)
    C = (Ln if modus == 'L' else buntheit(Lab)) * k
    cm = KG.r4_cmax(Ln, h)
    C = np.where(cm > 1e-6, np.minimum(C, cm * np.tanh(C / np.maximum(cm, 1e-6))), 0.0)
    hr = math.radians(h)
    return np.stack([Ln, C * math.cos(hr), C * math.sin(hr)], 1)


_KOMBI = np.array([[a, b, c] for a in range(-2, 4) for b in range(-2, 4) for c in range(-2, 4)])


def _lab15(c):
    return KG.lab(np.asarray(c, float) * 255.0 / 31.0)


def runde15(ziel, L0, h, fenster, cmin, nie_weiss=False):
    """15-Bit-Wahl je Eintrag (keine neue Farbe, nur der 5-Bit-Nachbar; kf_k5f.runde15_zeile mit einer Klasse): Farbtonfenster h +- fenster
    fuer Kandidaten mit C* >= cmin, Rangfolge nach L0 (G4-L*) erhalten, sonst kleinstes dE76. nie_weiss: 0x7FFF ausgeschlossen (V5)."""
    rgb = KG.rgb_aus_lab(ziel)
    n = len(ziel)
    out = [0] * n
    ii = sorted(range(n), key=lambda i: (L0[i], i))
    vorher, gr_L, gr_max = -1e9, None, -1e9
    for i in ii:
        if gr_L is None or L0[i] > gr_L + 1e-9:
            vorher = max(vorher, gr_max)
            gr_L = L0[i]
        kk = np.unique(np.clip(np.floor(np.asarray(rgb[i], float) * 31.0 / 255.0)[None, :] + _KOMBI, 0, 31), axis=0)
        if nie_weiss:
            kk = kk[~np.all(kk == 31, axis=1)]
        lk = _lab15(kk)
        d = np.linalg.norm(lk - ziel[i][None, :], axis=1)
        C = np.hypot(lk[:, 1], lk[:, 2])
        dh = np.abs((np.degrees(np.arctan2(lk[:, 2], lk[:, 1])) - h + 180.0) % 360.0 - 180.0)
        okf = (C < cmin) | (dh <= fenster)
        okm = lk[:, 0] >= vorher - 1e-6
        for ok in (okf & okm, okf, np.ones(len(kk), bool)):
            if ok.any():
                break
        j = int(np.argmin(np.where(ok, d, d + 1e9)))
        c = kk[j].astype(int)
        w = int(c[0]) | (int(c[1]) << 5) | (int(c[2]) << 10)
        out[i] = w
        gr_max = max(gr_max, float(lk[j, 0]))
    return out


def zeile_transformieren(quelle, f, k, h, kn, rp, nie_weiss=False, modus='L'):
    """Ganze 256er-Zeile (G4- oder K5c-Worte) mit EINER Transformation; 0x0000 bleibt transparent, STP-Bit bleibt."""
    q = [int(x) for x in quelle]
    z0 = np.array(q) != 0
    Lab = lab_zeile(q)
    ziel = transform(Lab, f, k, h, kn, modus)
    idx = [i for i in range(256) if z0[i]]
    w = runde15(ziel[idx], Lab[idx, 0], h, rp['fenster'], rp['cmin'], nie_weiss)
    neu = list(q)
    for j, i in enumerate(idx):
        neu[i] = ((w[j] & 0x7FFF) | (q[i] & 0x8000)) or 0x8000
    return neu


# ------------------------------------------------------------------ Fleck-Zonen: Texel je Traegerflaeche
TRAEGER = os.path.join(K5, 'k5g', 'K5g2', 'traeger_texel.json')


def fleck_zuordnung(Z, form, shape):
    """{traegerflaeche: {(x, y): zone}} - Fleck-Texel je Traegerflaeche aus traeger_texel.json (kf_k5g2_traeger.py: Mehrheitsregel der
    Grundlage je Flaeche und Texel; Vereinigung = zonen_v2.json). Konflikte (zwei Zonen auf derselben Flaeche und demselben Texel): VORRANG,
    sonst die kleinere Zone. Rueckgabe zusaetzlich die Konfliktzaehlung {"a/b": [Texel, Sieger]}."""
    tj = json.load(open(TRAEGER, encoding='utf-8'))['zonen']
    n_tex = {z['nr']: len(z['texel']['liste']) for z in Z['zonen'] if 'texel' in z}
    je = {}
    konflikte = {}
    for nr in sorted(tj, key=float):
        if nr not in FLECK_ART:
            continue
        for fs, tl in tj[nr].items():
            d = je.setdefault(schluessel(fs), {})
            for x, y in tl:
                xy = (int(x), int(y))
                if xy in d and d[xy] != nr:
                    a, b = sorted((d[xy], nr), key=float)
                    sieger = VORRANG.get((a, b)) or (a if n_tex[a] <= n_tex[b] else b)
                    konflikte.setdefault('%s/%s' % (a, b), [0, sieger])[0] += 1
                    d[xy] = sieger
                else:
                    d[xy] = nr
    return je, konflikte


def platzieren(belegt, maske):
    """Erste freie Lage (x0, y0) fuer `maske` (h x w bool) in `belegt` (256 x 512): keine Ueberlappung mit belegt (inkl. 1 Texel Abstand);
    Suche zuerst auf Seite 3 (x >= 384), dann links beginnend. Rueckgabe (x0, y0) oder None."""
    from scipy.signal import fftconvolve
    h, w = maske.shape
    occ = cv2.dilate(belegt.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(float)
    ov = fftconvolve(occ, maske[::-1, ::-1].astype(float), mode='valid')
    frei = ov < 0.5
    ys, xs = np.nonzero(frei)
    if not len(xs):
        return None
    ordn = np.lexsort((ys, xs + np.where(xs >= 384, 0, 1000)))
    for o in ordn:
        x0, y0 = int(xs[o]), int(ys[o])
        if (x0 + w - 1) - 128 * (x0 // 128) <= 255:
            return x0, y0
    return None
