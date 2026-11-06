#!/usr/bin/env python3
"""K5c Farbe, Runde 3: neue Textur der Riesenform K5c (Geometrie, Clips, UV, Dreiecke unveraendert; nur Texelindizes und CLUTs).
Grundlage MESSBLATT_FARBE_V1.md und die Pruefberichte der Runde 2 (FARBE_PRUEFUNG_r2_sicht.md P1/P2/P5/P9, FARBE_PRUEFUNG_r2_zahlen.md 1, 5, 6).
Kein RE-Wert: neues Modell, Gestaltung = PORT-WAHL (NUTZER_ENTSCHEIDE E8-E16, E24, E27 und die Ablehnung 2026-11-06).

Warum ein neuer Aufbau (statt Runde 2 nachzustellen):
  Sicht-Pruefer r2 P2: "gekachelte Foto-Fragmente statt gemalter Formen" - 46 Atlasfelder mit G4-Hautausschnitten, Zellnetz, Wiederholung
  (Zahlen-Pruefer r2 6.1: 3,1 % Doppelbloecke gegen EM30 0,3 %). Sicht-Pruefer r2 P1: Rot als Netz und Saum (verdeckte Unterseite an jedem
  Glied -> rote Umrisskanten). Zahlen-Pruefer r2 6.2: Rot ein einziger Ton (harte Drehung auf 25 Grad).
Runde 3 malt deshalb NICHT mehr je Atlasfeld, sondern auf der Koerperoberflaeche (3D, Ruhepose Clip 2): jeder Texel bekommt seine Farbe aus
seiner Lage am Koerper. Dadurch laufen Farbflaechen ueber Feld- und Gliedgrenzen durch (E8 "aus einem Guss", E9, E12 "keine UV-Naehte"),
nichts wiederholt sich, und die Formen sind koerpergross statt feldgross.

Aufbau je Texel (Lab D65, OpenCV float):
  1. Haut-Struktur = eigene Koernung von K5c (Textur H2, E16 "sehr viel besser", Messblatt 1: HF 0,105 in der Familie), Glanzflecken und
     Gaumenstreifen vorher entfernt (wie Runde 2). Keine G4-Hautausschnitte mehr (P2).
  2. Farbkarte aus den Screenshots (E11): Original Bild 1/2 (T wie p_zonen.rueck) auf die sichtbaren Texel; verdeckte Texel aus dem
     Spiegelpunkt (z -> -z, das Wesen ist links/rechts gebaut) und sonst harmonisch auf der Oberflaeche (3D-Nachbarschaftsgraph) ergaenzt;
     geglaettet mit SIG_GROB entlang der Oberflaeche (E12 "grossflaechig je Bereich"). Daraus je Texel Helligkeit, Rotanteil und Hautseite
     (warm/kuehl nach k-Mittel der Original-Hautpixel); Hautfarbe = Rampe der Seite (wenige Toene je Teil), dunkle Falten der warmen Seite in
     der G4-Dunkelrotstufe.
  3. Form (gemalte Anatomie, P2 / E8 "Lichtauffassung"): Hohlraum-Mass je Texel aus der Geometrie (Nachbarn ueber der Tangentialebene =
     Falte/Beuge/Achsel), dunkler in den Tiefen. Staerke so, dass die Helligkeitsstreuung des Koerpers die des Vorbilds RE2-G4 erreicht
     (Messblatt 1: L*-Streuung G4 14,9).
  4. Fleisch als wenige grosse Flaechen (P1, Sicht r2 4.1): wo die Screenshots mehrheitlich Rot zeigen (E14) und - verdeckt - an Brust/Bauch
     des RUMPFES (G4-Vorbild), nicht an den Gliedern (keine Saeume). Fleischfarbe mehrtoenig: Rampe ueber die Familien-Rotstufen nach Helligkeit
     (dunkelrote Tiefen, E10), Farbton aus dem Screenshot wo sichtbar (Z1 rosa), keine harte Drehung.
  5. Kiefer: sichtbares Band = Screenshot (Band + Zahnflecken, Messblatt 3), verdecktes Maulinneres = RE2-G4-Maul; Krallen RE2-G4; Augen E24.
  6. Eichung gegen die Kette (Zonenversatz L*, a*, b* je Bildzone wie Runde 2, dazu EIN Faktor fuer die Hautbuntheit); Endstand und Restfehler
     je Runde in kf_eich3.json, Abbruch bei Konvergenz.

    python kf_mal3.py bauen <aus>
    python kf_mal3.py eichen <aus> <runden>"""
import json
import os
import shutil
import sys

import cv2
import numpy as np
from scipy.sparse import coo_matrix, diags, identity
from scipy.sparse.linalg import spsolve
from scipy.spatial import cKDTree

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_mal as M1   # noqa: E402  Lader, Atlasfelder, G4-Material, Palette, Kette (Runde 1, unveraendert)
import kf_mal2 as M2  # noqa: E402  harmonisch(), clut_farben(), Glanz-Konstanten (Runde 2, unveraendert)

PL = M1.PL
FE = M1.FE
K5C = M1.K5C
EICH = os.path.join(HIER, 'kf_eich3.json')
SICHT = M1.SICHT

# ---------------------------------------------------------------- Konstanten (Quelle je Zeile)
SIG_GROB = M1.SIG_GROB          # 2,5 Texel: Glaettung der Farbkarte und Breite der Koernung (kf_mal.py:40-42); hier entlang der Oberflaeche
GLANZ_L, GLANZ_C, HOF_L = M2.GLANZ_L, M2.GLANZ_C, M2.HOF_L          # Glanzkeim L*>85 C*<10 (Riegel A5), Hof L*>58 (Messblatt 2 hellstes Band)
GAUMEN_L, GAUMEN_C = M2.GAUMEN_L, M2.GAUMEN_C                      # Zahnklasse Messblatt 3 im Kopffeld (B5)
EINSTREU_A2 = M2.EINSTREU_A2    # 12,8: Bild-2-Einstreuung in a* (Messblatt 2 letzter Punkt: Kette 15,5 - Albedo 2,7)
# Bild-2-Einstreuung in b*: gleiche Messung (p_zonen.json, K5c Figur Bild 2: Albedo b* -1,5 -> Kette b* 1,9), Feuerschein (Messblatt 6 K9)
EINSTREU_B2 = 1.9 - (-1.5)
B_MIN = M2.B_MIN                # -6: Blau-Klasse = b* < -6 UND C* > 8 (Sicht-Pruefer r1 P2, Riegel B3)
C_BLAU = 8.0                    # Buntheitsgrenze derselben Klasse: Toene mit b* < -6 werden auf C* <= 8 begrenzt (Farbton bleibt), nicht auf b* -6
#                                 gekappt. Runde 2 kappte b* und verlor damit die Bild-1-Farbkraft (K1 6,5; Original gekappt 7,99, Zahlen-Pruefer r2 1).
C_REST = M2.C_REST              # 5,5: Buntheit ausserhalb des Fensters (kf_mal.py:71-79)
# Fenster -125..80 Grad: H_BLAU -125 aus Runde 1 (kf_mal.py:71-79: Bild-1-Hinterbein r (W) a* 0,7, b* -7,9 = Farbton -85, "Blaugrau" E10);
# Runde 2 hatte -55 (dort zusammen mit der b*-Kappung). Tuerkis/Gruen (< -125) und Gelb (> 80) bleiben auf C* 5,5.
H_MIN, H_MAX = M1.H_BLAU, M2.H_MAX
# Hellste Hautstelle: Messblatt 2 "Helle Baender (H): grau, T L* 51-58" -> projizierte Screenshot-Helligkeit der Haut hoechstens 58
# (alles darueber ist Videouebersteuerung/Glanz, Messblatt 5.4; Riegel B4). Fleisch: hoechstens EM30-Haut p95 70,6 (Messblatt 1, Tabelle).
L_MAX_HAUT = HOF_L
L_MAX_FLEISCH = 70.6
# Fleisch-Rampe (Helligkeit -> Farbe) aus den Familien-Rotstufen, Messblatt 1 Paletten (Lab):
#   G4 (18,9/25,6/12,2) dunkelrote Tiefe; G4 (32,8/43,7/25,8) sattes Fleischrot; EM30 (36,5/37,4/20,5) sattes Rot;
#   Familienwunde (51,5/26,5/18,0) = Mittel EM30-Wunde (50/26/13) und G4-Wunde (53/27/23) (Sicht-Pruefer r1 P5); G4 (64,3/24,7/16,3) hellrosa.
#   Farbtoene 25,5 / 30,6 / 28,7 / 34,2 / 33,4 Grad, Buntheit 28-51: Wechsel zwischen Dunkelrot, sattem Rot und Rosa nach Helligkeit (E10).
#   Runde-3-Lauf v0 mit (29,7/26,0/12,4) und (39,3/21,3/14,4) statt der satten Stufen: Fleisch im Render unter der Rotklasse (B1 19,8 %).
RAMPE_FLEISCH = ((18.9, 25.6, 12.2), (32.8, 43.7, 25.8), (36.5, 37.4, 20.5), (51.5, 26.5, 18.0), (64.3, 24.7, 16.3))
# Hauttoene (E10 "Violett, Rotbraun, Blaugrau", E11 "aus dem Screenshot, vor allem Rueckansicht"): k-Mittel k = 2 in a*b* ueber die
# Nicht-Rot-Bildpunkte der Figur in Bild 1 und Bild 2 (Original T, Zonen ohne Boden/Logo, Bild 2 ohne Einstreuung) -> eine warme (Rotbraun) und
# eine kuehle Seite (Violett-/Blaugrau). Je Seite die Rampe Helligkeit -> a*b* = Mittel der Bildpunkte je L*-Stufe (Stufen L_STUFEN; Werte in
# bau.json 'hautseiten'). Blau-Klasse der Rampen auf C* <= 8 (B3). Je Texel: Anteil warmer Bildpunkte (sichtbar), Spiegelpunkt, harmonisch, dann Mehrheit 0,5 mit weicher Kante
# SIG_GROB (wenige Farben je Teil, E8/E13) - statt einer Mittelfarbe, deren Buntheit sich aufhebt (Runde-3-Lauf e1: globaler Buntheitsfaktor
# 3,9 -> Magenta/Orange/Tuerkis, verworfen).
L_STUFEN = (0, 10, 15, 20, 25, 30, 40, 60, 100)      # Verfahrensparameter: Stufen wie Messblatt-2-Klassen (dunkel < 20, hell >= 45) feiner geteilt
# Mehrheitsschwelle der Screenshot-Rotklasse je Texel (E14: Fleisch nur, wo der Screenshot Rot zeigt; Runde 2 FLEISCH_SCHWELLE)
FLEISCH_SCHWELLE = M2.FLEISCH_SCHWELLE
# Verdecktes Fleisch (G4-Vorbild Brust/Bauch, Sicht-Pruefer r2 4.1): nur Rumpf/Hals (Meshes RUMPF), Unterseite, weil Fleisch an den
# Gliedunterseiten im Render als roter Saum an jeder Umrisskante stand (Sicht-Pruefer r2 P1, Spiel 5080 "rote Polygonkanten").
G4_DUNKELROT = M2.G4_DUNKELROT  # (18,9/25,6/12,2) Messblatt 1, RE2-G4-Palette 7,6 %
FLEISCH_BAUCH_L = (32.8 + 36.5) / 2   # satte Fleischstufen G4/EM30 (Messblatt 1 Paletten), siehe Schritt 5
RUMPF = (0, 7, 10, 21)          # huefte, wirbel, brust, hals (K5c-Meshes, x_mesh-Auszug im Dossier FARBE.md 3)
# Hohlraum-Mass: Nachbarn je Texel (k-NN in 3D). Verfahrensparameter: 64 Nachbarn = Kreis von etwa 4,5 Texeln Radius, also die Breite
# einer Muskel-/Faltenform von 2 x SIG_GROB Texeln mit Rand; dazu eine grobe Stufe (Faktor 4 = Rumpf-/Gliedbreite).
K_NAH, K_FERN = 64, 256
# Ziel der Helligkeitsstreuung Koerper: RE2-G4 14,9 (Messblatt 1 Tabelle, "Vorbild fuer die Riesenform").
L_STREU_ZIEL = 14.9
# Graph: 8 naechste 3D-Nachbarn (= 8er-Nachbarschaft im Atlas) und die 4er-Nachbarn im Atlas. Verfahrensparameter.
K_GRAPH = 8
# Spiegelpunkt gilt, wenn ein gesehener Texel naeher als 2 Texelabstaende (Median des Nachbarabstands) liegt. Verfahrensparameter.
SPIEGEL_ABSTAND = 2.0
ZONEN = M1.ZONEN
AUGE_PUNKTE, AUGE_R = M1.AUGE_PUNKTE, M1.AUGE_R
F3 = M1.F3
ZAHN_LAB, AUGE_LAB = M1.ZAHN_LAB, M1.AUGE_LAB
ZAHN_GRUND = M2.ZAHN_GRUND
ZIEL_AUGE = M2.ZIEL_AUGE
SAAT = M2.SAAT

_GEO = {}


# ---------------------------------------------------------------- Oberflaechen-Werkzeuge
def oberflaeche():
    """Lage, Normale, Mesh, CLUT-Zeile je Texel und der Oberflaechengraph (einmal je Lauf)."""
    if _GEO:
        return _GEO
    import kf_geo
    mo, tim, rgb, mid = M1.lade_k5c()
    zeile, mesh, konflikt = M1.texel_karten(mo)
    pos, nrm, ges = kf_geo.geometrie(K5C)
    genutzt = zeile >= 0
    ys, xs = np.nonzero(genutzt)
    idx = -np.ones(genutzt.shape, int)
    idx[ys, xs] = np.arange(len(ys))
    P = pos[ys, xs].astype(np.float64)
    baum = cKDTree(P)
    d, nb = baum.query(P, k=K_GRAPH + 1)
    zeilen, spalten = [], []
    for j in range(1, K_GRAPH + 1):
        zeilen.append(np.arange(len(ys)))
        spalten.append(nb[:, j])
    for dy, dx in ((0, 1), (1, 0)):
        y2, x2 = ys + dy, xs + dx
        ok = (y2 < genutzt.shape[0]) & (x2 < genutzt.shape[1])
        ok[ok] &= genutzt[y2[ok], x2[ok]]
        ok[ok] &= mesh[y2[ok], x2[ok]] == mesh[ys[ok], xs[ok]]
        zeilen.append(np.nonzero(ok)[0])
        spalten.append(idx[y2[ok], x2[ok]])
    r = np.concatenate(zeilen)
    c = np.concatenate(spalten)
    A = coo_matrix((np.ones(len(r)), (r, c)), shape=(len(ys), len(ys))).tocsr()
    A = ((A + A.T) > 0).astype(np.float64)
    _GEO.update(mo=mo, tim=tim, rgb=rgb, zeile=zeile, mesh=mesh, konflikt=konflikt, pos=pos, nrm=nrm, genutzt=genutzt, ys=ys, xs=xs, idx=idx,
                P=P, N=nrm[ys, xs].astype(np.float64), baum=baum, A=A, abstand=float(np.median(d[:, 1])))
    return _GEO


def harm3d(werte, bekannt, gebiet, ersatz):
    """Laplace-Fuellung auf dem Oberflaechengraph: Knoten in gebiet & ~bekannt = Mittel ihrer Nachbarn (im Gebiet). Werte als (n,) oder (n, k)
    ueber alle genutzten Texel (Reihenfolge G['ys'], G['xs']). Zusammenhangsteile ohne bekannten Knoten -> ersatz."""
    G = oberflaeche()
    A = G['A']
    X = np.asarray(werte, np.float64).copy()
    eins = X.ndim == 1
    if eins:
        X = X[:, None]
    g = np.asarray(gebiet, bool)
    u = g & ~np.asarray(bekannt, bool)
    if not u.any():
        return X[:, 0] if eins else X
    Ag = A[g][:, g]
    from scipy.sparse.csgraph import connected_components
    n, lab = connected_components(Ag, directed=False)
    gi = np.nonzero(g)[0]
    hat = np.zeros(n, bool)
    hat[lab[~u[gi]]] = True
    ohne = ~hat[lab]
    if ohne.any():
        X[gi[ohne & u[gi]]] = ersatz
        u = u.copy()
        u[gi[ohne]] = False
    ui = np.nonzero(u)[0]
    Au = A[ui]
    Auu = Au[:, ui]
    k_mask = g & ~u
    deg = np.asarray(Au[:, np.nonzero(g)[0]].sum(1)).ravel()
    rhs = Au[:, np.nonzero(k_mask)[0]] @ X[k_mask]
    M = diags(np.maximum(deg, 1)) - Auu
    sol = spsolve(M.tocsc(), rhs)
    X[ui] = np.asarray(sol).reshape(len(ui), -1)
    return X[:, 0] if eins else X


def glaetten3d(werte, gebiet, sig_texel):
    """Glaettung entlang der Oberflaeche: wiederholtes Nachbarmittel im Gebiet. Schritte n so, dass die Streuung der Zufallsbewegung
    sigma^2 = n * d^2 / 2 (d = Nachbarabstand in Texeln ~ 1) der Vorgabe sig_texel entspricht."""
    G = oberflaeche()
    g = np.asarray(gebiet, bool)
    A = G['A'][g][:, g] + identity(int(g.sum()), format='csr')
    Wn = diags(1.0 / np.asarray(A.sum(1)).ravel()) @ A
    X = np.asarray(werte, np.float64).copy()
    Y = X[g]
    for _ in range(max(1, int(round(2 * sig_texel ** 2)))):
        Y = Wn @ Y
    X[g] = Y
    return X


def hohlraum():
    """Je Texel: mittlerer Sinus der Hoehe der Nachbarn ueber der Tangentialebene (positiv = Falte/Beuge/Achsel, negativ = Woelbung).
    Zwei Stufen (K_NAH, K_FERN), gemittelt. Nachbarn aus allen genutzten Texeln (auch andere Glieder: Achsel, Beinansatz)."""
    G = oberflaeche()
    P, N = G['P'], G['N']
    out = np.zeros(len(P))
    for k in (K_NAH, K_FERN):
        teil = np.zeros(len(P))
        for a in range(0, len(P), 8000):
            d, nb = G['baum'].query(P[a:a + 8000], k=k + 1)
            v = P[nb[:, 1:]] - P[a:a + 8000, None, :]
            ln = np.maximum(np.linalg.norm(v, axis=2), 1e-6)
            teil[a:a + 8000] = (np.einsum('ijk,ik->ij', v, N[a:a + 8000]) / ln).mean(1)
        # grobe Stufe nur Falten (positiv): duenne Glieder sind im groben Mass immer gewoelbt und wurden sonst als Ganzes heller
        # (Lauf v0: Schwanz-Glied-L* 20 -> 36 zur Spitze, Riegel A11 4,4)
        out += teil if k == K_NAH else np.maximum(teil, 0.0)
    return out


# ---------------------------------------------------------------- Screenshot -> Texel
_RUECK = {}


def original_lab(b):
    """Original Bild b als T (p_zonen.rueck) in Lab, Bild 2 ohne Einstreuung (a* - 12,8, b* - 3,4); Zonenmaske. Zwischengespeichert."""
    if b not in _RUECK:
        import p_zonen as PZ
        v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % b), cv2.COLOR_BGR2RGB)
        T, _clip = PZ.rueck(v, b)
        L = PL.lab(np.clip(T.reshape(-1, 3), 0, 255)).reshape(480, 640, 3)
        if b == 2:
            L[..., 1] -= EINSTREU_A2
            L[..., 2] -= EINSTREU_B2
        zone = np.zeros((480, 640), bool)
        for _z, m in PZ.regionen(b):
            zone |= m
        _RUECK[b] = (L, zone)
    return _RUECK[b]


def blau_grenze(ab):
    """Blau-Klasse (b* < B_MIN und C* > C_BLAU) auf C* = C_BLAU zurueck, Farbton bleibt."""
    ab = np.array(ab, np.float64, copy=True)
    C = np.hypot(ab[..., 0], ab[..., 1])
    f = np.where((ab[..., 1] < B_MIN) & (C > C_BLAU), C_BLAU / np.maximum(C, 1e-6), 1.0)
    return ab * f[..., None]


def hautseiten():
    """k-Mittel k = 2 (a*, b*) der Nicht-Rot-Bildpunkte der Figur in Bild 1 UND Bild 2 (Rueckseite und Vorderseite, E11) -> Zentren
    (warm, kuehl) und je Seite die Rampe (L*, a*, b*). Runde-3-Lauf v1 nur mit Bild 1: Zentren (2,3/2,0) und (3,7/-9,1), die warme Seite war
    Neutralgrau, das Rotbraun der Vorderseite fehlte."""
    X = []
    for b in (1, 2):
        L, m = original_lab(b)
        X.append(L[m])
    X = np.concatenate(X)
    C = np.hypot(X[:, 1], X[:, 2])
    X = X[~((X[:, 1] >= 12) & (C >= 15) & (X[:, 0] >= 20))]
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.01)
    cv2.setRNGSeed(SAAT)
    _c, lb, cen = cv2.kmeans(X[:, 1:].astype(np.float32), 2, None, crit, 5, cv2.KMEANS_PP_CENTERS)
    lb = lb.ravel()
    w = int(np.argmax(cen[:, 0] + cen[:, 1]))          # warm = groesseres a* + b*
    rampen = []
    for k in (w, 1 - w):
        Y = X[lb == k]
        st = []
        for lo, hi in zip(L_STUFEN[:-1], L_STUFEN[1:]):
            q = (Y[:, 0] >= lo) & (Y[:, 0] < hi)
            if q.sum() >= 30:
                st.append([float(x) for x in Y[q].mean(0)])
        st = np.array(st)
        st[:, 1:] = blau_grenze(st[:, 1:])
        rampen.append(st)
    return cen[[w, 1 - w]], rampen


def screenshot_texel(sicht, G, zentren=None):
    """Original Bild 1/2 (T wie p_zonen.rueck, Zonen ohne Boden/Logo) je genutztem Texel: Mittel Lab, Zahl der Bildpunkte, Rotanteil
    (Pruefer-Rotklasse a* >= 12, C* >= 15, L* >= 20, Messblatt 2), Zone. Bild 2: a* minus Einstreuung."""
    n_t = len(G['ys'])
    s = np.zeros((n_t, 3))
    n = np.zeros(n_t)
    rot = np.zeros(n_t)
    warm = np.zeros(n_t)
    for b in (1, 2):
        L, zone = original_lab(b)
        C = np.hypot(L[..., 1], L[..., 2])
        r = (L[..., 1] >= 12) & (C >= 15) & (L[..., 0] >= 20)
        tx, ty = sicht['px%d_tx' % b].astype(int), sicht['px%d_ty' % b].astype(int)
        ok = (tx >= 0) & zone
        ok[ok] &= G['genutzt'][ty[ok], tx[ok]]
        i = G['idx'][ty[ok], tx[ok]]
        np.add.at(s, i, L[ok])
        np.add.at(n, i, 1)
        np.add.at(rot, i, r[ok].astype(float))
        if zentren is not None:
            ab = L[..., 1:][ok]
            dw = ((ab - zentren[0]) ** 2).sum(1)
            dk = ((ab - zentren[1]) ** 2).sum(1)
            np.add.at(warm, i, ((dw < dk) & ~r[ok]).astype(float))
    m = n > 0
    s[m] /= n[m][:, None]
    warm[m] /= np.maximum(n[m] - rot[m], 1)          # Anteil warmer unter den Nicht-Rot-Bildpunkten
    rot[m] /= n[m]
    return s, n, rot, warm


def spiegeln(werte, bekannt, gebiet):
    """Verdeckte Texel im Gebiet uebernehmen den Wert des gesehenen Texels am Spiegelpunkt (x, y, -z), falls naeher als SPIEGEL_ABSTAND
    Nachbarabstaende. Gibt neue Werte und die neue Bekannt-Maske zurueck."""
    G = oberflaeche()
    kb = np.nonzero(bekannt & gebiet)[0]
    if len(kb) == 0:
        return werte, bekannt
    baum = cKDTree(G['P'][kb])
    q = np.nonzero(gebiet & ~bekannt)[0]
    Pm = G['P'][q] * np.array([1.0, 1.0, -1.0])
    d, j = baum.query(Pm)
    ok = d < SPIEGEL_ABSTAND * G['abstand']
    W = np.array(werte, np.float64, copy=True)
    W[q[ok]] = W[kb[j[ok]]]
    B = bekannt.copy()
    B[q[ok]] = True
    return W, B


def rampe(L, stufen):
    """Farbe (a*, b*) nach Helligkeit, linear zwischen den Stufen (Lab), ausserhalb geklemmt."""
    st = np.asarray(stufen, float)
    a = np.interp(L, st[:, 0], st[:, 1])
    b = np.interp(L, st[:, 0], st[:, 2])
    return a, b


def standard_eich():
    return dict(zonen={z: dict(d=[0.0, 0.0, 0.0]) for z in ZONEN}, auge=list(AUGE_LAB), g_haut=1.0, runden=[])


# ---------------------------------------------------------------- Bau
def bauen(aus, eich=None, sicht_pfad=SICHT, roh=None):
    eich = eich or (json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich())
    G = oberflaeche()
    tim, rgb, zeile, mesh, genutzt = G['tim'], G['rgb'], G['zeile'], G['mesh'], G['genutzt']
    ys, xs = G['ys'], G['xs']
    h, w = genutzt.shape
    sicht = dict(np.load(sicht_pfad))
    F = M1.felder()
    def feld(n):
        x, y, ww, hh = F[n]
        m = np.zeros((h, w), bool)
        m[y:y + hh, x:x + ww] = True
        return m & genutzt
    kiefer2, kralle2, zahn2, auge2, kopf2 = (feld('s3_' + n) for n in ('kiefer', 'kralle', 'zahn', 'auge', 'kopf'))
    kiefer2 &= mesh == 23           # Kiefer-Mesh (Unterkiefer, x_mesh: Mesh 23 = Feld s3_kiefer)
    sonder2 = kiefer2 | kralle2 | zahn2 | auge2
    haut2 = genutzt & ~sonder2
    T = lambda m2: m2[ys, xs]       # Atlas-Maske -> Knotenvektor
    haut, kiefer, kralle, kopf = T(haut2), T(kiefer2), T(kralle2), T(kopf2)
    msh = mesh[ys, xs]
    Lk = M1.lab_f(rgb.astype(np.float32))[ys, xs].astype(np.float64)
    herkunft = {}

    # 1. Koernung = eigene K5c-Struktur (H2), Glanz und Gaumen entfernt (harmonisch auf der Oberflaeche), Struktur = Bild minus Grobbild
    Ck = np.hypot(Lk[:, 1], Lk[:, 2])
    keim = haut & (Lk[:, 0] > GLANZ_L) & (Ck < GLANZ_C)
    g_keim = kopf & (Lk[:, 0] > GAUMEN_L) & (Ck < GAUMEN_C)
    hof = haut & (Lk[:, 0] > HOF_L)
    # Hof = zusammenhaengende helle Texel (Graph) um einen Keim
    from scipy.sparse.csgraph import connected_components
    hi = np.nonzero(hof)[0]
    _n, lab = connected_components(G['A'][hi][:, hi], directed=False)
    mit_keim = np.unique(lab[(keim | g_keim)[hi]])
    weg = keim | g_keim
    weg[hi[np.isin(lab, mit_keim)]] = True
    L0 = harm3d(Lk[:, 0], haut & ~weg, haut, float(Lk[haut & ~weg, 0].mean()))
    struktur = np.where(haut, L0 - glaetten3d(L0, haut, SIG_GROB), 0.0)
    herkunft['koernung'] = 'K5c-eigene Struktur (H2): L* minus Grobbild (Oberflaeche, sigma %.1f Texel); Glanz/Gaumen vorher entfernt: %d Texel (Keime %d + %d)' % (
        SIG_GROB, int(weg.sum()), int(keim.sum()), int(g_keim.sum()))

    # 2. Farbkarte aus den Screenshots (sichtbar), Spiegelpunkt, harmonisch, geglaettet
    zentren, hrampen = hautseiten()
    S, nS, rotS, warmS = screenshot_texel(sicht, G, zentren)
    gesehen = nS > 0
    Sh = S.copy()
    Sh[:, 0] = np.minimum(Sh[:, 0], L_MAX_HAUT)
    Sh[:, 1:] = blau_grenze(Sh[:, 1:])              # Blau-Klasse nie uebernehmen (B3)
    karte, bek = spiegeln(np.c_[Sh, rotS, warmS], gesehen & haut, haut)
    karte = harm3d(karte, bek, haut, np.r_[Sh[gesehen & haut].mean(0), 0.0, 0.5])
    karte = glaetten3d(karte, haut, SIG_GROB)
    herkunft['farbkarte'] = 'Screenshot T je Texel (Bild 1 %d / Bild 2 %d Texel sichtbar), Spiegelpunkt +%d Texel, Rest harmonisch, Glaettung %.1f Texel' % (
        int((sicht['fig1'][ys, xs] > 0).sum()), int((sicht['fig2'][ys, xs] > 0).sum()), int((bek & ~gesehen).sum()), SIG_GROB)

    # 3. Form: Hohlraum-Mass, Staerke auf Ziel-Helligkeitsstreuung
    hr = hohlraum()
    hr = glaetten3d(hr, genutzt[ys, xs], SIG_GROB)
    hz = (hr - hr[haut].mean()) / max(hr[haut].std(), 1e-6)
    koerper = haut & (xs < 384)
    basisL = karte[:, 0] + struktur
    lo, hi_ = 0.0, 40.0
    for _ in range(40):
        k_ = (lo + hi_) / 2
        if np.std((basisL - k_ * hz)[koerper]) < L_STREU_ZIEL:
            lo = k_
        else:
            hi_ = k_
    K_FORM = (lo + hi_) / 2
    formL = -K_FORM * hz
    herkunft['form'] = 'Hohlraum-Mass (k-NN %d/%d), L* = -%.2f x Standardwert (Koerper-L*-Streuung = %.1f wie RE2-G4)' % (K_NAH, K_FERN, K_FORM, L_STREU_ZIEL)

    # 4. Fleisch: Screenshot-Mehrheit (sichtbar + Spiegel + harmonisch) und verdeckt Rumpf-Unterseite
    w_s = karte[:, 3]
    fl_s = haut & (w_s >= FLEISCH_SCHWELLE)
    # verdeckt = in keinem Bild gesehen (E14 gilt fuer gesehene Stellen); gespiegelte Werte sind selbst nur Ergaenzung. Lauf v9 mit ~gespiegelt:
    # Bauchflaeche nur 7769 Texel, groesste Rotflaeche 3/4 vorn 2,7 % (B2).
    rumpf = haut & np.isin(msh, RUMPF) & ~gesehen
    ny = G['N'][:, 1]
    # Unterseite = Normale nach unten (ny > 0, PSX y nach unten), ohne Anteilsvorgabe. Lauf v2 mit Anteil 23,9 % der Koerpertexel: 80 % des
    # verdeckten Rumpfs rot, im Render ein roter Sattel ueber die ganze Flanke.
    fl_u = rumpf & (ny > 0)
    fl = fl_s | fl_u
    wf = glaetten3d(fl.astype(float), haut, SIG_GROB)
    wf = np.where(haut, wf, 0.0)
    herkunft['fleisch'] = 'Screenshot-Mehrheit %d Texel, verdeckte Rumpf-Unterseite (ny > 0) %d Texel (zusammen %.1f %% der Koerpertexel), Kante %.1f Texel' % (
        int(fl_s.sum()), int(fl_u.sum()), 100.0 * (fl & koerper).sum() / koerper.sum(), SIG_GROB)

    # 5. Haut und Fleisch zusammensetzen; Eichung: Zonenversatz (sichtbar, harmonisch fortgesetzt) und Hautbuntheit g
    n_t = len(ys)
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
    dz = harm3d(dz, bek_z & haut, haut, 0.0) * haut[:, None] + dz * kiefer[:, None]
    g = float(eich.get('g_haut', 1.0))
    haut_L = basisL + formL
    # Hautseite je Texel: Mehrheit warm (>= 0,5) mit weicher Kante; Farbe aus der Rampe der Seite nach der Hautgesamthelligkeit
    tw = glaetten3d((karte[:, 4] >= 0.5).astype(float), haut, SIG_GROB)
    wa, wb = rampe(haut_L, hrampen[0])
    ka_, kb_ = rampe(haut_L, hrampen[1])
    haut_ab = (np.c_[wa, wb] * tw[:, None] + np.c_[ka_, kb_] * (1 - tw[:, None])) * g
    # Dunkelrote Tiefen (E10 "dunkelrote Tiefen", G4: "Schwarzgrau plus Rot in Stufen", Messblatt 1): in Falten/Beugen (Hohlraum-Mass ueber dem
    # Mittel) Farbe der dunklen G4-Rotstufe (18,9/25,6/12,2); Uebergang ueber eine Standardabweichung des Masses (Verfahrensparameter).
    # Helligkeit bleibt die der Haut (dort ohnehin dunkel, Form-Schritt 3). Grund (gemessen): Lauf e2 erreichte mit den Screenshot-Rampen allein
    # Bild-1-Kette C* 6,6 (K1 >= 7,9); Tiefen unter L* 20 zaehlen nicht zur Rot-Klasse der Kette (K4).
    # nur auf der warmen Hautseite: die kuehle Seite (Violett-/Blaugrau) mit Rot gemischt ergab Magenta-Zwischentoene (Lauf v3: Rot-Farbton
    # A4 12 Grad); dort bleibt die Tiefe nur dunkel (Form-Schritt 3).
    wc = np.clip(hz, 0.0, 1.0) * tw
    # nur in dunklen Texeln (Messblatt-2-Klasse "dunkel" L* < 20, = Helligkeit der G4-Stufe 18,9): Lauf v7 tonte auch mittelhelle Falten am Kopf
    # -> verstreute Rotpunkte im Render (Erosionsrest B1 22,2 %, Sicht-Pruefer r2 P1 "Rot als Netz").
    wc = wc * (haut_L < 20.0)
    haut_ab = haut_ab * (1 - wc[:, None]) + np.asarray(G4_DUNKELROT[1:])[None, :] * wc[:, None]
    herkunft['tiefen'] = 'Falten (Hohlraum > Mittel, Uebergang 1 Standardabweichung) in G4-Dunkelrot %s: Gewicht >= 0,5 auf %.1f %% der Haut' % (
        G4_DUNKELROT, 100 * float((wc[haut] >= 0.5).mean()))
    herkunft['hautseiten'] = 'k-Mittel Bild 1: warm %s, kuehl %s; Rampe warm %s; Rampe kuehl %s; warm %.1f %% der Haut' % (
        np.round(zentren[0], 1).tolist(), np.round(zentren[1], 1).tolist(), np.round(hrampen[0], 1).tolist(), np.round(hrampen[1], 1).tolist(),
        100 * float((tw[haut] >= 0.5).mean()))
    # Fleisch: Helligkeit = Hautform um das Fleischmittel der Screenshots; Farbe = Rampe (Familie), dazu die Screenshot-Abweichung vom
    # Rampenwert dort, wo die Karte Fleisch zeigt (Z1 rosa bleibt rosa, E14)
    fL_s = float(S[gesehen & (rotS >= FLEISCH_SCHWELLE) & haut, 0].mean()) if (gesehen & (rotS >= FLEISCH_SCHWELLE) & haut).any() else 40.0
    # Verdeckte Bauchflaeche (keine Screenshot-Helligkeit): Mitte der satten Familien-Fleischstufen G4 (32,8/43,7/25,8) und EM30 (36,5/37,4/20,5)
    # = L* 34,65 (Messblatt 1). Laeufe: v5 Screenshot-Mittel 33,3 ohne Ausgleich der Form -> B1 22,2 %; v6 Familienwunde 51,5 -> in der Rampe nur
    # C* 32, im Render unter der Rotklasse (B1 14,8 %).
    u_b = glaetten3d((fl_u & ~fl_s).astype(float), haut, SIG_GROB)
    # Fleisch mit der Hautkoernung, aber auf +-1 Standardabweichung begrenzt (keine Einzeltexel-Loecher/-Lichter): Lauf v7 mit voller Koernung
    # -> Fleisch im Render durchloechert (Erosionsrest B1 22,2 %); Lauf v8 ganz ohne Koernung -> glatte Verlaeufe, die sich gleichen
    # (Wiederholung B6 1,62 % > 0,28).
    s_k = float(struktur[haut].std())
    fleisch_L = np.clip(fL_s * (1 - u_b) + FLEISCH_BAUCH_L * u_b + formL + np.clip(struktur, -s_k, s_k), 0, L_MAX_FLEISCH)
    ra, rb = rampe(fleisch_L, RAMPE_FLEISCH)
    ka, kb = rampe(karte[:, 0], RAMPE_FLEISCH)
    s_gew = np.clip(w_s, 0, 1)
    fleisch_ab = np.c_[ra, rb] + s_gew[:, None] * (karte[:, 1:3] - np.c_[ka, kb])
    X = np.zeros((n_t, 3))
    X[:, 0] = haut_L * (1 - wf) + fleisch_L * wf
    X[:, 1:] = haut_ab * (1 - wf[:, None]) + fleisch_ab * wf[:, None]
    # Zonenversatz nur auf die Haut (1 - wf), Fleisch behaelt Familienrampe + Screenshot. Lauf v3: der b*-Versatz der Rueckenzone (-11,7, noetig
    # fuer die gekappte kuehle Seite) drehte das Z1-Fleisch nach Magenta (A4 12 Grad, K4 1,0 %); Lauf v4: der L*-Versatz von Schwanz/Beinen
    # (-16, harmonisch fortgesetzt) machte die verdeckte Bauchflaeche so dunkel, dass sie im Render unter der Rotklasse lag (B1 21,4 %).
    dz_ = dz * (1 - wf)[:, None]
    X = np.where(haut[:, None], X + dz_, X)
    # Rohwerte fuer Tafeln/Pruefung
    herkunft['fleisch_farbe'] = 'L* Screenshot-Fleischmittel %.1f + Koernung + Form; Farbe Rampe %s + Screenshot-Abweichung' % (fL_s, RAMPE_FLEISCH)
    herkunft['eichung'] = 'Zonenversatz %s; Hautbuntheit g %.3f' % ({z: v['d'] for z, v in eich['zonen'].items()}, g)

    # 6. Kiefer (Mesh 23): sichtbares Band = Screenshot (+ Zonenversatz), verdeckt = RE2-G4-Maul (wie Runde 2, Versatz gleichverteilt)
    Gm, Um = M1.g4_material()
    x, y, ww, hh = F['s3_kiefer']
    Pm = M1.ausschnitt(Gm, Um, M1.G4_MAUL)
    Pm = np.rot90(Pm, 1)
    blend = int(round(2 * SIG_GROB))
    kop = -(-ww // (Pm.shape[1] - blend)) + 1
    Pm = M1.naht_reihe([np.roll(Pm, (j * Pm.shape[0]) // kop, 0) for j in range(kop)], ww, blend=blend)
    Pm = Pm[:hh] if Pm.shape[0] >= hh else M1.naht_reihe([Pm, Pm], hh, achse=0, blend=blend)
    maul = np.zeros((h, w, 3))
    maul[y:y + hh, x:x + ww] = Pm
    maul = maul[ys, xs]
    # Zahnfleisch: das G4-Maul ist im Ausschnitt dunkel und kaum rot (Zahn 1,5 / Rot 10,5 / Dunkel 46,1 %, a* p90 21,5; Lauf v3: Kiefer-Rot 12,3 %
    # < 20, Riegel A8). Nicht-Zahn-Texel (Zahnklasse L* > 60, C* < 35, Messblatt 3) bekommen die Familien-Fleischrampe nach ihrer Helligkeit,
    # gewichtet mit ihrem eigenen G4-Rot a*/p90 (21,5): das Zahnfleisch der Vorlage wird Fleisch, dunkles Maulinneres und Zaehne bleiben.
    Cm = np.hypot(maul[:, 1], maul[:, 2])
    zahn_m = (maul[:, 0] > 60) & (Cm < 35)
    gw = np.where(zahn_m, 0.0, np.clip(maul[:, 1] / 21.5, 0.0, 1.0))
    ma, mb = rampe(maul[:, 0], RAMPE_FLEISCH)
    maul[:, 1] = maul[:, 1] * (1 - gw) + ma * gw
    maul[:, 2] = maul[:, 2] * (1 - gw) + mb * gw
    band = kiefer & gesehen
    kb_ = harm3d(S, band, kiefer, S[band].mean(0)) if band.any() else S
    w_band = glaetten3d(band.astype(float), kiefer, SIG_GROB)
    X = np.where(kiefer[:, None], maul * (1 - w_band[:, None]) + (kb_ + dz) * w_band[:, None], X)
    herkunft['kiefer'] = 'Band: Screenshot Bild 2 auf %d sichtbaren Kiefertexeln (Zahnflecken mit), Kante %.1f Texel; Maulinneres RE2-G4 %s, %d Kopien' % (
        int(band.sum()), SIG_GROB, M1.G4_MAUL, kop)

    # 7. Krallen (RE2-G4, wie Runde 2), Zaehne, Augen
    x, y, ww, hh = F['s3_kralle']
    Pk = M1.ausschnitt(Gm, Um, M1.G4_KRALLE)
    kop = -(-ww // (Pk.shape[1] - blend)) + 1
    Pk = M1.naht_reihe([np.roll(Pk, (j * Pk.shape[0]) // kop, 0) for j in range(kop)], ww, blend=blend)
    Pk = Pk[:hh] if Pk.shape[0] >= hh else M1.naht_reihe([Pk, Pk], hh, achse=0, blend=blend)
    kr = np.zeros((h, w, 3))
    kr[y:y + hh, x:x + ww] = Pk
    X = np.where(kralle[:, None], kr[ys, xs], X)
    herkunft['kralle'] = 'RE2-G4 %s, %d Kopien' % (M1.G4_KRALLE, kop)
    Xa = np.zeros((h, w, 3))
    Xa[ys, xs] = X
    x, y, ww, hh = F3['zahn']
    t = np.linspace(1.0, ZAHN_GRUND, hh)[:, None] * np.ones((1, ww))
    Xa[y:y + hh, x:x + ww, 0] = ZAHN_LAB[0] * t
    Xa[y:y + hh, x:x + ww, 1] = ZAHN_LAB[1]
    Xa[y:y + hh, x:x + ww, 2] = ZAHN_LAB[2]
    x, y, ww, hh = F3['auge']
    Xa[y:y + hh, x:x + ww] = np.asarray(eich['auge'], np.float64)
    yy, xx = np.mgrid[:480, :640]
    n_auge = 0
    for (px_, py_) in AUGE_PUNKTE:
        d = ((xx - px_) ** 2 + (yy - py_) ** 2 <= AUGE_R ** 2) & (sicht['px2_tx'] >= 0)
        tx_, ty_ = sicht['px2_tx'][d].astype(int), sicht['px2_ty'][d].astype(int)
        ok = genutzt[ty_, tx_] & ~zahn2[ty_, tx_]
        Xa[ty_[ok], tx_[ok]] = np.asarray(eich['auge'], np.float64)
        n_auge += int(ok.sum())
    herkunft['augen'] = 'Augenfeld + %d Texel r <= %d px um E24 -> %s' % (n_auge, AUGE_R, eich['auge'])

    # 8. E9 Schwanz: Glied-Mittel 1-5 auf ihr gemeinsames Mittel (E9 "kein Sprung in Helligkeit"; Runde 2 Ausgleichsgerade: Lauf v0 Steigung
    #    3,9 je Glied -> A11 4,4). Spitze (Glied 6) frei (rosa-weisses Ende Bild 1, Messblatt 4).
    kp2 = genutzt & (np.arange(w)[None, :] < 384)
    gm = [float(Xa[..., 0][kp2 & (mesh == g_)].mean()) for g_ in range(1, 6)]
    for i, g_ in enumerate(range(1, 6)):
        Xa[..., 0][kp2 & (mesh == g_)] += float(np.mean(gm)) - gm[i]
    herkunft['schwanz_e9'] = 'Glied-L* vorher %s' % [round(v, 1) for v in gm]

    # 9. Farbfenster: Blau-Klasse (b* >= -6), Farbtoene ausserhalb -55..80 Grad hoechstens C* 5,5 (wie Runde 2), keine Drehung auf einen Ton
    fen = genutzt & ~zahn2 & ~auge2
    C_ = np.hypot(Xa[..., 1], Xa[..., 2])
    H_ = np.degrees(np.arctan2(Xa[..., 2], Xa[..., 1]))
    rest_ = fen & ((H_ < H_MIN) | (H_ > H_MAX)) & (C_ > C_REST)
    f_ = np.where(rest_, C_REST / np.maximum(C_, 1e-6), 1.0)
    Xa[..., 1] *= f_
    Xa[..., 2] *= f_
    blau = fen & (Xa[..., 2] < B_MIN) & (np.hypot(Xa[..., 1], Xa[..., 2]) > C_BLAU)
    Xa[..., 1:] = np.where(fen[..., None], blau_grenze(Xa[..., 1:]), Xa[..., 1:])
    Xa[..., 0] = np.clip(Xa[..., 0], 0, 100)
    herkunft['farbfenster'] = 'ausserhalb %.0f..%.0f Grad C* <= %.1f: %d, Blau-Klasse (b* < %.0f, C* > %.0f) auf C* %.0f: %d' % (H_MIN, H_MAX, C_REST, int(rest_.sum()), B_MIN, C_BLAU, C_BLAU, int(blau.sum()))
    wf2 = np.zeros((h, w))
    wf2[ys, xs] = wf
    out_rgb = M1.rgb_f(Xa.astype(np.float32))
    out_rgb[~genutzt] = 0
    vor = {}
    for z in eich['zonen']:
        c = sicht[z].astype(np.float64) * (kiefer2 if z == 'b2|Unterkiefer' else ~kiefer2)
        c = c * (wf2 < 0.5)                                 # nur Haut (der Farbversatz wirkt nur dort)
        vor[z] = float(Xa[..., 2][c > 0].max()) if (c > 0).any() else 0.0
    bau = schreiben3(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, dict(konflikt_zeilen=G['konflikt'], b_max_zone=vor))
    bau['konstanten'] = dict(SIG_GROB=SIG_GROB, L_MAX_HAUT=L_MAX_HAUT, L_MAX_FLEISCH=L_MAX_FLEISCH, RAMPE_FLEISCH=RAMPE_FLEISCH,
                             FLEISCH_SCHWELLE=FLEISCH_SCHWELLE, G4_DUNKELROT=G4_DUNKELROT, RUMPF=RUMPF, K_NAH=K_NAH, K_FERN=K_FERN,
                             L_STREU_ZIEL=L_STREU_ZIEL, K_FORM=K_FORM, K_GRAPH=K_GRAPH, SPIEGEL_ABSTAND=SPIEGEL_ABSTAND, EINSTREU_A2=EINSTREU_A2,
                             B_MIN=B_MIN, C_REST=C_REST, H_MIN=H_MIN, H_MAX=H_MAX, GLANZ_L=GLANZ_L, GLANZ_C=GLANZ_C, HOF_L=HOF_L, SAAT=SAAT)
    json.dump(bau, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    M = np.zeros((h, w), bool)
    fl2 = M.copy(); fl2[ys, xs] = fl
    wg = M.copy(); wg[ys, xs] = weg
    np.savez_compressed(os.path.join(aus, 'masken.npz'), fleisch=fl2, glanz_gaumen=wg)
    if roh is not None:
        roh.update(Xa=Xa, fl=fl2, hz=hz)
    return bau


def schreiben3(aus, tim, out_rgb, zeile, genutzt, eich, herkunft, info):
    os.makedirs(aus, exist_ok=True)
    h, w = genutzt.shape
    px = np.zeros((h, w), np.uint8)
    cl = []
    n_fix = 0
    for zl in range(len(tim['cluts'])):
        s = genutzt & (zeile == zl)
        c, lb = M1.palette(out_rgb[s], seed=SAAT)
        q, nf = M2.clut_farben(c)
        n_fix += nf
        px[s] = (lb + 1).astype(np.uint8)
        worte = [0] + [int(r | (g << 5) | (b << 10)) or 0x8000 for r, g, b in q]
        cl.append(worte + [0] * (256 - len(worte)))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, cl, tim['clut_x'], tim['clut_y'], tim['bild_x'], tim['bild_y']))
    shutil.copy(os.path.join(K5C, 'FORM.EMD'), os.path.join(aus, 'FORM.EMD'))
    fj = json.load(open(os.path.join(K5C, 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe_r3'
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    info = dict(info, clut_blau_stufen=n_fix)
    return dict(basis='K5c (varianten/K5/K5c): FORM.EMD byte-gleich, nur FORM.TIM neu (Runde 3, kf_mal3.py)',
                eich={k: v for k, v in eich.items() if k != 'runden'}, herkunft=herkunft, info=info)


# ---------------------------------------------------------------- Eichung
# Verfahren: Fixpunkt d <- d + (Original - Kette) je Zone (voller Schritt, kein Daempfungsfaktor), g <- g * C*_Original / C*_Kette (Bild 1, Figur,
# nur wenn die Kette ausserhalb 0,95..1,05 liegt). Abbruch, wenn die groesste Aenderung einer Zone unter 1 dE76 liegt (= unter dem Abstand
# benachbarter 5-Bit-CLUT-Stufen in dunklen Toenen, RGB 8/255 -> dE 2-4) oder nach <runden>. Klauen: wie Runde 2 nur a* nach oben (K9 ist
# Untergrenze, L* und b* Feuerschein). b*-Versatz wie Runde 2 begrenzt (groesstes b* der Zone an B_MIN).
def eichen(aus, runden):
    eich = json.load(open(EICH, encoding='utf-8')) if os.path.exists(EICH) else standard_eich()
    O = M1.original_messen()
    for r in range(runden):
        bmax = bauen(aus, eich)['info']['b_max_zone']
        V = M1.kette_messen(aus)
        prot, schritt = {}, 0.0
        for z in ZONEN:
            n = 1 if z.startswith('b1') else 2
            o, v = O[n][z[3:]], V[n][z[3:]]
            e = eich['zonen'][z]
            alt = list(e['d'])
            neu = [float(e['d'][i] + (o['lab'][i] - v['lab'][i])) for i in range(3)]
            if z.startswith('b2|Klaue'):
                neu[0] = 0.0
                neu[2] = 0.0
                neu[1] = max(0.0, neu[1]) if v['lab'][1] < o['lab'][1] else max(0.0, alt[1])
            neu[2] = max(neu[2], -C_BLAU - bmax[z] + alt[2])     # tiefer als b* -8 (C* <= 8) wirkt nicht
            # Saettigung: hat die letzte Aenderung eines Anteils (> 1) die Kette um weniger als ein Zehntel davon bewegt, wirkt der Versatz dort
            # nicht mehr (Blau-Grenze C* <= 8 kappt) -> Anteil bleibt stehen. Lauf e3: Hinterbein r (W) b* -26,7 -> -31,7 bei Kette -3,1 -> -2,9.
            # Einmal gesaettigt bleibt der Anteil gesperrt (Lauf e4 ohne Sperre: Hinterbein r (W) b* pendelte zwischen Sperre und Schritt,
            # Schritt 5,0 / 1,2 im Wechsel, kein Abbruch).
            vk = e.get('_kette')
            va = e.get('_d_vor')
            satt = e.setdefault('_satt', [False, False, False])
            if vk is not None and va is not None:
                for i in range(3):
                    if abs(alt[i] - va[i]) > 1.0 and abs(v['lab'][i] - vk[i]) < 0.1 * abs(alt[i] - va[i]):
                        satt[i] = True
            for i in range(3):
                if satt[i]:
                    neu[i] = alt[i]
            e['_kette'] = list(v['lab'])
            e['_d_vor'] = list(alt)
            e['d'] = [round(x, 2) for x in neu]
            schritt = max(schritt, float(np.linalg.norm(np.subtract(neu, alt))))
            prot[z] = dict(orig=o['lab'], kette=v['lab'], d=e['d'])
        c_o, c_v = O[1]['Figur']['chroma'], V[1]['Figur']['chroma']
        if not (0.95 <= c_v / c_o <= 1.05):
            # obere Grenze G_AB 1,48 = Bild-1-Kette C* Original 9,3 / K5c 6,3 (Messblatt 2, kf_mal.py:50); Lauf e1 ohne Grenze: g 3,9 -> Magenta/Orange
            eich['g_haut'] = round(float(np.clip(eich.get('g_haut', 1.0) * c_o / max(c_v, 1e-3), 1.0, M1.G_AB)), 3)
        yy, xx = np.mgrid[:480, :640]
        aug = []
        for (x, y) in AUGE_PUNKTE:
            mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
            aug.append(float(PL.lab(np.clip(V[2]['_T'][mm], 0, 255))[:, 0].max()))
        if max(aug) > ZIEL_AUGE:
            eich['auge'][0] = round(max(0.0, eich['auge'][0] - (max(aug) - ZIEL_AUGE)), 2)
        prot['g_haut'] = eich['g_haut']
        prot['chroma_b1'] = (c_v, c_o)
        prot['auge_Lmax'] = aug
        prot['rest_dE_max'] = max(float(np.linalg.norm(np.subtract(p['orig'], p['kette']))) for k, p in prot.items() if k in ZONEN)
        prot['schritt_max'] = schritt
        eich['runden'].append(prot)
        print('Runde %d: Rest dE max %.2f, Schritt %.2f, C* B1 %.2f/%.2f g %.3f, Auge %s' % (r, prot['rest_dE_max'], schritt, c_v, c_o, eich['g_haut'], aug), flush=True)
        for z in ZONEN:
            print('   %-22s orig %s kette %s d %s' % (z, prot[z]['orig'], prot[z]['kette'], prot[z]['d']))
        json.dump(eich, open(EICH, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        if schritt < 1.0 and (0.95 <= c_v / c_o <= 1.05 or eich['g_haut'] >= M1.G_AB):
            break
    bauen(aus, eich)
    return eich


if __name__ == '__main__':
    sys.path.insert(0, M1.PS)
    if sys.argv[1] == 'bauen':
        b = bauen(sys.argv[2])
        print(json.dumps(b['herkunft'], ensure_ascii=False, indent=0)[:4000])
    elif sys.argv[1] == 'eichen':
        eichen(sys.argv[2], int(sys.argv[3]))

