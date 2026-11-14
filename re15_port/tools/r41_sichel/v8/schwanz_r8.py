"""SF V8 Runde 8, Stufe q (Kette v8_bau.py --stufen ...fq): Schwanz der Sichel-Form neu - flacher Querschnitt, Glieder mit Fugen,
Skorpionstachel, Fleisch nur auf der Bauchseite. NUR Asset (Meshes 16-22 mit ihrer UV/CLUT); Knochen, Keyframes, Texturbild, CLUT-Zeilen
und alle anderen Meshes (Mesh 9 = V7-Kopf) bleiben bytegleich.

    python schwanz_r8.py <eingang.emd> <ausgang.emd> [a|b]       Stufe auf einen fertigen Stand anwenden (Standard-Farbe STANDARD)
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
ist PORT-WAHL, belegt mit Nutzer-Wortlaut, der Analyse oder der Messung von "suche" (SF_belege/v8/runde8/suche.txt, Runde 8b runde8b/suche_r8b.txt).

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
  Glied 22: S, C, Knolle und Verjuengung (STACHEL, rund), Nadel bis y = SPITZE_Y = 1300 entlang der Gliedachse (der Code nimmt die
  groesste Ecke entlang der Achse als Dornspitze, schwanz_r41.c sw_mesh_mass); kein End-Deckel, die Nadelspitze schliesst.
  Runde 8b (Pruefer pr8): Glieder 17-21 mit zwei Zwischenringen Z1/Z2 (Glied 18 um M); Keyframes, in denen eine Ecke von Mesh 22 unter der
  tiefsten Koerperecke laege, hebt heben() an (nur die Winkel der Knochen 17-22 dieser Keyframes; S6 streng).
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
  Runde 8c (Koordinator 14.11., Nutzer waehlt Probe b: "wobei das Original Video der Schwanz noch etwas heller ist und nicht ganz so rot"):
  bauch = Original-Zeile 480 (Asset 493), Seite 0, Feld C1 u 54-65 v 48-57 (schwanz_r8.json; ausgewaehlt unter 18 Original-Feldern nach der
          Messung im Spielbild F255-F320, Lab-Abstand zur Video-Mitte des Pruefers gruenblut 5,1 statt 8,7; SF_belege/v8/runde8c); grosse
          gefleckte Felder ergaben je Bild sehr verschiedene Toene (nur ein Teil des Felds sichtbar), gespiegelte Abbildung ein Augenmuster;
  band  = Probe b: Ring umlaufend auf allen 11 Sektoren der Fugen (Mantel S-C / D-E und Deckel), Original-Zeile 480 (Asset 493), Seite 0,
          Feld RB u 93-100 v 10-17 mit Verlauf hell (u 100, am Gelenk) -> dunkel (u 93, am vollen Querschnitt), ohne Texel unter L* 18;
  Bis Runde 8b:
  bauch = Bauchsektor: Original-Zeile 482 (Asset 495), schraeges Feld (Parallelogramm, suche(): v 114-123, Zeilenanfang u 1 -> 19, Breite 21, im Bau 20;
          Median L* 25,7 C* 24,2, hellere Partie des Original-Unterarm-Fleischs) - groesser als das 17 x 7-Feld der Runde 8 ("flaechig wie gemalt",
          Pruefer pr8); einmal quer ueber den Bauch, einmal laengs je Glied (je Glied gespiegelt). Gependelt je Flaeche (Runde 8b, erster Bau)
          ergab im Spiel ein regelmaessiges Rautenmuster und ist verworfen;
  band  = Probe b: die zwei mittleren Bauchfelder (BAND_J) in der Fuge (zwischen E und D bzw. S und C, dazu die Deckel): Original-Zeile 480
          (Asset 493, bytegleich), Seite 0, Feld u 58-73 v 19-48 (>= 60 % hellrot/rosa, Median L* 48,4 C* 17,4 h 22; Runde 8: Zeile 481 fast weiss
          L* 74 = "Klebeband", Pruefer pr8); Probe a: ohne Band (Fleisch).
  Proben (Koordinator: nur noch zwei, wenn sie sich sichtbar unterscheiden): a ohne Baender, b mit rosa Baendern (STANDARD; Runde 8c: Ringe).
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
MITTE = {3: 0.86, 4: 0.78}            # Hochachse an Gelenk 3 / 4 (Bauplan 2 "Schwanzmitte (s .5) 15-20 % duenner" und Messziel "Seitenansicht s .5 /
                                      # .7 / .9 = 23,5 / 18,5 / 15 +- 4"; Analyse 5.5: s .5 in PV 33 px gegen Video 23,5, Glieder 19/20); dazwischen
                                      # linear; uebrige Gelenke 1,0. Versuch x2_mitte (PV, f_rast, Breite in 0,5-px-Schritten, 07b6703f s .5 33,5 px):
                                      # 0,88/0,80 29,0 (-13,4 %); 0,86/0,80 28,0 (-16,4 %, aber +4,5 gegen 23,5); 0,86/0,78 27,0 (-19,4 %, +3,5);
                                      # 0,84/0,75 25,0 (-25,4 %)
GRENZE = (90.0, 60.0)                 # Fleischgrenze |theta| an der Wurzel (u 0) / am Spitzenende (u 7), linear (Auftrag Schritt 3: "vorn bei etwa
                                      # 85-95 Grad, zur Spitze hin etwa 60 Grad"; Video h90 rot auf 32-56 % der Breite innen, s >= .65 dunkel)
MANSCHETTE_R, MANSCHETTE_S = 0.99, 0.2   # wie v8_bau (Runde 2, Riegel S2b)
DORN_ALT = 48.0                       # Ende von Mesh 22 bis Runde 7e: Dorn 0,8 x r7 = 48 E. ueber dem Gelenk 7 (Stand 07b6703f, Ecke 22 y 589)
# Runde 8b (Pruefer pr8, PRUEFUNG.md (A)/(E)1: "Knolle in PV als Verdickung sichtbar ... ~18 px breit bei 150-190 px ab Wurzel", "sichtbarer Schwanz
# in PV bis y ~350", "die Nadel ist ein 1-2 px duenner Draht ... soll eine sichtbare Skorpionspitze sein"). Gemessen (n1_richtung.py): in PV ist
# bildabwaerts fast ueberall weltabwaerts (in 855-897 von 912 Keyframes), eine abgewinkelte Nadel waere also nicht bodensicherer als eine gerade;
# die gerade Nadel entlang der Gliedachse bildet der Code genau ab (sw_mesh_mass dorn_y), eine abgewinkelte nur als Radius um das Gelenk 7
# (mehr Anhebung). Deshalb: Knolle (Radius 190), Verjuengung, kegelige Nadel (Radius 75 -> 28) bis zur Spitze SPITZE_Y auf der Achse.
# Bodenfreiheit in den Keyframes stellt heben() her (S6 streng).
# (y, Radius) hinter Ring C, rund; Knolle D 380 E. Versuch x4_stachel (PV, Verfahren des Pruefers in festen Abstaenden): Knolle 168 / Spitze 1200
# tiefster sichtbarer Punkt y 346, Laenge 184,8; Knolle 190 / Spitze 1300 y 350, 185,8 (Breite bei 150 / 170 px 12,0 / 12,6); 210 / 1250 y 348.
# Die Knolle liegt in PV hinter dem Ende von Glied 21 (Achse 22 zeigt von der Kamera weg) - groesser macht sie dort kaum breiter.
STACHEL = ((200.0, 160.0), (330.0, 190.0), (450.0, 185.0), (560.0, 150.0), (660.0, 110.0), (760.0, 75.0), (920.0, 50.0), (1100.0, 28.0))
STACHEL_C = 128.0                     # Radius Ring C von Glied 22 (Knolle steigt aus der Kerbe 0,85 x 115,8 = 98 an)
SPITZE_Y = 1300.0                     # Nadelspitze (PV: Glied-22-Achse 3,5 px je 100 E. ab Gelenk 7 y_bild 325,8; sichtbar bis y 350, Pruefer "bis y ~350")
ENDE_Y = 760.0                        # Ring 1 von Mesh 22 = Nadelansatz (Radius 75) -> riegel_v8.s4 "Ende <= 10 px"
ZWISCHEN = {1: (0.3733, 0.6267), 2: (0.31, 0.69), 3: (0.3733, 0.6267), 4: (0.3733, 0.6267), 5: (0.3733, 0.6267)}
                                      # Runde 8b: zwei Zwischenringe Z1/Z2 je Glied 17-21 (Glied 18 um M bei 0,5) - das Fleischfeld liegt dreimal
                                      # (Runde 8b2: Fleisch nicht mehr gependelt, siehe uv_setzen; die Ringe bleiben fuer die Biegung)

# ------------------------------------------------------------------ Farbe (PORT-WAHL; Felder aus suche(), Beleg SF_belege/v8/runde8b/suche_r8b.txt)
ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 493, 493      # 487 = Schwanz-Oberseite seit V6; 493 = Original 480 (bytegleich); Runde 8c: Bauch und
SEITE_FLEISCH, SEITE_BAND = 0, 0                            # Ring beide auf 493 Seite 0 (Felder C1 / RB, schwanz_r8.json); bis Runde 8b Bauch 495 = Original 482
ZEILE_WURZEL = 495                                          # die 14 Wurzelflaechen von Mesh 16 (Stufe w/u) behalten Zeile 495 und ihre UV
# Runde 8b (Pruefer pr8 (B)): Band hellrot/rosa statt fast weiss - Original-Zeile 480, Seite 0, Feld aus suche() (L* 40-56, C* 12-28, Farbton 5-35
# >= 60 %, Klasse des Pruefer-Werkzeugs b_bandfeld.py); nur die beiden mittleren Bauchfelder (j 10 / 0) der Fuge, die aeusseren bleiben Fleisch
# ("nicht als Ring ueber die ganze Breite wie Klebeband"). Fleisch: schraeges Feld (Parallelogramm) der Original-Zeile 482 (suche()), um den Bauch
# zweimal und entlang des Glieds je Feld einmal gependelt (Spiegelung an jeder Kante, keine Naht).
BAND_J = (10, 0)
PENDEL = {9: 0.0, 10: 1.0, 0: 0.0, 1: 1.0, 2: 0.0}   # Bauch-Ecke j -> Lage im Feld quer (0 / 1), Spiegelung je Flaeche
FELD_SATT = (4, 37, 96, 106)          # (u0, u1, v0, v1) Seite 2 = unterseite_v8.json fleisch_r7b "feld" (heutiger Ton)
# Felder "grau" (je Glied), "gedeckt", "band": schwanz_r8.json (Ergebnis von suche() am Eingang 07b6703f, Protokoll runde8/suche.txt)
FARBEN = {'a': dict(fleisch='gedeckt', band=False, ring=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, ring=True, name='gedeckt, weiche rosa Ringe an den Gliedfugen')}
# Runde 8c (Koordinator 14.11., Nutzer waehlt b: "wobei das Original Video der Schwanz noch etwas heller ist und nicht ganz so rot"; Pruefer
# gruenblut: Port-Bauch Saettigung 0,50-0,55 gegen Video 0,38-0,41, Baender "2-4 kleine Flecken seitlich, keine weichen Ringe an den Gliedfugen"):
# - Bauch: ein anderes ORIGINAL-Feld (schwanz_r8.json felder.fleisch mit "zeile"/"seite"), ausgewaehlt nach der Vorhersage im Spiellicht
#   (x12_vorrat.py / Spiellauf), keine Farbverschiebung;
# - Probe b: Band-Klasse auf ALLEN 11 Sektoren der Fugen (Mantel S-C und D-E = KERBE_LAENGE x L um jedes Gelenk, dazu die Deckel), UV aus dem
#   Ring-Feld felder.ring (Original-Zeile 480 = Asset 493, Seite 0): quer gespiegelt |theta| / 180 (Bauch u0 -> Ruecken u1), laengs vom hellen
#   Ende am Gelenk (t 0) zum dunklen Ende am vollen Querschnitt (t 1) = weicher Verlauf aus den Original-Texeln des Felds.
STANDARD = 'b'
FLEISCH_V_AB = 114                    # Runde 8b: schraeges Fleischfeld ab Zeile 114 (n6b: ab 111 Median C* 24,4 / L* 23,7, ab 114 C* 24,2 / L* 25,7 =
                                      # naeher am Ton der Probe a L* 26,8 C* 20,3; die satte Partie v 93-112 ist Probe c, verworfen)
BAND_ROSA_ANTEIL = 0.6                # Runde 8b: Band-Feld mit >= 60 % hellrot/rosa Texeln (Pruefer b_bandfeld.py-Klasse)
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
    if g in ZWISCHEN:
        z1, z2 = ZWISCHEN[g]
        for nm, t in (('Z1', z1), ('M', 0.5), ('Z2', z2)):
            if nm == 'M' and g != 2:
                continue
            th = winkel(grenze(g + t))
            out.append((nm, ring(t * L, rr(t * L), quer(rr(t * L)), None, th, eh, eq), th, np.full(S_, g + t), False))
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
        achse['spitze'] = len(V_); V_.append(np.array([0.0, y_spitze, 0.0]))     # Nadelspitze SPITZE_Y
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
    # Fleisch (Runde 8b): schraeges Feld (Parallelogramm) in der Original-Nutzung der Zeile 482, Seite 2 - Zeilen v0..v1 der hellen Fleischpartie
    # unterhalb der satten (v0 >= FLEISCH_V_AB), Zeilenanfang linear von u_v0 nach u_v1, groesste Flaeche.
    lab = lab_zeile(482)
    nutz = A4.nutzung(None, 482)[:, 256:384]
    Lm = lab[to['pix'][:, 256:384]]
    best = None
    for v0 in range(FLEISCH_V_AB, FLEISCH_V_AB + 4):
        for v1 in range(v0 + 6, 126):
            for a in range(0, 30):
                for b in range(a, 40):
                    for w in range(40, 5, -1):
                        if all(0 <= int(round(a + (b - a) * (v - v0) / (v1 - v0))) and
                               nutz[v, int(round(a + (b - a) * (v - v0) / (v1 - v0))):int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w].all()
                               and int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w <= 128 for v in range(v0, v1 + 1)):
                            if best is None or w * (v1 - v0 + 1) > best[0]:
                                best = (w * (v1 - v0 + 1), v0, v1, a, b, w)
                            break
    _a, v0, v1, a, b, w = best
    # Probe mit dem Rasterer der Riegel (KB.raster: Flaeche + Kantenlinie): das Parallelogramm (u_v0 / u_v0 + w - 1 bei v0, u_v1 / u_v1 + w - 1 bei
    # v1) darf keinen Texel ausserhalb der Original-Nutzung beruehren; sonst Breite verkleinern (Runde 8b: bei Breite 22 lagen 2 Texel der Kante
    # (23,114), (25,115) in der Luecke, riegel_schwanz_r8 R8e (3) 232 fremde Texel)
    nutz_ganz = A4.nutzung(None, 482)
    while w > 4:
        uv = [a, v0, 0, a + w - 1, v0, 130, b, v1, 0, b + w - 1, v1, 0]
        if not (KB.raster(uv, 4, 256) & ~nutz_ganz).any():
            break
        w -= 1
    _a = w * (v1 - v0 + 1)
    sel = np.concatenate([Lm[v, int(round(a + (b - a) * (v - v0) / (v1 - v0))):int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w] for v in range(v0, v1 + 1)])
    erg['fleisch'] = dict(v0=v0, v1=v1, u_v0=a, u_v1=b, breite=w)
    log('Fleisch Zeile 482 Seite 2: schraeges Feld v %d-%d, Zeilenanfang u %d -> %d, Breite %d (%d Texel), Median L* %.1f C* %.1f, L*-Streuung %.1f'
        % (v0, v1, a, b, w, _a, float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2]))), float(np.std(sel[:, 0]))))
    # Band (Runde 8b): Original-Zeile 480 (Asset 493), Seite 0, groesstes Rechteck in der Original-Nutzung mit >= BAND_ROSA_ANTEIL hellrot/rosa Texeln
    lab = lab_zeile(480)
    nz = A4.nutzung(None, 480)[:, 0:128]
    Lm = lab[to['pix'][:, 0:128]]
    C = np.hypot(Lm[..., 1], Lm[..., 2]); hh = np.degrees(np.arctan2(Lm[..., 2], Lm[..., 1]))
    rosa = nz & (Lm[..., 0] >= 40) & (Lm[..., 0] <= 56) & (C >= 12) & (C <= 28) & (hh >= 5) & (hh <= 35)
    bb = None
    for v0 in range(0, 250):
        for v1 in range(v0 + 3, min(v0 + 30, 256)):
            for u0 in range(0, 125):
                for u1 in range(u0 + 3, min(u0 + 16, 128)):
                    if not nz[v0:v1 + 1, u0:u1 + 1].all():
                        break
                    if rosa[v0:v1 + 1, u0:u1 + 1].mean() >= BAND_ROSA_ANTEIL:
                        fl = (u1 - u0 + 1) * (v1 - v0 + 1)
                        if bb is None or fl > bb[0]:
                            bb = (fl, u0, u1, v0, v1)
    _f, u0, u1, v0, v1 = bb
    sel = Lm[v0:v1 + 1, u0:u1 + 1].reshape(-1, 3)
    log('Band Zeile 480 Seite 0: groesstes Rechteck mit >= %.0f %% hellrot/rosa (L* 40-56, C* 12-28, h 5-35) u %d-%d v %d-%d (%d x %d), Anteil %.0f %%, '
        'Median L* %.1f C* %.1f h %.0f' % (100 * BAND_ROSA_ANTEIL, u0, u1, v0, v1, u1 - u0 + 1, v1 - v0 + 1, 100 * rosa[v0:v1 + 1, u0:u1 + 1].mean(),
                                          float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2]))),
                                          float(np.median(np.degrees(np.arctan2(sel[:, 2], sel[:, 1]))))))
    erg['band'] = [int(u0), int(u1), int(v0), int(v1)]
    # Texel des Spiel-Assets in diesen Feldern = Original?
    ff = erg['fleisch']
    gl = {'band': bool(np.array_equal(t['pix'][v0:v1 + 1, u0:u1 + 1], to['pix'][v0:v1 + 1, u0:u1 + 1])),
          'fleisch': bool(np.array_equal(t['pix'][ff['v0']:ff['v1'] + 1, 256:256 + max(ff['u_v0'], ff['u_v1']) + ff['breite']],
                                         to['pix'][ff['v0']:ff['v1'] + 1, 256:256 + max(ff['u_v0'], ff['u_v1']) + ff['breite']])),
          'zeile_493_gleich_original_480': bool(np.array_equal(t['clut'][13] & 0x7FFF, to['clut'][0] & 0x7FFF))}
    log('Asset-Texel in den Feldern bytegleich Original: %s' % gl)
    erg['asset_gleich'] = gl
    return erg


# ------------------------------------------------------------------ UV
def _uv_feld(feld, s_um, s_lang):
    u0, u1, v0, v1 = feld
    return (u0 + s_um * (u1 - u0), v0 + s_lang * (v1 - v0))


BAUCH_J = (9, 10, 0, 1)                # Sektoren (Flaeche j = Ecke j -> j+1) zwischen -theta_b und +theta_b


def klasse_uv(uv, felder=None):
    """Runde 8c: Flaechenklasse einer gebauten Flaeche aus Zeile, Seite und UV -> 'grau' / 'bauch' / 'band' / None (fuer die Riegel: Bauch und
    Ring liegen beide auf Zeile 493, getrennt durch ihre Felder in schwanz_r8.json)."""
    felder = felder or felder_laden()
    z, seite = (uv[2] >> 6) & 0x1FF, uv[5] & 0xF
    us, vs = uv[0::3], uv[1::3]
    if z == ZEILE_GRAU:
        return 'grau'
    fr = felder.get('ring')
    if fr and (z, seite) == (fr['zeile'], fr['seite']) and all(fr['u0'] <= u <= fr['u1'] for u in us) and all(fr['v0'] <= v <= fr['v1'] for v in vs):
        return 'band'
    ff = felder['fleisch']
    if (z, seite) == (int(ff.get('zeile', ZEILE_FLEISCH)), int(ff.get('seite', SEITE_FLEISCH))):
        ulo, uhi = min(ff['u_v0'], ff['u_v1']), max(ff['u_v0'], ff['u_v1']) + ff['breite'] - 1
        if all(ulo <= u <= uhi for u in us) and all(ff['v0'] <= v <= ff['v1'] for v in vs):
            return 'bauch'
    return None


def klasse_von(f, farbe, g):
    """Flaechenklasse 'grau' / 'bauch' / 'band' (Kopf der Datei; Runde 8b: Band nur in den mittleren Bauchfeldern BAND_J)."""
    art, e, (ra, rb), j = f
    if art == 'spitze':
        return 'grau'
    if farbe.get('ring') and farbe['band']:
        if g == 6:
            fuge_r = art == 'deckel_anf' or (art == 'mantel' and 'S' in (ra, rb))
        else:
            fuge_r = art.startswith('deckel') or (art == 'mantel' and bool({ra, rb} & {'S', 'E'}))
        if fuge_r:
            return 'band'                                  # Runde 8c: Ring umlaufend (alle Sektoren j)
    if j not in BAUCH_J:
        return 'grau'
    if g == 6:
        fuge = art == 'deckel_anf' or (art == 'mantel' and 'S' in (ra, rb))
    else:
        fuge = art.startswith('deckel') or (art == 'mantel' and bool({ra, rb} & {'S', 'E'}))
    return 'band' if farbe['band'] and fuge and j in BAND_J else 'bauch'


def fleisch_uv(F_, s_um, v_seite):
    """Schraeges Fleischfeld: v = v0 / v1 (v_seite 0 / 1), u = Anfang der Zeile (linear zwischen u_v0 und u_v1) + s_um x (Breite - 1)."""
    v = F_['v0'] + v_seite * (F_['v1'] - F_['v0'])
    u0 = F_['u_v0'] + (F_['u_v1'] - F_['u_v0']) * (v - F_['v0']) / (F_['v1'] - F_['v0'])
    return (u0 + s_um * (F_['breite'] - 1), v)


def uv_setzen(g, V_, F, ringe, info, farbe, felder, L):
    """-> Liste (Flaeche, UV-Liste, Klasse). Runde 8b: Fleisch und Band gependelt (je Flaeche ein Feld, gespiegelt an jeder Kante)."""
    th_e, u_e, seite_e = {}, {}, {}
    folge = info['folge']
    for nm, P, th, u, _k in ringe:
        p0 = info['pos'][nm]
        for k in range(S_):
            th_e[p0 + k] = float(np.degrees(th[k]))
            u_e[p0 + k] = float(u[k])
            seite_e[p0 + k] = folge.index(nm) % 2          # Pendel entlang des Glieds: Ringe abwechselnd Feldanfang / Feldende
    ring_k = {}
    for nm, _P, _t, _u, _k in ringe:
        for k in range(S_):
            ring_k[info['pos'][nm] + k] = k
    ys = V_[:, 1]
    y_lo, y_hi = float(min(ys)), float(max(ys))
    out = []
    feld_grau = felder['grau'][str(16 + g)]
    feld_f = felder['fleisch']
    feld_band = felder['band']
    tb_glied = lambda i: grenze(u_e.get(i, g + 0.5))
    for f in F:
        art, e, (ra, rb), j = f
        kl = klasse_von(f, farbe, g)
        if g == 6 and kl != 'grau' and all(V_[i][1] >= ENDE_Y - 1.0 for i in e):
            kl = 'grau'                                    # Nadel grau (Video: dunkle Spitze)
        uv_e = []
        rs = [q for q in e if q in th_e]
        for i in e:
            if i in th_e:
                th = th_e[i]
                tb = tb_glied(i)
            else:                                          # Achs-Ecke: Mitte der Ring-Ecken der Flaeche
                th = float(np.degrees(np.angle(np.mean([np.exp(1j * np.radians(th_e[q])) for q in rs]))))
                tb = tb_glied(rs[0])
            th = th % 360.0
            s_lang = float(np.clip((V_[i][1] - y_lo) / max(y_hi - y_lo, 1.0), 0.0, 1.0))
            if kl == 'grau':
                s_um = float(np.clip((th - tb) / max(360.0 - 2 * tb, 1.0), 0.0, 1.0))
                uv_e.append(_uv_feld(feld_grau, s_um, s_lang))
                continue
            if i in ring_k:
                s_um = PENDEL.get(ring_k[i], 0.5)
                vs = seite_e[i]
            else:
                s_um = float(np.mean([PENDEL.get(ring_k[q], 0.5) for q in rs]))
                vs = seite_e[rs[0]]
            if kl == 'bauch':
                # Runde 8b2: EIN Feld ueber den ganzen Bauch (quer linear -theta_b .. +theta_b) und je Glied einmal laengs (gespiegelt je Glied);
                # gependelt je Flaeche ergab im Spiel ein regelmaessiges Rautenmuster (Tafel 21 Probe a/b, Pruefbild) - verworfen
                ts = th if th <= 180.0 else th - 360.0
                sq = float(np.clip((ts + tb) / (2 * tb), 0.0, 1.0))
                sl = s_lang if g % 2 == 0 else 1.0 - s_lang
                uv_e.append(fleisch_uv(feld_f, sq, sl))
            else:
                nm = [n_ for n_ in (ra, rb) if info['pos'][n_] <= i < info['pos'][n_] + S_] if i in ring_k else [ra]
                am_gelenk = 1.0 if (nm and nm[0] in ('S', 'E')) else 0.0
                if farbe.get('ring'):
                    # Runde 8c: Ring - quer gespiegelt |theta| / 180 (Bauch 0, Ruecken 1), laengs t 0 am Gelenk (helles Ende des Felds) -> 1 am vollen
                    # Querschnitt (dunkles Ende). "richt" = Achse und Seite des hellen Endes im Feld: v+ hell bei v0, v- bei v1, u+ bei u0, u- bei u1
                    ts = th if th <= 180.0 else th - 360.0
                    fr = felder['ring']
                    t_ = 1.0 - am_gelenk
                    q_ = abs(ts) / 180.0
                    if fr['richt'][0] == 'v':
                        vv = fr['v1'] - t_ * (fr['v1'] - fr['v0']) if fr['richt'] == 'v-' else fr['v0'] + t_ * (fr['v1'] - fr['v0'])
                        uv_e.append((fr['u0'] + q_ * (fr['u1'] - fr['u0']), vv))
                    else:
                        uu = fr['u1'] - t_ * (fr['u1'] - fr['u0']) if fr['richt'] == 'u-' else fr['u0'] + t_ * (fr['u1'] - fr['u0'])
                        uv_e.append((uu, fr['v0'] + q_ * (fr['v1'] - fr['v0'])))
                else:
                    # Band (Runde 8b): quer gependelt ueber die zwei mittleren Felder, laengs v0 am Gelenkring (S / E), v1 am vollen Querschnitt (C / D)
                    uv_e.append(_uv_feld(feld_band, s_um, 1.0 - am_gelenk))
        out.append((f, [(int(round(u)), int(round(v))) for u, v in uv_e], kl))
    return out


def zeilen_seiten(felder, farbe):
    """Klasse -> (Asset-Zeile, Seite). Runde 8c: Bauch aus felder.fleisch "zeile"/"seite" (Standard 495 / 2), Ring aus felder.ring."""
    ff = felder['fleisch']
    zs = {'grau': (ZEILE_GRAU, 2), 'bauch': (int(ff.get('zeile', ZEILE_FLEISCH)), int(ff.get('seite', SEITE_FLEISCH))), 'band': (ZEILE_BAND, SEITE_BAND)}
    if farbe.get('ring'):
        zs['band'] = (int(felder['ring']['zeile']), int(felder['ring']['seite']))
    return zs


def uv_eintrag(uvl, kl, zs=None):
    zeile, seite = (zs or {'grau': (ZEILE_GRAU, 2), 'bauch': (ZEILE_FLEISCH, SEITE_FLEISCH), 'band': (ZEILE_BAND, SEITE_BAND)})[kl]
    n = len(uvl)
    x = [0] * (9 if n == 3 else 12)
    for k, (u, v) in enumerate(uvl):
        x[3 * k], x[3 * k + 1] = u, v
    x[2] = zeile << 6
    x[5] = 128 + seite
    return x


# ------------------------------------------------------------------ Stufe
# ------------------------------------------------------------------ Bodenfreiheit (Runde 8b, S6 streng)
HEBEN_RAND = 0.0                      # wie Stufe s (kette_v8.json boden_rand 0): kein Punkt UNTER der tiefsten Koerperecke (Riegel S6 "unter" = y > Sohle)


def mesh22_tief(R8, T8, loks, rel, L, P22):
    """Groesstes Welt-y (Modellraum, y nach unten) der Ecken von Mesh 22 bei den Lokaldrehungen loks (Knochen 16-22, wie v8_bau.kette_punkte)."""
    import v8_bau as VB
    pts, W22 = VB.kette_punkte(R8, T8, loks, rel, L)
    return float((pts[6][None, :] + P22 @ W22.T)[:, 1].max())


def heben(blob, prot=None):
    """Runde 8b (Pruefer pr8 (C)/(E)1, Koordinator "S6 wieder STRENG mit Nadel: in 0 von 912 Keyframes unter der Sohle"; Weg "die Schwanz-Keyframes
    anheben ... es ist Form-Anpassung an unserem eigenen Modell"): je Keyframe, in dem eine Ecke von Mesh 22 (Knolle, Nadel) unter der tiefsten
    Koerperecke liegt, die Glieder 17-22 wie die Bodenregel der Stufe s (v8_bau.schwanz) gemeinsam um eine waagerechte Achse hochdrehen (1 Grad je
    Schritt, beide Richtungen, Knochen 16 fest, jedes Gelenk <= 25 Grad), bis Mesh 22 UND die Pruefung der Stufe s (Gelenke + Radius, Dorn 589) frei
    sind. Aendert nur die 12-Bit-Winkel der Knochen 17-22 in diesen Keyframes; alle anderen Keyframes und Knochen bleiben bytegleich. Danach Probe
    mit der echten Posenformel (EA/b2 pose) und, falls die Rundung der Winkel noch eine Ecke unter die Sohle bringt, weiter drehen."""
    import struct as _st
    import v8_bau as VB
    d, _o, ms, _tim = KB.teile(blob)
    mo = V.modell(blob)
    s = mo.skel
    par = _p('kette_v8.json')
    P = np.array(par['gelenke_b8'], float)
    L = int(par['L'])
    rad = [float(r) for r in par['radien']]
    lok_ruhe = VB.ruhe_lokal(P)
    kpv = VB.BB_kf(mo, V.PV)
    wmax = np.radians(VB.WINKEL_MAX_GRAD)
    Rp, _Tp = mo.pose(kpv)
    R8p = Rp[8].astype(float) / 4096.0
    W_, Dpv = R8p @ lok_ruhe[0], []
    for g in range(1, 7):
        W_ = W_ @ lok_ruhe[g]
        Dpv.append(W_ @ Y)
    Dpv = np.array(Dpv)
    k_pv = VB.kurs(R8p)
    dorn_alt = float(L + VB.DORN * rad[7])
    rel = [np.array(s.rel[b], float) for b in SCHWANZ]
    P22 = np.array([v[:3] for v in ms[22]['v']], float)
    emr = bytearray(blob[d[2]:d[3]])

    def frei(R8, T8, loks, boden):
        return (VB.kette_tief(R8, T8, loks, rel, L, rad, dorn_alt) <= boden and mesh22_tief(R8, T8, loks, rel, L, P22) <= boden)

    gehoben, nicht = {}, []
    for k in range(s.kf_count):
        R, T = mo.pose(k)
        R8, T8 = R[8].astype(float) / 4096.0, T[8].astype(float)
        boden = VB.tiefste_koerperecke(mo, R, T) - HEBEN_RAND
        R22 = R[22].astype(float) / 4096.0
        if float((T[22].astype(float)[None, :] + P22 @ R22.T)[:, 1].max()) <= boden:
            continue                                          # mit der echten Pose schon frei: Keyframe unveraendert
        Ry = VB.rot_y(VB.kurs(R8) - k_pv)
        D = Dpv @ Ry.T
        W16 = R8 @ lok_ruhe[0]
        loks0 = [lok_ruhe[0]] + VB.glieder_aus_richtungen(W16, D, wmax)
        pts, _W = VB.kette_punkte(R8, T8, loks0, rel, L)
        h = pts[-1] - pts[1]
        ax = np.cross(np.array([h[0], 0.0, h[2]]), Y)
        ax = ax / max(np.linalg.norm(ax), 1e-9)
        ok = None
        for n in range(1, 121):
            for sg in (1, -1):
                Rh = BF._exp(ax * np.radians(n) * sg)
                l2 = [lok_ruhe[0]] + VB.glieder_aus_richtungen(W16, D @ Rh.T, wmax)
                if frei(R8, T8, l2, boden):
                    ok = (n * sg, l2)
                    break
            if ok:
                break
        if not ok:
            nicht.append(k)
            continue
        for i, b in enumerate(SCHWANZ):
            if b > 16:
                VB.winkel_schreiben(emr, s, k, b, ok[1][i])
        gehoben[k] = ok[0]
    neu = bytes(blob[:d[2]]) + bytes(emr) + bytes(blob[d[3]:])
    # Probe mit der echten Posenformel
    mo2 = V.modell(neu)
    rest = []
    for k in range(s.kf_count):
        R, T = mo2.pose(k)
        boden = VB.tiefste_koerperecke(mo2, R, T)
        R22 = R[22].astype(float) / 4096.0
        if float((T[22].astype(float)[None, :] + P22 @ R22.T)[:, 1].max()) > boden:
            rest.append(k)
    if prot is not None:
        prot['heben'] = dict(keyframes=len(gehoben), grad={str(k): v for k, v in sorted(gehoben.items())}, nicht_frei=nicht, rest_echte_pose=rest,
                             pv_keyframe=kpv, pv_gehoben=kpv in gehoben)
    return neu, gehoben, rest



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
    y_spitze = SPITZE_Y
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
        zs = zeilen_seiten(felder, fa)
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
                m['qf'].append([x for i in e for x in (i, i)]); m['quv'].append(uv_eintrag(uvl, kl, zs))
            else:
                m['tf'].append([x for i in e for x in (i, i)]); m['tuv'].append(uv_eintrag(uvl, kl, zs))
            zahl[kl] = zahl.get(kl, 0) + 1
        ms[b] = m
        info_alle[b] = dict(ecken=len(m['v']), dreiecke=len(m['tf']), vierecke=len(m['qf']), klassen=zahl,
                            ringe=[(nm, round(float(np.abs((P - P.mean(0)) @ achsen(phi[b])[0]).max()), 1),
                                    round(float(np.abs((P - P.mean(0)) @ achsen(phi[b])[1]).max()), 1)) for nm, P, _t, _u, _k in ringe])
    neu = KB.packen(blob, d, ms, tim)
    hp = {}
    neu, gehoben, rest = heben(neu, hp)                    # Runde 8b: Keyframes mit Mesh 22 unter der Sohle anheben (S6 streng)
    assert not rest and not hp['heben']['nicht_frei'], ('Mesh 22 nach dem Anheben noch unter der Sohle', rest, hp['heben']['nicht_frei'])
    if prot is not None:
        prot['heben'] = hp['heben']
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
