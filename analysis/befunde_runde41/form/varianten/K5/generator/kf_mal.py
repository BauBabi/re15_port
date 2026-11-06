#!/usr/bin/env python3
"""K5c Farbe, Runde 1: neue Textur der Riesenform K5c (Geometrie, Clips, UV, Dreiecke unveraendert; nur Texelindizes und CLUTs).
Grundlage MESSBLATT_FARBE_V1.md (Abschnitte 1-4, Riegel Abschnitt 6). Kein RE-Wert: neues Modell, Gestaltung = PORT-WAHL (NUTZER_ENTSCHEIDE E8, E10-E16,
E24, E27 und die Ablehnung 2026-11-06). Jede Zahl unten traegt ihre Quelle.

Aufbau je Texel (Lab D65, OpenCV float):
  L  = L_K5c_grob + K_FEIN * (L_K5c - L_K5c_grob) + G_L * dL_G4 + dL_zone          (Helligkeit: Bild-1/2-Lage aus K5c, Malstruktur aus RE2-G4)
  ab = ab_K5c_grob * G_AB + g_zone * dab_G4 + dab_zone                              (Farbe: Zonenmittel aus der Kette, Adern/Fleisch aus RE2-G4)
  grob = normierte Gauss-Faltung im Atlasfeld, dG4 = G4-Texel minus eigenes Grobbild (nur die gemalte Form, nicht der G4-Grundton).
  dL/dab/g je Zone: Eichung Kette gegen Original (kf_eich.json), Zone eines Texels aus der Sichtkarte kf_sicht.npz, verdeckte Texel aus den sichtbaren
  ergaenzt (E11) durch normierte Faltung im selben Atlasfeld, sonst Mittel der CLUT-Zeile.
Kopfseite (x >= 384): Kopf wie Koerper; Kiefer aus dem G4-Maul (Zahnreihen, Zahnfleisch, dunkles Maulinneres); Zaehne nach Bild-2-Zahnflecken; Krallen aus
den G4-Krallen; Augen dunkel nach E24.
Ausgabe: <aus>/FORM.EMD (Kopie K5c), FORM.TIM (neu), form.json, bau.json (Herkunft jedes Atlasfelds, alle Konstanten).

    python kf_mal.py bauen <aus>
    python kf_mal.py eichen <aus> <runden>          (Kette je Zone messen, kf_eich.json fortschreiben)"""
import json
import os
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5DIR = os.path.normpath(os.path.join(HIER, '..'))
PS = os.path.join(K5DIR, 'farbe_lesung', 'P_skripte')
sys.path.insert(0, PS)
import p_lade as PL  # noqa: E402
from p_messen import atlas as p_atlas  # noqa: E402

FE = PL.FE
K5C = os.path.join(K5DIR, 'K5c')
J_BAU = os.path.join(K5DIR, '..', 'J_dicker', 'bau.json')
EICH = os.path.join(HIER, 'kf_eich.json')
SICHT = os.path.join(K5DIR, 'farbe', 'kf_sicht_k5c.npz')

# ---------------------------------------------------------------- Konstanten (Quelle je Zeile)
# Grobbild: Gauss sigma in Texeln. PORT-WAHL nach E12 ("Screenshot-Farben nur grossflaechig je Bereich, geglaettet entlang der Glieder") und
# Messblatt 1 (gemalte grosse Formen): 2,5 Texel = halbe Breite einer G4-Ader-Gruppe (G4-Atlas, Feld (130,96,20,62): Adern 2-3 Texel, Abstand 5-8).
SIG_GROB = 2.5
# Feinanteil der alten K5c-Struktur. PORT-WAHL: halbe Kraft, damit die H2-Struktur (E16 "sehr viel besser") die G4-Malform nicht ueberdeckt;
# Riegel A6 (HF 0,04-0,26) und A12 (Farbkorn <= 5,7) pruefen das Ergebnis.
K_FEIN = 0.5
# Gewicht der G4-Malform in L*. PORT-WAHL 1,0 = G4-Kontrast unveraendert (Messblatt 1: G4 L*-Streuung 14,9, K5c 12,6).
G_L = 1.0
# Saettigungsverstaerkung des K5c-Grobbilds. Quelle: Messblatt 2, Bild-1-Kette Figur C* Original 9,3 / K5c 6,3 = 1,48 (Farbton von K5c in Bild 1
# richtig, nur zu blass). Lauf v10 mit 1,0: K1 7,7 und N1 Seite 9,0 knapp ROT.
G_AB = 1.48
# Ergaenzung verdeckter Texel (E11): sigma der normierten Faltung im Atlasfeld. PORT-WAHL 6 Texel = 1/2 der Hoehe der kleinsten Glied-Felder
# (J bau.json: klaue_l4 14x17 ... schwanz2 66x26) - laengere Wege fuellt das Feld-/Zeilenmittel.
SIG_ERG = 6.0
# Neutralweiss-Riegel A5 (<= 0,20 %) und Bild-1-Hellklasse K5: L*-Obergrenze Koerper. Quelle: Familie L* p95 30,9-70,6 (Messblatt 1, Tabelle
# "L* Streuung / p5-p95"); 70 = EM30-Haut p95 (70,6), der hellste Familienwert. Darueber wird gestaucht: L = 70 + 0,25 (L - 70).
L_MAX_KOERPER = 70.0
# G4-Quellfelder (RE2 CDEMD0.EMS Typ 0x34, Atlas wie p_lade.re2(0x34)), voll gedeckt (Suche nach gedeckten Rechtecken im G4-Atlas, Dossier FARBE.md 3):
G4_HAUT = [(130, 96, 20, 62), (160, 98, 20, 62), (190, 96, 20, 60), (136, 172, 20, 76), (132, 26, 20, 34), (232, 22, 16, 60),
           (314, 12, 32, 30), (20, 158, 48, 24), (22, 110, 40, 24), (10, 202, 24, 34)]      # dunkle Haut mit roten Adern (Glieder, Hand, Ruecken)
G4_MAUL = (10, 44, 60, 40)          # G4-Maul: Zahnreihen oben/unten, Zahnfleisch, dunkles Inneres (Kopf-Mesh 26), 84 % gedeckt, Rest ergaenzt
G4_KRALLE = (231, 200, 20, 50)      # G4-Krallen (Meshes 4/8/14/19/22/25): Knochen hell -> dunkle Spitze, 82 % gedeckt
# Glaettung der G4-Farbform (a*, b*) vor dem Auftrag. Quelle: Riegel A12 Farbkorn <= 5,7 (Familie 2,70-5,61); v0 ohne Glaettung 6,95 (ROT).
# PORT-WAHL 0,7 Texel: v0 ohne Glaettung 6,95 ROT, v5 mit 0,5 6,06 ROT, 0,7 gruen (v1-v4 3,9-5,7). Die Helligkeitsform bleibt ungeglaettet.
SIG_AB_FEIN = 0.7
# Augenumfeld: Texel, die in Bild 2 innerhalb r <= 6 px um die E24-Augenpunkte (223/271), (288/269) liegen, bekommen die Augenfarbe.
# Quelle: Messblatt 3 / Riegel K10 (Original L* max 22,1 / 13,3; Umgebung Ring 6-12 px L* 17,4 / 15,0 = so dunkel wie das Auge);
# v0: am linken Auge (W) liegt helle Kopfhaut (Kopffeld x 384-386, y 14-16) in der Messscheibe, K10 36,2. r 6 = Innenkante des Umgebungsrings.
AUGE_PUNKTE = ((223, 271), (288, 269))
AUGE_R = 6
K_VERDECKT = 1.0
# Farbfenster (v7 zeigte im neutralen Render Gruen, Tuerkis, Gelb, Blau an Klauen und Rumpf; v8 Magenta-Weiss-Fleck auf dem Ruecken).
# H_MAX 80 = Familie p99 der Texel mit C* > 10 (RE2-G4 79, RE2-G5 84, Hund 64); H_WARM 10 = Familie p5 (G4 8, Hund 1..7, G5 22) und untere
# Grenze des Rotfarbtons A4 (15) mit 5 Reserve; H_MIN -55 = Bild-1-Grundhaut "warm-violett" a* 0-6, b* -1..-8 (Messblatt 2; atan2(-8, 6) = -53);
# H_BLAU -125 = Bild-1-Hinterbein r (W) a* 0,7, b* -7,9 (Farbton -85); das Spiellicht Bild 1 nimmt an den Beinen Blau weg (v11 gemessen:
# Albedo der sichtbaren Beintexel b* -6,3 -> Kette -2,1), darum braucht die Textur dort -100..-125 Grad (Blau, nicht Tuerkis ab ca. -140). C_VIO 14,5 = p75 der Saettigung der Original-Bildpunkte
# Bild 1 (T, Figur) mit Farbton -100..10 Grad (53 % der Figur; p50 10,0, p90 19,3); C_REST 5,5 = p50 der Original-Bildpunkte Bild 1 ausserhalb
# -100..80 Grad (27 % der Figur, p90 12,6). Lauf v9 mit C_VIO 10 / C_REST 4: K1 6,8 und N1 Seite 8,4 ROT. Ausgabe: Herkunft 'farbfenster'.
H_MIN, H_WARM, H_MAX, H_BLAU = -55, 10, 80, -125
C_VIO, C_REST = 14.5, 5.5
# Naehe zur Sicht (0 = in keinem Bild, 1 = gesehen): Anteil sichtbarer Texel im Umfeld sigma SIG_NAEHE, ab 50 % voll. PORT-WAHL 2 Texel Uebergang;
# Lauf v6 mit dem Faltungsgewicht von SIG_ERG (6) galten 83 % der Koerpertexel als gesehen, gezaehlt sichtbar sind 16-22 % (kf_sicht).
SIG_NAEHE = 2.0
# Kopfseite (k_form.ATLAS, K_alligator/generator/k_form.py:11)
F3 = {'kopf': (384, 0, 128, 104), 'kiefer': (384, 106, 128, 72), 'zahn': (384, 180, 24, 14), 'kralle': (410, 180, 100, 40),
      'auge': (384, 196, 24, 10), 'naht': (384, 222, 126, 32)}
# Zahnfarbe: Bild 2 Zahnflecken am Unterkiefer, T Lab (79,6, 1,4, 9,5) (Messblatt 3). Am Zahngrund dunkler (Zahnfleisch), Verlauf ueber die Zahnhoehe.
ZAHN_LAB = (79.6, 1.4, 9.5)
# Augen: Bild 2 an den E24-Punkten T Lab (14,6, 16,4, 2,6) / (12,1, 15,2, 11,1) (Messblatt 3); Mittel als Start, Eichung K10.
AUGE_LAB = (13.4, 15.8, 6.9)

ZONEN = ['b1|Schwanz', 'b1|Ruecken/Flanke', 'b1|Hinterbein l (W)', 'b1|Hinterbein r (W)', 'b1|Vorderglied r (W)',
         'b2|Oberkiefer', 'b2|Kopf/Rumpf', 'b2|Unterkiefer', 'b2|Klaue r (W)', 'b2|Klaue l (W)']


def lab_f(rgb):
    return cv2.cvtColor((np.asarray(rgb, np.float32) / 255.0).reshape(-1, 1, 3), cv2.COLOR_RGB2LAB).reshape(np.shape(rgb))


def rgb_f(L):
    a = cv2.cvtColor(np.asarray(L, np.float32).reshape(-1, 1, 3), cv2.COLOR_LAB2RGB).reshape(np.shape(L))
    return np.clip(a * 255.0, 0, 255)


def texel_karten(mo):
    """je Texel: CLUT-Zeile und Mesh der ersten Flaeche (wie p_lade.sammeln), Zahl der Zeilenkonflikte."""
    h, w = mo.tex.shape
    zeile = np.full((h, w), -1, int)
    mesh = np.full((h, w), -1, int)
    konflikt = 0
    for b, P, T, z in PL.flaechen(mo):
        u = np.array([t[0] for t in T], float)
        v = np.array([t[1] for t in T], float)
        A2 = (u[1] - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (v[1] - v[0])
        if abs(A2) < 1e-9:
            continue
        x0, x1, y0, y1 = int(np.floor(u.min())), int(np.ceil(u.max())), int(np.floor(v.min())), int(np.ceil(v.max()))
        X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        w1 = ((X - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (Y - v[0])) / A2
        w2 = ((u[1] - u[0]) * (Y - v[0]) - (X - u[0]) * (v[1] - v[0])) / A2
        inn = (w1 >= -1e-6) & (w2 >= -1e-6) & (1 - w1 - w2 >= -1e-6)
        xs = np.clip(np.floor(X[inn]).astype(int), 0, w - 1)
        ys = np.clip(np.floor(Y[inn]).astype(int), 0, h - 1)
        neu = zeile[ys, xs] < 0
        konflikt += int(((zeile[ys, xs] >= 0) & (zeile[ys, xs] != z)).sum())
        zeile[ys[neu], xs[neu]] = z
        mesh[ys[neu], xs[neu]] = b
    return zeile, mesh, konflikt


def felder():
    """Atlasfelder: J bau.json (Seiten 0-2) + Kopfseite F3. Name -> (x, y, b, h)."""
    out = {}
    for a in json.load(open(J_BAU))['atlas']:
        out[a['name']] = tuple(a['rechteck'])
    out.update({'s3_' + k: v for k, v in F3.items()})
    return out


def grob(X, m, sig):
    """normierte Gauss-Faltung (nur Texel in m)."""
    mf = m.astype(np.float32)
    n = cv2.GaussianBlur(mf, (0, 0), sig)
    if X.ndim == 2:
        return cv2.GaussianBlur((X * mf).astype(np.float32), (0, 0), sig) / np.maximum(n, 1e-6)
    return np.stack([cv2.GaussianBlur((X[..., c] * mf).astype(np.float32), (0, 0), sig) / np.maximum(n, 1e-6)
                     for c in range(X.shape[2])], -1)


_G4 = {}


def g4_material():
    if not _G4:
        mo = PL.re2(0x34)
        per, _ = PL.sammeln(mo)
        rgb, mid = p_atlas(mo, per)
        _G4['G'] = lab_f(rgb.astype(np.float32))
        _G4['U'] = mid >= 0
    return _G4['G'], _G4['U']


def ausschnitt(G, U, r):
    x, y, w, h = r
    P = G[y:y + h, x:x + w].copy()
    m = U[y:y + h, x:x + w]
    if not m.all():   # Luecken (nicht gedeckte G4-Texel) aus dem Umfeld ergaenzt
        f = grob(P, m, 2.0)
        for _ in range(4):
            f2 = grob(np.where(m[..., None], P, f), np.ones_like(m), 2.0)
            f = np.where(m[..., None], P, f2)
        P = np.where(m[..., None], P, f)
    return P


def malform(P, sig=SIG_GROB, roh=False):
    """G4-Ausschnitt -> nur die gemalte Form (Ausschnitt minus Grobbild); a*, b* mit SIG_AB_FEIN geglaettet (roh=True: ungeglaettet)."""
    d = P - grob(P, np.ones(P.shape[:2], bool), sig)
    if roh:
        return d
    for c in (1, 2):
        d[..., c] = cv2.GaussianBlur(d[..., c].astype(np.float32), (0, 0), SIG_AB_FEIN)
    return d


def naht_reihe(teile, w, achse=1, blend=4):
    """Teile nebeneinander (achse 1) bzw. untereinander (achse 0) mit linearer Ueberblendung ueber 'blend' Texel statt Spiegelung
    (v7: gespiegelte Streifen ergaben symmetrische 'Rorschach'-Adern im neutralen Render). Laenge w."""
    out = teile[0]
    for P in teile[1:]:
        k = min(blend, out.shape[achse], P.shape[achse])
        t = np.linspace(0.0, 1.0, k + 2)[1:-1]
        t = t[None, :, None] if achse == 1 else t[:, None, None]
        if achse == 1:
            mitte = out[:, -k:] * (1 - t) + P[:, :k] * t
            out = np.concatenate([out[:, :-k], mitte, P[:, k:]], 1)
        else:
            mitte = out[-k:] * (1 - t) + P[:k] * t
            out = np.concatenate([out[:-k], mitte, P[k:]], 0)
        if out.shape[achse] >= w:
            break
    return out[:, :w] if achse == 1 else out[:w]


def auf_hoehe(P, h, versatz):
    """auf h Zeilen ohne Skalierung: kuerzer -> laengs mit Ueberblendung angesetzt (zyklisch versetzt), laenger -> Schnitt ab Versatz."""
    if P.shape[0] < h:
        P = naht_reihe([np.roll(P, versatz * j, 0) for j in range(h // max(P.shape[0] - 4, 1) + 2)], h, achse=0)
    y0 = versatz % (P.shape[0] - h + 1)
    return P[y0:y0 + h]


def kacheln_haut(G, U, w, h, start, ton=False, roh=False):
    """Feld b x h aus G4-Hautfeldern, nebeneinander (x = Umfang wie bei J und G4), ueberblendet, nicht gespiegelt; Hoehe ab Versatz
    (7 * Nummer mod Laenge, deterministisch). ton=False: Malform (Ausschnitt minus Grobbild); ton=True: Grobbild (G4-Grundton)."""
    spalten = []
    i = start
    breite = 0
    while breite < w + 8:
        r = G4_HAUT[i % len(G4_HAUT)]
        A = ausschnitt(G, U, r)
        P = grob(A, np.ones(A.shape[:2], bool), SIG_GROB) if ton else malform(A, roh=roh)
        spalten.append(auf_hoehe(P, h, 7 * i))
        breite += P.shape[1] - 4
        i += 1
    return naht_reihe(spalten, w)


def lade_k5c():
    mo = PL.var(K5C)
    tim = FE.tim_lesen(open(os.path.join(K5C, 'FORM.TIM'), 'rb').read())
    per, _ = PL.sammeln(mo)
    rgb, mid = p_atlas(mo, per)
    return mo, tim, rgb, mid


def zonen_feld(sicht, werte, genutzt, zeile, feldmasken):
    """Je Texel Zonenwert: sichtbare Texel tragen ihren Zonenwert (gewichtet mit Bildpunkten), verdeckte aus der normierten Faltung im Atlasfeld,
    sonst halb Feldmittel der sichtbaren Texel, halb Mittel der CLUT-Zeile (E11: Verdecktes aus den sichtbaren Bereichen ergaenzen)."""
    h, w = genutzt.shape
    k = len(next(iter(werte.values())))
    num = np.zeros((h, w, k), np.float32)
    den = np.zeros((h, w), np.float32)
    for z, v in werte.items():
        c = sicht[z].astype(np.float32)
        num += c[..., None] * np.asarray(v, np.float32)
        den += c
    out = np.zeros((h, w, k), np.float32)
    naehe = np.zeros((h, w), np.float32)
    zeilen_mittel = {}
    for zl in np.unique(zeile[genutzt]):
        s = genutzt & (zeile == zl)
        d = den[s].sum()
        zeilen_mittel[zl] = num[s].sum(0) / d if d > 0 else None
    alle = num[genutzt].sum(0) / max(den[genutzt].sum(), 1)
    for fm in feldmasken:
        s = fm & genutzt
        if not s.any():
            continue
        ys, xs = np.nonzero(s)
        y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
        ss = s[y0:y1, x0:x1]
        sn = num[y0:y1, x0:x1] * ss[..., None]
        sd = den[y0:y1, x0:x1] * ss
        gn = np.stack([cv2.GaussianBlur(sn[..., c], (0, 0), SIG_ERG) for c in range(k)], -1)
        gd = cv2.GaussianBlur(sd, (0, 0), SIG_ERG)
        zl = np.bincount(zeile[s]).argmax()
        zm = zeilen_mittel.get(zl)
        fb = alle if zm is None else zm
        if sd.sum() > 0:
            fb = 0.5 * fb + 0.5 * sn.reshape(-1, k).sum(0) / sd.sum()
        wgt = np.clip(gd / 0.05, 0, 1)[..., None]       # 0,05 ~ Gewicht eines einzelnen Bildpunkts nach der Faltung; darunter gilt das Mittel
        val = gn / np.maximum(gd[..., None], 1e-6)
        loc = np.where(sd[..., None] > 0, sn / np.maximum(sd[..., None], 1e-6), wgt * val + (1 - wgt) * fb)
        o = out[y0:y1, x0:x1]
        o[ss] = loc[ss]
        nb = naehe[y0:y1, x0:x1]
        cov = cv2.GaussianBlur((sd > 0).astype(np.float32), (0, 0), SIG_NAEHE) / np.maximum(cv2.GaussianBlur(ss.astype(np.float32), (0, 0), SIG_NAEHE), 1e-6)
        nb[ss] = np.clip(cov / 0.5, 0, 1)[ss]
    return out, naehe


# Glaettung der Fleischkarte im Atlas. PORT-WAHL 1,5 Texel: Bild-1-Lage der Figur weicht vom Original um wenige Bildpunkte ab (Umriss Bild 1
# IoU 0,905, K5.md 6); ohne Glaettung entstuenden Einzeltexel-Inseln (E13 "Mischmasch", Riegel A12).
SIG_FLEISCH = 1.5
# Farbwechsel innerhalb der Zone aus dem Original (E10 "Haut hat mehr als eine Farbe", E11 "aus dem Screenshot uebernehmen"): a*b*-Abweichung vom
# Zonenmittel, voll (1,0), nur Farbe, nicht Helligkeit (keine Glanzpunkte, farbe.md 5). Quelle der Notwendigkeit: Riegel K2 (Bild-1-ab-Streuung
# >= 8,3), v3 7,6 ROT.
K_TON = 1.0


def original_karten(sicht, genutzt, n):
    """Original Bild n (T wie p_zonen.rueck) ueber die Sichtkarte auf die Texel: Gewicht der Rotklasse je Texel, Mittel der Rotklasse, und je Texel
    die Abweichung a*, b* vom Zonenmittel des Originals (Farbwechsel innerhalb der Zone, E10/E11), beides mit SIG_FLEISCH geglaettet."""
    import p_zonen as PZ
    v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
    T, _clip = PZ.rueck(v, n)
    L = PL.lab(np.clip(T.reshape(-1, 3), 0, 255)).reshape(480, 640, 3)
    C = np.hypot(L[..., 1], L[..., 2])
    zone = np.zeros((480, 640), bool)
    dab = np.zeros((480, 640, 2), np.float32)
    for _z, m in PZ.regionen(n):
        zone |= m
        dab[m] = L[m][:, 1:] - L[m][:, 1:].mean(0)
    rot = zone & (L[..., 1] >= 12) & (C >= 15) & (L[..., 0] >= 20)
    tx, ty = sicht['px%d_tx' % n].astype(int), sicht['px%d_ty' % n].astype(int)
    ok = (tx >= 0) & zone
    n_rot = np.zeros(genutzt.shape, np.float32)
    np.add.at(n_rot, (ty[ok & rot], tx[ok & rot]), 1)
    n_alle = np.zeros(genutzt.shape, np.float32)
    np.add.at(n_alle, (ty[ok], tx[ok]), 1)
    s_ab = np.zeros(genutzt.shape + (2,), np.float32)
    np.add.at(s_ab, (ty[ok], tx[ok]), dab[ok])
    da = n_alle > 0
    deck = cv2.GaussianBlur(da.astype(np.float32), (0, 0), SIG_FLEISCH)
    w = grob(np.where(da, n_rot / np.maximum(n_alle, 1), 0), da, SIG_FLEISCH) * (deck > 0.2)
    ab = grob(np.where(da[..., None], s_ab / np.maximum(n_alle, 1)[..., None], 0), da, SIG_FLEISCH) * np.clip(deck / 0.5, 0, 1)[..., None]
    w = np.where(genutzt, np.clip(w, 0, 1), 0)
    ab = np.where(genutzt[..., None], ab, 0)
    return w.astype(np.float32), L[rot].mean(0), ab.astype(np.float32), n_alle


def standard_eich():
    """Startwerte je Zone: dL/da/db = 0, g (G4-Farbform) = 1. Quelle: Messblatt 2 (Lesart je Region); Vorderglied r (W) 0 % Rot (E14) -> g 0."""
    e = {z: dict(d=[0.0, 0.0, 0.0], g=1.0) for z in ZONEN}
    e['b1|Vorderglied r (W)']['g'] = 0.0
    return dict(zonen=e, auge=list(AUGE_LAB), runden=[])


def bauen(aus, eich=None, sicht_pfad=SICHT):
    eich = eich or (json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich())
    mo, tim, rgb, mid = lade_k5c()
    zeile, mesh, konflikt = texel_karten(mo)
    genutzt = zeile >= 0
    h, w = genutzt.shape
    sicht = dict(np.load(sicht_pfad))
    F = felder()
    ordnung = sorted(F, key=lambda n: F[n][2] * F[n][3])          # kleinere Felder zuerst (geteilte Zehen-/Fang-Felder)
    feld_id = np.full((h, w), -1, int)
    for i, n in enumerate(ordnung):
        x, y, ww, hh = F[n]
        m = np.zeros((h, w), bool)
        m[y:y + hh, x:x + ww] = True
        feld_id[(feld_id < 0) & m & genutzt] = i
    rest = genutzt & (feld_id < 0)
    Lk = lab_f(rgb.astype(np.float32))
    G, U = g4_material()
    neu = np.zeros((h, w, 3), np.float32)
    herkunft = {}
    grobK = np.zeros_like(Lk)
    mal = np.zeros_like(Lk)
    g4ton = np.zeros_like(Lk)
    for i, n in enumerate(ordnung):
        s = feld_id == i
        if not s.any():
            continue
        x, y, ww, hh = F[n]
        sub = s[y:y + hh, x:x + ww]
        grobK[y:y + hh, x:x + ww][sub] = grob(Lk[y:y + hh, x:x + ww], sub, SIG_GROB)[sub]
        if n == 's3_kiefer':
            P = ausschnitt(G, U, G4_MAUL)
            P = np.rot90(P, 1)                                     # G4-Zahnreihen laufen laengs; im K5-Kiefer ist laengs = y (k_form.rohr: v laengs)
            P = naht_reihe([np.roll(P, 11 * j, 0) for j in range(5)], ww)   # um den Kiefer (x = Umfang) wiederholt, laengs versetzt, ueberblendet
            P = auf_hoehe(P, hh, 0)                                # Texelmassstab des G4 bleibt (v2: auf 72 Zeilen gestreckt -> A6 Kiefer 0,028 zu glatt)
            mal[y:y + hh, x:x + ww][sub] = P[sub]                  # hier das ganze G4-Bild (Ton + Form), nicht nur die Malform
            herkunft[n] = 'RE2-G4 Maul %s, 90 Grad gedreht, um den Umfang versetzt wiederholt und ueberblendet, G4-Texelmassstab; + Zonenwert (L nur sichtbar)' % (G4_MAUL,)
        elif n == 's3_kralle':
            P = ausschnitt(G, U, G4_KRALLE)
            P = naht_reihe([np.roll(P, 9 * j, 0) for j in range(7)], ww)
            P = auf_hoehe(P, hh, 5)
            mal[y:y + hh, x:x + ww][sub] = P[sub]
            herkunft[n] = 'RE2-G4 Krallen %s, um den Umfang versetzt wiederholt und ueberblendet, %d Zeilen, G4-Texelmassstab; + Zonenwert' % (G4_KRALLE, hh)
        elif n == 's3_zahn':
            herkunft[n] = 'gemalt: Zahnfarbe Bild 2 %s (Messblatt 3), Verlauf 100 -> 60 %% L* zum Zahngrund' % (ZAHN_LAB,)
        elif n == 's3_auge':
            herkunft[n] = 'gemalt: Augenfarbe E24 (Messblatt 3), geeicht K10: %s' % (eich['auge'],)
        else:
            P = kacheln_haut(G, U, ww, hh, i)
            mal[y:y + hh, x:x + ww][sub] = P[sub]
            g4ton[y:y + hh, x:x + ww][sub] = kacheln_haut(G, U, ww, hh, i, ton=True)[sub]
            herkunft[n] = 'K5c-Grobbild (sigma %.1f) + %.1f x K5c-Fein + RE2-G4-Malform (Hautfelder ab Nr. %d nebeneinander, ueberblendet) + Zonenwert' % (
                SIG_GROB, K_FEIN, i % len(G4_HAUT))
    zw = {z: list(v['d']) + [v['g']] for z, v in eich['zonen'].items()}
    Z, naehe = zonen_feld(sicht, zw, genutzt, zeile, [feld_id == i for i in range(len(ordnung))] + [rest])
    dz, gz = Z[..., :3].copy(), Z[..., 3]
    # Kiefer: die Helligkeit des sichtbaren Bandes (Bild 2 Unterkiefer, Lab 56,8/24,9/15,2) gilt nur dort; der verdeckte Teil (Maulinneres, Unterseite)
    # bleibt so dunkel wie das G4-Maul. Quelle: Riegel A8 (Dunkel >= 20 %, Familie 24-64 %); v2 mit Feldmittel ueberall: Dunkel 12,2 % (ROT).
    kf = feld_id == ordnung.index('s3_kiefer')
    dz[..., 0][kf] *= naehe[kf] ** 3     # hoch 3: nur das Band selbst (v4 mit naehe^1: Dunkel 18,8 %, Zahn 25,4 % ROT); PORT-WAHL
    neu[..., 0] = grobK[..., 0] + K_FEIN * (Lk[..., 0] - grobK[..., 0]) + G_L * mal[..., 0] + dz[..., 0]
    neu[..., 1] = grobK[..., 1] * G_AB + gz * mal[..., 1] + dz[..., 1]
    neu[..., 2] = grobK[..., 2] * G_AB + gz * mal[..., 2] + dz[..., 2]
    # Verdeckte Texel (in keinem Screenshot): G4-Grundton a*, b* dazu (Rot-/Fleischflaechen der G4-Haut), mit (1 - naehe) eingeblendet.
    # (Lauf v6b mit zusaetzlich ungeglaetteter G4-Aderfarbe: A3 17,1 %, aber Farbkorn A12 7,0 ROT - verworfen.)
    # Quelle: Messblatt 1 "RE2-G4 ... 24 % warmes Rot in vier Stufen. Das ist das Vorbild ... es fehlen die Rotstufen" und Riegel A3 (8-40 %);
    # 74 % der Koerpertexel sind in keinem Bild sichtbar (Messblatt Messgrenzen). Sichtbare Texel bleiben an der Kette geeicht. PORT-WAHL K_VERDECKT 1,0.
    neu[..., 1:] += K_VERDECKT * (1.0 - naehe)[..., None] * g4ton[..., 1:]
    # Die a*b*-Zonenwerte sind Ausgleich fuer das, was die Kette an sichtbaren Texeln zeigt (Adern/Fleisch gegen Zonenmittel); verdeckte Texel
    # tragen sie nur mit naehe (sonst zoege der Bild-1-Ausgleich a* -3 die G4-Rotstufen der verdeckten Seite unter die Rotschwelle, Lauf v6: a* Median 0).
    neu[..., 1:] -= (1.0 - naehe)[..., None] * dz[..., 1:]
    for n in ('s3_kiefer', 's3_kralle'):
        s = feld_id == ordnung.index(n)
        neu[s] = mal[s] + dz[s]
    # Fleischstellen Bild 1 (E11, E14: Fleischrot nur, wo der Screenshot Rot/Rosa zeigt): Bildpunkte der Pruefer-Rotklasse im Original Bild 1
    # (T: a* >= 12, C* >= 15, L* >= 20; Messblatt 2) ueber die Sichtkarte auf ihre Texel; Gewicht = Anteil roter Bildpunkte je Texel, im Atlas
    # mit SIG_FLEISCH geglaettet. Farbe = Mittel dieser Original-Bildpunkte (T) plus Eichwert 'fleisch'.
    lo = grob(neu, genutzt, SIG_GROB)
    for n in (1, 2):
        wf, fl_lab, ab, n_alle = original_karten(sicht, genutzt, n)
        anteil = n_alle / np.maximum(sicht['fig1'] + sicht['fig2'], 1)          # Texel in beiden Bildern: je Bild nach Bildpunkten gewichtet
        fl = np.asarray(fl_lab, np.float32) + np.asarray(eich.get('fleisch%d' % n, [0.0, 0.0, 0.0]), np.float32)
        if n == 1:
            # Fleischstellen nur nach Bild 1: in Bild 2 ist die Rotklasse zum Teil Feuerlicht/Einstreuung (Messblatt 2 letzter Punkt, farbe.md 4);
            # Lauf v4 mit Bild-2-Fleisch: Kette Bild 2 Rot 42 % gegen 34 % und Atlas-Rot A3 kleiner (Zonenmittel zog a* zurueck).
            neu += float(eich.get('fleisch_k', 1.0)) * wf[..., None] * (fl[None, None, :] - lo)
        neu[..., 1:] += K_TON * ab * np.where(n_alle > 0, anteil, 1.0)[..., None]
        herkunft['fleisch_bild%d' % n] = '%d Texel mit Gewicht > 0,1 (Rotklasse Original Bild %d), Farbe T %s, Staerke %s' % (
            int((wf > 0.1).sum()), n, [round(float(v), 1) for v in fl_lab], eich.get('fleisch_k', 1.0) if n == 1 else 0)
        herkunft['ton_bild%d' % n] = '%d Texel mit a*b*-Abweichung vom Zonenmittel des Originals Bild %d (x %.1f)' % (int((np.abs(ab).sum(2) > 0.5).sum()), n, K_TON)
    # Zaehne: Verlauf ueber die Zahnhoehe (k_form.zahn: Grund y0+h-1, Spitze y0+1) von 100 % (Spitze) auf 60 % (Grund) der Zahnhelligkeit.
    # PORT-WAHL; Quelle der Spanne: G4-Zaehne im Maulfeld (10,44,60,40) L* 45-80 vom Grund zur Spitze (gemessen am G4-Atlas, Dossier FARBE.md 3)
    x, y, ww, hh = F3['zahn']
    t = np.linspace(1.0, 0.6, hh)[:, None] * np.ones((1, ww))
    neu[y:y + hh, x:x + ww, 0] = ZAHN_LAB[0] * t
    neu[y:y + hh, x:x + ww, 1] = ZAHN_LAB[1]
    neu[y:y + hh, x:x + ww, 2] = ZAHN_LAB[2]
    x, y, ww, hh = F3['auge']
    neu[y:y + hh, x:x + ww] = np.asarray(eich['auge'], np.float32)
    zf0 = feld_id == ordnung.index('s3_zahn')
    yy, xx = np.mgrid[:480, :640]
    n_auge = 0
    for (px_, py_) in AUGE_PUNKTE:
        d = ((xx - px_) ** 2 + (yy - py_) ** 2 <= AUGE_R ** 2) & (sicht['px2_tx'] >= 0)
        tx_, ty_ = sicht['px2_tx'][d].astype(int), sicht['px2_ty'][d].astype(int)
        ok = genutzt[ty_, tx_] & ~zf0[ty_, tx_]
        neu[ty_[ok], tx_[ok]] = np.asarray(eich['auge'], np.float32)
        n_auge += int(ok.sum())
    herkunft['augenumfeld'] = '%d Texel unter r <= %d px um E24 in Bild 2 -> Augenfarbe' % (n_auge, AUGE_R)
    # E9 Schwanz ohne Teilstuecke: mittlere Helligkeit der Glieder 1-5 (Mesh 1-5) auf die Ausgleichsgerade ueber die Glieder gezogen
    # (Riegel A11: Sprung <= 4; v4 4,1 ROT durch Fleischstellen auf einzelnen Gliedern). Glied 6 (Spitze) frei (rosa-weisses Ende Bild 1).
    gm = [float(neu[..., 0][genutzt & (mesh == g) & (np.arange(w)[None, :] < 384)].mean()) for g in range(1, 6)]
    k_, d_ = np.polyfit(np.arange(5), gm, 1)
    for i, g in enumerate(range(1, 6)):
        sg = genutzt & (mesh == g) & (np.arange(w)[None, :] < 384)
        neu[..., 0][sg] += (k_ * i + d_) - gm[i]
    herkunft['schwanz_e9'] = 'Glied-L* vorher %s, Gerade %.2f + %.2f i' % ([round(v, 1) for v in gm], d_, k_)
    # Farbfenster der Familie (je Texel, nach Farbton h und Saettigung C*):
    #   h in [H_WARM, H_MAX]            : frei (Fleisch, Rotbraun, Ocker der Familie)
    #   h in [H_MIN, H_WARM), C* > C_VIO: Farbton auf H_WARM gedreht (Magenta-Rosa -> Fleischrosa; Messblatt 5.2: K5c-Rot -10 Grad = Magenta-Fehler)
    #   h in [H_BLAU, H_MIN)            : C* hoechstens C_VIO (Violett-/Blaugrau der Bild-1-Grundhaut)
    #   sonst (Tuerkis, Gruen, Gelbgruen): C* hoechstens C_REST
    C_ = np.hypot(neu[..., 1], neu[..., 2])
    H_ = np.degrees(np.arctan2(neu[..., 2], neu[..., 1]))
    dreh = genutzt & (H_ >= H_MIN) & (H_ < H_WARM) & (C_ > C_VIO)
    blau = genutzt & (H_ >= H_BLAU) & (H_ < H_MIN) & (C_ > C_VIO)
    rest_ = genutzt & ((H_ < H_BLAU) | (H_ > H_MAX)) & (C_ > C_REST)
    neu[..., 1] = np.where(dreh, C_ * np.cos(np.radians(H_WARM)), neu[..., 1])
    neu[..., 2] = np.where(dreh, C_ * np.sin(np.radians(H_WARM)), neu[..., 2])
    for m_, cmax in ((blau, C_VIO), (rest_, C_REST)):
        f_ = np.where(m_, cmax / np.maximum(C_, 1e-6), 1.0)
        neu[..., 1] *= f_
        neu[..., 2] *= f_
    herkunft['farbfenster'] = 'gedreht %d, Blau/Violett begrenzt %d, Tuerkis/Gruen/Gelbgruen begrenzt %d Texel (von %d)' % (
        int(dreh.sum()), int(blau.sum()), int(rest_.sum()), int(genutzt.sum()))
    zf = feld_id == ordnung.index('s3_zahn')
    s = genutzt & ~zf
    Lc = neu[..., 0]
    Lc[s] = np.where(Lc[s] > L_MAX_KOERPER, L_MAX_KOERPER + 0.25 * (Lc[s] - L_MAX_KOERPER), Lc[s])
    neu[..., 0] = np.clip(Lc, 0, 100)
    out_rgb = rgb_f(neu)
    out_rgb[~genutzt] = 0
    return schreiben(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, dict(konflikt_zeilen=konflikt, felder=len(ordnung), rest_texel=int(rest.sum())))


def palette(rgb, n=255, seed=7):
    """k-Mittel in Lab je CLUT-Zeile, 5-Bit-CLUT. PORT-WAHL: 255 = volle 8-bpp-Zeile (Index 0 transparent), wie K5c/J."""
    L = lab_f(rgb.reshape(-1, 1, 3).astype(np.float32)).reshape(-1, 3)
    cv2.setRNGSeed(seed)
    k = min(n, len(np.unique(np.round(rgb).reshape(-1, 3), axis=0)))
    _, lb, c = cv2.kmeans(L.astype(np.float32), k, None, (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 50, 0.1), 2, cv2.KMEANS_PP_CENTERS)
    crgb = rgb_f(c.reshape(-1, 1, 3)).reshape(-1, 3)
    return crgb, lb.ravel()


def schreiben(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, info):
    os.makedirs(aus, exist_ok=True)
    h, w = genutzt.shape
    px = np.zeros((h, w), np.uint8)
    cl = []
    for zl in range(len(tim['cluts'])):
        s = genutzt & (zeile == zl)
        c, lb = palette(out_rgb[s])
        px[s] = (lb + 1).astype(np.uint8)
        cl.append(FE.clut_aus_rgb([(0, 0, 0)] + [tuple(int(round(v)) for v in f) for f in c], transparent0=True))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, cl, tim['clut_x'], tim['clut_y'], tim['bild_x'], tim['bild_y']))
    shutil.copy(os.path.join(K5C, 'FORM.EMD'), os.path.join(aus, 'FORM.EMD'))
    fj = json.load(open(os.path.join(K5C, 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe'
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    eich_kurz = {k: v for k, v in eich.items() if k != 'runden'}       # Eichverlauf steht in generator/kf_eich.json
    bau = dict(basis='K5c (varianten/K5/K5c): FORM.EMD byte-gleich, nur FORM.TIM neu', eich=eich_kurz, herkunft=herkunft, info=info,
               konstanten=dict(H_MIN=H_MIN, H_WARM=H_WARM, H_MAX=H_MAX, H_BLAU=H_BLAU, C_VIO=C_VIO, C_REST=C_REST, SIG_NAEHE=SIG_NAEHE, K_VERDECKT=K_VERDECKT, SIG_FLEISCH=SIG_FLEISCH, K_TON=K_TON, SIG_GROB=SIG_GROB, SIG_AB_FEIN=SIG_AB_FEIN, AUGE_R=AUGE_R, K_FEIN=K_FEIN, G_L=G_L, G_AB=G_AB, SIG_ERG=SIG_ERG, L_MAX_KOERPER=L_MAX_KOERPER, G4_HAUT=G4_HAUT,
                               G4_MAUL=G4_MAUL, G4_KRALLE=G4_KRALLE, ZAHN_LAB=ZAHN_LAB, AUGE_LAB=AUGE_LAB))
    json.dump(bau, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    return bau


# ---------------------------------------------------------------- Eichung gegen die Kette (dieselbe wie p_riegel / p_zonen)
def kette_messen(vdir):
    import p_zonen as PZ
    k = PZ.P.Kandidat(os.path.join(vdir, 'form.json'))
    res = {}
    for n in (1, 2):
        sauber, alb, m, kk = k.rendern(n)
        fig = m & (alb.max(2) > 0)
        vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True)
        T, clip = PZ.rueck(vid, n)
        res[n] = PZ.messe(T, clip, PZ.regionen(n), fig)
        res[n]['_T'] = T
    return res


def original_messen():
    import p_zonen as PZ
    res = {}
    for n in (1, 2):
        v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        T, clip = PZ.rueck(v, n)
        res[n] = PZ.messe(T, clip, PZ.regionen(n))
    return res


# Begrenzung der Eichwerte (gegen Aufschaukeln, wenn das Farbfenster eine Zone kappt; v11: Hinterbein-Werte liefen auf b* -35/-43 ohne Wirkung).
# L* 45 = groesster gebrauchter Wert (Unterkiefer-Band +40,8, v11); a*, b* 20 = groesster gebrauchter Wert ohne Kappung (Unterkiefer +16,9).
D_MAX = (45.0, 20.0, 20.0)


def eichen(aus, runden, daempf=0.8):
    """Je Runde: bauen, Kette messen, je Zone d += daempf * (Original - Kette) in L*, a*, b*; g so, dass der Rotanteil (Pruefer-Klasse R) dem
    Original folgt (g *= sqrt((R_orig + 1) / (R_kette + 1)), Grenzen 0..2,5); Auge: L* gegen K10.
    daempf 0,8: PORT-WAHL gegen Ueberschwingen (die Kette ist nicht linear: Videofilter, Rauschen, 5-Bit-CLUT)."""
    eich = json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich()
    O = original_messen()
    for r in range(runden):
        bauen(aus, eich)
        V = kette_messen(aus)
        prot = {}
        for z in ZONEN:
            n = 1 if z.startswith('b1') else 2
            zn = z[3:]
            o, v = O[n][zn], V[n][zn]
            e = eich['zonen'][z]
            e['d'] = [round(float(np.clip(e['d'][i] + daempf * (o['lab'][i] - v['lab'][i]), -D_MAX[i], D_MAX[i])), 2) for i in range(3)]
            if z.startswith('b2|Klaue'):
                e['d'][2] = 0.0      # Klauen b*: Feuerschein, kein Riegel (Messblatt 6, K9) -> nicht geeicht (v7: b* +9..+11 machte die Krallen gelb)
            if z != 'b1|Vorderglied r (W)':
                e['g'] = round(float(np.clip(e['g'] * ((o['kl_p']['R'] + 1) / (v['kl_p']['R'] + 1)) ** 0.5, 0, 2.5)), 3)
            prot[z] = dict(orig=o['lab'], kette=v['lab'], R=(o['kl_p']['R'], v['kl_p']['R']), H=(o['kl_p']['H'], v['kl_p']['H']),
                           neu=dict(d=list(e['d']), g=e['g']))
        yy, xx = np.mgrid[:480, :640]
        aug = []
        for (x, y) in ((223, 271), (288, 269)):      # E24-Augenpunkte, Scheibe r 4 px (Riegel K10)
            mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
            aug.append(float(PL.lab(np.clip(V[2]['_T'][mm], 0, 255))[:, 0].max()))
        if max(aug) > 26.0:      # 26 = Riegel K10 (30) minus 4 Reserve fuer das Videorauschen (PORT-WAHL)
            eich['auge'][0] = round(max(4.0, eich['auge'][0] - 0.8 * (max(aug) - 22.1)), 2)
        # Staerke der Bild-1-Fleischstellen nach der Rotklasse der ganzen Figur (Riegel K4: 0,6-1,5 x Original 5,2 %; Ziel = Original)
        ro, rv = O[1]['Figur']['kl_p']['R'], V[1]['Figur']['kl_p']['R']
        eich['fleisch_k'] = round(float(np.clip(eich.get('fleisch_k', 1.0) * ((ro + 1) / (rv + 1)) ** 0.5, 0.2, 1.5)), 3)
        prot['fleisch_k'] = eich['fleisch_k']
        prot['auge_Lmax'] = aug
        prot['figur'] = {n: (V[n]['Figur']['lab'], V[n]['Figur']['chroma'], V[n]['Figur']['ab_streu'], V[n]['Figur']['kl_p']) for n in (1, 2)}
        eich['runden'].append(prot)
        print('Runde %d' % r, flush=True)
        for z, p in prot.items():
            print('  ', z, p, flush=True)
        json.dump(eich, open(EICH, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    bauen(aus, eich)
    return eich


if __name__ == '__main__':
    if sys.argv[1] == 'bauen':
        b = bauen(sys.argv[2])
        print(json.dumps(b['info']))
    elif sys.argv[1] == 'eichen':
        eichen(sys.argv[2], int(sys.argv[3]))
