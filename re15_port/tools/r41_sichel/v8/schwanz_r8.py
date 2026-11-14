"""SF V8 Runde 8, Stufe q (Kette v8_bau.py --stufen ...fq): Schwanz der Sichel-Form neu - flacher Querschnitt, Glieder mit Fugen,
Skorpionstachel, Fleisch nur auf der Bauchseite. NUR Asset (Meshes 16-22 mit ihrer UV/CLUT); Knochen, Keyframes, Texturbild, CLUT-Zeilen
und alle anderen Meshes (Mesh 9 = V7-Kopf) bleiben bytegleich.

    python schwanz_r8.py <eingang.emd> <ausgang.emd> [a|b|c]     Stufe auf einen fertigen Stand anwenden (Standard-Farbe STANDARD)
    python schwanz_r8.py suche [<eingang.emd>]                    Texelfelder nachrechnen (Ausgabe = Beleg von schwanz_r8.json "felder")

Nutzer 2026-11-14 (woertlich): "weder mit dem rot der Schwanzfarbe noch mit dem etwas zu steifen grossen Schwanz zufrieden", dazu
Version_80%/Areas*.png (nur gelesen): Umriss rot, Gliedfugen gruen eingezeichnet, Skorpionspitze (Areas_Tip.png).
Grundlage: Analyse scratchpad/schwanz_v8/analyse/ANALYSE.md (Bauplan Schritte 1, 2, 4; Messwerte im Videoraum 640 x 480):
  - Video-Schwanz FLACH (Ruecken -> Bauch hoch, seitlich schmal; Hoch : Schmal an der Wurzel 2,2-2,9 : 1). V8 bis Runde 7e rund
    (700-740 E. Durchmesser), von vorn 2,5-3x zu breit ("grosse rote Klinge").
  - Schritt 1: Glieder quer zur Bauchachse abflachen (Bauchachse = Innenrichtung des Bogens, unterseite_v8.json innen_phi 26,4 Grad in der
    Ringebene x-z), Querdicke 260-280 E.; Hochachse = V8-Profil.
  - Schritt 2: Schwanzmitte (s .5) 15-20 % duenner; an den 6 Gelenken Einschnuerung ~85 % und helles Band; Glied 22 = Stachel (Knolle
    260-280 E. + Nadel 450-550 E. ueber das heutige Ende); Gliedlaengen bleiben 541 (re15_schwanz_r41_v8_kette haengt daran).
  - Schritt 4: Fleisch nur auf der Bauchseite (Grenze an der Wurzel 85-95 Grad um die Achse, zur Spitze hin ~60 Grad), Flanken und Ruecken
    grau wie die Oberseite bis Runde 7e; Bauch aus Original-Texeln/-Eintraegen, keine neue CLUT-Zeile (16 belegt), keine Eichung am Video.

Kein RE-Befund: die Sichel-Form und ihr Schwanz existieren auf keiner Disc (SF_sichel.md); keine Zahl hier ist eine @0x-Konstante. Jede Zahl
ist PORT-WAHL, belegt mit Nutzer-Wortlaut, der Analyse oder der Messung von "suche" (SF_belege/v8/runde8/suche.txt).

Bau (je Glied g = 0..6 = Knochen/Mesh 16..22, Gliedraum: y entlang der Achse, Gelenk g bei y 0, Gelenk g+1 bei y L = 541):
  Querschnitt = Ellipse in der Ringebene: Hochachse e_h = (cos phi, 0, sin phi) (phi = innen_phi des Knochens; +e_h = Bauch, theta 0),
  Querachse e_q = (-sin phi, 0, cos phi). Halbachsen A (hoch) = KERBE(y) x MITTE(u) x r(y), B (quer) = KERBE(y) x min(MITTE(u) x r(y), QUER / 2);
  r(y) linear zwischen den Gelenkradien von kette_v8.json (wie der Bau der Stufe s). 11 Ecken je Ring (S_), Ecke 0 auf dem Bauch,
  Ecken 2 / 9 genau auf der Fleischgrenze +-theta_b(u) (u = Lage in Gliedern 0..7), Bauch 4 Felder, Ruecken 7 Felder.
  Ringe je Glied: S = Manschette (Gehrung am Gelenk g, KERBE, entlang des Vorglieds zurueckgesetzt wie v8_bau.glied_mesh), C (Gehrungsebene
  am Gelenk g um KERBE_LAENGE x L verschoben, voller Querschnitt), [M bei 0,5 L nur Glied 18], D (Gehrungsebene am Gelenk g+1 um
  KERBE_LAENGE x L zurueck), E (Gehrung am Gelenk g+1, KERBE). Deckel an S und E wie bisher (Mitte auf der Achse).
  Glied 16: Ring 0/1 = Wurzelring an der Haut (Stufe w), auf die Winkel der neuen Ringe umgetastet und teilweise abgeflacht (WURZEL_FLACH),
  Ring D halb flach (D16_QUER), E flach; die Wurzelflaechen (Rohr Ring 0 -> 1, Kegel) bleiben mit ihren Indizes und ihrer UV.
  Glied 22: S, C, Knolle und Verjuengung (STACHEL, rund), Nadel bis y = L + DORN_ALT + NADEL = 1139 entlang der Gliedachse (der Code nimmt die
  groesste Ecke entlang der Achse als Dornspitze, schwanz_r41.c sw_mesh_mass); kein End-Deckel, die Nadelspitze schliesst.
  Reihenfolge der Ecken wie die Riegel sie lesen (riegel_v8.ringe_bild: die ersten 2 bzw. 3 Ringe; szene_marker.wurzel_flaechen: Mesh 16
  Ecken 0-21 und 34 = Wurzel): 17/19/20/21 = C, D, End-Mitte, Anfangs-Mitte, S, E; 18 = C, M, D, End-Mitte, Anfangs-Mitte, S, E;
  16 = Ring 0, Ring 1, D, End-Mitte, Kegelspitze, E; 22 = C, Ring bei y 560 (Lage des alten Endrings = "Ende" fuer S4), Nadelspitze,
  Anfangs-Mitte, S, Stachel-Ringe.
  Normalen: Mittel der Mantel-Flaechennormalen je Ecke (Flaecheninhalt gewichtet), Laenge 4096; Achs-Ecken +-y.
  Windung wie alle EM30-Flaechen: (v1 - v0) x (v2 - v0) zeigt GEGEN die Normale.
Farbe (Flaechenklassen; Texel = Original-Texel, die Original-Flaechen DERSELBEN Zeile benutzen bzw. die grauen Oberseiten-Texel des Stands
  07b6703f; Felder aus "suche"):
  grau  = Rueckensektor, Nadel, Wurzelflaechen ueber der Grenze: Zeile 487, Feld je Glied = groesstes Rechteck in den Texeln, die die
          Oberseite (Zeile 487) dieses Glieds im Eingang benutzt;
  bauch = Bauchsektor: Variante a/b "gedeckt" = Original-Zeile 482 (Asset 495) Feld "gedeckt" (hellere, weniger satte Partie des
          Original-Unterarm-Fleischs, suche: u 21-37 v 118-124, Median L* 26,8 C* 20,3), Variante c "satt" = FELD_SATT (heutiger Ton,
          unterseite_v8.json fleisch_r7b feld u 4-37 v 96-106, L* 18,2 C* 28,4); u um den Bauch, v entlang des Glieds (je Glied gespiegelt);
  band  = Bauchsektor in der Fuge (zwischen E und D bzw. S und C, dazu die Deckel), Variante b/c: Original-Zeile 481 (Asset 494) Seite 1 Feld
          "band" (groesstes Feld mit >= 90 % hellen Texeln der Original-Nutzung, suche: u 36-44 v 28-41, L* 74,1 C* 5,5); Variante a: wie bauch.
  Variante (Auftrag: 3 Farbproben): a gedeckt ohne Baender, b gedeckt mit hellen Baendern (STANDARD: Bauplan 2 verlangt das helle Band an
  jeder Fuge), c heutiger satter Ton nur auf der Bauchseite mit hellen Baendern.
"""
import os
import sys
import json
import copy
import hashlib
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402

S_ = 11                               # Ecken je Ring (wie v8_bau.S_; riegel_v8.ringe_bild liest Gruppen zu 11)
Y = np.array([0.0, 1.0, 0.0])
SCHWANZ = tuple(range(16, 23))

# ------------------------------------------------------------------ Form (PORT-WAHL, Analyse Bauplan Schritte 1/2)
QUER = 270.0                          # Querdicke E. (Bauplan 1: 260-280; Analyse B_vorhersage_L270.json, Rollreihe B_rolle_L280.json)
KERBE = 0.85                          # Einschnuerung an den 6 Gelenken (Bauplan 2 "Ring ~85 %")
KERBE_J = {j: KERBE for j in range(1, 7)}   # je Gelenk 1..6 (Gelenk j zwischen Glied j-1 und j)
WURZEL_FLACH = 0.6                    # Wurzelring an der Haut (Glied 16 Ring 0/1): Querhalbachse = QUER / 2 + 0,6 (heute - QUER / 2). Versuch
                                      # x1_varianten / x3_wurzel (PV, Riegel-Funktionen; Vorderansicht s .1 aus der Rollreihe r2_rolle): ganz flach
                                      # S2c 56 px (Rumpf ueber der Schwanz-Oberkante an der Naht J; Nutzer "Tail directly smooth into the back - no
                                      # hill or anything..."), rund S2 gruen aber vorn s .1 23 / 31 / 34 px (h22/h24/h64); w 0,6: S2a 2,2/6, S2b 1,9,
                                      # S2c 2, S4 Wurzel 55,5, vorn s .1 22,5 / 26,5 / 26,5; w <= 0,55: S2b 3,0 (Soll <= 2)
D16_QUER = 0.15                       # Querhalbachse Ring D in Glied 16 = QUER / 2 + 0,15 (A - QUER / 2): Uebergang rund (Wurzel) -> flach (Gelenk 1).
                                      # Versuch x1_varianten: 0 (flach) S2b 2,2; 0,1-0,2 S2a 2,2-2,3 / 6, S2b 1,9, S2c 2; 0,3 S2a Max 7 (Soll <= 6)
KERBE_LAENGE = 0.12                   # Anteil der Gliedlaenge je Gelenkseite bis zum vollen Querschnitt (65 E.; Fuge ~4 px von vorn, Videobaender
                                      # 4-6 je sichtbarem Stueck bei 25-35 px Abstand, Analyse 2b; Gehrung kippt hoechstens 10 Grad -> Ring C/D
                                      # bleiben vor/hinter den Gehrungsringen)
MITTE = {3: 0.86, 4: 0.80}            # Hochachse an Gelenk 3 / 4 (Bauplan 2 "Schwanzmitte (s .5) 15-20 % duenner", Analyse 5.5: s .5 in PV 33 px
                                      # gegen Video 23,5, Glieder 19/20); dazwischen linear; uebrige Gelenke 1,0. Versuch x2_mitte (PV, f_rast, s .5
                                      # gegen 07b6703f 33,5 px): 0,88/0,80 -13,4 %; 0,86/0,80 -16,4 % (28,0 px); 0,86/0,78 -19,4 %; 0,84/0,75 -25,4 %
GRENZE = (90.0, 60.0)                 # Fleischgrenze |theta| an der Wurzel (u 0) / am Spitzenende (u 7), linear (Auftrag Schritt 3: "vorn bei etwa
                                      # 85-95 Grad, zur Spitze hin etwa 60 Grad"; Video h90 rot auf 32-56 % der Breite innen, s >= .65 dunkel)
MANSCHETTE_R, MANSCHETTE_S = 0.99, 0.2   # wie v8_bau (Runde 2, Riegel S2b)
DORN_ALT = 48.0                       # heutiges Ende von Mesh 22: Dorn 0,8 x r7 = 48 E. ueber dem Gelenk 7 (Stand 07b6703f, Ecke 22 y 589)
NADEL = 550.0                         # Nadel ueber das heutige Ende hinaus (Auftrag "450-550 E."; Messung PV: Glied-22-Achse 35 px je 1000 E., Spitze
                                      # y_bild 347 bei 550 - der obere Rand ist dem Messziel 349-356 am naechsten)
# Stachel (Glied 22, rund, Areas_Tip.png: Knolle 16-17 px breit mit Kerbe am Ansatz, auf 35 px verjuengt, Nadel <= 2 px breit):
# (y, Radius) hinter Ring C; Knolle D = 2 x 135 = 270 E. (Auftrag "Knolle 260-280 E."), Nadel bis zur Spitze y = 541 + 48 + 550
STACHEL = ((200.0, 135.0), (330.0, 118.0), (450.0, 80.0), (560.0, 45.0), (640.0, 26.0), (880.0, 17.0), (1020.0, 10.0))
# Nadel-Radien 26 / 17 / 10 E.: Video "Breite <= 2 px" = <= 30 E.; duenner (24 / 12) verschwand die Nadel in PV ab y_bild 332 zwischen den
# Bildpunktmitten (Versuch m1_pv: tiefster Schwanz-Bildpunkt y 332 bei Spitze 347)
STACHEL_C = 120.0                     # Radius Ring C von Glied 22 (Knolle steigt aus der Kerbe 0,85 x 115,8 = 98 an)
ENDE_Y = 560.0                        # Ring 1 von Mesh 22 = Ort des alten Endrings (y 541) -> riegel_v8.s4 "Ende" misst dieselbe Stelle

# ------------------------------------------------------------------ Farbe (PORT-WAHL; Felder aus suche(), Beleg SF_belege/v8/runde8/suche.txt)
ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 495, 494      # 487 = Schwanz-Oberseite seit V6; 495 = Original 482; 494 = Original 481
SEITE_FLEISCH, SEITE_BAND = 2, 1
FELD_SATT = (4, 37, 96, 106)          # (u0, u1, v0, v1) Seite 2 = unterseite_v8.json fleisch_r7b "feld" (heutiger Ton)
# Felder "grau" (je Glied), "gedeckt", "band": schwanz_r8.json (Ergebnis von suche() am Eingang 07b6703f, Protokoll runde8/suche.txt)
FARBEN = {'a': dict(fleisch='gedeckt', band=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, name='gedeckt, helle Baender'),
          'c': dict(fleisch='satt', band=True, name='heutiger Ton nur Bauchseite, helle Baender')}
STANDARD = 'b'
GED_L, GED_A, GED_C = 20.0, 15.0, 26.0  # suche(): "gedeckt" = Fleischzeile (a* >= 15) heller (L* >= 20) und weniger satt (C* <= 26) als die satte
                                        # Partie (L* 18, C* 28,4 im Feld FELD_SATT); Richtung aus der Analyse (Video "gedeckter"), keine Eichung
GRAU_MIN_BREITE = 12                  # Grau-Feld eines Glieds schmaler als 12 Texel (Glieder 19/20: 8 / 7) -> Feld von Glied 16 (24 x 56): der
                                      # Rueckensektor ist 7 Felder breit, 7-8 Texel daruber waeren 5x gestreckt


# ------------------------------------------------------------------ Hilfen
def _p(name):
    return json.load(open(os.path.join(HIER, name)))


def grenze(u):
    """Fleischgrenze |theta| (Grad) an der Lage u (Glieder ab der Wurzel, 0..7)."""
    t = float(np.clip(u / 7.0, 0.0, 1.0))
    return GRENZE[0] + t * (GRENZE[1] - GRENZE[0])


def winkel(tb):
    """11 Ring-Winkel (Grad, steigend): Bauch 0, +-tb/2, +-tb (Ecken 0, 1, 2, 10, 9), Ruecken 7 gleiche Felder von tb bis 360 - tb."""
    w = [0.0, tb / 2.0]
    span = 360.0 - 2.0 * tb
    w += [tb + span * k / 7.0 for k in range(8)]
    w += [360.0 - tb / 2.0]
    return np.radians(np.array(w))


def achsen(phi_grad):
    p = np.radians(phi_grad)
    return np.array([np.cos(p), 0.0, np.sin(p)]), np.array([-np.sin(p), 0.0, np.cos(p)])


def ring(y0, A, B, n, th, eh, eq):
    """Ellipsenring (A hoch, B quer) um die Achse bei y0, auf die Ebene durch (0, y0, 0) mit Normale n gelegt (None = senkrecht)."""
    off = np.outer(A * np.cos(th), eh) + np.outer(B * np.sin(th), eq)
    t = -(off @ n) / n[1] if n is not None else 0.0
    return off + np.array([0.0, y0, 0.0]) + (t[:, None] * Y if n is not None else 0.0)


def gelenk_radien(par):
    r = [float(x) for x in par['radien']]
    return [r[j] * MITTE.get(j, 1.0) for j in range(8)]


def quer(a):
    return min(a, QUER / 2.0)


# ------------------------------------------------------------------ Geometrie je Glied
def glied_ringe(g, L, R, n_anf, n_end, a_vor, phi, alt16=None):
    """Ringe des Glieds g in Achsfolge: Liste (Name, Ecken 11x3, theta-Winkel, Lage u je Ecke, Kerbe?)."""
    eh, eq = achsen(phi)
    ra, re = R[g], R[g + 1]
    rr = lambda y: ra + (re - ra) * y / L
    out = []
    kl = KERBE_LAENGE * L
    if g == 0:
        out += wurzel_ringe(alt16, eh, eq, phi)
    else:
        th = winkel(grenze(g))
        q = ring(0.0, KERBE_J[g] * ra, KERBE_J[g] * quer(ra), n_anf, th, eh, eq)
        a = a_vor / np.linalg.norm(a_vor)
        ax_ = (q @ a)[:, None] * a[None, :]
        S = ax_ + MANSCHETTE_R * (q - ax_) - MANSCHETTE_S * ra * a[None, :]
        out.append(('S', S, th, np.full(S_, float(g)), True))
        if g < 6:
            th = winkel(grenze(g + KERBE_LAENGE))
            out.append(('C', ring(kl, rr(kl), quer(rr(kl)), n_anf, th, eh, eq), th, np.full(S_, g + KERBE_LAENGE), False))
        else:
            th = winkel(grenze(g + KERBE_LAENGE))
            out.append(('C', ring(kl, STACHEL_C, STACHEL_C, n_anf, th, eh, eq), th, np.full(S_, g + KERBE_LAENGE), False))
    if g == 2:
        th = winkel(grenze(g + 0.5))
        out.append(('M', ring(0.5 * L, rr(0.5 * L), quer(rr(0.5 * L)), None, th, eh, eq), th, np.full(S_, g + 0.5), False))
    if g < 6:
        th = winkel(grenze(g + 1 - KERBE_LAENGE))
        aD = rr(L - kl)
        bD = quer(aD) if (g > 0 or D16_QUER is None) else QUER / 2.0 + D16_QUER * (aD - QUER / 2.0)
        out.append(('D', ring(L - kl, aD, bD, n_end, th, eh, eq), th, np.full(S_, g + 1 - KERBE_LAENGE), False))
        th = winkel(grenze(g + 1))
        out.append(('E', ring(L, KERBE_J[g + 1] * re, KERBE_J[g + 1] * quer(re), n_end, th, eh, eq), th, np.full(S_, float(g + 1)), True))
    else:
        for y, r in STACHEL:
            u = g + min(y, L) / L
            th = winkel(grenze(u))
            out.append(('E' if y == ENDE_Y else 'T%d' % int(y), ring(y, r, quer(r), None, th, eh, eq), th, np.full(S_, u), False))
    return out


def wurzel_ringe(alt16, eh, eq, phi):
    """Glied 16: Ring 1 (Wurzelring an der Haut, Stufe w; Ring 0 = Ring 1) auf die Winkel der neuen Ringe umgetastet (polar um die Ringmitte,
    linear auf dem alten Ringzug) und quer zur Bauchachse auf QUER / 2 abgeflacht (gleichmaessig je Ring)."""
    P = np.array([v[:3] for v in alt16['v'][S_:2 * S_]], float)
    c = P.mean(0)
    rel = P - c
    a_alt = np.arctan2(rel @ eq, rel @ eh) % (2 * np.pi)          # theta je alter Ecke (0 = Bauch)
    o = np.argsort(a_alt)
    a_s, P_s = a_alt[o], P[o]
    th = winkel(grenze(0.0))
    neu = []
    for t in th:
        t = t % (2 * np.pi)
        k = np.searchsorted(a_s, t) % len(a_s)
        k0 = (k - 1) % len(a_s)
        t0, t1 = a_s[k0], a_s[k]
        span = (t1 - t0) % (2 * np.pi)
        f = ((t - t0) % (2 * np.pi)) / span if span > 1e-9 else 0.0
        neu.append(P_s[k0] + f * (P_s[k] - P_s[k0]))
    neu = np.array(neu)
    qk = (neu - c) @ eq
    s = (QUER / 2.0) / max(float(np.abs(qk).max()), 1e-9)
    if WURZEL_FLACH is True:
        s = min(s, 1.0)
    elif WURZEL_FLACH is False or WURZEL_FLACH is None:
        s = 1.0
    else:                                            # Zahl w: Querhalbachse = QUER / 2 + w (heute - QUER / 2)
        q0 = float(np.abs(qk).max())
        s = min(1.0, (QUER / 2.0 + float(WURZEL_FLACH) * (q0 - QUER / 2.0)) / max(q0, 1e-9))
    neu = neu - (1.0 - s) * qk[:, None] * eq[None, :]
    return [('W0', neu.copy(), th, np.zeros(S_), False), ('W1', neu, th, np.zeros(S_), False)]


WURZEL_TIEF = 0.0                     # Ecken des Wurzelrings AUSSERHALB der Haut von Mesh 8 (PV) gleiten nach innen. Nicht 15 E. wie wurzel_r4: am
                                      # gebauten Ring lagen alle Ecken innen (3,7-87 E., m6_wurzel), die zwei unter 15 E. auf 15 E. geschoben
                                      # verbogen den Ring -> S2b 3,7, S2c 4 (Soll <= 2 / <= 3); S10 (Wurzel von hinten) in beiden Faellen 0


def wurzel_einbetten(ringe, blob, ms):
    """Ring W1 (= W0) nach dem Abflachen: Ecken, die weniger als WURZEL_TIEF unter der Haut von Mesh 8 liegen, gleiten wie in Stufe w
    (wurzel_r4) entlang der Gliedachse nach -y, bis sie WURZEL_TIEF tief liegen (Halbierung, 0 .. -600 E.). -> Liste (Ecke, Tiefe vorher, dy)."""
    import r3_gemein as R3
    mo = V.modell(blob)
    R, T = V.pose_r_t(mo)
    R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
    R16, T16 = R[16].astype(float) / 4096.0, T[16].astype(float)
    A = R8.T @ R16; t = R8.T @ (T16 - T8)
    tri = R3.dreiecke(ms[8])
    tief = lambda q: float(np.prod(R3.abstand_innen((q @ A.T + t)[None], tri)[0]))
    P = ringe[1][1]
    liste = []
    for i in range(S_):
        d0 = tief(P[i])
        liste.append((i, round(d0, 1), 0.0))
        if d0 >= WURZEL_TIEF:
            continue
        liste.pop()
        lo, hi = 0.0, 600.0
        for _ in range(30):
            mi = 0.5 * (lo + hi)
            if tief(P[i] - mi * Y) >= WURZEL_TIEF:
                hi = mi
            else:
                lo = mi
        P[i] = P[i] - hi * Y
        liste.append((i, round(d0, 1), round(-hi, 1)))
    ringe[0] = (ringe[0][0], P.copy()) + tuple(ringe[0][2:])
    ringe[1] = (ringe[1][0], P) + tuple(ringe[1][2:])
    return liste


# ------------------------------------------------------------------ Mesh bauen
def _kappe_mitte(R_):
    return R_.mean(0)


def mesh_bauen(g, ringe, L, alt, y_spitze):
    """-> (Ecken, Flaechen-Liste, Info). Flaeche = (Art, Ecken-Indizes, Ring-Namen, Sektor-Index j). Reihenfolge der Ecken siehe Kopf."""
    namen = [r[0] for r in ringe]
    pos = {}
    V_ = []

    ring_d = {r[0]: r for r in ringe}

    def setze(name):
        pos[name] = len(V_)
        V_.extend(list(ring_d[name][1]))

    if g == 0:
        ordnung = ['W0', 'W1', 'D']
    elif g == 2:
        ordnung = ['C', 'M', 'D']
    elif g == 6:
        ordnung = ['C', 'E']
    else:
        ordnung = ['C', 'D']
    for nm in ordnung:
        setze(nm)
    achse = {}
    if g == 0:
        achse['ende'] = len(V_); V_.append(np.array([0.0, L, 0.0]))
        achse['kegel'] = len(V_); V_.append(np.array(alt['v'][34][:3], float))
    elif g == 6:
        achse['spitze'] = len(V_); V_.append(np.array([0.0, y_spitze, 0.0]))
        achse['anfang'] = len(V_); V_.append(_kappe_mitte(ring_d['S'][1]))
    else:
        achse['ende'] = len(V_); V_.append(np.array([0.0, L, 0.0]))
        achse['anfang'] = len(V_); V_.append(_kappe_mitte(ring_d['S'][1]))
    for nm in namen:
        if nm not in pos:
            setze(nm)
    V_ = np.array(V_, float)
    F = []
    folge = namen
    for i in range(len(folge) - 1):
        a, b = folge[i], folge[i + 1]
        if a == 'W0':
            continue                                    # Wurzelrohr Ring 0 -> 1 hat seit Stufe w keine Flaeche (Laenge 0): alte Flaechen
        for j in range(S_):
            k = (j + 1) % S_
            F.append(('mantel', [pos[a] + j, pos[a] + k, pos[b] + j, pos[b] + k], (a, b), j))
    if g != 0:
        s0 = pos['S']
        for j in range(S_):
            k = (j + 1) % S_
            F.append(('deckel_anf', [s0 + k, s0 + j, achse['anfang']], ('S', 'S'), j))
    last = folge[-1]
    l0 = pos[last]
    zentrum = achse['spitze'] if g == 6 else achse['ende']
    for j in range(S_):
        k = (j + 1) % S_
        F.append(('spitze' if g == 6 else 'deckel_end', [l0 + j, l0 + k, zentrum], (last, last), j))
    return V_, F, dict(pos=pos, achse=achse, folge=folge)


def normalen(V_, F, achse_ecken):
    N = np.zeros_like(V_)
    for art, e, _r, _j in F:
        if art.startswith('deckel'):
            continue
        tris = [(e[0], e[1], e[3]), (e[0], e[3], e[2])] if len(e) == 4 else [tuple(e)]
        for a, b, c in tris:
            n = -np.cross(V_[b] - V_[a], V_[c] - V_[a])       # Windung: Kreuzprodukt zeigt nach innen
            for i in (a, b, c):
                N[i] += n
    out = []
    for i in range(len(V_)):
        if i in achse_ecken:
            n = achse_ecken[i]
        else:
            n = N[i]
            if np.linalg.norm(n) < 1e-9:
                n = np.array([V_[i][0], 0.0, V_[i][2]])
        n = n / max(np.linalg.norm(n), 1e-12)
        out.append([int(round(x * 4096)) for x in n])
    return out


# ------------------------------------------------------------------ Texelfelder
def _rechteck(mask, min_breite=4):
    """Groesstes achsparalleles Rechteck (Flaeche) ganz in mask -> (u0, u1, v0, v1) in Maskenspalten/-zeilen, inklusive."""
    H, W = mask.shape
    hoch = np.zeros(W, int)
    best = (0, None)
    for v in range(H):
        hoch = np.where(mask[v], hoch + 1, 0)
        st = []
        for u in range(W + 1):
            h = hoch[u] if u < W else 0
            anf = u
            while st and st[-1][1] >= h:
                anf, hh = st.pop()
                br = u - anf
                if br >= min_breite and hh * br > best[0]:
                    best = (hh * br, (anf, u - 1, v - hh + 1, v))
            st.append((anf, h))
    return best[1]


def grau_texel(ms_ein):
    """Je Glied: Texel (Seite 2, Bild 256 x 384), die die Flaechen der Zeile 487 dieses Glieds im Eingang benutzen."""
    out = {}
    for b in SCHWANZ:
        m = ms_ein[b]
        mk = np.zeros((256, 384), bool)
        for uv in m['tuv'] + m['quv']:
            if ((uv[2] >> 6) & 0x1FF) == ZEILE_GRAU:
                mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
        out[b] = mk
    return out


def suche(blob, log=print):
    """Texelfelder (Beleg): grau je Glied = groesstes Rechteck in den Oberseiten-Texeln des Eingangs (Seite 2); gedeckt = groesstes Rechteck
    in Original-Zeile 482 (Seite 2), dessen Texel Original-Flaechen der Zeile benutzen und dessen Median L* >= 22 und C* <= 25 ist, im
    Unterarm-Fleisch unterhalb der satten Partie (v >= 110); band = groesstes Rechteck in Original-Zeile 481 (Seite 1) mit >= 90 % hellen
    Texeln (L* >= 55, C* <= 18, Klasse 'hell_band' der Analyse farbe_vorrat.py), Texel von Original-Flaechen der Zeile benutzt."""
    import adern_r4 as A4
    import v4_gemein as G
    d, _o, ms, tim = KB.teile(blob)
    o = BF.lade_em30()
    _do, _oo, _mso, timo = KB.teile(o)
    to = KB.tim_teile(timo)
    t = KB.tim_teile(tim)
    erg = {}
    gt = grau_texel(ms)
    grau = {}
    for b in SCHWANZ:
        r = _rechteck(gt[b][:, 256:384])
        grau[b] = r
        log('grau Glied %d: Oberseiten-Texel %d, groesstes Rechteck u %d-%d v %d-%d (%d x %d)%s' % (b, int(gt[b].sum()), r[0], r[1], r[2], r[3],
            r[1] - r[0] + 1, r[3] - r[2] + 1, ' -> schmaler als %d, Feld von Glied 16' % GRAU_MIN_BREITE if r[1] - r[0] + 1 < GRAU_MIN_BREITE else ''))
    for b in SCHWANZ:
        if grau[b][1] - grau[b][0] + 1 < GRAU_MIN_BREITE:
            grau[b] = grau[16]
    erg['grau'] = {str(b): list(map(int, r)) for b, r in grau.items()}

    def lab_zeile(z):
        c = to['clut'][z - 480].astype(int)
        rgb = np.stack([(c & 31) * 255 / 31., ((c >> 5) & 31) * 255 / 31., ((c >> 10) & 31) * 255 / 31.], -1)
        return G.lab(rgb[None])[0]
    # gedeckt: Zeile 482, Seite 2 - Texelzeilen des Original-Unterarms, deren Median (Original-Nutzung) Fleisch, aber heller und weniger satt als
    # die satte Partie ist: L* >= GED_L, a* >= GED_A, C* <= GED_C (satte Partie v 93-112: L* 18, C* 25-30; Messung im Protokoll)
    lab = lab_zeile(482)
    nutz = A4.nutzung(None, 482)[:, 256:384]
    Lm = lab[to['pix'][:, 256:384]]
    zeilen = []
    for v in range(256):
        if nutz[v].sum() < 8:
            continue
        x = Lm[v][nutz[v]]
        Lz, az, Cz = float(np.median(x[:, 0])), float(np.median(x[:, 1])), float(np.median(np.hypot(x[:, 1], x[:, 2])))
        if Lz >= GED_L and az >= GED_A and Cz <= GED_C:
            zeilen.append(v)
            log('   gedeckt-Zeile v %d: L* %.1f a* %.1f C* %.1f (%d Texel)' % (v, Lz, az, Cz, int(nutz[v].sum())))
    mk = np.zeros_like(nutz)
    for v in zeilen:
        mk[v] = nutz[v]
    r = _rechteck(mk)
    sel = Lm[r[2]:r[3] + 1, r[0]:r[1] + 1].reshape(-1, 3)
    Ls, Cs = float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2])))
    log('gedeckt Zeile 482 Seite 2: groesstes Rechteck in diesen Zeilen und der Original-Nutzung u %d-%d v %d-%d (%d x %d), Median L* %.1f '
        'C* %.1f a* %.1f' % (r[0], r[1], r[2], r[3], r[1] - r[0] + 1, r[3] - r[2] + 1, Ls, Cs, float(np.median(sel[:, 1]))))
    erg['gedeckt'] = list(map(int, r))
    # satt (heutiger Ton): Feld pruefen
    u0, u1, v0, v1 = FELD_SATT
    ok = bool(nutz[v0:v1 + 1, u0:u1 + 1].all())
    sel = Lm[v0:v1 + 1, u0:u1 + 1].reshape(-1, 3)
    log('satt Zeile 482 Seite 2: Feld u %d-%d v %d-%d ganz in der Original-Nutzung: %s, Median L* %.1f C* %.1f' % (u0, u1, v0, v1, ok,
        float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2])))))
    erg['satt_ok'] = ok
    # band: Zeile 481, Seite 1
    lab = lab_zeile(481)
    nutz = A4.nutzung(None, 481)[:, 128:256]
    Lm = lab[to['pix'][:, 128:256]]
    hell = nutz & (Lm[..., 0] >= 55) & (np.hypot(Lm[..., 1], Lm[..., 2]) <= 18)
    from scipy import ndimage as ndi
    dichte = ndi.uniform_filter(hell.astype(float), 5)
    r = _rechteck(nutz & (dichte >= 0.9))
    sel = Lm[r[2]:r[3] + 1, r[0]:r[1] + 1].reshape(-1, 3)
    anteil = float(hell[r[2]:r[3] + 1, r[0]:r[1] + 1].mean())
    log('band Zeile 481 Seite 1: groesstes Rechteck (5x5-Dichte hell >= 0,9, Original-Nutzung) u %d-%d v %d-%d (%d x %d), hell %.0f %%, '
        'Median L* %.1f C* %.1f' % (r[0], r[1], r[2], r[3], r[1] - r[0] + 1, r[3] - r[2] + 1, 100 * anteil, float(np.median(sel[:, 0])),
                                    float(np.median(np.hypot(sel[:, 1], sel[:, 2])))))
    erg['band'] = list(map(int, r))
    # Texel des Spiel-Assets in diesen Feldern = Original?
    gl = {k: bool(np.array_equal(t['pix'][r_[2]:r_[3] + 1, s * 128 + r_[0]:s * 128 + r_[1] + 1],
                                 to['pix'][r_[2]:r_[3] + 1, s * 128 + r_[0]:s * 128 + r_[1] + 1]))
          for k, r_, s in (('gedeckt', erg['gedeckt'], 2), ('band', erg['band'], 1), ('satt', FELD_SATT, 2))}
    log('Asset-Texel in den Feldern bytegleich Original: %s' % gl)
    erg['asset_gleich'] = gl
    return erg


# ------------------------------------------------------------------ UV
def _uv_feld(feld, s_um, s_lang):
    u0, u1, v0, v1 = feld
    return (u0 + s_um * (u1 - u0), v0 + s_lang * (v1 - v0))


BAUCH_J = (9, 10, 0, 1)                # Sektoren (Flaeche j = Ecke j -> j+1) zwischen -theta_b und +theta_b


def klasse_von(f, farbe, g):
    """Flaechenklasse 'grau' / 'bauch' / 'band' (Kopf der Datei)."""
    art, e, (ra, rb), j = f
    if art == 'spitze' or j not in BAUCH_J:
        return 'grau'
    if g == 6:
        fuge = art == 'deckel_anf' or (art == 'mantel' and 'S' in (ra, rb))
    else:
        fuge = art.startswith('deckel') or (art == 'mantel' and bool({ra, rb} & {'S', 'E'}))
    return 'band' if farbe['band'] and fuge else 'bauch'


def uv_setzen(g, V_, F, ringe, info, farbe, felder, L):
    """-> Liste (Flaeche, UV-Liste fuer KB/BF-Format, Klasse)."""
    th_e = {}
    u_e = {}
    for nm, P, th, u, _k in ringe:
        p0 = info['pos'][nm]
        for k in range(S_):
            th_e[p0 + k] = float(np.degrees(th[k]))
            u_e[p0 + k] = float(u[k])
    ys = V_[:, 1]
    y_lo, y_hi = float(min(ys)), float(max(ys))
    out = []
    feld_grau = felder['grau'][str(16 + g)]
    feld_bauch = felder['gedeckt'] if farbe['fleisch'] == 'gedeckt' else list(FELD_SATT)
    feld_band = felder['band']
    tb_glied = lambda i: grenze(u_e.get(i, g + 0.5))
    for f in F:
        art, e, (ra, rb), j = f
        kl = klasse_von(f, farbe, g)
        if g == 6 and kl != 'grau' and all(V_[i][1] >= ENDE_Y - 1.0 for i in e):
            kl = 'grau'                                    # Nadel ganz grau (Video: duenne dunkle Spitze)
        uv_e = []
        for i in e:
            if i in th_e:
                th = th_e[i]
                tb = tb_glied(i)
            else:                                          # Achs-Ecke: Mitte der Ring-Ecken der Flaeche
                rs = [q for q in e if q in th_e]
                th = float(np.degrees(np.angle(np.mean([np.exp(1j * np.radians(th_e[q])) for q in rs]))))
                tb = tb_glied(rs[0])
            th = th % 360.0
            s_lang = float(np.clip((V_[i][1] - y_lo) / max(y_hi - y_lo, 1.0), 0.0, 1.0))
            if kl == 'grau':
                s_um = float(np.clip((th - tb) / max(360.0 - 2 * tb, 1.0), 0.0, 1.0))
                uv_e.append(_uv_feld(feld_grau, s_um, s_lang))
            else:
                ts = th if th <= 180.0 else th - 360.0
                s_um = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                if kl == 'bauch':
                    sl = s_lang if g % 2 == 0 else 1.0 - s_lang       # je Glied gespiegelt: an jedem Gelenk dieselbe Texelzeile
                    uv_e.append(_uv_feld(feld_bauch, s_um, sl))
                else:
                    # Band: v = Abstand zum Gelenk (v0 am Gelenk, v1 am vollen Querschnitt) - beide Seiten der Fuge gespiegelt gleich
                    yy = V_[i][1]
                    dist = min(abs(yy), abs(yy - L)) / (KERBE_LAENGE * L)
                    uv_e.append(_uv_feld(feld_band, s_um, float(np.clip(dist, 0.0, 1.0))))
        out.append((f, [(int(round(u)), int(round(v))) for u, v in uv_e], kl))
    return out


def uv_eintrag(uvl, kl):
    zeile, seite = {'grau': (ZEILE_GRAU, 2), 'bauch': (ZEILE_FLEISCH, SEITE_FLEISCH), 'band': (ZEILE_BAND, SEITE_BAND)}[kl]
    n = len(uvl)
    x = [0] * (9 if n == 3 else 12)
    for k, (u, v) in enumerate(uvl):
        x[3 * k], x[3 * k + 1] = u, v
    x[2] = zeile << 6
    x[5] = 128 + seite
    return x


# ------------------------------------------------------------------ Stufe
def felder_laden():
    p = os.path.join(HIER, 'schwanz_r8.json')
    return json.load(open(p))['felder'] if os.path.exists(p) else None


def stufe(blob, farbe=STANDARD, prot=None, felder=None):
    """Eingang (Stand nach Stufe f) -> Ausgang (bytes). Nur Meshes 16-22 (Ecken, Normalen, Flaechen, UV/CLUT-Felder) neu."""
    fa = FARBEN[farbe]
    d, _o, ms, tim = KB.teile(blob)
    felder = felder or felder_laden() or suche(blob, log=lambda *_: None)
    mo = V.modell(blob)
    L = float(mo.skel.rel[22][1])
    assert all(tuple(mo.skel.rel[b]) == (0, int(L), 0) for b in range(17, 23)) and int(L) == 541, 'Gliedlaengen nicht 541'
    par = _p('kette_v8.json')
    phi = {int(k): float(v) for k, v in _p('unterseite_v8.json')['innen_phi'].items()}
    import v8_bau as VB
    lok = VB.ruhe_lokal(np.array(par['gelenke_b8'], float))
    n_end, n_anf = VB.gehrungen(lok)
    R = gelenk_radien(par)
    y_spitze = L + DORN_ALT + NADEL
    alt = copy.deepcopy(ms)
    info_alle = {}
    for g in range(7):
        b = 16 + g
        a_vor = (lok[g].T @ Y) if g > 0 else None
        ringe = glied_ringe(g, L, R, n_anf[g], n_end[g] if g < 6 else None, a_vor, phi[b], alt16=alt[16] if g == 0 else None)
        if g == 0:
            eingebettet = wurzel_einbetten(ringe, blob, alt)
        V_, F, info = mesh_bauen(g, ringe, L, alt[b], y_spitze)
        ach = {}
        for nm, i in info['achse'].items():
            ach[i] = {'ende': Y, 'spitze': Y, 'anfang': -Y, 'kegel': np.array(alt[b]['n'][34][:3], float) if g == 0 else -Y}[nm]
        N = normalen(V_, F, ach)
        uvs = uv_setzen(g, V_, F, ringe, info, fa, felder, L)
        m = dict(v=[[int(round(x)) for x in p] + [0] for p in V_], n=[n + [0] for n in N], tf=[], tuv=[], qf=[], quv=[])
        zahl = {}
        if g == 0:
            # Wurzelflaechen (Rohr Ring 0 -> 1, Kegel Ring 0 -> Spitze 34) mit ihren alten Ecken-Indizes (0-21, 34) und UV bleiben
            for key, F_, U_ in (('tf', 'tf', 'tuv'), ('qf', 'qf', 'quv')):
                for f, uv in zip(alt[16][F_], alt[16][U_]):
                    if set(f[1::2]) <= set(range(22)) | {34}:
                        m[F_].append(list(f)); m[U_].append(list(uv))
            zahl['wurzel'] = len(m['tf']) + len(m['qf'])
        for (art, e, _r, _j), uvl, kl in uvs:
            if len(e) == 4:
                m['qf'].append([x for i in e for x in (i, i)]); m['quv'].append(uv_eintrag(uvl, kl))
            else:
                m['tf'].append([x for i in e for x in (i, i)]); m['tuv'].append(uv_eintrag(uvl, kl))
            zahl[kl] = zahl.get(kl, 0) + 1
        ms[b] = m
        info_alle[b] = dict(ecken=len(m['v']), dreiecke=len(m['tf']), vierecke=len(m['qf']), klassen=zahl,
                            ringe=[(nm, round(float(np.abs((P - P.mean(0)) @ achsen(phi[b])[0]).max()), 1),
                                    round(float(np.abs((P - P.mean(0)) @ achsen(phi[b])[1]).max()), 1)) for nm, P, _t, _u, _k in ringe])
    neu = KB.packen(blob, d, ms, tim)
    if prot is not None:
        prot['schwanz_r8'] = dict(wurzel_eingebettet=eingebettet, farbe=farbe, farbe_name=fa['name'], quer=QUER, kerbe=KERBE, kerbe_laenge=KERBE_LAENGE, mitte=MITTE,
                                  grenze=GRENZE, nadel_spitze_y=y_spitze, stachel=STACHEL, felder=felder, glieder=info_alle,
                                  eingang_sha256=hashlib.sha256(blob).hexdigest())
    return neu


def main():
    if sys.argv[1] == 'suche':
        blob = open(sys.argv[2] if len(sys.argv) > 2 else V.ASSET, 'rb').read()
        print('Eingang sha256', hashlib.sha256(blob).hexdigest())
        erg = suche(blob)
        print(json.dumps(erg))
        return
    blob = open(sys.argv[1], 'rb').read()
    farbe = sys.argv[3] if len(sys.argv) > 3 else STANDARD
    prot = {}
    neu = stufe(blob, farbe, prot)
    open(sys.argv[2], 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s), Farbe %s' % (sys.argv[2], len(neu), hashlib.sha256(neu).hexdigest(), farbe))
    print(json.dumps(prot['schwanz_r8']['glieder'], default=str)[:3000])


if __name__ == '__main__':
    main()
