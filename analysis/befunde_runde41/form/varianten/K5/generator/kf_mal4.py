#!/usr/bin/env python3
"""K5c Farbe, Versuch 2 (Runde 1): Binnenzeichnung auf den Runde-3-Stand (kf_mal3.py) gelegt.
Grundlage MESSBLATT_FARBE_V1.md und Nachtrag MESSBLATT_FARBE_V2.md (Abschnitte 1-5); Pruefung r3 (FARBE_PRUEFUNG_r3_sicht.md P1,
FARBE_PRUEFUNG_r3_zahlen.md 1, 5, 6). Geometrie, Clips, UV, Dreiecke unveraendert (FORM.EMD = K5c), neu nur Texelindizes und CLUTs.
Kein RE-Wert: neues Modell (PORT-WAHL Form, NUTZER_ENTSCHEIDE E5/E19/E20). Fuer die Malkonstanten liegt KEIN Nutzer-Wortlaut vor
(V2 Abschnitt 4 letzte Zeile); jede Konstante hier traegt deshalb eine Messquelle: Familienwert aus V2 Abschnitt 1 (EM30-Haut / RE2-G4 /
RE2-G5, Spalte "Familie"), V1-Zeile, Bild + Koordinaten, oder sie ist ein Rechenparameter mit Unabhaengigkeitsnachweis (Dossier FARBE.md "Versuch 2").

Was bleibt (r3, 28 gruene V1-Riegel): Koernung H2, Screenshot-Farbkarte (sichtbar / Spiegelpunkt / harmonisch), zwei Hautseiten mit Rampen,
Form aus dem Hohlraum-Mass, Fleisch = Screenshot-Mehrheit + verdeckte Rumpf-Unterseite, Kiefer/Krallen/Zaehne/Augen, E9-Schwanz, Farbfenster.
Was neu ist (V2 Abschnitt 5):
  1. Muskelachse je Mesh (PCA der Ruhepose-Lage der Texel), je Texel als Richtung im Atlas (lokale Jacobi-Matrix Atlas -> 3D).
  2. Adern im Fleisch: Polylinien entlang der Muskelachse, Saat Poisson-Scheibe in 3D, Dichte/Laenge/Breite/Kontrast aus der Familie.
  3. Wunden in harten Rotstufen: Rand 2 Texel G4-Dunkelrot, Innenrand 2 Texel G4-Fleischrot, Kern gestuft (Familien-Rampe), Abstand geodaetisch
     auf der Oberflaeche (keine UV-Naehte), Uebergang 0 Texel (keine Verlaeufe).
  4. Hautfalten als Linien quer zur Gliedachse an den Maxima des Hohlraum-Masses; die Flaechenfaerbung "dunkelrote Tiefen" (r3) entfaellt.
  5. Kopf: Platten-/Stirn-/Schnauzenlinien (laengs und quer zur Kopfachse) und Augenhoehlenraender.
  6. Bild-1-Ruecken: zusammenhaengende helle Wunde an der Stelle, wo das Original in Bild 1 Rot zeigt (E14), Kern Familienwunde L* 51,5.
  7. Haut-Buntheit wie V2 3.2 (V8): Eichfaktor g ohne V1-Grenze 1,48, gekappt a* <= 11 ausserhalb der Wunden, Blau-Klasse C* <= 8.

    python kf_mal4.py bauen <aus> [--k-graph N] [--spiegel X]
    python kf_mal4.py eichen <aus> <runden>
    python kf_mal4.py streuung <aus>          (Kettenstreuung ueber 3 Saaten des Videofilters -> kf_eich4.json 'kettenstreuung')"""
import json
import math
import os
import sys

import cv2
import numpy as np
from scipy.sparse import coo_matrix
from scipy.sparse.csgraph import connected_components, dijkstra
from scipy.spatial import cKDTree

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.normpath(os.path.join(HIER, '..', 'farbe_lesung', 'N2_skripte')))
import kf_mal3 as M3  # noqa: E402  Oberflaechen-Werkzeuge, Screenshot-Karte, Hautseiten (Runde 3)

M1, M2, PL, FE, K5C = M3.M1, M3.M2, M3.PL, M3.FE, M3.K5C
EICH = os.path.join(HIER, 'kf_eich4.json')
SICHT = M3.SICHT
CACHE = os.environ.get('KF4_CACHE', os.path.join(os.environ.get('TEMP', HIER), 'kf4_cache'))

# ---------------------------------------------------------------- Konstanten (Quelle je Zeile; V2 = MESSBLATT_FARBE_V2.md, V1 = MESSBLATT_FARBE_V1.md)
SAAT_MAL = 41                   # Saat fuer Linien-Saatpunkte und -Laengen (fest, genannt; V2 4 letzte Zeile "Saat fest und genannt")
# --- Hohlraum-Mass (r3 K_NAH/K_FERN 64/256 ohne Quelle, Pruefung r3 5.1): V2 4 Zeile 1.
#     K_NAH = pi r^2, r = halber Linienabstand der Familie im Fleisch. Linienabstand = Flaeche je Linienlaenge 1000/(n x Laenge) (V2 4):
#     EM30 52 / G4 31 / G5 19 Texel; Familienmitte (Median der drei) = G4 31 -> r 15,5 -> K_NAH = pi x 15,5^2 = 755.
LINIENABSTAND_FLEISCH = 31.0
K_NAH = int(round(math.pi * (LINIENABSTAND_FLEISCH / 2) ** 2))
#     K_FERN je Mesh = pi x (Gliedhalbbreite in Texeln)^2, Gliedhalbbreite aus der K5c-Geometrie: sqrt(l2 + l3) der PCA der Ruhepose-Lage
#     (Zylinder Radius rho: l2 = l3 = rho^2/2) geteilt durch den Texelabstand des Meshs (Median Nachbarabstand). Liste im Dossier.
# --- Lagestufen der Hautrampen (r3 ohne 45, Pruefung r3 5.2): V2 4 Zeile L_STUFEN. Pflicht 20 und 45 (V1 2 Klassen dunkel < 20, hell >= 45);
#     uebrige Grenzen = Dezile der Original-T-Helligkeit (Bild 1 + 2, Nicht-Rot-Figurpunkte), je Stufe >= 30 Bildpunkte (wie r3).
L_PFLICHT = (20.0, 45.0)
STUFE_MIN_PX = 30
# --- Zahnfleisch-Gewicht (r3 21,5 am lueckengefuellten Ausschnitt): V2 4 = a* p90 der 2011 genutzten G4-Texel in (10,44,60,40) = 19,30.
ZAHNFLEISCH_A = 19.30
# --- Muskelachse und Linien (V2 1 Tabelle, Spalte "Familie"; V2 5.2/5.4/5.5)
#     Fleisch-Adern: Laenge Median 11-16 / p90 21-43 Texel -> Familienmitte 13,5 / 32 (Lognormal: sigma = ln(32/13,5)/1,2816);
#     Breite Median 1,5-1,8 -> 1,65; Kontrast (Top-Hat L*) 11-15 -> 13. Farbe G4-Dunkelrot a*b* (25,6/12,2) (V2 5.2).
#     Gemessen wird die Laenge als 4 sqrt(l1) (n2_mass.linien_stat); ein gerades Stueck der Laenge l ergibt 4 sqrt(l^2/12) = 1,155 l.
#     Gezeichnet wird deshalb l = Ziel / 1,155 (Umrechnung der Messdefinition, keine Wahl).
LAENGE_MESS = 4.0 / math.sqrt(12.0)
ADER_LAENGE_MED, ADER_LAENGE_P90 = 13.5, 32.0
ADER_BREITE = 1.65
ADER_KONTRAST = 13.0
#     Haut-Falten: Breite 1,3-1,5 -> 1,4; Kontrast 9-12 -> 10,5; Abstand (Flaeche je Linienlaenge) aus V2 1 Haut-Zeilen:
#     Laenge = Anteil x 10 / (je1000 x Breite), Abstand = 1000 / (je1000 x Laenge): EM30 25,4 / G4 34,4 / G5 18,4 -> Median 25,4.
FALTE_BREITE = 1.4
FALTE_KONTRAST = 10.5
LINIENABSTAND_HAUT = 25.4
#     Kopf: Breite ~2 Texel, Kontrast ~16 L* (EM30-Kopf 2,1 / 16,2, V2 5.5).
KOPF_BREITE = 2.1
KOPF_KONTRAST = 16.2
#     Richtungsabweichung je Linie: V2 5.2 "Abweichung so, dass die Feldkohaerenz R in 0,21-0,51 bleibt". Fuer Linien mit normalverteilter
#     Abweichung sigma gilt R = exp(-2 sigma^2) (Doppelwinkel-Mittel); R = 0,51 (Familienmaximum G5) -> sigma = sqrt(-ln(0,51)/2) = 0,580 rad.
R_FELD_MAX = 0.51
SIGMA_WINKEL = math.sqrt(-math.log(R_FELD_MAX) / 2.0)
# --- Wundstufen (V2 5.3): Rand 1-2 Texel G4-Dunkelrot -> 2 Texel (= Aussenrand von Mass (c)); Innenrand 2 Texel (= Mass (f): Kern = Erosion
#     2 Texel); innen G4-Fleischrot (32,8/43,7/25,8); Kern Familien-Rampe ab EM30-Rot (36,5/37,4/20,5): (51,5/26,5/18,0) bzw. (64,3/24,7/16,3).
WUND_RAND = 2.0
WUND_INNEN = 4.0
G4_DUNKELROT = M3.G4_DUNKELROT                  # (18,9/25,6/12,2) V1 1 G4-Palette 7,6 %
G4_FLEISCHROT = (32.8, 43.7, 25.8)               # V1 1 G4-Palette 4,2 %
#     Kern zweistufig: dunkler Kern EM30-Rot (36,5/37,4/20,5) (V1 1 EM30 3,5 %), Hell-Kern Familienwunde (51,5/26,5/18,0) (r3 RAMPE_FLEISCH,
#     V2 3.2 K4b "helles Fleischrot L* 45-55, Farbton 34 Grad, C* 32"). Aufteilung nach der Form (Woelbung hell): Hell-Kern auf der helleren
#     Haelfte der Kerntexel (Median) = G4-Leitwert "Hell-Kern 50 %" (V2 1 (f), V2 5.3).
KERN_STUFEN = ((36.5, 37.4, 20.5), (51.5, 26.5, 18.0))
HELLKERN_ANTEIL = 0.50
FAMILIENWUNDE = KERN_STUFEN[1]
# --- Mindestflaeche einer Wunde / eines Lochs: 20 Texel = Rotflaeche-Mindestgroesse der V2-Messung (V2 1 "Rotflaeche = 8-Komponente >= 20 Texel")
FLAECHE_MIN = 20
# --- Fleischanteil des Koerpers (Atlas): RE2-G4 Fleischrot 23,9 % (V1 1 Tabelle "Fleischrot", G4 = Vorbild der Riesenform)
FLEISCH_ANTEIL = 0.239
# --- Liniendichte-Ziele (V2 1 "dunkle Linien % Fleisch / Haut", Familienmitte = Median der drei Werte EM30 / G4 / G5):
#     Fleisch 12,65 / 9,50 / 19,32 -> 12,65; Haut 5,35 / 3,89 / 8,29 -> 5,35; Kopf (V2 1 Kopf-Zeile) EM30 5,39 / G4 46,45 -> EM30 5,39
#     (G4-Kopf traegt die Zahnreihen im Kopffeld, K5c hat dafuer ein eigenes Zahnfeld, V1 B5).
ZIEL_ADERN, ZIEL_FALTEN, ZIEL_KOPF = 12.65, 5.35, 5.39
ZIEL_KOPF_CANNY = 29.6
# --- Adern ohne Hof (Linie selbst dunkler; Hof nur, wo die Haut zu dunkel ist): Laeufe exp_h0.5 gegen exp_h0: B13 21,9 gegen 22,3, kein Gewinn.
ADER_HOF = 0.0
# --- Kopf: Linien laengs zur Kopfachse, parallel (keine Richtungsabweichung), Laengen der laengsten Familie G5 (Median 15,6 / p90 42,9, V2 1).
#     Laeufe (Dossier): abwechselnd laengs/quer mit Abweichung -> kurze Striche in allen Richtungen ("Leopard"), Render-Kohaerenz B17 0,575-0,58;
#     laengs parallel 0,596; dazu Koernung laengs 0,616. Platten-/Stirnlinien laufen laengs (G4-Kopf: Rippen laengs, Atlas (0,0,60,40)).
KOPF_QUER, KOPF_SIGMA = False, 0.0
KOPF_LAENGE_MED, KOPF_LAENGE_P90 = 15.6, 42.9
# --- Obergrenze Hochfrequenz je Feld (V1 A6: 0,04-0,26 = Familie EM30-Kopf 0,040 ... Zombie-Kopf 0,259)
HF_MAX = 0.258   # im Float-Bau: A6-Grenze 0,26 minus gemessene Quantisierungsdrift (Lauf v13: Float 0,260 -> CLUT 0,261) mit Rundung
# --- Koernung laengs (Muskelstraenge, V2 5.1/5.2): die r3-Koernung (K5c-eigene Struktur H2) wird entlang der Muskelachse gemittelt, Laenge =
#     Linienlaenge Familienmitte 13,5 Texel, Streuung wie r3. Grund (gemessen): die Wirbel der r3-Koernung sind die inkohaerenten Kanten
#     (B17 Atlas ohne Koernung 0,747, mit r3-Koernung 0,57; Render 0,635 / 0,563). EM30 und G5 sind laengs gestreift (Tafel 03).
KOERNUNG_LAENGE = ADER_LAENGE_MED
# --- Fleisch-Koernung (V2 4 Zeile "Fleisch-Koernung +-1 sigma"): Faktor auf die Laengs-Koernung im Fleisch so, dass die Atlas-L*-Streuung der
#     Rotflaechen (gepoolt, n2_mass Mass (e)) den Familien-Median 11,2 (EM30 10,9 / G4 11,2 / G5 14,5, V2 1) trifft; Halbierung 0..3.
ZIEL_ROT_SDL = 11.2
FLEISCH_KOERNUNG = 1.0
RUECKEN_L = (45.0, 55.0)      # V2 3.2 K4b "helles Fleischrot L* 45-55"
# --- Rot-Farbton-Streuung (V1 B7 >= 5,8 Grad, Familie G5 5,8 / G4 9,4 / EM30 13,2): die harten Stufen der Familien-Rampe liegen bei 25-34 Grad
#     (Lauf v20: 4,0 Grad). Fleisch-ab je Texel um phi = k x Koernung/sigma gedreht (Farbton folgt den Laengsstreifen), k so, dass die
#     Farbton-Streuung der Rottexel den Familien-Median G4 9,4 Grad trifft.
ZIEL_ROT_TON_SD = 9.4
# --- Obergrenze der Linienzahl (gegen Zellnetz/Rauschen, V2 2 Zusatzbedingung): Linien je 1000 Texel Fleisch / Haut hoechstens Familienmaximum
#     (V2 1: Fleisch 1,52 / 3,01 / 3,45, Haut 2,37 / 2,22 / 3,50); der Kopf hat keine eigene Zeile und bekommt die Hautgrenze.
JE1000_FLEISCH, JE1000_HAUT = 3.45, 3.50
JE1000_KOPF = 1.22     # EM30-Kopf, Linien je 1000 Texel (n2_mass an EM30 Mesh 9, farbe_lesung/N2_messung.json 'EM30_kopf')

# --- Bild-1-Ruecken (V2 3.2 K4b): 15-18 % der in Bild 1 sichtbaren Ruecken/Flanke-Pixel helles Fleisch -> Ziel Mitte 16,5 % Kernpixel.
RUECKEN_ZIEL = 0.165
# --- Haut-Buntheit (V2 3.2 K1/K2, V8): sichtbare Haut ~1,5 x r3, gekappt a* <= 11 ausserhalb der Wunden (unter der Ketten-Rotklasse a* >= 12),
#     kuehl C* <= 8 (Blau-Klasse B3, kf_mal3.C_BLAU). Start der Eichung g = 1,48 (r3-Endstand) x 1,5 (V8).
G_START = 1.48 * 1.5
A_MAX_HAUT = 11.0
# --- K1-Ziel der Eichung (V2 3.2): Rundlauf des Originals C* 7,85; Fenster 0,85-1,2 x 7,85 = 6,67-9,42.
K1_RUNDLAUF = 7.85
K1_FENSTER = (0.85 * K1_RUNDLAUF, 1.2 * K1_RUNDLAUF)
# --- Rechenparameter (V2 4: nur mit Unabhaengigkeitsnachweis; Laeufe K_GRAPH 4/16 und SPIEGEL_ABSTAND 1,0/4,0 im Dossier)
K_GRAPH = M3.K_GRAPH            # 8
SPIEGEL_ABSTAND = M3.SPIEGEL_ABSTAND   # 2,0

_CACHE = {}


def _cache_pfad(name):
    os.makedirs(CACHE, exist_ok=True)
    return os.path.join(CACHE, name)


# ---------------------------------------------------------------- Geometrie: Achsen, Gliedbreiten, Abstaende
def mesh_geometrie(G):
    """Je Mesh: Achse (PCA, groesster Eigenvektor), Gliedhalbbreite rho = sqrt(l2 + l3), Texelabstand (Median Nachbarabstand im Mesh),
    Gliedhalbbreite in Texeln. Je Knoten: lokaler Texelabstand des Meshs."""
    if 'mg' in _CACHE:
        return _CACHE['mg']
    P, msh = G['P'], G['mesh'][G['ys'], G['xs']]
    out, ab_knoten = {}, np.zeros(len(P))
    for m in np.unique(msh):
        q = msh == m
        X = P[q]
        if len(X) < 8:
            ab_knoten[q] = G['abstand']
            continue
        ev, evec = np.linalg.eigh(np.cov(X.T))
        d, _ = cKDTree(X).query(X, k=2)
        ab = float(np.median(d[:, 1]))
        rho = float(math.sqrt(max(ev[0] + ev[1], 0.0)))
        out[int(m)] = dict(achse=evec[:, 2].tolist(), rho=rho, abstand=ab, halbbreite_texel=rho / ab, k_fern=int(round(math.pi * (rho / ab) ** 2)),
                           texel=int(q.sum()))
        ab_knoten[q] = ab
    _CACHE['mg'] = (out, ab_knoten)
    return out, ab_knoten


def hohlraum4(G):
    """Wie kf_mal3.hohlraum, aber K_NAH = 755 (Familien-Linienabstand) und K_FERN je Mesh = pi x Gliedhalbbreite^2 (V2 4). Zwischengespeichert."""
    mg, _ab = mesh_geometrie(G)
    kf = {m: v['k_fern'] for m, v in mg.items()}
    import hashlib
    schl = 'hohl_%d_%s_%d_%d' % (K_NAH, '_'.join('%d-%d' % (m, k) for m, k in sorted(kf.items())), len(G['P']), M3.K_GRAPH)
    p = _cache_pfad('hohl_%s.npy' % hashlib.sha1(schl.encode()).hexdigest()[:16])
    if os.path.exists(p):
        return np.load(p)
    P, N = G['P'], G['N']
    msh = G['mesh'][G['ys'], G['xs']]
    n = len(P)
    kmax = min(max(kf.values()) + 1, n)

    def mass(idx, k):
        out = np.zeros(len(idx))
        for a in range(0, len(idx), max(1, 4000000 // (k + 1))):
            ii = idx[a:a + max(1, 4000000 // (k + 1))]
            _d, nb = G['baum'].query(P[ii], k=min(k + 1, n))
            v = P[nb[:, 1:]] - P[ii, None, :]
            ln = np.maximum(np.linalg.norm(v, axis=2), 1e-6)
            out[a:a + len(ii)] = (np.einsum('ijk,ik->ij', v, N[ii]) / ln).mean(1)
        return out
    nah = mass(np.arange(n), K_NAH)
    fern = np.zeros(n)
    for m in np.unique(msh):
        q = np.nonzero(msh == m)[0]
        k = min(kf.get(int(m), K_NAH), kmax)
        fern[q] = mass(q, k)
    # grobe Stufe nur Falten (positiv), wie r3 (Lauf v0: duenne Glieder sonst als Ganzes heller, A11)
    hr = nah + np.maximum(fern, 0.0)
    np.save(p, hr)
    return hr


def achsfeld(G):
    """Je Knoten: Richtung der Muskelachse im Atlas (Winkel theta, Bogenmass, gegen +x). 3D-Achse des Meshs in die Tangentialebene
    projiziert, ueber die lokale Jacobi-Matrix (dP/dx, dP/dy aus den Atlas-Nachbarn im selben Mesh) nach (du, dv) abgebildet,
    danach im Doppelwinkel im Mesh geglaettet (Gauss sigma = Glaettung der Farbkarte SIG_GROB 2,5 Texel, kf_mal.py:40-42)."""
    if 'th' in _CACHE:
        return _CACHE['th']
    mg, _ab = mesh_geometrie(G)
    pos, mesh, gen = G['pos'], G['mesh'], G['genutzt']
    h, w = gen.shape
    ys, xs = G['ys'], G['xs']

    def nachbar(dy, dx):
        y2, x2 = ys + dy, xs + dx
        ok = (y2 >= 0) & (y2 < h) & (x2 >= 0) & (x2 < w)
        ok[ok] &= gen[y2[ok], x2[ok]] & (mesh[y2[ok], x2[ok]] == mesh[ys[ok], xs[ok]])
        P2 = np.zeros((len(ys), 3))
        P2[ok] = pos[y2[ok], x2[ok]]
        return ok, P2
    P0 = pos[ys, xs].astype(np.float64)
    J = []
    for (d1, d2) in (((0, 1), (0, -1)), ((1, 0), (-1, 0))):
        o1, p1 = nachbar(*d1)
        o2, p2 = nachbar(*d2)
        g = np.where((o1 & o2)[:, None], (p1 - p2) / 2, np.where(o1[:, None], p1 - P0, np.where(o2[:, None], P0 - p2, np.nan)))
        J.append(g)
    Jx, Jy = J
    msh = mesh[ys, xs]
    A = np.array([mg.get(int(m), dict(achse=[1, 0, 0]))['achse'] for m in msh])
    N = G['N']
    At = A - (A * N).sum(1)[:, None] * N
    # kleinste Quadrate [Jx Jy] (du, dv) = At
    a11, a12, a22 = (Jx * Jx).sum(1), (Jx * Jy).sum(1), (Jy * Jy).sum(1)
    b1, b2 = (Jx * At).sum(1), (Jy * At).sum(1)
    det = a11 * a22 - a12 * a12
    du = (a22 * b1 - a12 * b2) / np.where(np.abs(det) > 1e-12, det, np.nan)
    dv = (a11 * b2 - a12 * b1) / np.where(np.abs(det) > 1e-12, det, np.nan)
    th = np.arctan2(dv, du)
    ok = np.isfinite(th)
    c2 = np.zeros((h, w))
    s2 = np.zeros((h, w))
    wt = np.zeros((h, w))
    c2[ys[ok], xs[ok]] = np.cos(2 * th[ok])
    s2[ys[ok], xs[ok]] = np.sin(2 * th[ok])
    wt[ys[ok], xs[ok]] = 1.0
    C = np.zeros((h, w))
    S = np.zeros((h, w))
    for m in np.unique(msh):
        mm = (mesh == m) & gen
        W = cv2.GaussianBlur(wt * mm, (0, 0), M3.SIG_GROB)
        C += np.where(mm, cv2.GaussianBlur(c2 * mm, (0, 0), M3.SIG_GROB) / np.maximum(W, 1e-9), 0)
        S += np.where(mm, cv2.GaussianBlur(s2 * mm, (0, 0), M3.SIG_GROB) / np.maximum(W, 1e-9), 0)
    thA = 0.5 * np.arctan2(S, C)
    _CACHE['th'] = thA
    return thA


# ---------------------------------------------------------------- Linien zeichnen
def poisson3d(G, kand, r, rng):
    """Saatpunkte: Kandidaten (Knotenindizes) in zufaelliger Reihenfolge (Saat), angenommen wenn kein angenommener Punkt naeher als r Texel
    (3D, in Texeln des Meshs) liegt (Poisson-Scheibe in 3D, V2 5.2)."""
    if len(kand) == 0:
        return np.zeros(0, int)
    _mg, ab = mesh_geometrie(G)
    P = G['P'][kand] / ab[kand][:, None]
    baum = cKDTree(P)
    frei = np.ones(len(kand), bool)
    an = []
    for i in rng.permutation(len(kand)):
        if not frei[i]:
            continue
        an.append(i)
        frei[baum.query_ball_point(P[i], r)] = False
    return kand[np.array(an, int)]


def linie(start_y, start_x, theta, erlaubt, laenge, breite, rng, quer=False, versatz=0.0, besetzt=None):
    """Polylinie im Atlas ab (start_x, start_y) entlang des Achsfelds theta (+ 90 Grad bei quer, + versatz), Schritt 0,5 Texel, beidseitig
    je laenge/2; Abbruch beim Verlassen von 'erlaubt' (Zone und Mesh). Rueckgabe: Texelmaske-Indizes (ys, xs) und gezeichnete Laenge."""
    h, w = erlaubt.shape
    pts = []
    for sgn in (1.0, -1.0):
        x, y = start_x + 0.5, start_y + 0.5
        vor = None
        s = 0.0
        seite = []
        while s < laenge / 2:
            ix, iy = int(x), int(y)
            if not (0 <= ix < w and 0 <= iy < h) or not erlaubt[iy, ix] or (besetzt is not None and besetzt[iy, ix]):
                break
            t = theta[iy, ix] + (math.pi / 2 if quer else 0.0) + versatz
            d = np.array([math.cos(t), math.sin(t)])
            if vor is None:
                d = d * sgn
            elif d @ vor < 0:
                d = -d
            seite.append((x, y))
            vor = d
            x, y = x + 0.5 * d[0], y + 0.5 * d[1]
            s += 0.5
        pts = (seite[::-1] + pts) if sgn < 0 else (pts + seite)
    if len(pts) < 2:
        return None, 0.0
    P = np.array(pts)
    r = breite / 2
    x0, x1 = int(max(P[:, 0].min() - r - 1, 0)), int(min(P[:, 0].max() + r + 1, w - 1))
    y0, y1 = int(max(P[:, 1].min() - r - 1, 0)), int(min(P[:, 1].max() + r + 1, h - 1))
    YY, XX = np.mgrid[y0:y1 + 1, x0:x1 + 1]
    C = np.stack([XX.ravel() + 0.5, YY.ravel() + 0.5], 1)
    d = cKDTree(P).query(C)[0]
    sel = (d <= r) & erlaubt[YY.ravel(), XX.ravel()]
    lang = float(np.linalg.norm(np.diff(P, axis=0), axis=1).sum())
    return (YY.ravel()[sel], XX.ravel()[sel]), lang


def linien_feld(G, kand_knoten, erlaubt2, theta, messen, ziel, l_med, l_p90, breite, rng, quer=False, je1000_max=None, frei=None, sigma=None):
    """Linien in der Zone der Kandidaten. Dichte: Saatabstand r (Poisson-Scheibe 3D) per Halbierung so, dass messen(maske) - das V2-Mass
    (n2_mass.linien_stat, Anteil dunkler Linien in % der gueltigen Zonentexel) am fertigen Atlas - den Zielwert trifft (Ziel = Familienwert,
    Quelle beim Aufruf). Laengen lognormal (Median l_med, p90 l_p90 als Messlaenge 4 sqrt(l1)), Richtung Achsfeld (+90 Grad bei quer)
    mit normalverteilter Abweichung SIGMA_WINKEL je Linie."""
    if len(kand_knoten) == 0:
        return np.zeros(erlaubt2.shape, bool), dict(linien=0)
    ys, xs, msh = G['ys'], G['xs'], G['mesh']
    sig = math.log(l_p90 / l_med) / 1.2816
    best = None
    lo, hi = 1.0, 80.0
    for _ in range(10):
        r = math.sqrt(lo * hi)
        rr = np.random.default_rng(rng)
        saat = poisson3d(G, kand_knoten, r, rr)
        mask = np.zeros(erlaubt2.shape, bool)
        besetzt = np.zeros(erlaubt2.shape, bool) if frei is None else frei.copy()
        n_lin, laengen = 0, []
        for j, k in enumerate(saat):
            y, x = ys[k], xs[k]
            l_mess = float(rr.lognormal(math.log(l_med), sig))
            erl = erlaubt2 & (msh == msh[y, x])
            v = float(rr.normal(0.0, SIGMA_WINKEL if sigma is None else sigma)) if (sigma is None or sigma > 0) else 0.0
            if quer == 'beste':
                # laengs der Achse; ist das Fleischband dort kuerzer als die Linie, quer (entlang des Bandes) - die laengere gewinnt
                p1, g1 = linie(y, x, theta, erl, l_mess / LAENGE_MESS, breite, rr, quer=False, versatz=v, besetzt=besetzt)
                p2, g2 = linie(y, x, theta, erl, l_mess / LAENGE_MESS, breite, rr, quer=True, versatz=v, besetzt=besetzt)
                px, lg = (p1, g1) if g1 >= g2 else (p2, g2)
            else:
                q = (j % 2 == 1) if quer == 'wechsel' else quer
                px, lg = linie(y, x, theta, erl, l_mess / LAENGE_MESS, breite, rr, quer=q, versatz=v, besetzt=besetzt)
            if px is None:
                continue
            mask[px] = True
            # keine Kreuzung/Beruehrung: Abstand 2 Texel (= Halbbreite der 5x5-Schliessung des Top-Hat-Masses, V2 1 (a)), sonst Netz statt Linien
            y0, y1 = max(px[0].min() - 2, 0), min(px[0].max() + 3, besetzt.shape[0])
            x0, x1 = max(px[1].min() - 2, 0), min(px[1].max() + 3, besetzt.shape[1])
            sub = np.zeros((y1 - y0, x1 - x0), np.uint8)
            sub[px[0] - y0, px[1] - x0] = 1
            besetzt[y0:y1, x0:x1] |= cv2.dilate(sub, np.ones((5, 5), np.uint8)).astype(bool)
            n_lin += 1
            laengen.append(lg * LAENGE_MESS)
        wert, je1000 = messen(mask)
        if os.environ.get('KF4_DEBUG'):
            print('   linien r %.2f n %d texel %d -> %.2f %% je1000 %.2f (Ziel %.2f)' % (r, n_lin, int(mask.sum()), wert, je1000, ziel), flush=True)
        zu_dicht = je1000_max is not None and je1000 > je1000_max
        kand = (mask, dict(linien=n_lin, saatabstand=round(r, 2), mass=round(wert, 2), je1000=je1000, ziel=ziel, je1000_max=je1000_max,
                           laenge_med_gezeichnet=round(float(np.median(laengen)), 1) if laengen else 0))
        if not zu_dicht and (best is None or abs(wert - ziel) < abs(best[1]['mass'] - ziel)):
            best = kand
        if wert < ziel and not zu_dicht:
            hi = r
        else:
            lo = r
    return best if best is not None else kand


def auftragen(Lmap, maske, kontrast, gen, mesh, hof_anteil=0.0):
    """Linie mit garantiertem Kontrast: Linientexel L* - kontrast (nicht unter 0); wo die Haut dafuer zu dunkel ist, wird der fehlende Rest als
    Aufhellung des Hofs (2 Texel = Halbbreite der 5x5-Schliessung des Top-Hat-Masses, V2 1 (a)) aufgetragen - die Linie ist dann eine Fuge
    zwischen zwei Wuelsten. Hof nur im selben Mesh, nicht auf anderen Linien."""
    L = Lmap.copy()
    neu = np.maximum(L - kontrast * (1 - hof_anteil), 0.0)
    rest = np.where(maske, kontrast - (L - neu), 0.0)
    L = np.where(maske, neu, L)
    if (rest > 1e-6).any():
        k5 = np.ones((5, 5), np.uint8)
        hof = cv2.dilate(maske.astype(np.uint8), k5).astype(bool) & ~maske & gen
        r5 = cv2.dilate(rest.astype(np.float32), k5)
        mm = cv2.dilate(np.where(maske, mesh, -1).astype(np.float32), k5)
        hof &= (mm == mesh)
        L = np.where(hof, L + r5, L)
    return L


def atlas_linien(G, X, zone_kn, fleisch_von_rot=True):
    """V2-Mass (n2_mass.linien_stat) am Lab-Knotenvektor X: Atlas-L* (ungenutzt aus dem naechsten genutzten gefuellt), gueltig = 3x3 im selben
    Mesh; Zone 'fleisch' = Rotklasse A3 + 2 Texel (5x5) in der Zone, 'haut' = Rest der Zone, 'zone' = alles. Rueckgabe Funktion
    (maske_dunkel, kontrast, teil) -> Anteil %."""
    import n2_mass as N
    ys, xs, gen, mesh = G['ys'], G['xs'], G['genutzt'], G['mesh']
    h, w = gen.shape
    Lab = np.full((h, w, 3), np.nan, np.float32)
    Lab[ys, xs] = X
    mid = np.where(gen, mesh, -1)
    from scipy import ndimage as ND
    mmin = ND.minimum_filter(np.where(gen, mid, -1), 3, mode='constant', cval=-1)
    mmax = ND.maximum_filter(np.where(gen, mid, 10 ** 6), 3, mode='constant', cval=10 ** 6)
    valid = gen & (mmin == mid) & (mmax == mid)
    zone = np.zeros((h, w), bool)
    zone[ys[zone_kn], xs[zone_kn]] = True
    rot = N.rotklasse(np.nan_to_num(Lab)) & zone
    fleisch = cv2.dilate(rot.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & zone
    teile = dict(fleisch=fleisch, haut=zone & ~fleisch, zone=zone)
    L0 = np.nan_to_num(Lab[..., 0])

    def f(maske, kontrast, teil, canny=False, hof=0.0):
        L1 = auftragen(L0, maske, kontrast, gen, mesh, hof)
        L0f = N.fuellen(L1, gen)
        if canny:
            e = N.kanten(L0f, valid, N.SCHW['6/12'])
            z = teile[teil]
            return 100.0 * float((e & z).sum()) / max(int((z & valid).sum()), 1), N.linien_stat(L0f, valid, z)[0]['je1000']
        st = N.linien_stat(L0f, valid, teile[teil])[0]
        return st['anteil_p'], st['je1000']
    return f


# ---------------------------------------------------------------- Bau
def standard_eich():
    e = json.load(open(os.path.join(HIER, 'kf_eich3.json'), encoding='utf-8'))
    out = dict(zonen={z: dict(d=list(v['d'])) for z, v in e['zonen'].items()}, auge=list(e['auge']), g_haut=G_START, runden=[],
               start='kf_eich3.json (r3-Endstand), g = 1,48 x 1,5 (V8)')
    return out


def bauen(aus, eich=None, sicht_pfad=SICHT, roh=None, k_graph=None, spiegel=None):
    if k_graph is not None:
        M3.K_GRAPH = k_graph
    if spiegel is not None:
        M3.SPIEGEL_ABSTAND = spiegel
    eich = eich or (json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich())
    G = M3.oberflaeche()
    tim, rgb, zeile, mesh, genutzt = G['tim'], G['rgb'], G['zeile'], G['mesh'], G['genutzt']
    ys, xs = G['ys'], G['xs']
    h, w = genutzt.shape
    n_t = len(ys)
    sicht = dict(np.load(sicht_pfad))
    F = M1.felder()

    def feld(n):
        x, y, ww, hh = F[n]
        m = np.zeros((h, w), bool)
        m[y:y + hh, x:x + ww] = True
        return m & genutzt
    kiefer2, kralle2, zahn2, auge2, kopf2 = (feld('s3_' + n) for n in ('kiefer', 'kralle', 'zahn', 'auge', 'kopf'))
    kiefer2 &= mesh == 23
    sonder2 = kiefer2 | kralle2 | zahn2 | auge2
    haut2 = genutzt & ~sonder2
    T = lambda m2: m2[ys, xs]   # noqa: E731
    haut, kiefer, kralle, kopf = T(haut2), T(kiefer2), T(kralle2), T(kopf2)
    msh = mesh[ys, xs]
    Lk = M1.lab_f(rgb.astype(np.float32))[ys, xs].astype(np.float64)
    herkunft = {}
    konst = {}
    mg, ab_kn = mesh_geometrie(G)
    konst['mesh_geometrie'] = {m: dict(halbbreite_texel=round(v['halbbreite_texel'], 1), k_fern=v['k_fern'], abstand=round(v['abstand'], 2))
                               for m, v in mg.items()}

    # 1. Koernung (r3 Schritt 1, unveraendert)
    Ck = np.hypot(Lk[:, 1], Lk[:, 2])
    keim = haut & (Lk[:, 0] > M3.GLANZ_L) & (Ck < M3.GLANZ_C)
    g_keim = kopf & (Lk[:, 0] > M3.GAUMEN_L) & (Ck < M3.GAUMEN_C)
    hof = haut & (Lk[:, 0] > M3.HOF_L)
    hi = np.nonzero(hof)[0]
    _n, lab = connected_components(G['A'][hi][:, hi], directed=False)
    mit_keim = np.unique(lab[(keim | g_keim)[hi]])
    weg = keim | g_keim
    weg[hi[np.isin(lab, mit_keim)]] = True
    L0 = M3.harm3d(Lk[:, 0], haut & ~weg, haut, float(Lk[haut & ~weg, 0].mean()))
    struktur = np.where(haut, L0 - M3.glaetten3d(L0, haut, M3.SIG_GROB), 0.0)
    struktur_roh = struktur.copy()
    struktur = koernung_laengs(G, struktur, haut, KOERNUNG_LAENGE)
    struktur_kopf = struktur
    herkunft['koernung'] = 'K5c-eigene Struktur (H2), sigma %.1f; Glanz/Gaumen entfernt %d Texel' % (M3.SIG_GROB, int(weg.sum()))

    # 2. Farbkarte, Hautseiten mit Dezil-Stufen und gemessener Warm-Schwelle
    zentren, hrampen, stufen, warm_anteil = hautseiten4()
    konst['L_STUFEN'] = [round(x, 2) for x in stufen]
    S, nS, rotS, warmS = M3.screenshot_texel(sicht, G, zentren)
    gesehen = nS > 0
    Sh = S.copy()
    Sh[:, 0] = np.minimum(Sh[:, 0], M3.L_MAX_HAUT)
    Sh[:, 1:] = M3.blau_grenze(Sh[:, 1:])
    karte, bek = M3.spiegeln(np.c_[Sh, rotS, warmS], gesehen & haut, haut)
    karte = M3.harm3d(karte, bek, haut, np.r_[Sh[gesehen & haut].mean(0), 0.0, 0.5])
    karte = M3.glaetten3d(karte, haut, M3.SIG_GROB)
    herkunft['farbkarte'] = 'Screenshot T (Bild 1 %d / Bild 2 %d Texel sichtbar), Spiegelpunkt +%d, harmonisch, Glaettung %.1f' % (
        int((sicht['fig1'][ys, xs] > 0).sum()), int((sicht['fig2'][ys, xs] > 0).sum()), int((bek & ~gesehen).sum()), M3.SIG_GROB)

    # 3. Form (Hohlraum-Mass mit V2-Nachbarzahlen), Staerke auf L*-Streuung RE2-G4 14,9 (V1 1)
    hr_roh = hohlraum4(G)
    hr = M3.glaetten3d(hr_roh, genutzt[ys, xs], M3.SIG_GROB)
    hz = (hr - hr[haut].mean()) / max(hr[haut].std(), 1e-6)
    koerper = haut & (xs < 384)
    basisL = karte[:, 0] + struktur
    lo, hi_ = 0.0, 40.0
    for _ in range(40):
        k_ = (lo + hi_) / 2
        if np.std((basisL - k_ * hz)[koerper]) < M3.L_STREU_ZIEL:
            lo = k_
        else:
            hi_ = k_
    K_FORM = (lo + hi_) / 2
    formL = -K_FORM * hz
    konst['K_FORM'] = round(K_FORM, 3)
    herkunft['form'] = 'Hohlraum K_NAH %d (pi x 15,5^2), K_FERN je Mesh pi x Gliedhalbbreite^2; L* = -%.2f x Standardwert (G4 14,9)' % (K_NAH, K_FORM)

    # 4. Fleisch: Screenshot-Mehrheit + verdeckte Rumpf-Unterseite (r3) + Bild-1-Ruecken-Wunde (V2 3.2 K4b)
    w_s = karte[:, 3]
    # Screenshot-Fleisch nur auf gesehenen oder gespiegelten Texeln (bek), nicht auf harmonisch fortgesetzten: r3 liess das Klauen-Rot so um
    # die ganzen Glieder laufen -> Rot im Umrissband (B9 17,4 %; Zuordnung Lauf b9_*: Arme/Klauen 17,4 -> 12,4, Kopf/Krallen -> 11,0).
    fl_s = haut & (w_s >= M3.FLEISCH_SCHWELLE) & bek
    # verdeckt = in keinem der beiden Bilder als Figurpunkt sichtbar (kf_sicht 'fig1'/'fig2'), nicht nur ausserhalb der Nutzer-Zonen (r3 ~gesehen):
    # Lauf v14 legte sonst untere Flanke, die Bild 1 zeigt, als verdeckt rot (K4b 25 %).
    sichtbar_fig = (sicht['fig1'][ys, xs] > 0) | (sicht['fig2'][ys, xs] > 0)
    rumpf = haut & np.isin(msh, M3.RUMPF) & ~gesehen & ~sichtbar_fig
    ny = G['N'][:, 1]
    rk, rinfo = ruecken_wunde(G, sicht, S, rotS, haut & ~kopf, fl_s, ab_kn)
    # Verdeckte Rumpf-Wunde: r3 nahm die Unterseite ny > 0 (B9 17,4 %: das Bauchrot lag im Seitenblick am unteren Umriss, die Flanke blieb leer).
    # Jetzt: verdeckte Rumpftexel nach ny absteigend (Unterseite zuerst, dann untere Flanke), bis das Fleisch FLEISCH_ANTEIL der Koerpertexel
    # deckt = Fleischrot RE2-G4 23,9 % (V1 1, Vorbild der Riesenform). Laeufe ny > -0,2 / -0,4 / -0,6: B9 11,1 / 9,3 / 8,0 %, A3 18,8 / 21,3 / 23,5 %.
    basis = (fl_s | rk) & koerper
    n_ziel = int(round(FLEISCH_ANTEIL * koerper.sum())) - int(basis.sum())
    kand = np.nonzero(rumpf & ~basis)[0]
    # Reihenfolge: Flanke vor Bauch (|ny| aufsteigend, V2 5.7 "groesste Wunde an Flanke/Bauch"). Laeufe: Unterseite zuerst (ny absteigend) ->
    # Rotflaechen im neutralen Render im Schatten (L* 19-24), Render-B14 7,8-8,2; Flanke zuerst -> 10,5, B9 7,9 %; Oberseite zuerst -> 10,8, B10 6,7.
    # zusammenhaengend gewachsen (Prioritaet |ny|, Start am flankigsten Texel) statt global sortiert: Lauf v21 mit globaler Sortierung -> mehrere
    # Flecken, groesste Rotflaeche Seite 3,05 % (B2 >= 3,2)
    fl_u = np.zeros(n_t, bool)
    if n_ziel > 0 and len(kand):
        import heapq
        ok_k = np.zeros(n_t, bool)
        ok_k[kand] = True
        A_ = G['A']
        start_k = int(kand[np.argmin(np.abs(ny[kand]))])
        heap = [(abs(ny[start_k]), start_k)]
        drin_ = {start_k}
        n_ = 0
        while heap and n_ < n_ziel:
            _p, i = heapq.heappop(heap)
            fl_u[i] = True
            n_ += 1
            for j in A_.indices[A_.indptr[i]:A_.indptr[i + 1]]:
                if ok_k[j] and j not in drin_:
                    drin_.add(j)
                    heapq.heappush(heap, (abs(ny[j]), j))
    ny_schwelle = float(np.abs(ny[fl_u]).max()) if fl_u.any() else 0.0
    konst['rumpf_ny_schwelle'] = round(ny_schwelle, 3)
    fl = fl_s | fl_u | rk
    # Bereinigung (Uebergang <= 1 Texel, V2 5.3): Kante wie r3 (Glaettung SIG_GROB, Mehrheit 0,5), danach Inseln und Loecher < FLAECHE_MIN
    roh_fl = fl.copy()
    fl = haut & (M3.glaetten3d(fl.astype(float), haut, M3.SIG_GROB) >= 0.5)
    fl = fl | rk
    fl = flaechen_bereinigen(G, fl, haut)
    # E14 streng: auf in Bild 1/2 sichtbaren Texeln nur Screenshot-Fleisch bzw. Ruecken-Wunde; Glaettung und Lochfuellung legten sonst 855 sichtbare
    # Texel (Huefte/Wirbel neben der verdeckten Rumpf-Wunde) rot (Lauf dbg, K4b 27 %). Danach nur noch Inseln < FLAECHE_MIN entfernen.
    fl &= ~sichtbar_fig | fl_s | rk
    fl = flaechen_bereinigen(G, fl, haut, nur_inseln=True)
    herkunft['fleisch_bereinigung'] = 'vorher %d, nachher %d Texel' % (int(roh_fl.sum()), int(fl.sum()))
    herkunft['fleisch'] = 'Screenshot-Mehrheit %d, Rumpf-Unterseite %d, Bild-1-Ruecken-Wunde %d Texel (%s); zusammen %.1f %% der Koerpertexel' % (
        int(fl_s.sum()), int(fl_u.sum()), int(rk.sum()), rinfo, 100.0 * (fl & koerper).sum() / koerper.sum())

    # 5. Wundstufen: geodaetischer Abstand zum Fleischrand auf der Oberflaeche (Texel des Meshs), harte Baender
    dist = wund_abstand(G, fl, ab_kn)
    fL_s = float(S[gesehen & (rotS >= M3.FLEISCH_SCHWELLE) & haut, 0].mean())
    u_b = (fl_u & ~fl_s).astype(float)
    kernL = fL_s * (1 - u_b) + M3.FLEISCH_BAUCH_L * u_b + formL
    kern_m = fl & (dist >= WUND_INNEN)
    # Hell-Kern je Wunde (Zusammenhangskomponente des Fleischs auf dem Oberflaechengraph): helle Haelfte der Kerntexel jeder Wunde (Median
    # je Wunde). Lauf v18 mit einem globalen Median: grosse Wunden ganz dunkel oder ganz hell -> Render-Streuung der Rotflaechen 7,8 (B14 < 8,2).
    fi = np.nonzero(fl)[0]
    _nw, wlab = connected_components(G['A'][fi][:, fi], directed=False)
    kern_i = np.zeros(n_t, int)
    for wnr in range(_nw):
        kk_ = fi[(wlab == wnr)]
        kk_ = kk_[kern_m[kk_]]
        if len(kk_):
            kern_i[kk_] = (kernL[kk_] >= np.quantile(kernL[kk_], 1 - HELLKERN_ANTEIL)).astype(int)
    kern_i = np.where(rk, 1, kern_i)                         # Ruecken-Wunde: Familienwunde (V2 3.2 K4b)
    fleisch = np.zeros((n_t, 3))
    fleisch[:] = np.asarray(KERN_STUFEN)[kern_i]
    innen = fl & (dist < WUND_INNEN)
    rand = fl & (dist < WUND_RAND)
    fleisch[innen] = G4_FLEISCHROT
    fleisch[rand] = G4_DUNKELROT
    # Screenshot-Farbabweichung wo sichtbar (r3: Z1 bleibt Z1; haelt die Rot-Farbton-Streuung B7), glatt -> Stufen bleiben hart
    ka, kb = M3.rampe(karte[:, 0], M3.RAMPE_FLEISCH)
    s_gew = np.clip(w_s, 0, 1)

    fleisch[:, 1:] += s_gew[:, None] * (karte[:, 1:3] - np.c_[ka, kb])
    stufe = np.where(rand, 0, np.where(innen, 1, 2 + kern_i))
    herkunft['wunden'] = 'Rand < %.0f Texel G4-Dunkelrot %d, Innenrand < %.0f Texel G4-Fleischrot %d, Kern %s Texel' % (
        WUND_RAND, int(rand.sum()), WUND_INNEN, int((innen & ~rand).sum()),
        [int(((~innen) & fl & (kern_i == i)).sum()) for i in range(len(KERN_STUFEN))])

    # 6. Haut: Rampen der Seite, Buntheit g, ohne Faltenfaerbung (V2 4: Falten werden Linien)
    g = float(eich.get('g_haut', G_START))
    haut_L = basisL + formL
    t_warm = float(np.quantile(karte[haut, 4], 1 - warm_anteil))
    konst['warm_schwelle'] = round(t_warm, 3)
    konst['warm_anteil_bild'] = round(warm_anteil, 3)
    tw = M3.glaetten3d((karte[:, 4] >= t_warm).astype(float), haut, M3.SIG_GROB)
    wa, wb = M3.rampe(haut_L, hrampen[0])
    ka_, kb_ = M3.rampe(haut_L, hrampen[1])
    haut_ab = (np.c_[wa, wb] * tw[:, None] + np.c_[ka_, kb_] * (1 - tw[:, None])) * g
    herkunft['hautseiten'] = 'k-Mittel Bild 1 und 2: warm %s, kuehl %s; Warm-Anteil Bildpunkte %.3f -> Schwelle %.3f; warm %.1f %% der Haut; g %.3f' % (
        np.round(zentren[0], 1).tolist(), np.round(zentren[1], 1).tolist(), warm_anteil, t_warm, 100 * float((tw[haut] >= 0.5).mean()), g)

    X = np.zeros((n_t, 3))
    X[:, 0] = np.where(fl, fleisch[:, 0], haut_L)
    X[:, 1:] = np.where(fl[:, None], fleisch[:, 1:], haut_ab)

    # 7. Zonenversatz nur auf die Haut (r3)
    num = np.zeros((n_t, 3))
    den = np.zeros(n_t)
    for z, v in eich['zonen'].items():
        c = sicht[z][ys, xs].astype(np.float64)
        c = c * (kiefer if z == 'b2|Unterkiefer' else ~kiefer)
        num += c[:, None] * np.asarray(v['d'], np.float64)
        den += c
    bek_z = den > 0
    dz = np.zeros((n_t, 3))
    dz[bek_z] = num[bek_z] / den[bek_z][:, None]
    dz = M3.harm3d(dz, bek_z & haut, haut, 0.0) * haut[:, None] + dz * kiefer[:, None]
    hautm = haut & ~fl
    X = np.where(hautm[:, None], X + dz, X)
    # Haut-Buntheit gekappt (V2 3.2 K1/V8): a* <= 11 ausserhalb der Wunden; Blau-Klasse folgt im Farbfenster (C* <= 8)
    X[:, 1] = np.where(hautm, np.minimum(X[:, 1], A_MAX_HAUT), X[:, 1])

    # Fleisch-Koernung (V2 4): Laengs-Koernung x c_f im Fleisch, c_f so, dass die gepoolte Atlas-L*-Streuung der Rotflaechen OHNE Linien den
    # Familien-Median 11,2 trifft (EM30 10,9 / G4 11,2 / G5 14,5, V2 1); die Adern kommen danach obendrauf (gemessen MIT Koernung).
    # Laeufe: Koernung nach den Adern (v16/v17: c_f 0, Fleisch flach) -> Render-B14 7,7-7,8 < 8,2; c_f 1 / 1,5 / 2 nachtraeglich auf die
    # kalibrierten Adern -> B10 5,3 / 3,6 / 2,9 (Streifen verschmelzen mit den Adern). Reihenfolge Koernung -> Adern (wie v4: alle B10-B17 gruen).
    c_kal, sdl0 = fleisch_koernung(G, X, fl, struktur, (xs < 384) & genutzt[ys, xs])
    # Ergebnis der Halbierung: c 0 (die Stufen allein geben 11,2). Damit bleibt das Fleisch im Render flach (Render-B14 7,8 < 8,2; Rotflaechen
    # liegen an der Flanke im Halbschatten, Licht 0,55-1,0). Deshalb Faktor 1 = dieselbe Koernung wie die Haut (E8 "aus einem Guss"); Pruefung:
    # die Atlas-Streuung bleibt in der Familienspanne 10,9-14,5 (V2 4), Laeufe cf 1 / 1,5 / 2,5: Atlas 12,1 / 12,8 / 13,7, B10 7,9 / 4,7 / 1,7.
    c_f = FLEISCH_KOERNUNG
    X[:, 0] = np.where(fl, X[:, 0] + c_f * struktur, X[:, 0])
    konst['fleisch_koernung'] = dict(faktor=c_f, halbierung_faktor=round(c_kal, 3), sdL_pool_ohne=round(sdl0, 2), ziel=ZIEL_ROT_SDL)
    # Bild-1-Ruecken-Wunde: Kern helles Fleisch L* 45-55 (V2 3.2 K4b); die Koernung darf ihn nicht darueber heben (K5: geclippte Bildpunkte
    # Bild 1 0,9 % > 0,75 % in Lauf v20)
    X[:, 0] = np.where(rk & (dist >= WUND_INNEN), np.clip(X[:, 0], RUECKEN_L[0], RUECKEN_L[1]), X[:, 0])

    # 8. Linien: Adern im Fleisch, Falten in der Haut, Kopf
    theta = achsfeld(G)
    rng = SAAT_MAL
    Xa_L = np.zeros((h, w))
    Xa_L[ys, xs] = X[:, 0]
    fl2 = np.zeros((h, w), bool)
    fl2[ys, xs] = fl
    in2 = np.zeros((h, w), bool)
    in2[ys, xs] = fl & ~rand
    kopf_k = kopf & ~fl
    koerper_kn = genutzt[ys, xs] & (xs < 384)
    mess_k = atlas_linien(G, X, koerper_kn)
    # Adern: Saat im Fleisch ausserhalb des dunklen Rands; Linien laufen im ganzen Fleisch (auch durch Rand/Innenrand) und im Mesh; Ziel B10
    fl2k = fl2 & ~kopf2
    ader, ainfo = linien_feld(G, np.nonzero(fl & ~rand & ~kopf)[0], fl2k, theta, lambda m: mess_k(m, ADER_KONTRAST, 'fleisch', hof=ADER_HOF),
                              ZIEL_ADERN, ADER_LAENGE_MED, ADER_LAENGE_P90, ADER_BREITE, rng, je1000_max=JE1000_FLEISCH, quer='beste')
    Lm0 = np.zeros((h, w))
    Lm0[ys, xs] = X[:, 0]
    Xm = X.copy()
    Xm[:, 0] = auftragen(Lm0, ader, ADER_KONTRAST, genutzt, mesh, ADER_HOF)[ys, xs]
    mess_h = atlas_linien(G, Xm, koerper_kn)
    # Falten: Saat in Hohlraeumen der Haut (Hohlraum-Mass > Mittel = Falte/Beuge/Achsel, r3-Definition), quer zur Achse; Ziel B11
    hk = hautm & ~kopf & (hz > 0)
    haut2m = np.zeros((h, w), bool)
    haut2m[ys, xs] = hautm & ~kopf
    frei_a = cv2.dilate(ader.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
    falte, finfo = linien_feld(G, np.nonzero(hk)[0], haut2m, theta, lambda m: mess_h(m, FALTE_KONTRAST, 'haut'),
                               ZIEL_FALTEN, ADER_LAENGE_MED, ADER_LAENGE_P90, FALTE_BREITE, rng + 1, quer=True, je1000_max=JE1000_HAUT, frei=frei_a)
    # Kopf: Linien abwechselnd laengs (Platten/Stirn) und quer (Schnauzenfalten) zur Kopfachse; Dichte nach der Canny-Dichte des Kopffelds,
    # Ziel B12 = Familienminimum EM30-Kopf 29,6 (V2 1 Kopf-Zeile: EM30 29,6 / G4 30,1)
    kopf2m = np.zeros((h, w), bool)
    kopf2m[ys, xs] = kopf_k
    mess_kopf = atlas_linien(G, X, kopf)
    # Kopflinien: Dichte nach Linien je 1000 Texel = EM30-Kopf 1,22 (n2_mass, Familie EM30 1,22 / G4 1,02); Lauf kopf_3.5_2.1: Dichte 6,9 je
    # 1000 (Canny-Ziel ueber Linien allein) -> HF Kopf 0,262 (A6 > 0,26), A7 0,247 > 0,19, "Leopard"-Striche.
    def _je(m):
        v = mess_kopf(m, KOPF_KONTRAST, 'zone')[1]
        return v, 0.0
    kl1, k1info = linien_feld(G, np.nonzero(kopf_k)[0], kopf2m, theta, _je, JE1000_KOPF, KOPF_LAENGE_MED, KOPF_LAENGE_P90, KOPF_BREITE,
                              rng + 2, quer=KOPF_QUER, sigma=KOPF_SIGMA)
    # Kopf-Koernung: Laengs-Koernung x c_k auf dem Kopffeld, c_k so, dass die Canny-Dichte des Kopffelds (mit Linien) das Familienminimum
    # EM30-Kopf 29,6 (V2 1) trifft. EM30 holt die Kanten aus gemalter Flaechenstruktur, nicht aus Linien (1,22 je 1000).
    Lk0 = np.zeros((h, w))
    Lk0[ys, xs] = X[:, 0]
    Lk0 = auftragen(Lk0, kl1, KOPF_KONTRAST, genutzt, mesh)
    kopf_nk = kopf & ~fl

    from p_messen import hf_fft
    kopf2 = np.zeros((h, w), bool)
    kopf2[ys[kopf], xs[kopf]] = True

    def _canny(c):
        Y = X.copy()
        Y[:, 0] = Lk0[ys, xs] + np.where(kopf_nk, c * struktur_kopf, 0.0)
        Lm_ = np.zeros((h, w))
        Lm_[ys, xs] = Y[:, 0]
        hf = hf_fft(Lm_, np.where(genutzt, mesh, -1), kopf2)
        return atlas_linien(G, Y, kopf)(np.zeros((h, w), bool), 0.0, 'zone', canny=True)[0], hf
    # Halbierung: Canny-Ziel 29,6; Obergrenze HF_fft Kopffeld 0,26 (V1 A6, Familienmaximum Zombie-Kopf 0,259) am Float-Atlas
    lo_, hi_ = 0.0, 8.0
    for _ in range(10):
        m_ = (lo_ + hi_) / 2
        ca, hf = _canny(m_)
        if ca < ZIEL_KOPF_CANNY and hf <= HF_MAX:
            lo_ = m_
        else:
            hi_ = m_
    c_k = lo_
    X[:, 0] = np.where(kopf_nk, X[:, 0] + c_k * struktur_kopf, X[:, 0])
    ca, hf = _canny(c_k)
    konst['kopf_koernung'] = dict(faktor=round(c_k, 3), canny=round(ca, 2), hf_fft=round(hf, 3), ziel=ZIEL_KOPF_CANNY, hf_max=HF_MAX)
    kl2, k2info = np.zeros_like(kl1), {}
    kopflin = kl1 | kl2
    _ohne = os.environ.get('KF4_OHNE', '')
    if 'ader' in _ohne:
        ader = np.zeros_like(ader)
    if 'falte' in _ohne:
        falte = np.zeros_like(falte)
    if 'kopf' in _ohne:
        kopflin = np.zeros_like(kopflin)
    ader_k, falte_k, kopf_kn = ader[ys, xs], falte[ys, xs], kopflin[ys, xs]
    Lm = np.zeros((h, w))
    Lm[ys, xs] = X[:, 0]
    Lm = auftragen(Lm, ader, ADER_KONTRAST, genutzt, mesh, ADER_HOF)
    Lm = auftragen(Lm, falte & ~ader, FALTE_KONTRAST, genutzt, mesh)
    Lm = auftragen(Lm, kopflin & ~ader, KOPF_KONTRAST, genutzt, mesh)
    X[:, 0] = Lm[ys, xs]
    X[ader_k, 1:] = np.asarray(G4_DUNKELROT[1:])
    k_ton, sd_ton = farbton_streuung(X, fl, struktur, (xs < 384) & haut)
    konst['farbton_drehung'] = dict(k_grad=round(k_ton, 2), sd=round(sd_ton, 2), ziel=ZIEL_ROT_TON_SD)
    phi = np.radians(k_ton * struktur / max(float(struktur[haut].std()), 1e-9))
    a_, b_ = X[:, 1].copy(), X[:, 2].copy()
    X[:, 1] = np.where(fl, a_ * np.cos(phi) - b_ * np.sin(phi), a_)
    X[:, 2] = np.where(fl, a_ * np.sin(phi) + b_ * np.cos(phi), b_)
    herkunft['linien'] = dict(adern=ainfo, falten=finfo, kopf_laengs=k1info, kopf_quer=k2info,
                              texel=dict(adern=int(ader_k.sum()), falten=int(falte_k.sum()), kopf=int(kopf_kn.sum())))

    # Haut hoechstens L_MAX_HAUT 58 (V1 2: hellstes Original-Band T L* 51-58) auch nach Koernung/Form/Linien: Lauf v21 hatte Hauttexel bis L* 61
    # (Schwanz) -> geclippte Bildpunkte Bild 1 0,8 % > 0,75 % (K5)
    X[:, 0] = np.where(hautm, np.minimum(X[:, 0], M3.L_MAX_HAUT), X[:, 0])

    # 9. Kiefer, Krallen, Zaehne, Augen, Schwanz E9, Farbfenster (r3), Zahnfleisch 19,30
    Xa = kopfseite(G, X, sicht, S, gesehen, kiefer, kralle, dz, eich, F, zahn2, herkunft)
    kp2 = genutzt & (np.arange(w)[None, :] < 384)
    gm = [float(Xa[..., 0][kp2 & (mesh == g_)].mean()) for g_ in range(1, 6)]
    for i, g_ in enumerate(range(1, 6)):
        Xa[..., 0][kp2 & (mesh == g_)] += float(np.mean(gm)) - gm[i]
    herkunft['schwanz_e9'] = 'Glied-L* vorher %s' % [round(v, 1) for v in gm]
    fen = genutzt & ~zahn2 & ~auge2
    C_ = np.hypot(Xa[..., 1], Xa[..., 2])
    H_ = np.degrees(np.arctan2(Xa[..., 2], Xa[..., 1]))
    rest_ = fen & ((H_ < M3.H_MIN) | (H_ > M3.H_MAX)) & (C_ > M3.C_REST)
    f_ = np.where(rest_, M3.C_REST / np.maximum(C_, 1e-6), 1.0)
    Xa[..., 1] *= f_
    Xa[..., 2] *= f_
    blau = fen & (Xa[..., 2] < M3.B_MIN) & (np.hypot(Xa[..., 1], Xa[..., 2]) > M3.C_BLAU)
    Xa[..., 1:] = np.where(fen[..., None], M3.blau_grenze(Xa[..., 1:]), Xa[..., 1:])
    Xa[..., 0] = np.clip(Xa[..., 0], 0, 100)
    herkunft['farbfenster'] = 'ausserhalb %.0f..%.0f Grad C* <= %.1f: %d; Blau-Klasse (b* < %.0f, C* > %.0f) auf C* %.0f: %d' % (
        M3.H_MIN, M3.H_MAX, M3.C_REST, int(rest_.sum()), M3.B_MIN, M3.C_BLAU, M3.C_BLAU, int(blau.sum()))

    if os.environ.get('KF4_KAT'):            # nur Diagnose: Fleischherkunft einfaerben (gruen Screenshot, blau Rumpf, gelb Ruecken-Wunde)
        for m_, col in ((fl_s, (60, -60, 50)), (fl_u, (40, 20, -70)), (rk, (85, -5, 80))):
            mm_ = np.zeros((h, w), bool)
            mm_[ys[m_ & fl], xs[m_ & fl]] = True
            Xa[mm_] = col
    if os.environ.get('KF4_GRAU'):           # nur Diagnose (B9-Zuordnung): Meshes bzw. Felder grau
        for tok in os.environ['KF4_GRAU'].split(','):
            mm = (mesh == int(tok)) if tok.isdigit() else feld('s3_' + tok)
            Xa[mm & genutzt] = (30.0, 0.0, 0.0)
    out_rgb = M1.rgb_f(Xa.astype(np.float32))
    out_rgb[~genutzt] = 0
    hm2 = np.zeros((h, w), bool)
    hm2[ys, xs] = hautm
    vor = {}
    for z in eich['zonen']:
        c = sicht[z].astype(np.float64) * (kiefer2 if z == 'b2|Unterkiefer' else ~kiefer2)
        c = c * hm2
        vor[z] = float(Xa[..., 2][c > 0].max()) if (c > 0).any() else 0.0
    bau = M3.schreiben3(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, dict(konflikt_zeilen=G['konflikt'], b_max_zone=vor))
    fj = json.load(open(os.path.join(aus, 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe_v2'
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    bau['basis'] = 'K5c (varianten/K5/K5c): FORM.EMD byte-gleich, nur FORM.TIM neu (Versuch 2, kf_mal4.py)'
    bau['konstanten'] = dict(konst, SAAT_MAL=SAAT_MAL, K_NAH=K_NAH, LINIENABSTAND_FLEISCH=LINIENABSTAND_FLEISCH, LINIENABSTAND_HAUT=LINIENABSTAND_HAUT,
                             ADER=(ADER_LAENGE_MED, ADER_LAENGE_P90, ADER_BREITE, ADER_KONTRAST), FALTE=(FALTE_BREITE, FALTE_KONTRAST),
                             KOPF=(KOPF_BREITE, KOPF_KONTRAST), SIGMA_WINKEL=round(SIGMA_WINKEL, 4), WUND=(WUND_RAND, WUND_INNEN),
                             G4_DUNKELROT=G4_DUNKELROT, G4_FLEISCHROT=G4_FLEISCHROT, KERN_STUFEN=KERN_STUFEN, RUECKEN_ZIEL=RUECKEN_ZIEL,
                             A_MAX_HAUT=A_MAX_HAUT, ZAHNFLEISCH_A=ZAHNFLEISCH_A, K_GRAPH=M3.K_GRAPH, SPIEGEL_ABSTAND=M3.SPIEGEL_ABSTAND,
                             SIG_GROB=M3.SIG_GROB, L_MAX_HAUT=M3.L_MAX_HAUT, FLEISCH_SCHWELLE=M3.FLEISCH_SCHWELLE, RUMPF=M3.RUMPF,
                             L_STREU_ZIEL=M3.L_STREU_ZIEL, EINSTREU_A2=M3.EINSTREU_A2, B_MIN=M3.B_MIN, C_REST=M3.C_REST, H_MIN=M3.H_MIN,
                             H_MAX=M3.H_MAX, SAAT_CLUT=M3.SAAT)
    json.dump(bau, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=str)
    st2 = np.full((h, w), -1, np.int8)
    st2[ys, xs] = np.where(fl, stufe, -1)
    np.savez_compressed(os.path.join(aus, 'masken.npz'), fleisch=fl2, stufe=st2, adern=ader, falten=falte, kopflinien=kopflin)
    if roh is not None:
        roh.update(Xa=Xa, fl=fl2, hz=hz, stufe=st2, ader=ader, falte=falte, kopf=kopflin, theta=theta, dist=dist, fl_s=fl_s, fl_u=fl_u, rk=rk,
                   fl_kn=fl, roh_fl=roh_fl)
    return bau


def farbton_streuung(X, fl, struktur, zone_kn):
    """Halbierung k (0..40 Grad je sigma): Standardabweichung des Farbtons der Rottexel (a* > 15, C* > 20, -40..70 Grad, wie V1 B7) im Koerper
    nach Drehung der Fleisch-ab um k x Koernung/sigma."""
    z = struktur / max(float(struktur[zone_kn].std()), 1e-9)

    def sd(k):
        phi = np.radians(k * z)
        a = np.where(fl, X[:, 1] * np.cos(phi) - X[:, 2] * np.sin(phi), X[:, 1])
        b = np.where(fl, X[:, 1] * np.sin(phi) + X[:, 2] * np.cos(phi), X[:, 2])
        C = np.hypot(a, b)
        hh = np.degrees(np.arctan2(b, a))
        r = zone_kn & (a > 15) & (C > 20) & (hh > -40) & (hh < 70)
        return float(hh[r].std()) if r.sum() > 20 else 0.0
    lo, hi = 0.0, 40.0
    if sd(0.0) >= ZIEL_ROT_TON_SD:
        return 0.0, sd(0.0)
    for _ in range(12):
        m = (lo + hi) / 2
        if sd(m) < ZIEL_ROT_TON_SD:
            lo = m
        else:
            hi = m
    return hi, sd(hi)


def fleisch_koernung(G, X, fl, struktur, zone_kn):
    """Halbierung des Faktors c (0..3) fuer L*(Fleisch) + c x Koernung, bis die gepoolte L*-Streuung der Rotflaechen (n2_mass.rot_flaechen,
    Mindestflaeche 20 Texel) ZIEL_ROT_SDL erreicht."""
    import n2_mass as N
    ys, xs, gen = G['ys'], G['xs'], G['genutzt']
    h, w = gen.shape
    zone = np.zeros((h, w), bool)
    zone[ys[zone_kn], xs[zone_kn]] = True

    def sd(c):
        Lab = np.full((h, w, 3), np.nan, np.float32)
        Y = X.copy()
        Y[:, 0] = np.where(fl, Y[:, 0] + c * struktur, Y[:, 0])
        Lab[ys, xs] = Y
        f, _r = N.rot_flaechen(Lab, zone, gen & zone)
        return N.zus_rot(f).get('sdL_pool', 0.0)
    lo, hi = 0.0, 3.0
    if sd(lo) >= ZIEL_ROT_SDL:
        return 0.0, sd(0.0)
    for _ in range(10):
        m = (lo + hi) / 2
        if sd(m) < ZIEL_ROT_SDL:
            lo = m
        else:
            hi = m
    return hi, sd(hi)


def koernung_laengs(G, struktur, haut, laenge):
    """Koernung entlang der Muskelachse mitteln (Linienfaltung im Atlas, Schritt 0,5 Texel, Gesamtlaenge 'laenge', nur Texel desselben Meshs),
    danach auf die alte Streuung zurueckskaliert: aus den Wirbeln der r3-Koernung werden Straenge laengs der Achse."""
    th = achsfeld(G)
    ys, xs, gen, mesh = G['ys'], G['xs'], G['genutzt'], G['mesh']
    h, w = gen.shape
    Sm = np.zeros((h, w))
    hm = np.zeros((h, w), bool)
    Sm[ys, xs] = struktur
    hm[ys, xs] = haut
    t = th[ys, xs]
    c, s_ = np.cos(t), np.sin(t)
    acc = np.zeros(len(ys))
    n = np.zeros(len(ys))
    for k in np.arange(-laenge / 2, laenge / 2 + 1e-6, 0.5):
        x2 = np.floor(xs + 0.5 + k * c).astype(int)
        y2 = np.floor(ys + 0.5 + k * s_).astype(int)
        ok = (x2 >= 0) & (x2 < w) & (y2 >= 0) & (y2 < h)
        ok[ok] &= hm[y2[ok], x2[ok]] & (mesh[y2[ok], x2[ok]] == mesh[ys[ok], xs[ok]])
        acc[ok] += Sm[y2[ok], x2[ok]]
        n[ok] += 1
    neu = np.where(n > 0, acc / np.maximum(n, 1), 0.0)
    sd0, sd1 = float(struktur[haut].std()), float(neu[haut].std())
    return np.where(haut, neu * sd0 / max(sd1, 1e-9), 0.0)


def flaechen_bereinigen(G, fl, gebiet, nur_inseln=False):
    """Fleisch-Inseln und Haut-Loecher kleiner FLAECHE_MIN Texel (Zusammenhang auf dem Oberflaechengraph) entfernen bzw. fuellen."""
    A = G['A']
    out = fl.copy()
    for wert in ((True,) if nur_inseln else (True, False)):
        m = np.nonzero((out == wert) & gebiet)[0]
        if len(m) == 0:
            continue
        n, lab = connected_components(A[m][:, m], directed=False)
        gr = np.bincount(lab, minlength=n)
        klein = gr[lab] < FLAECHE_MIN
        out[m[klein]] = not wert
    return out


def hautseiten4():
    """Wie kf_mal3.hautseiten (k-Mittel k = 2, Bild 1 UND 2, Nicht-Rot-Figurpunkte), aber Stufengrenzen = Pflicht 20/45 + Dezile der
    Original-Helligkeit derselben Punkte (V2 4), und Warm-Anteil der Bildpunkte (k-Mittel-Gewicht) fuer die Hautseiten-Schwelle."""
    X = []
    for b in (1, 2):
        L, m = M3.original_lab(b)
        X.append(L[m])
    X = np.concatenate(X)
    C = np.hypot(X[:, 1], X[:, 2])
    X = X[~((X[:, 1] >= 12) & (C >= 15) & (X[:, 0] >= 20))]
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.01)
    cv2.setRNGSeed(M3.SAAT)
    _c, lb, cen = cv2.kmeans(X[:, 1:].astype(np.float32), 2, None, crit, 5, cv2.KMEANS_PP_CENTERS)
    lb = lb.ravel()
    w = int(np.argmax(cen[:, 0] + cen[:, 1]))
    dez = list(np.percentile(X[:, 0], np.arange(10, 100, 10)))
    gr = sorted(set([0.0, 100.0] + list(L_PFLICHT) + [float(d) for d in dez]))
    # Dezilgrenzen naeher als 1 L* an einer Pflichtgrenze fallen weg (sonst Stufe < 30 Bildpunkte)
    gr = [g for g in gr if g in (0.0, 100.0) or g in L_PFLICHT or min(abs(g - p) for p in L_PFLICHT) >= 1.0]
    rampen = []
    for k in (w, 1 - w):
        Y = X[lb == k]
        st = []
        for lo, hi in zip(gr[:-1], gr[1:]):
            q = (Y[:, 0] >= lo) & (Y[:, 0] < hi)
            if q.sum() >= STUFE_MIN_PX:
                st.append([float(x) for x in Y[q].mean(0)])
        st = np.array(st)
        st[:, 1:] = M3.blau_grenze(st[:, 1:])
        rampen.append(st)
    return cen[[w, 1 - w]], rampen, gr, float((lb == w).mean())


def wund_abstand(G, fl, ab_kn):
    """Geodaetischer Abstand jedes Fleischknotens zum naechsten Nicht-Fleisch-Knoten, auf dem Oberflaechengraph (3D-Kanten), in Texeln des
    Meshs (Kantenlaenge / Texelabstand des Meshs)."""
    P = G['P']
    if 'W' not in _CACHE:
        A = G['A'].tocoo()
        r, c = A.row, A.col
        wgt = np.linalg.norm(P[r] - P[c], axis=1) / (0.5 * (ab_kn[r] + ab_kn[c]))
        _CACHE['W'] = coo_matrix((np.maximum(wgt, 1e-3), (r, c)), shape=A.shape).tocsr()
    W = _CACHE['W']
    nf = ~fl
    # Randknoten = Nicht-Fleisch-Knoten mit Fleisch-Nachbar; Abstand ab dort, minus eine halbe Kante
    has = np.asarray(G['A'][:, np.nonzero(fl)[0]].sum(1)).ravel() > 0
    rand_kn = nf & has
    if not rand_kn.any():
        return np.full(len(P), 1e9)
    keep = fl | rand_kn
    ki = np.nonzero(keep)[0]
    Ws = W[ki][:, ki]
    src = np.nonzero(rand_kn[ki])[0]
    d = dijkstra(Ws, directed=False, indices=src, min_only=True)
    out = np.full(len(P), 1e9)
    out[ki] = d - 0.5
    out[~fl] = 0.0
    return out


def ruecken_wunde(G, sicht, S, rotS, erlaubt, fl, ab_kn):
    """Bild-1-Ruecken (V2 3.2 K4b, 5.6): zusammenhaengende Wunde an der Stelle, wo das Original in Bild 1 Ruecken/Flanke am roetesten ist (E14),
    gewachsen nach Original-a* (absteigend) ueber den Oberflaechengraph, bis die Wunde (alle Stufen) RUECKEN_ZIEL der Zonen-Bildpunkte deckt
    (Gewicht = Zahl der Bild-1-Bildpunkte je Texel aus kf_sicht). Lauf v4 mit 'Kern = Ziel': Wunde mit Rand/Innenrand 70 % der Zone rot (K4 21 %)."""
    ys, xs = G['ys'], G['xs']
    zw = sicht['b1|Ruecken/Flanke'][ys, xs].astype(float)
    ges = float(zw.sum())
    kand = erlaubt & (zw > 0)
    if not kand.any():
        return np.zeros(len(ys), bool), 'keine Ruecken-Texel'
    a_orig = np.where(kand, S[:, 1], -1e9)
    start = int(np.argmax(a_orig))
    A = G['A'].tocsr()
    import heapq
    # Start: vorhandenes Screenshot-Fleisch der Zone zaehlt mit (Lauf v6: Wunde 16,5 % + Screenshot-Fleisch -> K4b 21,9 % > 17,9)
    drin = fl & erlaubt
    heap = [(-a_orig[start], start)]
    gesehen = {start}
    anteil = float(zw[drin].sum()) / ges
    schritte = 0
    while heap:
        _p, i = heapq.heappop(heap)
        drin[i] = True
        schritte += 1
        for j in A.indices[A.indptr[i]:A.indptr[i + 1]]:
            if j not in gesehen and erlaubt[j]:
                gesehen.add(j)
                pr = a_orig[j] if kand[j] else -1e6 + S[j, 1]
                heapq.heappush(heap, (-pr, j))
        anteil += zw[i] / ges
        if anteil >= RUECKEN_ZIEL:
            break
    return drin, 'Start Texel (%d,%d) Original-a* %.1f; %d Texel, Anteil %.3f der Zonen-Bildpunkte' % (
        xs[start], ys[start], S[start, 1], int(drin.sum()), anteil)


def kopfseite(G, X, sicht, S, gesehen, kiefer, kralle, dz, eich, F, zahn2, herkunft):
    """Kiefer (Band = Screenshot, Maulinneres RE2-G4), Krallen RE2-G4, Zaehne, Augen wie r3; Zahnfleisch-Gewicht 19,30 (V2 4)."""
    ys, xs, genutzt = G['ys'], G['xs'], G['genutzt']
    h, w = genutzt.shape
    Gm, Um = M1.g4_material()
    blend = int(round(2 * M3.SIG_GROB))
    x, y, ww, hh = F['s3_kiefer']
    # V2 4 (Kopienzahl +1, Zahlen-Pruefer r2 5.3): r3 kachelte das G4-Maul 5-fach -> Wiederholung im Feld 11,1 % (B6 je Feld <= 0,28 %).
    # Jetzt EINE Kopie, 90 Grad gedreht und auf das Feld gestreckt (bilinear); Lueckenfuellung kf_mal.ausschnitt (sigma 2, 4 Durchlaeufe) bleibt
    # nach V2 4 zulaessig, solange A6-A8 und B6 je Feld gruen sind.
    Pm = M1.ausschnitt(Gm, Um, M1.G4_MAUL)
    Pm = np.rot90(Pm, 1)
    Pm = cv2.resize(np.ascontiguousarray(Pm).astype(np.float32), (ww, hh), interpolation=cv2.INTER_LINEAR).astype(np.float64)
    kop = 1
    maul = np.zeros((h, w, 3))
    maul[y:y + hh, x:x + ww] = Pm
    maul = maul[ys, xs]
    Cm = np.hypot(maul[:, 1], maul[:, 2])
    zahn_m = (maul[:, 0] > 60) & (Cm < 35)
    gw = np.where(zahn_m, 0.0, np.clip(maul[:, 1] / ZAHNFLEISCH_A, 0.0, 1.0))
    ma, mb = M3.rampe(maul[:, 0], M3.RAMPE_FLEISCH)
    maul[:, 1] = maul[:, 1] * (1 - gw) + ma * gw
    maul[:, 2] = maul[:, 2] * (1 - gw) + mb * gw
    band = kiefer & gesehen
    kb_ = M3.harm3d(S, band, kiefer, S[band].mean(0)) if band.any() else S
    w_band = M3.glaetten3d(band.astype(float), kiefer, M3.SIG_GROB)
    X = np.where(kiefer[:, None], maul * (1 - w_band[:, None]) + (kb_ + dz) * w_band[:, None], X)
    herkunft['kiefer'] = 'Band Screenshot Bild 2 %d Texel; Maulinneres RE2-G4 %s, %d Kopien; Zahnfleisch a*/%.2f' % (int(band.sum()), M1.G4_MAUL, kop, ZAHNFLEISCH_A)
    x, y, ww, hh = F['s3_kralle']
    # Krallen: r3 8 Kopien -> Wiederholung im Feld 85 % (B6). Jetzt EINE G4-Kralle, auf das Feld gestreckt (wie Maul).
    Pk = M1.ausschnitt(Gm, Um, M1.G4_KRALLE)
    # naechster Nachbar statt bilinear: Lauf v7 bilinear -> HF_fft Kralle 0,038 < 0,04 (A6, Familienminimum)
    Pk = cv2.resize(np.ascontiguousarray(Pk).astype(np.float32), (ww, hh), interpolation=cv2.INTER_NEAREST).astype(np.float64)
    kop = 1
    kr = np.zeros((h, w, 3))
    kr[y:y + hh, x:x + ww] = Pk
    X = np.where(kralle[:, None], kr[ys, xs], X)
    herkunft['kralle'] = 'RE2-G4 %s, %d Kopien' % (M1.G4_KRALLE, kop)
    Xa = np.zeros((h, w, 3))
    Xa[ys, xs] = X
    x, y, ww, hh = M1.F3['zahn']
    t = np.linspace(1.0, M2.ZAHN_GRUND, hh)[:, None] * np.ones((1, ww))
    Xa[y:y + hh, x:x + ww, 0] = M1.ZAHN_LAB[0] * t
    Xa[y:y + hh, x:x + ww, 1] = M1.ZAHN_LAB[1]
    Xa[y:y + hh, x:x + ww, 2] = M1.ZAHN_LAB[2]
    x, y, ww, hh = M1.F3['auge']
    Xa[y:y + hh, x:x + ww] = np.asarray(eich['auge'], np.float64)
    yy, xx = np.mgrid[:480, :640]
    n_auge = 0
    for (px_, py_) in M1.AUGE_PUNKTE:
        d = ((xx - px_) ** 2 + (yy - py_) ** 2 <= M1.AUGE_R ** 2) & (sicht['px2_tx'] >= 0)
        tx_, ty_ = sicht['px2_tx'][d].astype(int), sicht['px2_ty'][d].astype(int)
        ok = genutzt[ty_, tx_] & ~zahn2[ty_, tx_]
        Xa[ty_[ok], tx_[ok]] = np.asarray(eich['auge'], np.float64)
        n_auge += int(ok.sum())
    herkunft['augen'] = 'Augenfeld + %d Texel r <= %d px um E24 -> %s' % (n_auge, M1.AUGE_R, eich['auge'])
    return Xa


# ---------------------------------------------------------------- Eichung (V2 4: Abbruch = 2 x Kettenstreuung, kein 0,95-1,05-Fenster)
def streuung(aus):
    """Kettenstreuung: dieselbe Variante durch die Kette mit 3 Saaten des Videofilters (15 = Riegel, 12345 = Zahlen-Pruefer r3, 7);
    je Zone und Kanal die Standardabweichung der Lab-Mittel; Ausgabe in kf_eich4.json 'kettenstreuung' (Mittel ueber Zonen und Kanaele, Max)."""
    import p_zonen as PZ
    k = PZ.P.Kandidat(os.path.join(aus, 'form.json'))
    werte = {}
    for saat in (15, 12345, 7):
        for n in (1, 2):
            sauber, alb, m, kk = k.rendern(n)
            fig = m & (alb.max(2) > 0)
            vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=saat)
            Tt, clip = PZ.rueck(vid, n)
            r = PZ.messe(Tt, clip, PZ.regionen(n), fig)
            for z, v in r.items():
                if z.startswith('_'):
                    continue
                werte.setdefault('b%d|%s' % (n, z), []).append(v['lab'] + [v['chroma'], v['ab_streu']])
    sd = {z: np.std(np.array(v), 0).round(3).tolist() for z, v in werte.items()}
    lab_sd = [x for z, v in sd.items() for x in v[:3]]
    out = dict(saaten=[15, 12345, 7], je_zone=sd, mittel=round(float(np.mean(lab_sd)), 3), max=round(float(np.max(lab_sd)), 3),
               k1_C=[v[3] for v in werte['b1|Figur']], k2_ab=[v[4] for v in werte['b1|Figur']])
    e = json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich()
    e['kettenstreuung'] = out
    json.dump(e, open(EICH, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print(json.dumps(out, ensure_ascii=False))
    return out


def eichen(aus, runden):
    """Fixpunkt wie r3 (d <- d + (Original - Kette) je Zone, voller Schritt; Klauen nur a* aufwaerts; b* nicht tiefer als die Blau-Grenze wirkt).
    V2 4: Abbruch, wenn die groesste Zonenaenderung < 2 x Kettenstreuung (max ueber Zonen/Kanaele, 3 Saaten); Sperre eines Anteils, wenn eine
    Versatzaenderung > 2 x Kettenstreuung die Kette um weniger als 1 x Kettenstreuung bewegt (statt r3-Regel 0,1); g so, dass die Bild-1-Kette
    C* den Rundlauf 7,85 trifft, solange sie ausserhalb des V2-K1-Fensters 6,67-9,42 liegt (statt 0,95-1,05 x Original)."""
    eich = json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich()
    ks = eich.get('kettenstreuung')
    if not ks:
        raise SystemExit('zuerst: kf_mal4.py streuung <aus> (Kettenstreuung messen)')
    sk = float(ks['max'])
    O = M1.original_messen()
    for r in range(runden):
        bmax = bauen(aus, eich)['info']['b_max_zone']
        V = M1.kette_messen(aus)
        prot, schritt = {}, 0.0
        for z in M1.ZONEN:
            n = 1 if z.startswith('b1') else 2
            o, v = O[n][z[3:]], V[n][z[3:]]
            e = eich['zonen'][z]
            alt = list(e['d'])
            neu = [float(e['d'][i] + (o['lab'][i] - v['lab'][i])) for i in range(3)]
            if z.startswith('b2|Klaue'):
                neu[0] = 0.0
                neu[2] = 0.0
                neu[1] = max(0.0, neu[1]) if v['lab'][1] < o['lab'][1] else max(0.0, alt[1])
            neu[2] = max(neu[2], -M3.C_BLAU - bmax[z] + alt[2])
            vk, va = e.get('_kette'), e.get('_d_vor')
            satt = e.setdefault('_satt', [False, False, False])
            if vk is not None and va is not None:
                for i in range(3):
                    if abs(alt[i] - va[i]) > 2 * sk and abs(v['lab'][i] - vk[i]) < sk:
                        satt[i] = True
            for i in range(3):
                if satt[i]:
                    neu[i] = alt[i]
            e['_kette'] = list(v['lab'])
            e['_d_vor'] = list(alt)
            e['d'] = [round(x, 2) for x in neu]
            schritt = max(schritt, float(np.linalg.norm(np.subtract(neu, alt))))
            prot[z] = dict(orig=o['lab'], kette=v['lab'], d=e['d'])
        c_v = V[1]['Figur']['chroma']
        # g auf die Mitte des V2-K1-Bezugs (Rundlauf 7,85) ziehen, nicht nur ins Fenster (Stand v22: 6,7 = Fensterrand 6,67)
        g_alt = eich.get('g_haut', G_START)
        # Obergrenze G_START = 1,5 x r3 (V2 3.2 K1-Zeile "Haut-Buntheit ~1,5 x r3"): Lauf e1 ohne Grenze trieb g 2,22 -> 3,09, die Kette blieb
        # wegen der Kappen a* <= 11 / C*kuehl <= 8 bei C* 6,6-6,7 (Orangestich ohne Wirkung)
        eich['g_haut'] = round(float(min(G_START, max(1.0, g_alt * K1_RUNDLAUF / max(c_v, 1e-3)))), 3)
        schritt = max(schritt, abs(c_v - K1_RUNDLAUF))
        yy, xx = np.mgrid[:480, :640]
        aug = []
        for (x, y) in M1.AUGE_PUNKTE:
            mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
            aug.append(float(PL.lab(np.clip(V[2]['_T'][mm], 0, 255))[:, 0].max()))
        if max(aug) > M2.ZIEL_AUGE:
            eich['auge'][0] = round(max(0.0, eich['auge'][0] - (max(aug) - M2.ZIEL_AUGE)), 2)
        prot.update(g_haut=eich['g_haut'], chroma_b1=c_v, ab_b1=V[1]['Figur']['ab_streu'], auge_Lmax=aug, schritt_max=schritt, abbruch=2 * sk,
                    rest_dE_max=max(float(np.linalg.norm(np.subtract(p['orig'], p['kette']))) for k, p in prot.items() if k in M1.ZONEN))
        eich['runden'].append(prot)
        print('Runde %d: Rest dE max %.2f, Schritt %.2f (Abbruch < %.2f), C* B1 %.2f ab %.2f, g %.3f, Auge %s' % (
            r, prot['rest_dE_max'], schritt, 2 * sk, c_v, V[1]['Figur']['ab_streu'], eich['g_haut'], aug), flush=True)
        for z in M1.ZONEN:
            print('   %-22s orig %s kette %s d %s' % (z, prot[z]['orig'], prot[z]['kette'], prot[z]['d']))
        json.dump(eich, open(EICH, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        if schritt < 2 * sk:
            break
    bauen(aus, eich)
    return eich


if __name__ == '__main__':
    sys.path.insert(0, M1.PS)
    a = sys.argv
    kg = int(a[a.index('--k-graph') + 1]) if '--k-graph' in a else None
    sp = float(a[a.index('--spiegel') + 1]) if '--spiegel' in a else None
    if a[1] == 'bauen':
        b = bauen(a[2], k_graph=kg, spiegel=sp)
        print(json.dumps(b['herkunft'], ensure_ascii=False, indent=0, default=str)[:6000])
    elif a[1] == 'eichen':
        eichen(a[2], int(a[3]))
    elif a[1] == 'streuung':
        streuung(a[2])
