#!/usr/bin/env python3
"""K5c Farbe, Runde 2: neue Textur der Riesenform K5c (Geometrie, Clips, UV, Dreiecke unveraendert; nur Texelindizes und CLUTs).
Grundlage MESSBLATT_FARBE_V1.md und die Pruefberichte der Runde 1 (FARBE_PRUEFUNG_r1_sicht.md P1-P15, FARBE_PRUEFUNG_r1_zahlen.md 4 und 6).
Kein RE-Wert: neues Modell, Gestaltung = PORT-WAHL (NUTZER_ENTSCHEIDE E8-E16, E24, E27 und die Ablehnung 2026-11-06). Jede Zahl unten traegt
ihre Quelle (Messblatt-Zeile, Datei + Koordinaten, Familienmessung) oder ist ein Verfahrensparameter, der das Ergebnis nicht bestimmt
(Abbruchgrenzen, Saatwerte; im Dossier FARBE.md Abschnitt 2 je Name belegt).

Unterschiede zu Runde 1 (kf_mal.py bleibt fuer den Nachbau von Runde 1 unveraendert):
  1. Glanz (Zahlen-Pruefer 6.1, Messblatt 5.4): die rueckprojizierten Weissflecken werden aus dem K5c-Ausgangsbild ENTFERNT (harmonisch aus der
     Umgebung gefuellt), nicht oberhalb L* 70 gestaucht. Ebenso der Gaumenstreifen im Kopffeld (Zahlen-Pruefer 6.6, Sicht-Pruefer P4, Messblatt 3).
  2. Rot als Flaeche (Sicht-Pruefer P1): Fleischflaechen dort, wo Bild 1 oder Bild 2 Rot zeigen (E11, E14), als zusammenhaengende Partien aus der
     RE2-G4-Fleischtextur (Ton + Form, nicht nur die Malform). Die G4-Adern bleiben als Richtung in der Haut, aber DUNKLER als die Haut
     (EM30-Regel, Sicht-Pruefer P1: EM30-Adern L* 23 gegen Haut 27) und unter der Rotklasse (C* <= 1,5 L*, G4-Dunkelrot).
  3. Kein Blau (Sicht-Pruefer P2): Koerpertexel b* >= -6 (Grenze der Blau-Klasse); das offene Farbfenster -125..-55 Grad entfaellt.
  4. Rot-Farbton (Zahlen-Pruefer 1, A4 knapp): Magenta wird auf 25 Grad gedreht (EM30-Rot, unterster Familienwert), nicht auf 10.
  5. Keine Konstante ohne Quelle (Zahlen-Pruefer 4): Kachelversaetze und Kopienzahl aus Quell- und Feldmass, Ergaenzung verdeckter Texel harmonisch
     (Laplace, ohne Radius), keine Aderstaerke-Eichung g, keine Klemmen; die Eichung hat nur noch den Zonenversatz (Klauen nur a* nach oben)
     und die Augenhelligkeit. Fleischschwelle fest 0,5 (Mehrheit), keine Farbkraft-Eichung.
  6. Zone = Bauteil: die Bildzone Unterkiefer wirkt nur auf das Kieferfeld (Runde 1 hellte ueber sie die rechte Pfote auf, Sicht-Pruefer P3).
  7. Unterseite (verdeckt, 23,9 % = G4-Fleischanteil) in der dunklen G4-Rotstufe; CLUT mit gerundeten 5-Bit-Stufen und Blau-Regel.

    python kf_mal2.py bauen <aus>
    python kf_mal2.py eichen <aus> <runden>"""
import json
import os
import shutil
import sys

import cv2
import numpy as np
from scipy.sparse import coo_matrix
from scipy.sparse.linalg import spsolve

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_mal as M1  # noqa: E402   (Lader, Atlasfelder, G4-Material, Palette, Kette aus Runde 1; unveraendert)

PL = M1.PL
FE = M1.FE
K5C = M1.K5C
EICH = os.path.join(HIER, 'kf_eich2.json')
SICHT = M1.SICHT                      # Sichtkarte K5c (kf_sicht.py), gleiche UV wie die Variante

# ---------------------------------------------------------------- Konstanten (Quelle je Zeile)
SIG_GROB = M1.SIG_GROB                # 2,5 Texel: halbe Breite einer G4-Adergruppe (kf_mal.py:40-42, G4-Atlas (130,96,20,62))
G_AB = M1.G_AB                        # 1,48 = Bild-1-Kette C* Original 9,3 / K5c 6,3 (Messblatt 2)
SIG_AB_FEIN = M1.SIG_AB_FEIN          # 0,7 Texel Glaettung der G4-Farbform: PORT-WAHL E13 ("Pixel-Mischmasch muss weg"), Wert aus Lauf v0-v5 (Riegel A12)
# Glanzflecken: Keim = Neutralweiss nach Riegel A5 (L* > 85, C* < 10); Hof = angrenzende Texel heller als das hellste Original-Band.
# Quelle 58: Messblatt 2 "Helle Baender (H): grau, T L* 51-58" - was im Atlas heller ist als das hellste Band im Screenshot, gehoert zum Fleck.
GLANZ_L, GLANZ_C, HOF_L = 85.0, 10.0, 58.0
# Gaumenstreifen im Kopffeld: Zahnklasse des Messblatts 3 (L* > 60, C* < 35; K5c-Kopf 15,1 % "Gaumenstreifen"). Der Kopf traegt keine Zaehne
# (eigenes Zahnfeld F3['zahn']), deshalb wird die Klasse im Kopffeld entfernt.
GAUMEN_L, GAUMEN_C = 60.0, 35.0
# Adern dunkler als die Haut: EM30-Adern L* 23 gegen Haut 27 im Render (Sicht-Pruefer r1 P1) -> 23/27 = 0,85.
ADER_L = 23.0 / 27.0
# Aderfarbe hoechstens so bunt wie das G4-Dunkelrot (Messblatt 1, G4-Palette (18,9/25,6/12,2): C* 28,4 bei L* 18,9 -> C*/L* 1,50).
ADER_CL = float(np.hypot(25.6, 12.2) / 18.9)
# Rotklasse der Fleischtexel und der G4-Adern: Riegel A3 (a* > 15, C* > 20, Farbton -40..70 Grad).
# Fleischflaechen aus RE2-G4 (CDEMD0.EMS Typ 0x34, Atlas wie p_lade.re2(0x34)), voll gedeckte Rechtecke mit hohem Rotanteil (Suche im G4-Atlas,
# Dossier FARBE.md 3): (181,224,20,30) rosa Fleisch mit Faltenstraengen, Rot 0,84, Lab (33,4/24,6/11,4); (179,229,24,24) Rot 0,89, Lab (37,2/25,6/11,6).
# Fleischfarbe der Screenshot-Fleischstellen: Form aus diesen G4-Quellen, Helligkeit und Buntheit auf das Mittel der Familienwunden skaliert
# (Sicht-Pruefer r1 P5: EM30-Wunde (50/26/13), G4-Wunde (53/27/23) -> (51,5/26,5/18,0)), Farbton der Quelle bleibt.
# Verworfen (Lauf r2-e17): die G4-"Rose" (89,189,18,34), Lab (42,2/58,3/32,9), C* 67 als dritte Quelle -> Z1 im Bild-1-Vergleich (Tafel 01)
# knallrot statt rosa; Bild 1 zeigt dort rosa (T Lab (54/17/-3), Messblatt 2).
# Nicht genommen: das Schulterauge (x 270-321, y 160-245, Messblatt 1 ausgenommen).
FLEISCH_ZIEL = ((50.0 + 53.0) / 2, (26.0 + 27.0) / 2, (13.0 + 23.0) / 2)
G4_FLEISCH = [(181, 224, 20, 30), (179, 229, 24, 24)]
# Haut-Quellen: aus M1.G4_HAUT (kf_mal.py:58) nur die mit Aderanteil (Rotklasse A3) <= 10 %: (130,96,20,62) 6 %, (190,96,20,60) 9 %, (314,12,32,30) 7 %;
# dazu die voll gedeckten G4-Rechtecke (269,4,24,32) 0 % und (29,152,24,32) 9,5 % (Suche im G4-Atlas, Dossier FARBE.md 3). Grenze 10 % = EM30-Linienrot
# im Render 4,6-10,2 % der Figur (Rotpixel ohne erosionsfesten Kern, r2-Messung wie Riegel B1) - "Adern nur als Akzent" (Sicht-Pruefer r1 P1).
G4_HAUT2 = [(130, 96, 20, 62), (190, 96, 20, 60), (314, 12, 32, 30), (269, 4, 24, 32), (29, 152, 24, 32)]
# Bild-2-Rot nur nach Abzug der Einstreuung: Messblatt 2 letzter Punkt, K5c-Albedo a* 2,7 -> Kette a* 15,5, also +12,8 in a* durch Feuer/Kette.
EINSTREU_A2 = 15.5 - 2.7
# Verdeckte Fleischpartien (74 % der Koerpertexel sind in keinem Bild sichtbar, Messblatt Messgrenzen): Unterseite (Bauch, Kehle, Innenseiten),
# wie die grossen Fleischflaechen der RE2-G4 (Sicht-Pruefer r1, Abschnitt 4.1 "Brust/Bauch/Kiefer wie G4"). Umfang: Anteil der Hauttexel mit der
# staerksten Normalen nach unten (Ruhepose Clip 2, kf_geo.py) = RE2-G4-Fleischrot 23,9 % (Messblatt 1). Gesehene Texel folgen nur dem Screenshot (E14).
FLEISCH_ANTEIL = 0.239
# Fleischflaeche, wo die Mehrheit der Bildpunkte eines Texels (nach Glaettung) rot ist: 0,5 (E14: Fleischrot nur, wo der Screenshot Rot zeigt).
FLEISCH_SCHWELLE = 0.5
# Umring: Uebergangsbreite der Unterseite = 1,2 x Fleischanteil (dunkelroter Umring 19-20 % der Wunde, Sicht-Pruefer r1 P5: EM30 19 %, G4 20 %).
FLEISCH_UMRING = 1.2
G4_DUNKELROT = (18.9, 25.6, 12.2)      # Messblatt 1, RE2-G4-Palette, 7,6 %
# Farbfenster: H_WARM 25 = Farbton des EM30-Rots (Messblatt 1, unterster Familienwert 25..48; Riegel A4 15-50); H_MIN -55, H_MAX 80, C_VIO 14,5,
# C_REST 5,5 wie Runde 1 (kf_mal.py:71-79, Quellen dort). Blau: b* >= -6 = Grenze der Blau-Klasse (b* < -6, C* > 8) der Sicht-Pruefung r1 P2,
# Familie G4/G5 0,0 % (Blau nur bei EM30-Jeans).
H_MIN, H_WARM, H_MAX = -55.0, 25.0, 80.0
C_VIO, C_REST = M1.C_VIO, M1.C_REST
B_MIN = -6.0
# b* < B_MIN: b* auf B_MIN gesetzt (a* bleibt; Blau -> Grau, Violett -> Rotviolett). Gemessen am Original Bild 1 (T, Figur): 29,9 % der Bildpunkte
# liegen in der Blau-Klasse; mit b* auf -6 gekappt bleibt C* 7,99 (Original 9,25, Riegel K1 >= 7,9), mit Chroma-Skalierung nur 7,40 (Lauf r2-e5,
# Dossier FARBE.md 4). Violett (Messblatt 2: Grundhaut "fast unbunt bis leicht warm-violett,
# a* 0-6, b* -1..-8") bleibt bis b* -6 erlaubt; Blau/Tuerkis (Farbton < -55) ist durch C_REST 5,5 begrenzt. Verworfen (Laeufe r2-e1/e2): Farbton
# nicht unter -14 Grad (Projektion bzw. C* <= 6) -> Bild-1-Kette C* 6,2 / ab-Streuung 6,8 (K1/K2 ROT), neutraler Render C* 6,9-9,1 (N1 ROT).
# Augenumfeld r 6 px um die E24-Punkte (Messblatt 3: Umgebungsring 6-12 px so dunkel wie das Auge; kf_mal.py:65-69).
AUGE_PUNKTE, AUGE_R = M1.AUGE_PUNKTE, M1.AUGE_R
F3 = M1.F3
ZAHN_LAB, AUGE_LAB = M1.ZAHN_LAB, M1.AUGE_LAB
# Zahnverlauf Spitze -> Grund 100 -> 60 % L*: G4-Zaehne im Maulfeld (10,44,60,40) L* 45-80 vom Grund zur Spitze (kf_mal.py:419-421) -> 45/80 = 0,56.
ZAHN_GRUND = 45.0 / 80.0
ZONEN = M1.ZONEN
# Ziel der Augeneichung: Original L* max 22,1 (Messblatt 3 / Riegel K10, rechtes Auge (W)); die Zonenziele sind die Original-Zonenmittel (Messblatt 2).
ZIEL_AUGE = 22.1
# Verfahrensparameter der Eichung (Weg, nicht Ziel; das Ergebnis ist der Stand nach der letzten Runde, Restfehler je Runde in kf_eich2.json):
DAEMPF = 0.8                          # Schrittweite des Fixpunktverfahrens (Kette nicht linear: Videofilter, Rauschen, 5-Bit-CLUT)
SAAT = 7                              # k-Mittel der CLUT (kf_mal.palette), fest fuer byte-gleichen Nachbau


# ---------------------------------------------------------------- Hilfen
def harmonisch(werte, bekannt, gebiet, ersatz):
    """Laplace-Fuellung: Texel in gebiet & ~bekannt bekommen die harmonische Fortsetzung der bekannten Werte (4-Nachbarn im Gebiet, Rand frei).
    Zusammenhangsteile ohne bekannten Texel bekommen 'ersatz'. Kein Radius, keine Schwelle."""
    h, w = gebiet.shape
    X = np.asarray(werte, np.float64)
    k = 1 if X.ndim == 2 else X.shape[2]
    X = X.reshape(h, w, k).copy()
    n, lab = cv2.connectedComponents(gebiet.astype(np.uint8), connectivity=4)
    for c in range(1, n):
        s = lab == c
        unb = s & ~bekannt
        if not unb.any():
            continue
        if not (s & bekannt).any():
            X[unb] = ersatz
            continue
        ys, xs = np.nonzero(unb)
        idx = -np.ones((h, w), int)
        idx[ys, xs] = np.arange(len(ys))
        rows, cols, vals = [], [], []
        rhs = np.zeros((len(ys), k))
        deg = np.zeros(len(ys))
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            y2, x2 = ys + dy, xs + dx
            ok = (y2 >= 0) & (y2 < h) & (x2 >= 0) & (x2 < w)
            ok[ok] &= s[y2[ok], x2[ok]]
            deg += ok
            j = np.where(ok)[0]
            nb = idx[y2[j], x2[j]]
            unbek = nb >= 0
            rows += list(j[unbek]); cols += list(nb[unbek]); vals += [-1.0] * int(unbek.sum())
            rhs[j[~unbek]] += X[y2[j[~unbek]], x2[j[~unbek]]]
        rows += list(range(len(ys))); cols += list(range(len(ys))); vals += list(np.maximum(deg, 1))
        A = coo_matrix((vals, (rows, cols)), shape=(len(ys), len(ys))).tocsr()
        sol = spsolve(A, rhs)
        X[ys, xs] = np.asarray(sol).reshape(len(ys), k)
    return X if k > 1 else X[..., 0]


def kachel_reihe(G, U, quellen, w, h, nr, n_felder, ton):
    """Feld b x h aus G4-Quellen nebeneinander (x = Umfang wie bei J und G4), ueberblendet ueber 2 x SIG_GROB Texel (kf_mal.naht_reihe),
    nicht gespiegelt. Startquelle = Feldnummer, Startzeile je Quelle gleichverteilt: (nr * Quellhoehe) // n_felder (kein freier Versatz).
    ton=True: ganzes G4-Bild (Ton + Form); False: nur die Malform (Bild minus Grobbild)."""
    blend = int(round(2 * SIG_GROB))
    spalten, breite, i = [], 0, nr
    while breite < w + blend:
        r = quellen[i % len(quellen)]
        A = M1.ausschnitt(G, U, r)
        P = A if ton else A - M1.grob(A, np.ones(A.shape[:2], bool), SIG_GROB)
        y0 = (nr * P.shape[0]) // max(n_felder, 1)
        if P.shape[0] < h:
            n_st = h // max(P.shape[0] - blend, 1) + 2      # so viele Stuecke, dass die Feldhoehe sicher gedeckt ist
            stuecke = [np.roll(P, -((y0 + (j * P.shape[0]) // n_st) % P.shape[0]), 0) for j in range(n_st)]   # Versatz gleichverteilt
            P = M1.naht_reihe(stuecke, h, achse=0, blend=blend)
            y0 = 0
        y0 = y0 % (P.shape[0] - h + 1)
        spalten.append(P[y0:y0 + h])
        breite += P.shape[1] - blend
        i += 1
    return M1.naht_reihe(spalten, w, blend=blend)


def rotklasse(L):
    C = np.hypot(L[..., 1], L[..., 2])
    H = np.degrees(np.arctan2(L[..., 2], L[..., 1]))
    return (L[..., 1] > 15) & (C > 20) & (H > -40) & (H < 70)


def original_rot(sicht, genutzt, n):
    """Anteil roter Original-Bildpunkte je sichtbarem Texel (Bild n, T wie p_zonen.rueck, nur in den Nutzerzonen).
    Rotklasse Pruefer (a* >= 12, C* >= 15, L* >= 20, Messblatt 2); Bild 2 mit a* um die Einstreuung EINSTREU_A2 verringert."""
    import p_zonen as PZ
    v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
    T, _clip = PZ.rueck(v, n)
    L = PL.lab(np.clip(T.reshape(-1, 3), 0, 255)).reshape(480, 640, 3)
    if n == 2:
        L[..., 1] -= EINSTREU_A2
    C = np.hypot(L[..., 1], L[..., 2])
    zone = np.zeros((480, 640), bool)
    for _z, m in PZ.regionen(n):
        zone |= m
    rot = zone & (L[..., 1] >= 12) & (C >= 15) & (L[..., 0] >= 20)
    tx, ty = sicht['px%d_tx' % n].astype(int), sicht['px%d_ty' % n].astype(int)
    ok = (tx >= 0) & zone
    n_rot = np.zeros(genutzt.shape, np.float32)
    n_alle = np.zeros(genutzt.shape, np.float32)
    np.add.at(n_rot, (ty[ok & rot], tx[ok & rot]), 1)
    np.add.at(n_alle, (ty[ok], tx[ok]), 1)
    return n_rot, n_alle


def original_ton(sicht, genutzt):
    """Farbwechsel innerhalb der Zone aus dem Original (E10 "Haut hat mehr als eine Farbe", E11 "aus dem Screenshot uebernehmen"): je Bildpunkt
    a*, b* minus Zonenmittel des Originals (T wie p_zonen.rueck; Bild 2: die Einstreuung faellt als Zonenkonstante heraus), je sichtbarem Texel
    das Mittel ueber seine Bildpunkte beider Bilder. Nur Farbe, nie Helligkeit (keine Glanzpunkte). Quelle der Notwendigkeit: Riegel K2
    (Bild-1-ab-Streuung >= 0,85 x 9,8); Lauf r2-e3 ohne: 6,7 ROT."""
    import p_zonen as PZ
    s_ab = np.zeros(genutzt.shape + (2,), np.float32)
    n_ab = np.zeros(genutzt.shape, np.float32)
    for n in (1, 2):
        v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        T, _clip = PZ.rueck(v, n)
        L = PL.lab(np.clip(T.reshape(-1, 3), 0, 255)).reshape(480, 640, 3)
        dab = np.zeros((480, 640, 2), np.float32)
        zone = np.zeros((480, 640), bool)
        for _z, m in PZ.regionen(n):
            dab[m] = L[m][:, 1:] - L[m][:, 1:].mean(0)
            zone |= m
        # rote Bildpunkte (Rotklasse wie original_rot, Bild 2 nach Abzug der Einstreuung) tragen die Fleischflaeche (Schritt 4), nicht den
        # Farbwechsel: sonst zaehlt dieselbe Stelle doppelt (Lauf r2-e4: Fleischschwelle stieg auf 0,84, Fleisch schrumpfte zu Linien)
        a_ = L[..., 1] - (EINSTREU_A2 if n == 2 else 0.0)
        rot = (a_ >= 12) & (np.hypot(a_, L[..., 2]) >= 15) & (L[..., 0] >= 20)
        tx, ty = sicht['px%d_tx' % n].astype(int), sicht['px%d_ty' % n].astype(int)
        ok = (tx >= 0) & zone & ~rot
        np.add.at(s_ab, (ty[ok], tx[ok]), dab[ok])
        np.add.at(n_ab, (ty[ok], tx[ok]), 1)
    return s_ab / np.maximum(n_ab, 1)[..., None], n_ab > 0


def standard_eich():
    return dict(zonen={z: dict(d=[0.0, 0.0, 0.0]) for z in ZONEN}, auge=list(AUGE_LAB), runden=[])


# ---------------------------------------------------------------- Bau
def bauen(aus, eich=None, sicht_pfad=SICHT):
    eich = eich or (json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich())
    mo, tim, rgb, mid = M1.lade_k5c()
    zeile, mesh, konflikt = M1.texel_karten(mo)
    genutzt = zeile >= 0
    h, w = genutzt.shape
    sicht = dict(np.load(sicht_pfad))
    F = M1.felder()
    ordnung = sorted(F, key=lambda n: F[n][2] * F[n][3])
    feld_id = np.full((h, w), -1, int)
    for i, n in enumerate(ordnung):
        x, y, ww, hh = F[n]
        m = np.zeros((h, w), bool)
        m[y:y + hh, x:x + ww] = True
        feld_id[(feld_id < 0) & m & genutzt] = i
    rest = genutzt & (feld_id < 0)
    fid = {n: feld_id == i for i, n in enumerate(ordnung)}
    sonder = fid['s3_kiefer'] | fid['s3_kralle'] | fid['s3_zahn'] | fid['s3_auge']
    haut = genutzt & ~sonder                    # Koerper (x < 384) + Kopf
    Lk = M1.lab_f(rgb.astype(np.float32))
    herkunft = {}

    # 1. Glanzflecken und Gaumenstreifen aus dem Ausgangsbild entfernen (harmonisch aus der Umgebung, je Atlasfeld)
    Ck = np.hypot(Lk[..., 1], Lk[..., 2])
    keim = haut & (Lk[..., 0] > GLANZ_L) & (Ck < GLANZ_C)
    hof = haut & (Lk[..., 0] > HOF_L)
    n_k, lab_h = cv2.connectedComponents(hof.astype(np.uint8), connectivity=8)
    glanz = np.isin(lab_h, np.unique(lab_h[keim])) & hof | keim
    g_keim = fid['s3_kopf'] & (Lk[..., 0] > GAUMEN_L) & (Ck < GAUMEN_C)
    g_hof = fid['s3_kopf'] & (Lk[..., 0] > HOF_L)            # Hof wie beim Glanz: heller als das hellste Original-Band (Messblatt 2)
    _n, lab_g = cv2.connectedComponents(g_hof.astype(np.uint8), connectivity=8)
    gaumen = (np.isin(lab_g, np.unique(lab_g[g_keim])) & g_hof) | g_keim
    weg = glanz | gaumen
    Lc = Lk.copy()
    for i in range(len(ordnung)):
        s = (feld_id == i) & haut
        if (s & weg).any():
            Lc = np.where(s[..., None], harmonisch(Lc, s & ~weg, s, Lk[haut & ~weg].mean(0)), Lc)
    herkunft['glanz_entfernt'] = '%d Texel (Keim L*>%.0f C*<%.0f: %d, Hof L*>%.0f), Gaumenstreifen Kopf %d Texel; harmonisch gefuellt' % (
        int(glanz.sum()), GLANZ_L, GLANZ_C, int(keim.sum()), HOF_L, int(gaumen.sum()))

    # 2. Ausgangsbild grob (Lage der Screenshot-Farben, E12 "grossflaechig je Bereich"), G4-Haut und G4-Fleisch je Feld
    G, U = M1.g4_material()
    basis = np.zeros_like(Lk)
    g4h = np.zeros_like(Lk)       # G4-Haut, ganzes Bild
    g4f = np.zeros_like(Lk)       # G4-Fleisch, ganzes Bild
    n_haut = [n for n in ordnung if n not in ('s3_kiefer', 's3_kralle', 's3_zahn', 's3_auge')]
    for j, n in enumerate(n_haut):
        s = fid[n] & haut
        if not s.any():
            continue
        x, y, ww, hh = F[n]
        sub = s[y:y + hh, x:x + ww]
        basis[y:y + hh, x:x + ww][sub] = M1.grob(Lc[y:y + hh, x:x + ww], sub, SIG_GROB)[sub]
        g4h[y:y + hh, x:x + ww][sub] = kachel_reihe(G, U, G4_HAUT2, ww, hh, j, len(n_haut), True)[sub]
        g4f[y:y + hh, x:x + ww][sub] = kachel_reihe(G, U, G4_FLEISCH, ww, hh, j, len(n_haut), True)[sub]
        herkunft[n] = 'K5c-Grobbild ohne Glanz (sigma %.1f) + RE2-G4-Haut %s ab Quelle %d (Adern dunkel), Fleisch RE2-G4 %s wo Bild 1/2 Rot zeigen' % (
            SIG_GROB, 'G4_HAUT2', j % len(G4_HAUT2), G4_FLEISCH)
    s = rest & haut
    basis[s] = Lc[s]

    # 3. Haut: Grundton aus dem Ausgangsbild, Helligkeits- und Farbform aus G4 (Ausschnitt minus Grobbild), aber aus dem G4-Bild OHNE seine Adern
    #    (Adern harmonisch aus der Umgebung gefuellt; sonst traegt die Malform den roten Hof der Adern als Sprenkel, Lauf r2-v0: Rotpixel
    #    nur 6-20 % erosionsfest). Die G4-Adern (Rotklasse) selbst werden danach dunkler als die Haut gemalt.
    ader = haut & rotklasse(g4h)
    dG = np.zeros_like(Lk)
    for j, n in enumerate(n_haut):
        s = fid[n] & haut
        if not s.any():
            continue
        x, y, ww, hh = F[n]
        sub = s[y:y + hh, x:x + ww]
        P = harmonisch(g4h[y:y + hh, x:x + ww], sub & ~ader[y:y + hh, x:x + ww], sub, g4h[y:y + hh, x:x + ww][sub].mean(0))
        d = P - M1.grob(P, sub, SIG_GROB)
        for c in (1, 2):
            d[..., c] = cv2.GaussianBlur(d[..., c].astype(np.float32), (0, 0), SIG_AB_FEIN)
        dG[y:y + hh, x:x + ww][sub] = d[sub]
    neu = np.zeros_like(Lk)
    neu[..., 0] = basis[..., 0] + dG[..., 0]
    neu[..., 1:] = basis[..., 1:] * G_AB + dG[..., 1:]
    neu[..., 0] = np.where(ader, ADER_L * basis[..., 0], neu[..., 0])
    Ca = np.hypot(g4h[..., 1], g4h[..., 2])
    fa = np.minimum(1.0, ADER_CL * neu[..., 0] / np.maximum(Ca, 1e-6))
    neu[..., 1] = np.where(ader, g4h[..., 1] * fa, neu[..., 1])
    neu[..., 2] = np.where(ader, g4h[..., 2] * fa, neu[..., 2])
    herkunft['adern'] = '%d Texel (%.1f %% der Haut) G4-Adern: L* x %.2f der Haut, C* <= %.2f x L*' % (int(ader.sum()), 100 * ader[haut].mean(), ADER_L, ADER_CL)

    # 4. Fleischflaechen.
    #    a) Screenshot: Anteil roter Original-Bildpunkte je Texel (Bild 1 und Bild 2), harmonisch in die verdeckten Texel des Feldes fortgesetzt
    #       (E11), grob geglaettet (E12/E13: grosse Formen, keine Inseln), Mehrheitsschwelle FLEISCH_SCHWELLE -> zusammenhaengende Partie.
    #    b) verdeckte Unterseite (G4-Vorbild): weicher Uebergang ueber den Normalen-Bereich vom Fleischanteil 23,9 % bis 1,2 x 23,9 % (Umring
    #       dunkelrot 19-20 % der Wundflaeche bei EM30/G4, Sicht-Pruefer r1 P5), damit die Unterseite an Umrisskanten nicht als rote Linie steht
    #       (Lauf r2-e9: harte Grenze -> rote Saeume an Beinen und Bauch, B1 21 %). Nur verdeckte Texel (E14).
    r1, a1 = original_rot(sicht, genutzt, 1)
    r2, a2 = original_rot(sicht, genutzt, 2)
    anteil = (r1 + r2) / np.maximum(a1 + a2, 1)
    gesehen = (a1 + a2) > 0
    import kf_geo
    _pos, nrm, ges_geo = kf_geo.geometrie(K5C)
    ny = nrm[..., 1]                                     # PSX: y nach unten -> ny > 0 = Unterseite
    q = float(np.quantile(ny[haut & ges_geo], 1.0 - FLEISCH_ANTEIL))
    q_r = float(np.quantile(ny[haut & ges_geo], 1.0 - FLEISCH_UMRING * FLEISCH_ANTEIL))
    unten = haut & ges_geo & (ny >= q)
    # Kopffeld ausgenommen: die Kopf-Unterseite ist Kiefer/Kehle aus Bild 2 (Messblatt 3) und das Kopffeld eigenes Bauteil; Lauf r2-e15 mit
    # dunkler Kopf-Unterseite: A6 Kopf HF 0,281 > 0,26.
    u_w = np.where(haut & ges_geo & ~gesehen & ~fid['s3_kopf'], np.clip((ny - q_r) / max(q - q_r, 1e-6), 0, 1), 0).astype(np.float32)
    wf = np.zeros((h, w), np.float32)
    uw = np.zeros((h, w), np.float32)
    for j, n in enumerate(n_haut):
        s = fid[n] & haut
        if not s.any():
            continue
        x, y, ww, hh = F[n]
        sub = s[y:y + hh, x:x + ww]
        fe = harmonisch(anteil[y:y + hh, x:x + ww], sub & gesehen[y:y + hh, x:x + ww], sub, 0.0)
        wf[y:y + hh, x:x + ww][sub] = M1.grob(fe, sub, SIG_GROB)[sub]
        uw[y:y + hh, x:x + ww][sub] = M1.grob(u_w[y:y + hh, x:x + ww], sub, SIG_AB_FEIN)[sub]
    fl_maske = haut & ((wf >= FLEISCH_SCHWELLE) | (uw >= FLEISCH_SCHWELLE))
    # weiche Kante: Glaettung der Screenshot-Maske mit SIG_AB_FEIN (E13: Bereichskanten sauber, ohne Einzeltexel-Saum), Unterseite mit ihrem Verlauf
    fw = cv2.GaussianBlur((haut & (wf >= FLEISCH_SCHWELLE)).astype(np.float32), (0, 0), SIG_AB_FEIN) * haut
    sel_f = haut & (fw > 0)
    if sel_f.any():
        fL = float(g4f[..., 0][sel_f].mean())
        fC = float(np.hypot(g4f[..., 1], g4f[..., 2])[sel_f].mean())
        g4w = g4f.copy()
        g4w[..., 0] *= FLEISCH_ZIEL[0] / max(fL, 1e-3)
        g4w[..., 1:] *= float(np.hypot(FLEISCH_ZIEL[1], FLEISCH_ZIEL[2])) / max(fC, 1e-3)
    else:
        g4w = g4f
    # Unterseite in der dunklen G4-Rotstufe (Messblatt 1, G4-Palette (18,9/25,6/12,2), "dunkelrote Tiefen" E10): G4-Fleisch in L* und C* auf das
    # Mittel dieser Stufe skaliert (Form und Farbton bleiben; je Kanal skaliert gab Rauschen, Lauf r2-e14 A6 Kopf 0,276). Lauf r2-e12 mit hellem Fleisch: Unterseite stand an Umrisskanten als hellrote Linie.
    sel = haut & (uw > 0)
    mL = float(g4f[..., 0][sel].mean()) if sel.any() else 1.0
    mC = float(np.hypot(g4f[..., 1], g4f[..., 2])[sel].mean()) if sel.any() else 1.0
    g4d = g4f.copy()
    g4d[..., 0] *= G4_DUNKELROT[0] / max(mL, 1e-3)                                  # Helligkeit auf die Stufe
    g4d[..., 1:] *= float(np.hypot(G4_DUNKELROT[1], G4_DUNKELROT[2])) / max(mC, 1e-3)   # Buntheit auf die Stufe, Farbton bleibt
    neu = neu * (1 - uw[..., None]) + g4d * uw[..., None]
    neu = neu * (1 - fw[..., None]) + g4w * fw[..., None]
    herkunft['fleisch'] = '%d Texel (%.1f %% der Haut) Fleischflaeche (Gewicht >= 0,5), Schwelle %.3f; gesehen Bild 1 %d / Bild 2 %d Texel; Unterseite ny >= %.2f (%d Texel, verdeckt %d)' % (
        int(fl_maske.sum()), 100 * fl_maske[haut].mean(), FLEISCH_SCHWELLE, int((a1 > 0).sum()), int((a2 > 0).sum()), q, int(unten.sum()),
        int((unten & ~gesehen).sum()))

    # 5. Kiefer (RE2-G4-Maul) und Krallen (RE2-G4-Krallen) wie Runde 1, Versaetze gleichverteilt statt 11 x j / 9 x j
    for n, quelle in (('s3_kiefer', M1.G4_MAUL), ('s3_kralle', M1.G4_KRALLE)):
        s = fid[n]
        x, y, ww, hh = F[n]
        sub = s[y:y + hh, x:x + ww]
        P = M1.ausschnitt(G, U, quelle)
        if n == 's3_kiefer':
            P = np.rot90(P, 1)                   # G4-Zahnreihen laengs (k_form.rohr: v laengs), kf_mal.py:361
        kop = -(-ww // (P.shape[1] - int(round(2 * SIG_GROB)))) + 1     # Kopien, bis der Umfang gedeckt ist (Runde 1: 5 / 7 fest)
        P = M1.naht_reihe([np.roll(P, (j * P.shape[0]) // kop, 0) for j in range(kop)], ww, blend=int(round(2 * SIG_GROB)))
        P = P[:hh] if P.shape[0] >= hh else M1.naht_reihe([P, P], hh, achse=0, blend=int(round(2 * SIG_GROB)))
        neu[y:y + hh, x:x + ww][sub] = P[sub]
        herkunft[n] = 'RE2-G4 %s, %d Kopien um den Umfang, Zeilenversatz (j x Hoehe) / %d, G4-Texelmassstab' % (quelle, kop, kop)

    # 6. Zonenversatz (Eichung gegen die Kette): sichtbare Texel nach Bildpunkten je Zone, verdeckte harmonisch im Feld fortgesetzt
    num = np.zeros((h, w, 3), np.float32)
    den = np.zeros((h, w), np.float32)
    # Zone = Bauteil: die Bildzone "Unterkiefer" wirkt nur auf das Kieferfeld, alle anderen Zonen nie auf das Kieferfeld. Lauf r2-e2: die
    # Unterkiefer-Zone (L* +38,7) traf ueber Bildpunkte vor dem Maul hand_r (217 px), Kopf (502 px) und Unterschenkel (107 px) -> helle Pfote
    # (Sicht-Pruefer r1 P3) und Glanzrest B4 +22.
    kfm = fid['s3_kiefer']
    vor = {}
    for z, v in eich['zonen'].items():
        c = sicht[z].astype(np.float32) * (kfm if z == 'b2|Unterkiefer' else ~kfm)
        vor[z] = [round(float(x), 2) for x in (neu[c > 0] * c[c > 0][:, None]).sum(0) / max(float(c.sum()), 1.0)]
        vor[z].append(round(float(neu[..., 2][c > 0].max()), 2) if (c > 0).any() else 0.0)     # groesstes b* der Zone (Grenze der b*-Eichung)
        num += c[..., None] * np.asarray(v['d'], np.float32)
        den += c
    dz = np.zeros((h, w, 3), np.float32)
    bek = den > 0
    dz[bek] = num[bek] / den[bek][:, None]
    alle_felder = [fid[n] for n in ordnung] + [rest]
    for s in alle_felder:
        if s.any():
            dz = np.where(s[..., None], harmonisch(dz, s & bek, s, 0.0), dz)
    # Kiefer: Helligkeitsversatz nur am sichtbaren Band (Bild 2 Unterkiefer); das verdeckte Maulinnere bleibt so dunkel wie das G4-Maul
    # (Riegel A8 Dunkel >= 20 %, Familie 24-64 %). Ohne Fortsetzung, ohne Exponent.
    kf = fid['s3_kiefer']
    dz[..., 0] = np.where(kf & ~bek, 0.0, dz[..., 0])
    # Kante Band/Maulinneres weich (Glaettung SIG_GROB im Kieferfeld; Lauf r2-e7: harte Stufe -> A7 HF_rest Kiefer 0,195 > 0,19)
    x, y, ww, hh = F['s3_kiefer']
    sub = kf[y:y + hh, x:x + ww]
    dz[y:y + hh, x:x + ww, 0][sub] = M1.grob(dz[y:y + hh, x:x + ww, 0], sub, SIG_GROB)[sub]
    neu += dz
    # Farbwechsel aus dem Original (nur Haut, nicht Kiefer/Krallen/Zaehne/Augen), verdeckte Texel harmonisch im Feld, Glaettung SIG_AB_FEIN (E13)
    tab, tsicht = original_ton(sicht, genutzt)
    ton = np.zeros((h, w, 2), np.float32)
    for n in n_haut:
        s_ = fid[n] & haut
        if s_.any():
            x, y, ww, hh = F[n]
            sub = s_[y:y + hh, x:x + ww]
            t_ = harmonisch(tab[y:y + hh, x:x + ww], sub & tsicht[y:y + hh, x:x + ww], sub, 0.0)
            t_ = M1.grob(t_, sub, SIG_AB_FEIN)
            ton[y:y + hh, x:x + ww][sub] = t_[sub]
    neu[..., 1:] += ton
    herkunft['ton'] = '%d sichtbare Hauttexel mit a*b*-Abweichung vom Zonenmittel des Originals (Bild 1+2), Streuung %.1f' % (
        int((tsicht & haut).sum()), float(np.sqrt((ton[haut] ** 2).sum(1).mean())))

    # 7. Zaehne, Augen, Augenumfeld (wie Runde 1)
    x, y, ww, hh = F3['zahn']
    t = np.linspace(1.0, ZAHN_GRUND, hh)[:, None] * np.ones((1, ww))
    neu[y:y + hh, x:x + ww, 0] = ZAHN_LAB[0] * t
    neu[y:y + hh, x:x + ww, 1] = ZAHN_LAB[1]
    neu[y:y + hh, x:x + ww, 2] = ZAHN_LAB[2]
    x, y, ww, hh = F3['auge']
    neu[y:y + hh, x:x + ww] = np.asarray(eich['auge'], np.float32)
    yy, xx = np.mgrid[:480, :640]
    n_auge = 0
    for (px_, py_) in AUGE_PUNKTE:
        d = ((xx - px_) ** 2 + (yy - py_) ** 2 <= AUGE_R ** 2) & (sicht['px2_tx'] >= 0)
        tx_, ty_ = sicht['px2_tx'][d].astype(int), sicht['px2_ty'][d].astype(int)
        ok = genutzt[ty_, tx_] & ~fid['s3_zahn'][ty_, tx_]
        neu[ty_[ok], tx_[ok]] = np.asarray(eich['auge'], np.float32)
        n_auge += int(ok.sum())
    herkunft['augenumfeld'] = '%d Texel unter r <= %d px um E24 in Bild 2 -> Augenfarbe %s' % (n_auge, AUGE_R, eich['auge'])

    # 8. E9 Schwanz: Glied-Mittel 1-5 auf die Ausgleichsgerade (wie Runde 1, ohne Parameter)
    koerper = genutzt & (np.arange(w)[None, :] < 384)
    gm = [float(neu[..., 0][koerper & (mesh == g)].mean()) for g in range(1, 6)]
    k_, d_ = np.polyfit(np.arange(5), gm, 1)
    for i, g in enumerate(range(1, 6)):
        neu[..., 0][koerper & (mesh == g)] += (k_ * i + d_) - gm[i]
    herkunft['schwanz_e9'] = 'Glied-L* vorher %s, Gerade %.2f + %.2f i' % ([round(v, 1) for v in gm], d_, k_)

    # 9. Farbfenster (Koerper, Kopf, Krallen, Kiefer; nicht Zaehne/Augen)
    fen = genutzt & ~fid['s3_zahn'] & ~fid['s3_auge']
    C_ = np.hypot(neu[..., 1], neu[..., 2])
    H_ = np.degrees(np.arctan2(neu[..., 2], neu[..., 1]))
    dreh = fen & (H_ >= H_MIN) & (H_ < H_WARM) & (C_ > C_VIO)
    rest_ = fen & ((H_ < H_MIN) | (H_ > H_MAX)) & (C_ > C_REST)
    neu[..., 1] = np.where(dreh, C_ * np.cos(np.radians(H_WARM)), neu[..., 1])
    neu[..., 2] = np.where(dreh, C_ * np.sin(np.radians(H_WARM)), neu[..., 2])
    f_ = np.where(rest_, C_REST / np.maximum(C_, 1e-6), 1.0)
    neu[..., 1] *= f_
    neu[..., 2] *= f_
    blau = fen & (neu[..., 2] < B_MIN)
    neu[..., 2] = np.where(blau, B_MIN, neu[..., 2])
    herkunft['farbfenster'] = 'auf %.0f Grad gedreht %d, ausserhalb %.0f..%.0f Grad C* <= %.1f: %d, b* < %.0f begrenzt %d Texel (von %d)' % (
        H_WARM, int(dreh.sum()), H_MIN, H_MAX, C_REST, int(rest_.sum()), B_MIN, int(blau.sum()), int(genutzt.sum()))
    # Adern (ausserhalb der Fleischflaechen) zuletzt: C* <= ADER_CL x L* nach allen Versaetzen, damit sie dunkelrot bleiben und nicht in die
    # Rotklasse fallen (Lauf r2-v2: 2204 Adertexel nach den Zonenversaetzen rot).
    av = ader & ~fl_maske
    Cn = np.hypot(neu[..., 1], neu[..., 2])
    fa = np.where(av, np.minimum(1.0, ADER_CL * np.maximum(neu[..., 0], 0) / np.maximum(Cn, 1e-6)), 1.0)
    neu[..., 1] *= fa
    neu[..., 2] *= fa
    neu[..., 0] = np.clip(neu[..., 0], 0, 100)
    out_rgb = M1.rgb_f(neu)
    out_rgb[~genutzt] = 0
    bau = schreiben2(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, dict(konflikt_zeilen=konflikt, felder=len(ordnung), rest_texel=int(rest.sum()),
                                                                          zone_vor_versatz=vor))
    bau['basis'] = 'K5c (varianten/K5/K5c): FORM.EMD byte-gleich, nur FORM.TIM neu (Runde 2, kf_mal2.py)'
    bau['konstanten'] = dict(SIG_GROB=SIG_GROB, G_AB=G_AB, SIG_AB_FEIN=SIG_AB_FEIN, GLANZ_L=GLANZ_L, GLANZ_C=GLANZ_C, HOF_L=HOF_L, GAUMEN_L=GAUMEN_L,
                             GAUMEN_C=GAUMEN_C, ADER_L=ADER_L, ADER_CL=ADER_CL, G4_HAUT2=G4_HAUT2, G4_FLEISCH=G4_FLEISCH, FLEISCH_ZIEL=FLEISCH_ZIEL, G4_MAUL=M1.G4_MAUL,
                             G4_KRALLE=M1.G4_KRALLE, EINSTREU_A2=EINSTREU_A2, H_MIN=H_MIN, H_WARM=H_WARM, H_MAX=H_MAX, C_VIO=C_VIO, C_REST=C_REST,
                             B_MIN=B_MIN, FLEISCH_SCHWELLE=FLEISCH_SCHWELLE, FLEISCH_ANTEIL=FLEISCH_ANTEIL, FLEISCH_UMRING=FLEISCH_UMRING, G4_DUNKELROT=G4_DUNKELROT, AUGE_R=AUGE_R, ZAHN_LAB=ZAHN_LAB, ZAHN_GRUND=ZAHN_GRUND, DAEMPF=DAEMPF, SAAT=SAAT)
    bau['masken'] = dict(glanz=int(glanz.sum()), gaumen=int(gaumen.sum()), ader=int(ader.sum()), fleisch=int(fl_maske.sum()))
    json.dump(bau, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    np.savez_compressed(os.path.join(aus, 'masken.npz'), glanz=glanz, gaumen=gaumen, ader=ader, fleisch=fl_maske, keim=keim)
    return bau


def clut_farben(c):
    """Palettenzentren (RGB float) -> 5-Bit-Stufen, gerundet (nicht abgeschnitten wie form_emd.clut_wort: v >> 3 machte dunkle Farben dunkler,
    Zahlen-Pruefer r1 A8 +2,5). Danach Blau-Regel an der Spielfarbe (Dekodierung x 255 // 31 wie emd_ansichtsblatt.py:176): liegt eine Farbe in
    der Blau-Klasse (b* < -6, C* > 8), wird Blau um eine 5-Bit-Stufe gesenkt (bei Blau 0: Rot gehoben), bis sie es nicht mehr ist.
    Dunkle Farben kippen sonst durch eine einzige Blaustufe in die Klasse (RGB (16,16,24): b* -6,7)."""
    q = np.clip(np.round(np.asarray(c, float) * 31.0 / 255.0), 0, 31).astype(int)
    n_fix = 0
    for i in range(len(q)):
        for _ in range(62):
            d = (q[i] * 255 // 31).astype(np.float32)
            L = M1.lab_f(d[None, :])[0]
            if not (L[2] < B_MIN and np.hypot(L[1], L[2]) > 8):
                break
            n_fix += 1
            if q[i][2] > 0:
                q[i][2] -= 1
            else:
                q[i][0] = min(31, q[i][0] + 1)
    return q, n_fix


def schreiben2(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, info):
    os.makedirs(aus, exist_ok=True)
    h, w = genutzt.shape
    px = np.zeros((h, w), np.uint8)
    cl = []
    n_fix = 0
    for zl in range(len(tim['cluts'])):
        s = genutzt & (zeile == zl)
        c, lb = M1.palette(out_rgb[s], seed=SAAT)
        q, nf = clut_farben(c)
        n_fix += nf
        px[s] = (lb + 1).astype(np.uint8)
        worte = [0] + [int(r | (g << 5) | (b << 10)) or 0x8000 for r, g, b in q]
        cl.append(worte + [0] * (256 - len(worte)))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, cl, tim['clut_x'], tim['clut_y'], tim['bild_x'], tim['bild_y']))
    shutil.copy(os.path.join(K5C, 'FORM.EMD'), os.path.join(aus, 'FORM.EMD'))
    fj = json.load(open(os.path.join(K5C, 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe_r2'
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    info = dict(info, clut_blau_stufen=n_fix)
    return dict(eich={k: v for k, v in eich.items() if k != 'runden'}, herkunft=herkunft, info=info)


# ---------------------------------------------------------------- Eichung (Zonenversatz, Fleischschwelle, Auge)
def eichen(aus, runden):
    eich = json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich()
    O = M1.original_messen()
    for r in range(runden):
        vor_z = bauen(aus, eich)['info']['zone_vor_versatz']
        V = M1.kette_messen(aus)
        prot = {}
        for z in ZONEN:
            n = 1 if z.startswith('b1') else 2
            o, v = O[n][z[3:]], V[n][z[3:]]
            e = eich['zonen'][z]
            e['d'] = [round(float(e['d'][i] + DAEMPF * (o['lab'][i] - v['lab'][i])), 2) for i in range(3)]
            if z.startswith('b2|Klaue'):
                # Klauen: Helligkeit und b* sind Feuerschein (Messblatt 6 K9: "b* kein Riegel"; Messblatt 8 Messgrenzen), a* ist eine Untergrenze
                # (K9 >= 0,75 x Original). Daher L* und b* nicht geeicht, a* nur nach oben. Lauf r2-e7 mit L*-Eichung: Klaue l (W) L* +10, a* -14 ->
                # hellgraue Vorderpfote im neutralen Licht (Sicht-Pruefer r1 P3).
                alt = eich['zonen'][z].get('_alt', [0.0, 0.0, 0.0])
                e['d'][0] = 0.0
                e['d'][2] = 0.0
                e['d'][1] = round(max(0.0, float(e['d'][1])) if v['lab'][1] < o['lab'][1] else max(0.0, float(alt[1])), 2)
            # b*-Versatz nur so weit, wie er wirken kann: bis auch der Texel mit dem groessten b* der Zone an B_MIN liegt (darueber hinaus kappt
            # das Farbfenster jeden Texel, der Versatz waere wirkungslos). Lauf r2-e18 ohne Grenze: Hinterbein r (W) d_b lief auf -147;
            # Lauf r2-e20 mit Grenze am Zonenmittel: Kette b* -0,6 gegen Original -7,9 (K3 dE 7,3 ROT), weil nur die Haelfte der Texel die Grenze erreichte.
            e['d'][2] = round(max(float(e['d'][2]), B_MIN - float(vor_z[z][3])), 2)
            e['_alt'] = list(e['d'])
            prot[z] = dict(orig=o['lab'], kette=v['lab'], R=(o['kl_p']['R'], v['kl_p']['R']), d=list(e['d']))
        yy, xx = np.mgrid[:480, :640]
        aug = []
        for (x, y) in AUGE_PUNKTE:
            mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
            aug.append(float(PL.lab(np.clip(V[2]['_T'][mm], 0, 255))[:, 0].max()))
        if max(aug) > ZIEL_AUGE:
            eich['auge'][0] = round(max(0.0, eich['auge'][0] - DAEMPF * (max(aug) - ZIEL_AUGE)), 2)
        # keine Farbkraft-Eichung mehr (Laeufe r2-e5..e9: Ziel K1 trieb die Rotklasse K4 auf 9,9-10,2 %, Ziel K4 = Original 5,2 % entsaettigte die
        # Haut auf 0,74-0,79 -> A1/N1 ROT). Die Haut traegt die Farbkraft des Ausgangsbilds (G_AB) ohne weiteren Faktor; K4 ist ein Fenster.
        prot['ab_gain'] = 1.0
        prot['auge_Lmax'] = aug
        prot['figur'] = {n: (V[n]['Figur']['lab'], V[n]['Figur']['chroma'], V[n]['Figur']['ab_streu'], V[n]['Figur']['kl_p']) for n in (1, 2)}
        prot['rest_dE_max'] = max(float(np.linalg.norm(np.subtract(p['orig'], p['kette']))) for k, p in prot.items() if k in ZONEN)
        eich['runden'].append(prot)
        print('Runde %d: Rest dE max %.2f, Rot Bild 1 %.1f / %.1f, Auge %s' % (r, prot['rest_dE_max'], V[1]['Figur']['kl_p']['R'], O[1]['Figur']['kl_p']['R'], aug), flush=True)
        json.dump(eich, open(EICH, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    bauen(aus, eich)
    return eich


if __name__ == '__main__':
    sys.path.insert(0, M1.PS)
    if sys.argv[1] == 'bauen':
        b = bauen(sys.argv[2])
        print(json.dumps(b['herkunft'], ensure_ascii=False, indent=0)[:3000])
    elif sys.argv[1] == 'eichen':
        eichen(sys.argv[2], int(sys.argv[3]))
