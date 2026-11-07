#!/usr/bin/env python3
"""Farbe Versuch 3 der Riesenform K5c: G4-Uebertragung (MESSBLATT_FARBE_V3.md, Bauer Runde 3; Pruefberichte FARBE_PRUEFUNG_v3_r1_*.md, v3_r2_*.md).

Nutzer 2026-11-07: "Ich koennte mir vorstellen, das er so schwarz rot ist in seiner 4 beider Form, weil er verbrannt ist, wenn der generator
explodiert. ... das Gesicht vorne irgendwie in flammenfarben leuchtet. Bei der vorderansicht ja auch vielleicht hat er mit der Mutatione die
feuer Farbe beim Mund nach der explosion angenommen."

Was der Bau tut (Messblatt V3 2.2/2.3/3):
- FORM.EMD = K5c (varianten/K5/K5c/FORM.EMD, sha256 a71f8d08...): Ecken, Normalen, Flaechenindizes, Knochen, Clips unveraendert (Riegel G0).
  Neu sind nur je Flaeche u/v, CLUT-Wort und TPage (Messblatt V3 2.2 "Verfahren (b2)").
- FORM.TIM: Seiten 0-2 = G4-Atlas (RE2 CDEMD0.EMS Typ 0x34, TIM @0x65e800) bytegleich, CLUT-Zeilen 0-2 = G4-CLUT bytegleich.
  Seite 3 traegt nur die Sonderfelder (F1-Stelle, Maul-Band), zusammengesetzt aus G4-Indizes.
- Jede K5c-Flaeche bekommt die UV des G4-Dreiecks, das ihren Schwerpunkt in (theta, T) enthaelt (Zylinder um die Teil-Laengsachse,
  T entlang der Kette). Die affine Abbildung dieses Dreiecks gilt fuer alle Ecken (Pruefer-Verfahren stich.py). Liegt das UV-Dreieck
  nicht ganz in der Insel des Quellfelds, wird es um seinen Schwerpunkt verkleinert, bis es ganz drin liegt (Riegel S1).
- Ausgeschlossene G4-Felder (Messblatt 2.3 "nicht uebertragen", F3 Rippen hell, Augen F16, Fleisch F7/F8/F9) fehlen in der Quelle; die
  Winkelluecke, die sie lassen, wird je Quellmesh zusammengeschoben (theta-Verdichtung), damit keine Flaeche auf eine Lochkante faellt.
- CLUT-Zeilen (Messblatt 5 H2): 0-2 G4 bytegleich; 3-5 "Bild-1-Aderrot" = G4-Zeile 0-2, rote Eintraege im Farbton auf das Bild-1-Rot
  gedreht und in C* auf dessen Buntheit gesetzt, L* unveraendert (Rangfolge haelt); 6 = F1-Stelle, 7 = Maul-Band, 8 = Zaehne.

Runde 3 (Pruefberichte FARBE_PRUEFUNG_v3_r2_*.md): Krallen je K5c-Kralle ein G4-Krallenstreifen (krallen_uv), F1-Fleck nach Mehrheitsregel mit
dunklem Rand, Band auf das Ketten-Ziel ZIEL_BAND_KETTE, Seite-1-Buntheit auf K12 geeicht, Bild-1-Zeilen am Vorderglied r nur auf den in Bild 1
sichtbaren Flaechen (B1_SICHTBAR_NR).

Runde 4 (weniger Rot; Nutzer 2026-11-07 ~17:10, woertlich, bindend: "bist du noch an der Verbesserung von k5c ran, und entfernst die
übertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"; Ziele MESSBLATT_FARBE_V3_NACHTRAG_ROT.md 5, Riegel RV1-RV4):
- Zeilen 0-2 bleiben G4 bytegleich (H2). Jede Koerperflaeche liest eine abgeleitete Zeile ihrer Gruppe (Gruppe x Seite, R4_GRUPPEN).
- Haut (G4-Eintraege ohne Rot-Gewicht): L* x a (a = 1 Bild-1-Gruppen), Farbton der Gruppe, Buntheit C x s(L) x ((1 - g) + g x
  G4-Hautbuntheit / C75), weich an den sRGB-Rand (r4_cmax am ungekappten linearen sRGB).
- Adern (G4-Rotklasse, Rampe ROT_C0..ROT_C1): rangtreu linear auf [Haut-p10, q x Haut-Median] gestaucht, Buntheit kappa x mittlere
  Haut-Buntheit (q, kappa <= 1). Die Feuerrillen sind damit entfernt; die Aderzeichnung bleibt als dunkle Struktur (Riegel RV6).
- Augen lesen G4-Zeile 0 (F10, dunkel), Maulinneres F2 bleibt auf den G4-Zeilen, Sonderzeilen 6-8 (Leuchtstellen) wie Runde 3.
- Baufehler A (Zahlen-Pruefer V3 r3): die Rand-Texel des F1-Flecks werden mit umgefaerbt (kein rohes G4-Blutrot mehr).

Runde 5 (nur die Krallen; Pruefer Runde 4: Vorderkrallen gesprenkelt, A7 / P2 Klauen ROT): die Krallenflaechen (R5_KRALLEN) lesen wieder ihre
G4-Zeile wie Runde 3 (dunkelrote Klaue, khaki Spitze); alles andere bytegleich Runde 4 (Riegel AE1 in riegel_farbe_v3.py).

Aufruf: python kf_g4.py [--aus <Ordner>]   (Default: farbe/K5c_farbe_v3; dazu farbe/K5C_FARBE_V3.EMD/.TIM und Scratchpad formfarbe3/emd)
"""
import argparse
import copy
import hashlib
import json
import math
import os
import re
import shutil
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.normpath(os.path.join(HIER, '..'))
WT = os.path.normpath(os.path.join(K5, '..', '..', '..', '..', '..'))
TOOLS = os.path.join(WT, 're15_port', 'tools')
sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA  # noqa: E402
import form_emd as FE  # noqa: E402

DATEN = os.path.join(HIER, 'v3_daten')
SCRATCH = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4'   # Runde 4 (vorher formfarbe3)
K5C_EMD_SHA = 'a71f8d08e8d45b2f00346096dd8fbc7fa3a7dfc078c6e6bc9c80013750edbad2'   # Auftrag / Messblatt V3 Kopf

# ---------------------------------------------------------------- Konstanten (jede mit Quelle)
G4_KIND = 0x34                   # RE2 CDEMD0.EMS Typ 0x34 = G4 (Messblatt V3 1.1; FARBE_V3_LESER_A.md 1: EMD @0x677000, TIM @0x65e800)
CLUT_Y = 480                     # G4-TIM und K5c-TIM: CLUT bei y 480, x 0 (gemessen: FE.tim_lesen beider TIM, 2026-11-07)
Z_G4 = (0, 1, 2)                 # G4-Seite p nutzt CLUT-Zeile p (Leser A 1: 1427/1427 Flaechen; gemessen 0x7800/0x7840/0x7880 = Seite 0/1/2)
Z_B1 = (3, 4, 5)                 # Bild-1-Aderrot-Zeilen zu Seite 0/1/2 (Messblatt V3 5 H2 "eigene Zeilen fuer ... Bild-1-Aderrot")
Z_F1, Z_BAND, Z_ZAHN = 6, 7, 8   # Sonderfeld-Zeilen (Messblatt V3 5 H2)

KAPPE_NA = 0.8                   # Quelldreiecke mit |N.a| > 0,8 sind Kappen und fehlen in der Quelle (Leser A uebertrag.py koord, Messblatt 2.2)
# Runde 2: VERSCHIEBE_N und SCHRUMPF entfallen (einpassen sucht per Erosion die naechste passende Lage, Bisektion statt Stufen).

# Bild-1-Aderrot (Messblatt V3 4, Tabelle "Koerper schwarz-rot"): Bild 1 Figur Rot Lab (39,1 / 21,7 / -1,2), G4-Atlas Rot Lab (38,0 / 35,5 / 20,8).
B1_ROT_LAB = (39.1, 21.7, -1.2)
G4_ROT_LAB = (38.0, 35.5, 20.8)
B1_TON = math.degrees(math.atan2(B1_ROT_LAB[2], B1_ROT_LAB[1]))           # -3,2 Grad
G4_TON = math.degrees(math.atan2(G4_ROT_LAB[2], G4_ROT_LAB[1]))           # +30,4 Grad
B1_C_FAKTOR = math.hypot(*B1_ROT_LAB[1:]) / math.hypot(*G4_ROT_LAB[1:])   # 21,7 / 41,1 = 0,53 (Versuch 3 Lauf 1, verworfen, s. u.)
# Buntheit je Seite (Quelle: Messblatt V3 2.3 Schwanz "Auflage K12 ... Erlaubt sind ... Adern-CLUT nach H2" und Messblatt V3 4 "1 Schwanz" Rot-Lab;
# Seiten 0/2 = G4-Buntheit unveraendert, nur der Farbton gedreht. Belegt durch die Laeufe der Runde 1, Dossier FARBE.md "Versuch 3" V3.1 Punkt 4):
#  Lauf 1 (alle Seiten x 0,53): K12 gruen, aber K1v2 Bild-1-Chroma 5,7 < 6,6, K2v2 5,7 < 6,8, K4b Ruecken-Rot 5,2 % < 7,1 % -> ROT.
#  Lauf 2 (alle Seiten x 1,0): K1v2/K2v2/K4b gruen, aber K12 Schwanz 17,1 % > 8,9 % -> ROT.
#  Seite 1 traegt Schwanz (F17/F19) und Vorderglied r: dort die Schwanz-Buntheit aus Messblatt V3 4 "1 Schwanz" Rot Lab (39,7 / 23,6 / -0,4):
#  C* 23,6 / 41,1 = 0,574. Seiten 0 und 2 (Becken, Lende, Hinterbeine) behalten die G4-Buntheit, nur der Farbton wird gedreht.
# Runde 3 (Zahlen-Pruefer V3 r2 Befund C: K12 Schwanz 8,7 / 8,9 / 9,0 % mit Saat 15 / 12345 / 7, Grenze 8,9, ohne Reserve): Seite 1 auf das
# K12-Original geeicht (Messblatt V3 5 K12 "Original Schwanz 5,9 %"; Messblatt 2.3 Schwanz "Adern-CLUT nach H2" erlaubt). Laeufe (r3/k12_saaten.py,
# Schwanz % je Saat 15/12345/7): 0,35 -> 1,3/1,4/1,3; 0,45 -> 2,8/3,0/3,0; 0,515 -> 5,3/5,7/5,3; 0,535 -> 6,1/6,5/6,2; 0,574 -> 8,7/9,0/9,2.
# Gewaehlt 0,535: Mittel 6,3 % am naechsten an 5,9 % (0,515: 5,4 %). Runde 1/2: 0,574 = C* 23,6 / 41,1 (Messblatt V3 4 "1 Schwanz").
B1_C_SEITE1_EICH = 0.535
B1_C_JE_SEITE = (1.0, B1_C_SEITE1_EICH, 1.0)
# Rot-Gewicht eines CLUT-Eintrags fuer die Bild-1-Zeilen, als Rampe zwischen den beiden Rotklassen der Messblaetter (Runde 2, keine eigene Wahl):
#  ROT_C0 = 15: Untergrenze der Ketten-Rotklasse K11 (MESSBLATT_FARBE_V3 5 K11 "a* >= 12, C* >= 15, L* >= 20"); darunter zaehlt der Eintrag
#               in der Bild-1-Kette nicht als Rot und bleibt G4.
#  ROT_C1 = 20: Grenze der Atlas-Rotklasse A3 (MESSBLATT_FARBE_V1 6 A3 / Leser A 1 "a* > 15, C* > 20, Farbton -40..70 Grad"); ab hier voll gedreht.
#  ROT_H0/H1 = -40/70 Grad: Farbtonfenster derselben A3-Klasse.
ROT_C0, ROT_C1, ROT_H0, ROT_H1 = 15.0, 20.0, -40.0, 70.0

# Zielfarben der Sonderfelder (Messblatt V3 3.1, 3.2, 5 F1t/F2t; Vorwaerts-Gegenprobe gegenprobe.json)
ZIEL_F1 = (149, 103, 122)        # Lab (48,9 / 21,4 / -3,1), -8,3 Grad
ZIEL_BAND = (184, 90, 85)        # Lab (49,6 / 37,5 / 20,8), 29,0 Grad
# Runde 2: ZIEL_BAND_BAU (183,99,92) entfaellt (Zahlen-Pruefer (5): PORT-WAHL ohne Nutzer-Wortlaut). Das Band wird auf ZIEL_BAND gebaut.
# Runde 3: Bauziel des Bands aus der Bild-2-Kette (Sicht-Pruefer V3 r2: "Fuer das Auge zaehlt K15, nicht F2t ... In der Kette muesste das Band
# weniger rot sein"; Zahlen-Pruefer V3 r2 Befund A: auch ein einfarbiges Band auf ZIEL_BAND ergibt K15 dE 12,7). Gemessen an den Bandpixeln auf
# Knochen 23 (1282 px, Scratchpad formfarbe3/r3/band_knochen.py + kette_band.npz, Variante r3/v3): Die Kette gibt das Texel fast unveraendert
# wieder (Ketten-T / Texel = 1,017 / 0,963 / 1,030 je Kanal); der Rundlauf des Originals liegt dort bei T (185,5 / 112,7 / 105,4). Bauziel =
# Rundlauf / Faktor = (182, 117, 102), Lab (55,7 / 24,2 / 18,8). Abstand zu ZIEL_BAND (Riegel F2t) dE 14,8: F2t wird dadurch ROT; der
# Widerspruch Messblatt 3.2 <-> Kette liegt beim Messblatt-Pruefer (Dossier V3R3.3).
ZIEL_BAND_KETTE = (182, 117, 102)
ZIEL_ZAHN = (199, 191, 183)      # Leser B (Lab 77,7 / 2,1 / 4,8), Messblatt 3.2 "Zaehne: L* >= 70, C* <= 12"

# K5c-Flaechen der Sonderfelder (Messblatt V3 1.2 / Leser B 1.3; Zaehlung je Mesh: tf, danach qf)
F1_FLAECHEN = {0: [('q', 7), ('q', 8), ('q', 19), ('q', 20), ('q', 31), ('q', 32)]}
K5C_ZAHNRECHT = (384, 180, 24, 14)   # K5c-Zahnfeld Seite 3 (Messblatt V3 2.3 "Zaehne (Seite 3, 384,180,24,14)")
K5C_AUGENRECHT = (384, 196, 24, 10)  # K5c-Augenfeld (p_riegel.atlas_zonen 'auge', k_form.py:11)

# G4-Felder (Leser A g4_felder.json / felder.py: Mesh -> Inseln)
FELD = {
    'F1': {26: [0, 102]}, 'F2': {26: [5]}, 'F3': {10: [200]}, 'F4': {10: [41]}, 'F5': {10: [90]}, 'F6': {10: [223]}, 'F7': {10: [84]},
    'F8': {10: [229]}, 'F9': {10: [130, 36]}, 'F10': {9: [0, 5, 9]}, 'F11': {0: [0, 3, 5, 20]}, 'F12': {1: [2], 5: [9, 10, 13]},
    'F13': {2: [0, 22]}, 'F14': {3: [1, 8, 10, 12]}, 'F15': {4: [2, 3, 7, 10, 15], 8: [3, 7, 10, 15]},
    'F16': {5: [0], 10: [47, 58], 16: [132]}, 'F17': {11: [7], 15: [0]}, 'F18': {16: [1, 12, 19, 72]},
    'F19': {12: [2, 14, 15], 17: [0, 14, 15]}, 'F20': {13: [14, 16]}, 'F21': {14: [7, 10, 15, 16], 19: [2, 7, 10, 15], 13: [1]},
}

# Teile-Zuordnung (Messblatt V3 2.3, verbindlich). (Name, K5c-Meshes, G4-Meshes, Art, Optionen)
#  art 'kette': durchgehendes T ueber die Meshes (E9), je Mesh eigener Rahmen; 'gruppe': ein Rahmen fuer alle (Kopf); 'einzeln': je Ziel-Mesh dieselbe Quelle.
#  nur = erlaubte G4-Felder (sonst alle Inseln der Quellmeshes ausser 'ohne'); zeilen = 'g4' oder 'b1' (Bild-1-Aderrot, Messblatt V3 4 "Bild-1-sichtbare Teile").
OHNE_BRUST = ['F3', 'F7', 'F8', 'F9', 'F16']      # Messblatt 2.3 Rumpf: "ohne F3 (Rippen, hell), F7, F8, F9, F16"
ZUORDNUNG = [
    ('Rumpf', [0, 7], [0, 9], 'kette', dict(achse=(1, 0, 0), ohne=OHNE_BRUST, zeilen='b1')),
    # Runde 2, Abweichung von Messblatt V3 2.3 (Brust 10 <- G4 10 F4/F5/F6) nach Messung der Bild-2-Kette (Messblatt V3 4 "Massstab fuer die Haut
    # ist die Kette"): mit F4/F5/F6 war die Brust (Knochen 10) in Bild 2 zu 76,7 % Rotklasse und 19,1 % dunkel, die Zone Kopf/Rumpf 38,5 % rot /
    # 58,6 % dunkel gegen das Original 11,0 % / 73,1 % (Messblatt V3 4). Sicht-Pruefer V3 r1: "lachsfarbene" Schultern. Brust und Hals bekommen
    # darum dasselbe dunkle Hautfeld wie der Kopf (F18, Messblatt 2.3 Kopf "dunkle Hautfelder F18"), Kopf-Hals-Brust aus einem Guss.
    ('Brust', [10], [16], 'kette', dict(achse=(1, 0, 0), nur=['F18'], ohne_inseln={16: [72]}, zeilen='g4')),
    ('Schwanz', [1, 2, 3, 4, 5, 6], [11, 12], 'kette', dict(zeilen='b1')),
    ('Hals', [21], [16], 'kette', dict(achse=(1, 0, 0), nur=['F18'], ohne_inseln={16: [72]}, zeilen='g4')),
    ('Hinterbein l (W) Oberschenkel', [8], [5], 'kette', dict(ohne=['F16'], ohne_inseln={5: [13]}, zeilen='b1')),
    ('Hinterbein l (W) Unterschenkel', [24], [6], 'kette', dict(zeilen='b1')),
    ('Hinterbein l (W) Fuss', [25], [7], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Oberschenkel', [9], [1], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Unterschenkel', [26], [2], 'kette', dict(zeilen='b1')),
    ('Hinterbein r (W) Fuss', [27], [3], 'kette', dict(zeilen='b1')),
    ('Vorderbein l (W) Oberarm', [11], [11], 'kette', dict(zeilen='g4')),
    ('Vorderbein l (W) Unterarm', [12], [12], 'kette', dict(zeilen='g4')),
    ('Vorderbein l (W) Hand', [13], [13], 'kette', dict(nur=['F20'], zeilen='g4', laengs_einmal=True)),
    # Runde 3 (Sicht-Pruefer V3 r2: Vorderklaue in Bild 2 'Mosaik aus grossen beige-weiss-braunen Dreiecken'): je K5c-Kralle EIN G4-Krallenstreifen,
    # durchgehend (krallen_uv) statt Zylinderprojektion im Handrahmen (die Flaechen einer Kralle fielen dort auf verschiedene G4-Krallen und Inseln).
    ('Klauen l (W)', [14, 15, 28, 30], [14], 'kralle', dict(zeilen='g4')),
    # Runde 2: ohne das rosa Wundoval G4 16 Insel 72 (Sicht-Pruefer V3 r1: 'lachsfarbenes Oval' an der Schulter; beim Kopf schon ausgeschlossen).
    ('Vorderbein r (W) Oberarm', [16], [15, 16], 'gruppe16', dict(ohne=['F16'], ohne_inseln={16: [72]}, zeilen='b1')),
    ('Vorderbein r (W) Unterarm', [17], [17], 'kette', dict(zeilen='b1')),
    ('Vorderbein r (W) Hand', [18], [18], 'kette', dict(nur=['F20'], zeilen='b1', laengs_einmal=True)),
    ('Klauen r (W)', [19, 20, 29, 31], [19], 'kralle', dict(zeilen='g4')),
    # Kopf aussen aus F18 (Messblatt V3 2.3 "dunkle Hautfelder F18 (Oberarm r: schwarz mit Laengsadern) oder F4/F5"). Gewaehlt nach Messung
    # (Dossier FARBE.md Versuch 3 Runde 1, V3.1 Punkt 6): mit F4/F5/F6 K13 Bild-2-Dunkel 31,2 % < 46,4 %, K6 dE 9,1; mit F18 K13 46,2 %, K6 dE 4,7
    # (farbe/v3r1/riegel_K5c_farbe_v3r1.txt).
    # Ohne das rosa Wundoval (G4 16 Insel 72, L* 21,4, 38 % rot, gemessen t7.py): es laege sonst am rechten Auge (K10 L* 35,1 > 30).
    ('Kopf + Kiefer aussen', [22, 23], [16], 'kopf', dict(achse=(1, 0, 0), nur=['F18'], ohne_inseln={16: [72]}, zeilen='g4')),
]
# Runde 3: Bild-1-Zeilen an Oberarm r 16 und Unterarm r 17 nur auf den Flaechen, die Bild 1 zeigt (Sicht-Pruefer V3 r2: "2-3 waagerechte rosa Strichel an
# Flaechengrenzen" an Schulter/Brust, "Hand A ... mauve-lila ... Ursache sind die Bild-1-Zeilen (Magenta) am Vorderglied"). Gemessen
# (Scratchpad formfarbe3/r3/strichel.py): rosa-magenta Texel (Rotklasse, Farbton < 10 Grad) gibt es nur auf Mesh 16 in den Bild-1-Zeilen,
# 7,1 % im Inselinneren und 3,8 % am Inselrand; es sind also die umgefaerbten G4-Adern, kein Randtexel einer Sonderzeile.
# Flaechen-Nr. (tf, danach qf) mit mindestens 1 px in Bild 1: Leser B fid1.npz (flaechen_id.py, K5c-Geometrie = G0), Knochen 16: 2521 px auf 39,
# 17: 1667 px auf 37 Flaechen. Die uebrigen Flaechen dieser Meshes nehmen die G4-Zeile (Messblatt V3 4: das Bild-1-Aderrot
# dient Bild 1; H2: G4-CLUT bytegleich).
B1_SICHTBAR_NR = {
    16: (1, 3, 5, 7, 16, 17, 18, 19, 20, 21, 22, 66, 67, 68, 74, 75, 76, 81, 82, 83, 84, 90, 91, 92, 93, 98, 99, 105, 106, 107, 108, 109, 114, 115,
         116, 130, 131, 132, 139),
    17: (18, 27, 28, 36, 37, 42, 43, 44, 45, 50, 51, 52, 53, 58, 59, 60, 61, 66, 67, 68, 69, 74, 75, 76, 77, 82, 84, 85, 93, 98, 106, 107, 108, 114,
         115, 116, 117),
}
# Hand 18 (Bild 1: 1213 px auf 31 Flaechen) behaelt die Bild-1-Zeilen auf allen Flaechen: Mit der Teilung lag die Hand in Bild 2 auf der Bandmaske
# (192 px Knochen 18) rot statt mauve, K15 dE 5,2 -> 6,0 ROT und K6 6,7 -> 7,5 (Lauf farbe/riegel vor dem Endstand, Dossier V3R3.1).
TEILE_H1 = {   # Messblatt 5 H1 "je Teil (Rumpf, Schwanz, Hals, Beine, Arme, Klauen, Kopf/Kiefer aussen)"; Rumpf = Huefte, Wirbel, Brust
    'Rumpf': [0, 7, 10], 'Schwanz': [1, 2, 3, 4, 5, 6], 'Hals': [21], 'Beine': [8, 24, 25, 9, 26, 27],
    'Arme': [11, 12, 13, 16, 17, 18], 'Klauen': [14, 15, 28, 30, 19, 20, 29, 31], 'Kopf/Kiefer aussen': [22, 23],
}


# ================================================================ Runde 4: weniger Rot (Versuch 3 Runde 4)
R4_NUTZER = ('bist du noch an der Verbesserung von k5c ran, und entfernst die übertriebenen roten feuerrillen wieder, und stellst es wieder '
             'nach vorlage um?')     # Nutzer 2026-11-07 ~17:10, woertlich, bindend
# Gruppen: je Gruppe und Seite eine abgeleitete CLUT-Zeile. Begruendung je Teilung (Bauer-Lauf farbe_lesung/V3R4_bau/, Dossier V3R4.2):
#  - Rumpf Ruecken / Rumpf Flanke: mit EINER Rumpfzeile lag das Bild-1-Rot ganz auf dem Ruecken (Rumpf-Pixel 46 % / 35 % rot) und 0 % auf der
#    Flanke (such4.diag); die Flanke liegt im Bild-1-Licht tiefer und braucht mehr Buntheit.
#  - Brust (Mesh 10 Ruecken + Flanke) / Hals Bauch (Hals + Mesh 10 Brust/Bauch) / Kopf: Bild-2-Farbton-Fenster (Nachtrag 5) Ruecken -12,9..2,3,
#    Brust/Bauch -0,1..15,9, Hals 1,7..20,3, Kopf 13,1..24,7. Mit Kopf + Hals + Bauch in einer Zeile blieb nur ein Fenster von rund 1 Grad
#    (scan: Gruppenfarbton -24 / -18 / -12 -> Kopf 12,7 / 18,4 / 22,0, Brust/Bauch 14,2 / 18,8 / 22,3, Hals 19,1 / 22,1 / 24,5 Grad): Der Kopf
#    erscheint in der Kette weniger orange als Hals und Bauch, in der Vorlage ist es umgekehrt. Mit Ruecken + Bauch in einer Zeile kippte
#    Bild 2 zwischen "Ruecken zu wenig Rot" und "Bauch-Rotflaeche zu gross" (3 Saaten: C 39 -> Ruecken GRUEN, Bauch-Flaeche 19,6 > 17,81).
#  - Vorderbein r B1 (Flaechen mit Runde-3-Bild-1-Zeile) / Vorderbeine (G4-Zeile): Bild 1 Vorderglied Rot <= 7,1 %, Bild 2 Vorderbeine 56,4-74,5 %
#    bei 34,5..38,8 Grad (Nachtrag 5).
#  Zeilenzahl 17 (0-2 G4, 6-8 Sonder, 11 Gruppenzeilen) = 4352 Texel hohe Port-Textur (render_pc.c: je Zeile eine Kopie); Windows-Port
#  Direct3D 11 (Grenze 16384). Offen fuer ein Spiel-Asset auf GLES2-Geraeten mit 4096 (Dossier V3R4.5).
R4_GRUPPEN = ('Schwanz', 'Hinterbeine', 'Rumpf Ruecken', 'Rumpf Flanke', 'Vorderbein r B1', 'Brust', 'Hals Bauch', 'Kopf', 'Vorderbeine')
R4_ZEILEN_FREI = (3, 4, 5) + tuple(range(9, 30))   # 3-5 (Runde-3-Bild-1-Zeilen entfallen), 6-8 Sonderzeilen bleiben, danach 9 ff.
R4_N_GRENZE = 0.5          # Region je Flaeche wie MESSBLATT_FARBE_V3_NACHTRAG_ROT.md 1.2 (basis.N_GRENZE: Ruecken ny < -0,5, Bauch ny > 0,5)
# Helle Nicht-Ader-Eintraege behalten ihre G4-Farbe (Horn, Krallenspitzen; Sicht-Pruefer V3 r3: "khakifarbene Spitze ... wie die G4-Krallen").
# Grenze = Mitte der Luecke in der L*-Verteilung der genutzten Nicht-Ader-Eintraege: kein Eintrag zwischen L* 45,3 und 59,5 (gemessen,
# Dossier V3R4.2); darueber nur Farbton 71-118 Grad (khaki-gelb).
R4_L_HELL = 52.4
# Parameter je Gruppe in v3_daten/r4_par.json (eine Quelle fuer Bau und Riegel; Herkunft je Wert dort unter "quelle"):
#  h = Farbton der Gruppe (Grad, Texel), C = Haut-Buntheit, beta = Buntheit dunkler Haut (s(L) = 1 + beta (Lm - L) / Lm,
#  begrenzt auf R4_SMIN..R4_SMAX), g = Anteil G4-Hautmuster, a = Hauthelligkeit, q = obere Aderhelligkeit (x Hautmedian),
#  kappa = Ader-Buntheit (x mittlere Haut-Buntheit). Lauf 2, Dossier V3R4.
R4_PAR = None
R4_PAR_DATEI = os.path.join(HIER, 'v3_daten', 'r4_par.json')
# s(L) begrenzt auf 0..3: gemessen ohne Einfluss auf das Ergebnis (scan smax 3 / 5, beta 5: Hals dL -1,7 / -1,7, Kopf dL 0,2 / 0,3).
R4_SMAX = 3.0
R4_SMIN = 0.0   # Runde 4 Lauf 2: Untergrenze von s(L) (Suchgrenze, Dossier V3R4)

# ================================================================ Runde 5: nur die Krallen (Versuch 3 Runde 5)
# Pruefer Runde 4 (FARBE_PRUEFUNG_v3_r4_sicht.md "Krallen", _zahlen.md A7 0,349 / P2 Klauen ROT): Runde 4 legte die Krallenflaechen auf die
# Gruppenzeile Vorderbeine (Zeile 16): dunkle Wurzelzone gestaucht, helle Spitzen (L* >= R4_L_HELL) G4 -> schwarze Klaue mit khaki Tupfen
# (linke Klaue 1099 px / 83 Komponenten gegen Runde 3 426 / 53). Runde 3 (bestanden, Sicht V3 r3: dunkelrote Klaue, khaki Spitze, durchgehender
# Verlauf) las die Krallen mit G4-Zeile 1 (farbe/K5c_farbe_v3r3/bau.json flaechen_herkunft: alle 252 Krallenflaechen Zeile 1). Zeile 1 ist in
# Runde 4 weiter bytegleich G4 (Riegel H2; Diff r3/r4 gemessen: Zeilen 0-2 gleich, Texel 0 Unterschiede, UV/TPage 0 Unterschiede). Darum lesen
# die Krallen wieder G4-Zeile Z_G4[seite] = Runde-3-Klauenfarbe; eine eigene Kopie dieser Zeile waere bytegleich und machte die Port-Textur
# 256 Texel hoeher (Dossier V3R4.5 Punkt 3). Die Statistik der Gruppenzeile Vorderbeine zaehlt die Kralltexel weiter mit (zaehl vor dem
# Umhaengen), damit Zeile 16 und alle uebrigen Bytes gleich Runde 4 bleiben (Riegel AE1 Aenderungsumfang).
R5_KRALLEN = ('Klauen l (W)', 'Klauen r (W)')     # Teilnamen aus ZUORDNUNG (art 'kralle'), Meshes 14/15/28/30 und 19/20/29/31


def r4_par():
    global R4_PAR
    if R4_PAR is None:
        R4_PAR = json.load(open(R4_PAR_DATEI, encoding='utf-8'))['par']
    return R4_PAR


def r4_flaechen_normalen(mo):
    """Je Mesh: lokale Normale je Flaeche (tf, dann qf), Mittel der Eckennormalen (wie V3R4_skripte/basis.flaechen_normalen)."""
    out = []
    for m in mo.meshes:
        nn = []
        for norms, faces in ((m.tn, m.tf), (m.qn, m.qf)):
            if not faces:
                continue
            N = np.array([x[:3] for x in norms], float) / 4096.0
            for g in faces:
                v = N[list(g[0::2])].mean(0)
                nn.append(v / max(np.linalg.norm(v), 1e-9))
        out.append(np.array(nn) if nn else np.zeros((0, 3)))
    return out


def r4_region(b, nx, ny):
    """K5c-Region einer Flaeche (wie V3R4_skripte/basis.region_k5c; Nachtrag 1.2)."""
    if b in (0, 7, 10):
        if ny < -R4_N_GRENZE:
            return 'Ruecken'
        if ny > R4_N_GRENZE or (b == 10 and nx > R4_N_GRENZE):
            return 'Brust/Bauch'
        return 'Flanke'
    if 1 <= b <= 6:
        return 'Schwanz'
    if b in (8, 9, 24, 25, 26, 27):
        return 'Hinterbeine'
    if 11 <= b <= 20 or 28 <= b <= 31:
        return 'Vorderbeine'
    if b == 21:
        return 'Hals'
    if b in (22, 23):
        return 'Kopf'
    return None


def r4_gruppe(key, familie, k5, fn):
    """Gruppe einer Koerperflaeche. familie = 'b1' / 'g4' (Zeilenfamilie der Runde 3: b1 = Flaeche mit Bild-1-Aderrot)."""
    mi, teil, fi = key
    nr = fi if teil == 't' else len(k5.meshes[mi].tf) + fi
    r = r4_region(mi, fn[mi][nr][0], fn[mi][nr][1])
    if r in ('Schwanz', 'Hinterbeine', 'Kopf'):
        return r
    if r == 'Vorderbeine':
        return 'Vorderbein r B1' if familie == 'b1' else 'Vorderbeine'
    if r == 'Hals':
        return 'Hals Bauch'
    if mi in (0, 7):
        return 'Rumpf Ruecken' if r == 'Ruecken' else 'Rumpf Flanke'
    if mi == 10:
        return 'Hals Bauch' if r == 'Brust/Bauch' else 'Brust'
    raise SystemExit('Runde 4: Flaeche %s ohne Gruppe (Region %s)' % (key, r))


def r4_eintraege(zeile):
    """Lab, C*, Farbton und Rot-Gewicht w je Eintrag (w wie zeile_b1: Rampe ROT_C0..ROT_C1 im Farbtonfenster ROT_H0..ROT_H1 = Ader)."""
    L = lab(wort_rgb(zeile))
    C = np.hypot(L[:, 1], L[:, 2])
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - ROT_C0) / (ROT_C1 - ROT_C0), 0, 1) * ((h > ROT_H0) & (h < ROT_H1))
    return L, C, h, w


def r4_statistik(zeile, cnt):
    """Je Gruppe und Seite (texelgewichtet, cnt = Texel je Index): Hautmedian L* (Haut = w 0) und C75 der Haut-Buntheit (Haut unter R4_L_HELL)."""
    L, C, h, w = r4_eintraege(zeile)
    cnt = np.asarray(cnt, float)
    haut = (w == 0) & (cnt > 0) & (np.array(zeile) != 0)
    haut_d = haut & (L[:, 0] < R4_L_HELL)

    def wq(x, sel, q):
        o = np.argsort(x[sel])
        cs = np.cumsum(cnt[sel][o])
        return float(x[sel][o][np.searchsorted(cs, q * cs[-1])])
    return dict(cnt=cnt, Lm=wq(L[:, 0], haut, 0.5), C75=wq(C, haut_d, 0.75))


def _lab_lin(Lab):
    """Lab (D65, wie OpenCV float) -> lineares sRGB ohne Kappung (fuer den Gamut-Test; OpenCV kappt negative Kanaele still)."""
    Lab = np.asarray(Lab, np.float64).reshape(-1, 3)
    fy = (Lab[:, 0] + 16.0) / 116.0
    fx = fy + Lab[:, 1] / 500.0
    fz = fy - Lab[:, 2] / 200.0

    def finv(t):
        return np.where(t > 6.0 / 29.0, t ** 3, 3 * (6.0 / 29.0) ** 2 * (t - 4.0 / 29.0))
    X, Y, Z = 0.950456 * finv(fx), finv(fy), 1.088754 * finv(fz)
    M = np.array([[3.240479, -1.53715, -0.498535], [-0.969256, 1.875991, 0.041556], [0.055648, -0.204043, 1.057311]])
    return np.stack([X, Y, Z], 1) @ M.T


def r4_cmax(L, h_grad):
    """Groesste Buntheit im sRGB-Raum bei L* und Farbton (Bisektion am ungekappten linearen sRGB)."""
    L = np.asarray(L, np.float64).reshape(-1)
    hr = math.radians(h_grad)
    lo, hi = np.zeros(len(L)), np.full(len(L), 160.0)
    for _ in range(30):
        m = 0.5 * (lo + hi)
        x = _lab_lin(np.stack([L, m * math.cos(hr), m * math.sin(hr)], 1))
        ok = (x.min(1) >= -1e-6) & (x.max(1) <= 1 + 1e-6)
        lo, hi = np.where(ok, m, lo), np.where(ok, hi, m)
    return lo


def r4_zeile(g4zeile, par, st):
    """Abgeleitete Zeile einer Gruppe aus der G4-Zeile ihrer Seite (keine Neuquantisierung: je Eintrag eine neue Farbe, Index bleibt).
    Runde 4 Lauf 2 (Bauer, nach dem Riegel-Lauf am Lauf-1-Stand, Dossier V3R4):
    - Haut (w = 0): L* x a (a = 1 Bild-1-Gruppen), Farbton par h, Wunsch-Buntheit C x s(L) x ((1 - g) + g x C*(G4) / C75) mit
      s(L) = clip(1 + beta (Lm - L) / Lm, R4_SMIN, R4_SMAX) (beta > 0: Rot sitzt im Dunkeln, Vorlage Bild 2 dL -2,5..-7,4).
    - Ader (w > 0): rangtreu. Die genutzten Ader-L* (G4) werden linear auf [Haut-L*-p10, q x Haut-L*-Median] (genutzte Haut der Gruppe,
      texelgewichtet) gestaucht; Buntheit = kappa x mittlere Wunsch-Buntheit der Haut der Gruppe.
      Nutzer 2026-11-07: Adern "nie heller und nie roter als die umgebende Haut" (Auftrag V3 R4) -> q <= 1, kappa <= 1.
    - Buntheit weich an den sRGB-Rand: C = Cmax(L*, h) x tanh(Wunsch / Cmax) (keine Kappung, die viele Eintraege auf eine Farbe legt).
    Lauf 1 (Deckel min(L*, q x Hautmedian), min(C, kappa x C_haut), beta bis 5, Gamut ueber rgb_aus_lab) legte Adern und dunkle Haut auf
    wenige Farben (P1 dL0 61,5 % bei <= 30; je Gruppenzeile 1-10 Fleischfarben) und hob dunkle Eintraege ueber rgb_aus_lab heller
    (OpenCV kappt negative Kanaele still: L* 12 / C 60 -> RGB (95,0,0) mit L* 17,5). Darum hier r4_cmax am ungekappten linearen sRGB.
    Helle Nicht-Ader-Eintraege (L* >= R4_L_HELL) und 0x0000 bleiben."""
    L, C, h, w = r4_eintraege(g4zeile)
    Lm = st['Lm']
    q, kap = par['q'], par['kappa']
    cnt = np.asarray(st['cnt'], float)
    z0 = np.array(g4zeile) != 0
    behalten = (w == 0) & (L[:, 0] >= R4_L_HELL)
    haut = (w == 0) & (cnt > 0) & ~behalten & z0

    def s(Lx):
        return np.clip(1 + par['beta'] * (Lm - Lx) / Lm, R4_SMIN, R4_SMAX)

    def wquant(x, wt, pp):
        o = np.argsort(x)
        cs = np.cumsum(wt[o])
        return np.interp(pp, (cs - 0.5 * wt[o]) / cs[-1], x[o])
    Ls = L[:, 0] * par.get('a', 1.0)        # Haut-Helligkeit: a = 1 Bild-1-Gruppen (dunkler Grundton), a >= 1 Bild-2-Gruppen (Haut nach Vorlage)
    g = par.get('g', 1.0)                   # Anteil G4-Hautmuster an der Buntheit: 1 = nur getoente G4-Eintraege, 0 = gleichmaessig
    Cs = par['C'] * s(L[:, 0]) * ((1 - g) + g * C / st['C75'])
    ad = w > 0
    adg = ad & (cnt > 0) & z0
    hl = haut & (C >= 0)
    if adg.any() and hl.any():
        # Lauf 2b: lineare, rangtreue Stauchung der genutzten Ader-L* (G4) auf [Haut-p10, q x Haut-Median] (Lauf 2a bildete auf die
        # Haut-Quantile ab; die Haut hat wenige grosse Eintraege, darum fielen viele Adern auf dieselbe Farbe).
        l_lo, l_hi = float(wquant(Ls[hl], cnt[hl], 0.10)), q * float(wquant(Ls[hl], cnt[hl], 0.50))
        v0, v1 = float(L[adg, 0].min()), float(L[adg, 0].max())
        Lv = l_lo + (L[:, 0] - v0) / max(v1 - v0, 1e-6) * max(l_hi - l_lo, 0.0)
        Lv = np.clip(Lv, min(l_lo, l_hi), l_hi)
        # Buntheit: kappa x mittlere Wunsch-Buntheit der Haut (texelgewichtet); Lauf 2a nahm den Median der G4-Hautbuntheit, der bei
        # grauen G4-Hauteintraegen 0 ist (graue Adern).
        Cv = np.full(len(L), kap * float((Cs[hl] * cnt[hl]).sum() / cnt[hl].sum()))
        c50 = float(wquant(C[hl], cnt[hl], 0.5))
    else:
        Lv = np.minimum(L[:, 0], q * Lm)
        Cv = np.full(len(L), kap * par['C'])
        c50 = st['C75']
    Ln = (1 - w) * Ls + w * Lv
    Cw = (1 - w) * Cs + w * Cv
    cm = r4_cmax(Ln, par['h'])
    Cn = np.where(cm > 1e-6, cm * np.tanh(Cw / np.maximum(cm, 1e-6)), 0.0)
    hr = math.radians(par['h'])
    rgb = rgb_aus_lab(np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1))
    neu = list(g4zeile)
    for i in range(256):
        if g4zeile[i] == 0 or behalten[i]:
            continue
        wn = wort(rgb[i]) | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000          # 0x0000 waere transparent (re15_tim_texel_argb); Schwarz wie G4 als 0x8000
    Lab_n = lab(wort_rgb(neu))
    C_haut = float((np.hypot(Lab_n[haut, 1], Lab_n[haut, 2]) * cnt[haut]).sum() / max(cnt[haut].sum(), 1))
    return neu, dict(Lm=round(Lm, 2), C75=round(st['C75'], 2), C50=round(c50, 2), C_haut=round(C_haut, 2), adern=int(adg.sum()),
                     haut=int(haut.sum()), behalten=int((behalten & (cnt > 0)).sum()),
                     farben=len({neu[i] for i in range(256) if cnt[i] > 0}))


# ================================================================ Laden
def g4_roh():
    """(emd, tim, Quelle) aus RE2 CDEMD0.EMS ueber die TOC des Ports (wie Leser A lade.re2)."""
    inc = open(os.path.join(WT, 're15_port', 'engine', 'src', 'gen', 're2_ems_toc.inc'), encoding='utf-8', errors='replace').read()
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
    ems = open(os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'CDEMD0.EMS'), 'rb').read()

    def rec(r):
        i = ((G4_KIND - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]], toc[i] * 0x800
    e, eo = rec(3)
    t, to = rec(2)
    return e, t, dict(ems='re15_port/shared_assets/RE2/CDEMD0.EMS', kind=hex(G4_KIND), emd_off=hex(eo), tim_off=hex(to))


def g4_modell():
    import birkin_bahnsteig as BB
    e, t, q = g4_roh()
    BB.KIND = G4_KIND
    return BB.modell(e, t), t, q


def k5c_bytes():
    emd = open(os.path.join(K5, 'K5c', 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(K5, 'K5c', 'FORM.TIM'), 'rb').read()
    sha = hashlib.sha256(emd).hexdigest()
    if sha != K5C_EMD_SHA:
        raise SystemExit('K5c/FORM.EMD sha256 %s, erwartet %s' % (sha, K5C_EMD_SHA))
    return emd, tim


def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(-1, 3) / 255.0
    return cv2.cvtColor(a[None], cv2.COLOR_RGB2LAB)[0].astype(np.float64)


def rgb_aus_lab(L):
    """Lab -> RGB 0..255. Ausserhalb des sRGB-Raums wird die Buntheit bei festem L* und Farbton verkleinert (statt Kanal-Kappen, das L* verschiebt)."""
    L = np.asarray(L, np.float64).reshape(-1, 3)
    out = np.zeros((len(L), 3), int)
    for i, (l, a, b) in enumerate(L):
        lo, hi = 0.0, 1.0
        x = cv2.cvtColor(np.array([[[l, a, b]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
        if x.min() < -1e-4 or x.max() > 1 + 1e-4:
            for _ in range(20):
                k = (lo + hi) / 2
                x = cv2.cvtColor(np.array([[[l, a * k, b * k]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
                if x.min() < -1e-4 or x.max() > 1 + 1e-4:
                    hi = k
                else:
                    lo = k
            x = cv2.cvtColor(np.array([[[l, a * lo, b * lo]]], np.float32), cv2.COLOR_LAB2RGB)[0, 0]
        out[i] = np.clip(np.round(x * 255), 0, 255).astype(int)
    return out


def rang_reparatur(neu, basis, eintraege, soll_lab=None):
    """Messblatt V3 H2: L*-Rangfolge der genutzten Eintraege wie im G4-Eintrag (keine Vertauschung bei dL >= 2). Die 15-Bit-Rundung kann
    knappe Paare vertauschen; dann wird der Eintrag in der 5-Bit-Nachbarschaft (+-2 je Kanal) auf die naechste Farbe gesetzt, die die Rangfolge
    haelt. Ein Durchgang aufsteigend nach G4-L*."""
    neu = list(neu)
    e = [i for i in eintraege if basis[i] != 0]
    Lb = {i: float(lab(wort_rgb([basis[i]]))[0, 0]) for i in e}
    ordn = sorted(e, key=lambda i: Lb[i])
    Ln = {i: float(lab(wort_rgb([neu[i]]))[0, 0]) for i in e}
    geaendert = 0
    for i in ordn:
        unten = [Ln[j] for j in ordn if Lb[j] <= Lb[i] - 2]
        oben = [Ln[j] for j in ordn if Lb[j] >= Lb[i] + 2]
        lo = max(unten) if unten else -1
        hi = min(oben) if oben else 101
        if lo <= Ln[i] <= hi:
            continue
        w = neu[i]
        r0, g0, b0 = w & 31, (w >> 5) & 31, (w >> 10) & 31
        ziel = lab(wort_rgb([w]))[0] if soll_lab is None else soll_lab[i]
        best = None
        for dr in range(-2, 3):
            for dg in range(-2, 3):
                for db in range(-2, 3):
                    r, g, b = r0 + dr, g0 + dg, b0 + db
                    if min(r, g, b) < 0 or max(r, g, b) > 31:
                        continue
                    ww = r | (g << 5) | (b << 10)
                    if ww == 0:
                        continue
                    X = lab(wort_rgb([ww]))[0]
                    if lo <= X[0] <= hi:
                        d = float(np.linalg.norm(X - ziel))
                        if best is None or d < best[0]:
                            best = (d, ww, X[0])
        if best is not None:
            neu[i] = best[1] | (w & 0x8000)
            Ln[i] = best[2]
            geaendert += 1
    return neu, geaendert


def wort_rgb(w):
    """PSX-15-Bit -> RGB 0..255 (wie EA.tim_decode: c5 * 255 / 31)."""
    w = np.asarray(w, np.int64)
    r, g, b = w & 31, (w >> 5) & 31, (w >> 10) & 31
    return np.stack([r, g, b], -1) * 255.0 / 31.0


# ================================================================ Flaechen, Inseln (wie Leser A kern3.py, gleiche Inselnummern)
def flaechen(mo, mi):
    """Dreiecke eines Meshes: (V lokal 3x3, UV Atlas 3x2 mit Seitenversatz, CLUT-Wort, (vier, fi))."""
    m = mo.meshes[mi]
    aus = []
    for (verts, faces, uvs, vier) in ((m.tv, m.tf, m.tuv, 0), (m.qv, m.qf, m.quv, 1)):
        if not faces:
            continue
        V = np.array([x[:3] for x in verts], float)
        for fi, (g, uv) in enumerate(zip(faces, uvs)):
            ie = g[1::2]
            po = (uv[5] & 0xF) * 128
            if vier:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for a, b, c in ((0, 1, 3), (0, 3, 2)):
                    aus.append((V[[ie[a], ie[b], ie[c]]], np.array([tx[a], tx[b], tx[c]], float), uv[2], (vier, fi)))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((V[list(ie[:3])], np.array(tx, float), uv[2], (vier, fi)))
    return aus


def insel_je_flaeche(fl):
    par = list(range(len(fl)))

    def f(i):
        while par[i] != i:
            par[i] = par[par[i]]
            i = par[i]
        return i
    ecke = {}
    for k, (V, T, cw, q) in enumerate(fl):
        for p in T:
            key = (int(p[0]), int(p[1]))
            if key in ecke:
                par[f(k)] = f(ecke[key])
            else:
                ecke[key] = k
    return np.array([f(k) for k in range(len(fl))])


def texel_im_dreieck(T, tol=1e-6):
    """Texel (x, y), deren Mitte im geschlossenen UV-Dreieck liegt (wie p_lade.sammeln / Leser A texel_punkte)."""
    u, v = T[:, 0], T[:, 1]
    A2 = (u[1] - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (v[1] - v[0])
    if abs(A2) < 1e-9:
        return np.zeros((0, 2), int)
    x0, x1 = int(np.floor(u.min())), int(np.ceil(u.max()))
    y0, y1 = int(np.floor(v.min())), int(np.ceil(v.max()))
    X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
    w1 = ((X - u[0]) * (v[2] - v[0]) - (u[2] - u[0]) * (Y - v[0])) / A2
    w2 = ((u[1] - u[0]) * (Y - v[0]) - (X - u[0]) * (v[1] - v[0])) / A2
    w0 = 1 - w1 - w2
    inn = (w0 >= -tol) & (w1 >= -tol) & (w2 >= -tol)
    return np.stack([np.floor(X[inn]).astype(int), np.floor(Y[inn]).astype(int)], 1)


class G4Quelle:
    """G4-Dreiecke je Mesh mit Insel, Feld, Texelmasken je Insel und je Feld."""

    def __init__(self, mo):
        self.mo = mo
        self.fl = {mi: flaechen(mo, mi) for mi in range(len(mo.meshes))}
        self.ins = {mi: insel_je_flaeche(self.fl[mi]) if self.fl[mi] else np.zeros(0, int) for mi in self.fl}
        self.feld_von = {}
        for fn, spec in FELD.items():
            for me, isl in spec.items():
                for i in isl:
                    self.feld_von[(me, i)] = fn
        H, W = mo.tex.shape
        self.H, self.W = H, W
        self.inselmaske = {}          # (mesh, insel) -> bool HxW
        self.global_maske = np.zeros((H, W), bool)
        self.texel_flaeche = np.zeros((H, W), float)   # 3D-Flaeche je Texel (fuer D1)
        for mi, fl in self.fl.items():
            for k, (V, T, cw, q) in enumerate(fl):
                xy = texel_psx(T)
                if not len(xy):
                    continue
                key = (mi, int(self.ins[mi][k]))
                mk = self.inselmaske.setdefault(key, np.zeros((H, W), bool))
                xy = xy[(xy[:, 0] >= 0) & (xy[:, 0] < W) & (xy[:, 1] >= 0) & (xy[:, 1] < H)]
                mk[xy[:, 1], xy[:, 0]] = True
                self.global_maske[xy[:, 1], xy[:, 0]] = True
                a3 = 0.5 * np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))
                if len(xy):
                    self.texel_flaeche[xy[:, 1], xy[:, 0]] = np.maximum(self.texel_flaeche[xy[:, 1], xy[:, 0]], a3 / len(xy))

        # Inseln anderer Meshes, die dieselben Texel nutzen (G4 6 = 2, 7 = 3, 18 = 13, ...), bekommen das Feld mit der groessten Ueberdeckung
        fm = {fn: self.feldmaske(fn) for fn in FELD}
        for key, mk in self.inselmaske.items():
            if key in self.feld_von:
                continue
            n = mk.sum()
            best = max(fm, key=lambda fn: (fm[fn] & mk).sum())
            if n and (fm[best] & mk).sum() >= 0.5 * n:
                self.feld_von[key] = best

    def feld(self, mi, insel):
        return self.feld_von.get((mi, int(insel)))

    def feldmaske(self, name):
        m = np.zeros((self.H, self.W), bool)
        for me, isl in FELD[name].items():
            for i in isl:
                if (me, i) in self.inselmaske:
                    m |= self.inselmaske[(me, i)]
        return m


# ================================================================ Parameter (theta, T): ein Rahmen je Teil (Gruppe der Meshes in Ruhelage)
# Grundlage Leser A uebertrag.py (rahmen, mitte_t, koord). Geaendert nach Messung t6.py (2026-11-07): Die Hauptachse je Mesh kippt bei breiten
# Teilen zur Seite (K5c-Brust 10: PCA-Achse (0,04/0,02/-1,00), G4-Brust (0,92/0,32/-0,22)). Darum je Teil EIN Rahmen ueber alle seine Meshes
# (Versatz aus dem Gerippe in Ruhelage, ohne Bindedrehung, Leser A 2), Rumpf/Hals/Kopf mit fester Achse x (vorn, beide Modelle),
# und die G4-Winkelreferenz wird aus der K5c-Referenz durch die kleinste Drehung Achse G4 -> Achse K5c gewonnen (gleiche Seiten in beiden).
UP = np.array([0., -1., 0.])
VOR = np.array([1., 0., 0.])
T_BINS = 12                      # wie Leser A mitte_t (nb = 12)
T_FEIN = 48                      # PORT-WAHL Bauer: Aufloesung der T-Verdichtung (4 je Mitte-Abschnitt)


def ruhe_lage(mo):
    """Knochenursprung in Ruhelage: Summe der Versaetze entlang der Elternkette (EMR rel, keine Bindedrehung)."""
    n = mo.skel.bone_count
    pos = [None] * n
    for b in range(n):
        p = mo.skel.parent[b]
        r = np.array(mo.skel.rel[b], float)
        pos[b] = r if (p < 0 or p >= b) else pos[p] + r
    return pos


def mesh_ecken(mo, mi):
    m = mo.meshes[mi]
    return np.array([v[:3] for v in m.tv + m.qv], float)


def drehung(a, b):
    """Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues)."""
    a = a / np.linalg.norm(a)
    b = b / np.linalg.norm(b)
    v = np.cross(a, b)
    c = float(a @ b)
    if np.linalg.norm(v) < 1e-9:
        if c > 0:
            return np.eye(3)
        o = np.array([1., 0, 0]) if abs(a[0]) < 0.9 else np.array([0, 1., 0])
        v = np.cross(a, o)
        v /= np.linalg.norm(v)
        return 2 * np.outer(v, v) - np.eye(3)
    vx = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
    return np.eye(3) + vx + vx @ vx * (1 / (1 + c))


class Rahmen:
    """Zylinderrahmen eines Teils: Achse a (distal positiv), Referenz r (theta = 0), b = a x r; T = 0..1 entlang a; Mitte je T-Abschnitt."""

    def __init__(self, mo, meshes, achse=None, ref=None, offs=None):
        lage = ruhe_lage(mo)
        o0 = lage[meshes[0]]
        self.offs = {mi: (np.asarray(offs[k], float) if offs is not None else lage[mi] - o0) for k, mi in enumerate(meshes)}
        V = np.vstack([mesh_ecken(mo, mi) + self.offs[mi] for mi in meshes])
        if achse is None:
            c = V.mean(0)
            _w, E = np.linalg.eigh(np.cov((V - c).T))
            a = E[:, -1]
            if (V.mean(0) @ a) < 0:
                a = -a                                   # distal positiv (Wurzel am Ursprung)
        else:
            a = np.asarray(achse, float)
            a = a / np.linalg.norm(a)
        if ref is None:
            r = UP if abs(a @ UP) < 0.7 else VOR
        else:
            r = np.asarray(ref, float)
        r = r - (r @ a) * a
        r /= np.linalg.norm(r)
        self.a, self.r, self.b = a, r, np.cross(a, r)
        s = V @ a
        self.s0, self.s1 = float(s.min()), float(s.max())
        self.L = self.s1 - self.s0
        t = (s - self.s0) / max(self.L, 1e-9)
        q = V - np.outer(s, a)
        kn = np.linspace(0, 1, T_BINS + 1)
        mt = []
        for i in range(T_BINS):
            sel = (t >= kn[i] - 0.05) & (t <= kn[i + 1] + 0.05)
            mt.append(q[sel].mean(0) if sel.any() else q.mean(0))
        self.tc, self.M = (kn[:-1] + kn[1:]) / 2, np.array(mt)
        self.rad = float(np.median(self._rad(V)))

    def _rad(self, P):
        T, th, rad = self._p(P)
        return rad

    def _p(self, P):
        s = P @ self.a
        t = (s - self.s0) / max(self.L, 1e-9)
        cq = np.stack([np.interp(np.clip(t, 0, 1), self.tc, self.M[:, k]) for k in range(3)], 1)
        q = P - np.outer(s, self.a) - cq
        return np.clip(t, 0, 1), np.arctan2(q @ self.b, q @ self.r), np.linalg.norm(q, axis=1)

    def p(self, mi, P):
        T, th, _r = self._p(np.asarray(P, float) + self.offs[mi])
        return T, th

    def achse(self, mi):
        return self.a


def rahmen_paar(k5, zm, g4, qm, achse=None, offs_z=None, offs_q=None):
    """K5c-Rahmen und G4-Rahmen mit gleicher Winkelreferenz (G4-Referenz = K5c-Referenz, zurueckgedreht auf die G4-Achse)."""
    Rz = Rahmen(k5, zm, achse=achse, offs=offs_z)
    Rq0 = Rahmen(g4, qm, achse=achse, offs=offs_q)
    D = drehung(Rq0.a, Rz.a)
    Rq = Rahmen(g4, qm, achse=Rq0.a, ref=D.T @ Rz.r, offs=offs_q)
    return Rz, Rq


# ================================================================ Projektion (b2)
class Theta:
    """theta-Verdichtung je T-Abschnitt: Zielwinkel gleichmaessig auf die Winkel, die erlaubte Quelldreiecke decken (Luecken = ausgeschlossene
    Felder werden uebersprungen, die Naht liegt dort, wo die Luecke war)."""

    def __init__(self, P):
        # T-Verdichtung: T-Abschnitte ohne erlaubte Quelle (z. B. Brust vorn = F7/F8 ausgeschlossen) werden uebersprungen
        tb = np.zeros(T_FEIN, bool)
        for tri in P:
            tb[int(np.clip(tri[:, 1].min() * T_FEIN, 0, T_FEIN - 1)):int(np.clip(tri[:, 1].max() * T_FEIN, 0, T_FEIN - 1)) + 1] = True
        self.t_ged = np.nonzero(tb)[0]
        self.b = np.zeros((T_BINS, 360), bool)
        for tri in P:                                   # tri: 3 x (theta unwrapped, T)
            t0, t1 = tri[:, 1].min(), tri[:, 1].max()
            h0, h1 = np.degrees(tri[:, 0].min()), np.degrees(tri[:, 0].max())
            i0, i1 = int(np.clip(t0 * T_BINS, 0, T_BINS - 1)), int(np.clip(t1 * T_BINS, 0, T_BINS - 1))
            hs = (np.arange(int(np.floor(h0)), int(np.ceil(h1)) + 1) + 180) % 360
            self.b[i0:i1 + 1, hs] = True
        # leere T-Abschnitte erben vom naechsten gefuellten
        voll = np.nonzero(self.b.any(1))[0]
        for i in range(T_BINS):
            if not self.b[i].any() and len(voll):
                self.b[i] = self.b[voll[np.argmin(np.abs(voll - i))]]

    def t_abbilden(self, T):
        T = np.asarray(T, float)
        if len(self.t_ged) == T_FEIN or not len(self.t_ged):
            return T
        f = np.clip(T, 0, 1) * len(self.t_ged)
        k = np.clip(np.floor(f).astype(int), 0, len(self.t_ged) - 1)
        return (self.t_ged[k] + (f - k)) / T_FEIN

    def abbilden(self, T, th):
        T = np.asarray(T, float)
        out = np.asarray(th, float).copy()
        for j in range(len(T)):
            b = self.b[int(np.clip(T[j] * T_BINS, 0, T_BINS - 1))]
            if b.all() or not b.any():
                continue
            ged = np.nonzero(b)[0]
            f = ((np.degrees(out[j]) + 180.0) % 360.0) / 360.0 * len(ged)
            k = int(np.clip(np.floor(f), 0, len(ged) - 1))
            out[j] = np.radians(ged[k] + (f - k) - 180.0)
        return out


def quell_dreiecke(G, Rq, meshes, erlaubt_fn):
    """G4-Dreiecke in (theta, T) mit UV, Seite, Insel; Kappen und nicht erlaubte Inseln fehlen."""
    P, UV, S, I, A = [], [], [], [], []
    for mi in meshes:
        fl = G.fl[mi]
        isl = G.ins[mi]
        for k, (V, T, cw, q) in enumerate(fl):
            if not erlaubt_fn(mi, isl[k]):
                continue
            n = np.cross(V[1] - V[0], V[2] - V[0])
            nn = np.linalg.norm(n)
            if nn < 1e-9 or abs(n @ Rq.a) / nn > KAPPE_NA:
                continue
            t, th = Rq.p(mi, V)
            th = th.copy()
            for i in (1, 2):
                th[i] = th[0] + (th[i] - th[0] + math.pi) % (2 * math.pi) - math.pi
            P.append(np.stack([th, t], 1))
            UV.append(T)
            S.append(int(min(T[:, 0]) // 128))
            I.append((mi, int(isl[k])))
            A.append(0.5 * nn)
    return np.array(P), np.array(UV), np.array(S), I, np.array(A)


def affin(pq, tq, p):
    """UV der Punkte p (n x 2) unter der affinen Abbildung Quelldreieck (theta,T) -> UV."""
    A = np.array([pq[1] - pq[0], pq[2] - pq[0]]).T
    l = np.linalg.pinv(A) @ (p - pq[0]).T
    return tq[0] + np.outer(l[0], tq[1] - tq[0]) + np.outer(l[1], tq[2] - tq[0])


def einpassen(uv, maske, seite, rand=None, quote=None):
    """UV-Dreieck/-Viereck (Atlas, float) so legen, dass ALLE Texel, die die PSX liest (texel_psx), in der Maske liegen. -> (uv int, Massstab, 0).
    Runde 2 (ohne Stellgroessen; Runde 1 hatte SCHRUMPF-Stufen und VERSCHIEBE_N = 300, Zahlen-Pruefer (5)):
    1. volle Groesse am Ort;
    2. volle Groesse an der naechsten zulaessigen Lage: Erosion der Maske mit der Texelform des Dreiecks liefert alle Lagen, an denen es ganz
       in der Maske liegt, auf einmal; gewaehlt wird die naechste;
    3. erst wenn die volle Groesse nirgends passt: Bisektion auf den groessten Massstab < 1, der passt (bis 1 Texel Genauigkeit), naechste Lage;
    4. passt nichts: Punkt auf das naechste Maskentexel (Massstab 0, im Bericht 'kollabiert').
    Die Seite (128er-Fenster + 256 Texel u8-Bereich) begrenzt die Lage. rand/quote bleiben fuer alte Aufrufe ohne Wirkung."""
    H, W = maske.shape
    mk = np.zeros((H, W), np.uint8)
    x0s, x1s = seite * 128, min(W, seite * 128 + 256)
    mk[:, x0s:x1s] = maske[:, x0s:x1s]
    c0 = uv.mean(0)

    def lage(f):
        q = np.round(c0 + (uv - c0) * f).astype(int)
        xy = texel_psx(q)
        inb = (xy[:, 0] >= 0) & (xy[:, 0] < W) & (xy[:, 1] >= 0) & (xy[:, 1] < H)
        if inb.all() and mk[xy[:, 1], xy[:, 0]].all():
            return q
        x0, y0 = xy.min(0)
        kern = np.zeros((int(xy[:, 1].max() - y0) + 1, int(xy[:, 0].max() - x0) + 1), np.uint8)
        kern[xy[:, 1] - y0, xy[:, 0] - x0] = 1
        if kern.shape[0] > H or kern.shape[1] > W:
            return None
        er = cv2.erode(mk, kern, anchor=(0, 0), borderType=cv2.BORDER_CONSTANT, borderValue=0)
        py, px_ = np.nonzero(er)
        if not len(px_):
            return None
        j = int(np.argmin((px_ - x0) ** 2 + (py - y0) ** 2))
        return q + np.array([px_[j] - x0, py[j] - y0])
    q = lage(1.0)
    if q is not None:
        return q, 1.0, 0
    ext = max(float(np.ptp(uv[:, 0])), float(np.ptp(uv[:, 1])), 1e-9)
    lo, hi, best = 0.0, 1.0, None
    while (hi - lo) * ext > 1.0:
        f = (lo + hi) / 2
        q = lage(f)
        if q is not None:
            lo, best = f, q
        else:
            hi = f
    if best is not None:
        return best, lo, 0
    ys, xs = np.nonzero(mk)
    j = int(np.argmin((xs + 0.5 - c0[0]) ** 2 + (ys + 0.5 - c0[1]) ** 2))
    return np.tile(np.array([xs[j], ys[j]]), (len(uv), 1)), 0.0, 0


_BARY_N = 40   # 861 baryzentrische Punkte je Dreieck, Abtastung wie FARBE_PRUEFUNG_v3_r1_zahlen.md (Zahlen-Pruefer: "jedes UV-Dreieck mit 861
               # baryzentrischen Punkten abgetastet, Texel = floor(u), floor(v). So waehlt auch die PSX-GPU den Texel.")
_i, _j = np.meshgrid(np.arange(_BARY_N + 1), np.arange(_BARY_N + 1))
_m = (_i + _j) <= _BARY_N
_BARY = np.stack([1.0 - (_i[_m] + _j[_m]) / _BARY_N, _i[_m] / _BARY_N, _j[_m] / _BARY_N], 1)
del _i, _j, _m


def texel_psx(q):
    """Texel, die die PSX-GPU aus einem UV-Dreieck oder -Viereck liest (floor der UV an 861 baryzentrischen Punkten je Dreieck, Ecken eingeschlossen).
    Anders als texel_im_dreieck (Texelmitte im Dreieck) liefert das auch fuer UV-Punkte und -Linien die tatsaechlich gezeichneten Texel
    (Runde 2, Befund A des Zahlen-Pruefers: Flaechen ohne Texelmittelpunkt fielen aus H1/S1/F1t/F2t heraus)."""
    q = np.asarray(q, float)
    tris = [q] if len(q) == 3 else [q[[0, 1, 3]], q[[0, 3, 2]]]
    pts = np.vstack([_BARY @ t for t in tris])
    return np.unique(np.floor(pts + 1e-9).astype(int), axis=0)


def texel_set(q):
    """Texel eines UV-Dreiecks oder -Vierecks, wie die PSX sie liest (texel_psx; Viereck = Dreiecke 0,1,3 und 0,3,2)."""
    return texel_psx(q)


def _flaeche2(q):
    """Flaecheninhalt eines UV-Dreiecks/-Vierecks (Viereck = Dreiecke 0,1,3 und 0,3,2)."""
    q = np.asarray(q, float)
    tris = [q] if len(q) == 3 else [q[[0, 1, 3]], q[[0, 3, 2]]]
    return sum(0.5 * abs((t[1, 0] - t[0, 0]) * (t[2, 1] - t[0, 1]) - (t[2, 0] - t[0, 0]) * (t[1, 1] - t[0, 1])) for t in tris)


def _flaeche3(P):
    P = np.asarray(P, float)
    tris = [P] if len(P) == 3 else [P[[0, 1, 3]], P[[0, 3, 2]]]
    return sum(0.5 * np.linalg.norm(np.cross(t[1] - t[0], t[2] - t[0])) for t in tris)


def _ebene3(P):
    a = P[1] - P[0]
    n = np.cross(a, P[2] - P[0])
    e1 = a / max(np.linalg.norm(a), 1e-9)
    e2 = np.cross(n, e1)
    return e1, e2 / max(np.linalg.norm(e2), 1e-9)


def _t_abb(TH, T):
    """TH.t_abbilden mit linearer Fortsetzung ausserhalb 0..1 (Ecken jenseits einer Spiegelkante behalten ihre Groesse)."""
    T = np.asarray(T, float)
    if len(TH.t_ged) == T_FEIN or not len(TH.t_ged):
        return T
    k = len(TH.t_ged) / T_FEIN
    out = TH.t_abbilden(np.clip(T, 0, 1))
    return out + np.where(T < 0, T * k, 0.0) + np.where(T > 1, (T - 1) * k, 0.0)


def projektion(k5, G, zm, qm, art, op, k5skel_rel=None, auswahl=None):
    """-> ({(mesh, 't'|'q', fi): (UV-Ecken Atlas int, Seite, (G4-Mesh, Insel), Massstab)}, Statistik, Quellmaske).
    auswahl: Menge (mesh, teil, fi), die diese Zuordnung bekommt (None = alle Flaechen der Ziel-Meshes).
    Runde 2 (Pruefung V3 r1: D1 Kopf ortsgenau Median 4,0; P2 Rumpf, Klauen, Kopf > +8; Sicht "gedehnte Texelbloecke"):
    Texeldichte wie die G4-Quelle.
    - Umfang: theta wird n_th-mal wiederholt, n_th = round(Umfang K5c / gedeckter Umfang der Quelle) >= 1. Ganze Zahl, damit die Wiederholung
      rundum ohne neue Naht schliesst. G4-Machart: G4 nutzt dieselben Texel selbst mehrfach (Mesh 6 = 2, 7 = 3, 18 = 13, Leser A 1).
    - Laenge: T wird mit s_T = Laenge K5c / gedeckte Laenge der Quelle gestreckt und an den Enden gespiegelt (Spiegelkachel). Die Adern laufen
      ueber die Spiegelkante durch (E9 "aus einem Guss"); die Kante liegt nicht an den Gliedgrenzen.
    - Flaechen, deren UV nach dem Runden ein Punkt oder eine Linie ist, obwohl sie bei der Dichte ihres Quelldreiecks >= 1 Texel^2 decken
      (Kappen quer zur Achse), bekommen ihre eigene Ebene in dieser Dichte um denselben UV-Mittelpunkt (Befund A Zahlen-Pruefer: kollabierte UV)."""
    ohne = set(op.get('ohne', []))
    nur = op.get('nur')
    ohne_inseln = op.get('ohne_inseln', {})

    def erlaubt(mi, insel):
        fn = G.feld(mi, insel)
        if int(insel) in ohne_inseln.get(mi, []):
            return False
        if nur is not None:
            return fn in nur
        return fn not in ohne

    Rz, Rq = rahmen_paar(k5, zm, G.mo, qm, achse=op.get('achse'))
    P, UV, S, I, A3 = quell_dreiecke(G, Rq, qm, erlaubt)
    TH = Theta(P)
    erl_maske = np.zeros((G.H, G.W), bool)
    for key in set(I):
        erl_maske |= G.inselmaske[key]
    c_th = float(TH.b[TH.b.any(1)].mean()) if TH.b.any() else 1.0
    f_T = len(TH.t_ged) / T_FEIN if len(TH.t_ged) else 1.0
    C_g, L_g = 2 * math.pi * Rq.rad * c_th, Rq.L * f_T
    C_k, L_k = 2 * math.pi * Rz.rad, Rz.L
    n_th = max(1, int(round(C_k / max(C_g, 1e-9))))
    s_T = L_k / max(L_g, 1e-9)
    if op.get('laengs_einmal'):
        s_T = 1.0      # Runde 2: Glied-Enden mit Spitze/Flammen am distalen Ende (Haende F20, Krallen F21) laengs genau einmal. Mit Spiegelkachel
                       # (s_T 2,2-5,1) wiederholten sich die hellen Krallenspitzen als Querbaender ("Tarnmuster", Sicht-Pruefer V3 r1) und die
                       # Handflammen lagen auch auf dem Handruecken (Bild 1 K12 Vorderglied r 9,8 % gegen Grenze 3,0 %; laengs einmal: 2,1 %).
    rho2 = np.array([_flaeche2(UV[i]) / max(A3[i], 1e-9) for i in range(len(UV))])    # Texel^2 je Einheit^2 je Quelldreieck
    k = Rq.rad / max(Rq.L, 1e-9)
    Pc = P.mean(1)
    A_ = P[:, 1] - P[:, 0]
    B_ = P[:, 2] - P[:, 0]
    det = A_[:, 0] * B_[:, 1] - A_[:, 1] * B_[:, 0]
    ok = np.abs(det) > 1e-12
    dd = np.where(ok, det, 1.0)
    neu = {}
    # Kappen (Runde 2, Sicht-Pruefer V3 r1 "flache rote Keile"): Flaechen quer zur Teilachse (|N.a| > KAPPE_NA, dieselbe Grenze wie fuer die
    # Quell-Kappen) haben im Zylinder keinen Winkel; theta laeuft dort zusammen und dehnt die Texel zu Keilen. Sie bekommen je Ende (vorn/hinten)
    # EINE gemeinsame Ebene quer zur Achse in Quelldichte, verankert am Quelldreieck, auf das der Mittelpunkt der Kappe faellt.
    kappe = {}
    for mi in zm:
        m = k5.meshes[mi]
        for teil, verts, faces in (('t', m.tv, m.tf), ('q', m.qv, m.qf)):
            V = np.array([x[:3] for x in verts], float)
            for fi, g in enumerate(faces):
                if auswahl is not None and (mi, teil, fi) not in auswahl:
                    continue
                Pf = V[list(g[1::2])] + Rz.offs[mi]
                n = np.cross(Pf[1] - Pf[0], Pf[2] - Pf[0])
                nn = np.linalg.norm(n)
                if nn > 1e-9 and abs(n @ Rz.a) / nn > KAPPE_NA:
                    kappe[(mi, teil, fi)] = (1 if (n @ Rz.a) > 0 else -1, Pf)
    kanker = {}
    rho_med = float(np.sqrt(np.median(rho2))) if len(rho2) else 0.0
    for ende in (1, -1):
        Ps = [P_ for e_, P_ in kappe.values() if e_ == ende]
        if not Ps:
            continue
        Pm = np.vstack(Ps)
        cm = Pm.mean(0)
        tt = float(np.clip(((cm @ Rz.a) - Rz.s0) / max(Rz.L, 1e-9), 0, 1))
        t_ = tt * s_T
        ms = math.floor(t_)
        tq = _t_abb(TH, np.array([(t_ - ms) if ms % 2 == 0 else (ms + 1 - t_)]))[0]
        tq = float(np.clip(tq, 0, 1))
        d = (Pc[:, 1] - tq) ** 2
        j = int(np.argmin(d))
        kanker[ende] = (cm, affin(P[j], UV[j], Pc[j][None])[0], int(S[j]), j)
    neu = {}
    st = dict(flaechen=0, im_dreieck=0, naechstes=0, verlegt=0, kleiner=0, kollabiert=0, eben=0, kappen=len(kappe),
              n_theta=n_th, s_T=round(s_T, 3), umfang_k5c_quelle=round(C_k / max(C_g, 1e-9), 3))
    for mi in zm:
        m = k5.meshes[mi]
        for teil, verts, faces in (('t', m.tv, m.tf), ('q', m.qv, m.qf)):
            V = np.array([x[:3] for x in verts], float)
            for fi, g in enumerate(faces):
                if auswahl is not None and (mi, teil, fi) not in auswahl:
                    continue
                ie = list(g[1::2])
                T, th = Rz.p(mi, V[ie])
                t = T * s_T
                mseg = math.floor(float(t.mean()))
                Tq = (t - mseg) if mseg % 2 == 0 else (mseg + 1 - t)
                Tq = _t_abb(TH, Tq)
                thq = (th * n_th + math.pi) % (2 * math.pi) - math.pi
                thq = TH.abbilden(np.clip(Tq, 0, 1), thq)
                for i in range(1, len(ie)):
                    thq[i] = thq[0] + (thq[i] - thq[0] + math.pi) % (2 * math.pi) - math.pi
                p = np.stack([thq, Tq], 1)
                cen = p.mean(0)
                best, bw = None, 0.0
                for sh in (0.0, 2 * math.pi, -2 * math.pi):
                    Cc = cen + np.array([sh, 0.0]) - P[:, 0]
                    l1 = np.where(ok, (Cc[:, 0] * B_[:, 1] - Cc[:, 1] * B_[:, 0]) / dd, -9)
                    l2 = np.where(ok, (A_[:, 0] * Cc[:, 1] - A_[:, 1] * Cc[:, 0]) / dd, -9)
                    inn = ok & (l1 >= -1e-6) & (l2 >= -1e-6) & (l1 + l2 <= 1 + 1e-6)
                    if inn.any():
                        best, bw = int(np.nonzero(inn)[0][0]), sh
                        break
                st['flaechen'] += 1
                if best is None:
                    dth = (Pc[:, 0] - cen[0] + math.pi) % (2 * math.pi) - math.pi
                    d = (k * dth) ** 2 + (Pc[:, 1] - cen[1]) ** 2
                    best = int(np.argmin(d))
                    bw = float(np.round((Pc[best, 0] - cen[0] - dth[best]) / (2 * math.pi)) * 2 * math.pi)
                    st['naechstes'] += 1
                else:
                    st['im_dreieck'] += 1
                uv = affin(P[best], UV[best], p + np.array([bw, 0.0]))
                seite = int(S[best])
                Pf = V[ie] + Rz.offs[mi]
                if (mi, teil, fi) in kappe:
                    ende = kappe[(mi, teil, fi)][0]
                    cm, uv0, seite, best = kanker[ende]
                    uv = uv0 + np.stack([(Pf - cm) @ Rz.r, (Pf - cm) @ Rz.b], 1) * rho_med
                soll = _flaeche3(Pf) * rho2[best]
                if _flaeche2(np.round(uv)) == 0 and soll >= 1.0:
                    if len(Pf) > 3:
                        c_, e1, e2 = ebene(Pf)
                    else:
                        c_ = Pf.mean(0)
                        e1, e2 = _ebene3(Pf)
                    r = math.sqrt(rho2[best])
                    uv = uv.mean(0) + np.stack([(Pf - c_) @ e1, (Pf - c_) @ e2], 1) * r
                    st['eben'] += 1
                q, f, _fr = einpassen(uv, erl_maske, seite)
                if f < 1.0 or not np.array_equal(q, np.round(uv).astype(int)):
                    st['verlegt'] += 1
                if 0 < f < 1.0:
                    st['kleiner'] += 1
                if f == 0.0:
                    st['kollabiert'] += 1
                neu[(mi, teil, fi)] = (q, seite, I[best], f)
    return neu, st, erl_maske


# ================================================================ Krallen (Runde 3)
# G4-Krallenstreifen (Messung r3/krallen_streifen, 2026-11-07, G4-Zeile 1 = Seite 1): Die Krallen-Inseln von G4 14 und 19 liegen auf zwei Streifen,
#  links u 230-240 (Inseln 14/7, 14/15, 19/7, 19/15, 19/2; L* Mittel 40-49) und rechts u 242-253 (14/10, 14/16, 19/10; L* Mittel 53-60, p90 98-100).
#  Jede G4-Kralle legt jede ihrer Seitenflaechen ueber den ganzen Streifen: v laeuft von der Wurzel (v klein, Abstand zum Knochen 87-250) zur
#  Spitze (v gross, 282-458). Gewaehlt ist der linke Streifen: Wurzel dunkelrot (Zeilen 201-213, a* 24-41), Mitte beige (L* 44-55), Spitze dunkel
#  (L* 15-30). Der rechte Streifen hat Texel mit L* > 85 (Glanz-Riegel N2) und ist im G4 die helle Seite derselben Krallen.
KRALLE_INSELN = ((14, 7), (14, 15), (19, 7), (19, 15))   # linker Streifen (gleiche Texel, Bbox u 229-240, v 200-254)


def krallen_streifen(G):
    """Streifen als Maske, Zeilenbereich je v (u_links, u_rechts), Spitze/Wurzel und Laengsdichte (Texel je Einheit) des G4-Streifens."""
    mk = np.zeros((G.H, G.W), bool)
    for key in KRALLE_INSELN:
        mk |= G.inselmaske[key]
    zeilen = {}
    for y in range(G.H):
        xs = np.nonzero(mk[y])[0]
        if len(xs) >= 2:
            zeilen[y] = (int(xs.min()), int(xs.max()))
    # Laengsdichte: Kante Spitzenecke -> Wurzelecke je G4-Flaeche (|dv| > 20 Texel), Median (gemessen 0,087-0,102 je Insel)
    r = []
    for mi, i in KRALLE_INSELN:
        for k, (V, T, cw, qq) in enumerate(G.fl[mi]):
            if G.ins[mi][k] != i:
                continue
            j = int(np.argmax(T[:, 1]))
            for o in range(3):
                if o != j and abs(T[j, 1] - T[o, 1]) > 20:
                    r.append(abs(T[j, 1] - T[o, 1]) / np.linalg.norm(V[j] - V[o]))
    ys = sorted(zeilen)
    # Wurzel: erste Zeile, ab der der Streifen mindestens 8 Texel breit ist (die oberen Zeilen 200-205 sind 2-7 Texel schmal und laufen spitz zu)
    v_w = next(y for y in ys if zeilen[y][1] - zeilen[y][0] + 1 >= 8)
    # Spitze: letzte Zeile mit einem Index != 0 in der Streifenmitte (Index 0 = 0x0000 = durchsichtig auf der PSX)
    return mk, zeilen, v_w, max(ys), float(np.median(r))


def krallen_uv(k5, G, mi, px_g4, cl_g4):
    """UV einer K5c-Kralle, zweiteilig (Runde 3; Weg aus Dossier V3R2.5 Punkt 3 "Krallen laengs zweiteilig", Sicht-Pruefer V3 r2: im Original eine
    "lange dunkelrote Klaue"). Topologie gemessen an allen 8 Krallen-Meshes: sechseckige Ringe (Ecke 6r+j), danach Wurzelmitte und Spitze.
    - Spitzenteil: die letzten k Glieder (Ring R-k -> Spitze) tragen den Rest des Streifens v_r .. v_s einmal, linear in T (beige Mitte, dunkle
      Spitze, wie die G4-Kralle); k so, dass die Vergroesserung dort der G4-Dichte am naechsten kommt (gemessen k = 2..4, Vergroesserung 0,78-1,21);
    - Ansatz: die Glieder davor tragen die Wurzelzeilen des Streifens (v_w .. v_r = dunkelrote Zeilen, Klasse A3), Ring fuer Ring gespiegelt
      (Ringe abwechselnd auf v_r und v_w; die Spiegelkante liegt auf der Ringkante, also ohne Naht);
    Gemessen (Scratchpad formfarbe3/r3, Laeufe v1/v2/v12): Streifen einmal je Kralle (v1) P2 Klauen +13,5/+15,9, Kralle beige; nur die Spitze im
    letzten Glied (v2) P2 +21,6/+24,4; Spitzenteil in Quelldichte (v12, gebaut) P2 +16,3/+21,0, Kralle dunkelrot mit heller Spitze.
    - u: Seiten j abwechselnd an den linken / rechten Streifenrand der Zeile (wie G4: jede Seitenflaeche ueber die ganze Streifenbreite, Nachbarn
      gespiegelt, ohne Naht); Spitze und Wurzelmitte in der Streifenmitte. -> ({(mesh, teil, fi): UV-Ecken Atlas float}, Statistik)"""
    mk, zeilen, v_w, v_s, rho = krallen_streifen(G)
    while v_s > v_w and px_g4[v_s, (zeilen[v_s][0] + zeilen[v_s][1]) // 2] == 0:
        v_s -= 1
    # Ende des Ansatzes: letzte Zeile ab v_w, deren Zeilenmittel (G4-Zeile 1, Seite 1) in der Rotklasse A3 liegt
    # (MESSBLATT_FARBE_V1 6 A3 / Leser A 1: a* > 15, C* > 20, Farbton -40..70 Grad); gemessen Zeilen 201-213 a* 23-41, Zeile 214 a* 12,7
    rgb1 = wort_rgb(np.array(cl_g4[1]))
    v_r = v_w
    for y in range(v_w, v_s + 1):
        L = lab(rgb1[px_g4[y, zeilen[y][0]:zeilen[y][1] + 1]]).mean(0)
        C, h = math.hypot(L[1], L[2]), math.degrees(math.atan2(L[2], L[1]))
        if L[1] > 15 and C > 20 and -40 < h < 70:
            v_r = y
        else:
            break
    # Streifenbreite der Ringecken: Schnitt der Zeilen v_w..v_r (so liegen die Kanten zwischen den Ringen und zur Spitze ganz im Streifen, S1)
    ul_a = max(zeilen[y][0] for y in range(v_w, v_r + 1))
    ur_a = min(zeilen[y][1] for y in range(v_w, v_r + 1))
    m = k5.meshes[mi]
    nv = len(m.qv)
    nr = (nv - 2) // 6
    R = Rahmen(k5, [mi])
    V = np.array([x[:3] for x in m.qv], float)
    T, th = R.p(mi, V)
    t_min = float(T.min())

    # Spitzenteil in Quelldichte: die letzten k Glieder (Ring nr-k .. Spitze) tragen v_r .. v_s, k so, dass ihre Laenge bei G4-Dichte
    # (rho Texel je Einheit) den v_s - v_r Streifenzeilen am naechsten kommt (Vergroesserung ~ 1, D1 "Texeldichte wie die Quelle");
    # davor der Ansatz, Ring fuer Ring gespiegelt.
    tr = [float(np.mean(T[6 * r:6 * r + 6])) for r in range(nr)]
    k_sp = min(range(1, nr + 1), key=lambda k: abs((1.0 - tr[nr - k]) * R.L * rho / max(v_s - v_r, 1) - 1.0))
    r0 = nr - k_sp                                        # erster Ring des Spitzenteils (liegt auf v_r)

    def uv_ecke(i):
        if i == nv - 1:                                   # Spitze
            v = v_s
        elif i == nv - 2:                                 # Wurzelmitte (Kappe am Handansatz): Gegenseite von Ring 0
            v = (v_w if r0 % 2 == 0 else v_r) if r0 > 0 else v_r - (tr[0] - t_min) / max(1.0 - tr[0], 1e-9) * (v_s - v_r)
        else:
            r = i // 6
            if r >= r0:
                v = v_r + (T[i] - tr[r0]) / max(1.0 - tr[r0], 1e-9) * (v_s - v_r)
            else:
                v = v_r if (r0 - r) % 2 == 0 else v_w
        v = float(np.clip(v, v_w, v_s))
        if i >= nv - 2:
            return np.array([(ul_a + ur_a) / 2.0, float(v)])
        return np.array([float(ul_a) if (i % 6) % 2 == 0 else float(ur_a), float(v)])
    out = {}
    for teil, faces in (('t', m.tf), ('q', m.qf)):
        for fi, g in enumerate(faces):
            out[(mi, teil, fi)] = np.array([uv_ecke(i) for i in g[1::2]])
    # Vergroesserung laengs je Glied (Texel bei G4-Dichte / Streifenzeilen)
    glied = [(tr[r + 1] - tr[r]) * R.L * rho / max(v_r - v_w, 1) for r in range(r0)]
    spitze = (1.0 - tr[r0]) * R.L * rho / max(v_s - v_r, 1)
    return out, dict(streifen_v=[int(v_w), int(v_r), int(v_s)], streifen_u=[int(ul_a), int(ur_a)], ringe=nr, spitzen_glieder=k_sp, rho_laengs=round(rho, 4), laenge=round(R.L, 1),
                     vergroesserung_ansatz=[round(x, 2) for x in glied], vergroesserung_spitze=round(spitze, 2), mk=mk)


# ================================================================ CLUT-Zeilen
def g4_cluts(g4tim):
    t = FE.tim_lesen(g4tim)
    if t['clut_x'] != 0 or t['clut_y'] != CLUT_Y or len(t['cluts']) != 3:
        raise SystemExit('G4-TIM unerwartet: clut %d,%d, %d Zeilen' % (t['clut_x'], t['clut_y'], len(t['cluts'])))
    return t['pixel'], [list(z) for z in t['cluts']]


def wort(rgb):
    return FE.clut_wort(*[int(x) for x in rgb])


def zeile_b1(zeile, c_faktor=B1_C_FAKTOR):
    """Bild-1-Aderrot: rote Eintraege im Farbton von G4_TON auf B1_TON gedreht, C* mal B1_C_FAKTOR, L* unveraendert.
    Gewicht w (Rampe ROT_C0..ROT_C1 im Farbtonfenster) mischt zwischen G4-Eintrag und gedrehtem Eintrag. 0x0000 (transparent) bleibt."""
    neu = list(zeile)
    rgb = wort_rgb(zeile)
    L = lab(rgb)
    C = np.hypot(L[:, 1], L[:, 2])
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - ROT_C0) / (ROT_C1 - ROT_C0), 0, 1) * ((h > ROT_H0) & (h < ROT_H1))
    dh = math.radians(B1_TON - G4_TON)
    liste = []
    for i in range(256):
        if zeile[i] == 0 or w[i] <= 0:
            continue
        ww = float(w[i])
        cc = C[i] * (1 + ww * (c_faktor - 1))
        hh = math.radians(h[i]) + ww * dh
        Ln = np.array([[L[i, 0], cc * math.cos(hh), cc * math.sin(hh)]])
        r = rgb_aus_lab(Ln)[0]

        neu[i] = wort(r) | (zeile[i] & 0x8000)
        liste.append(i)
    neu, _n = rang_reparatur(neu, zeile, [i for i in range(256) if zeile[i] != 0])
    return neu, liste


def zeile_ziel(zeile, eintraege, gewichte, ziel_rgb, iterationen=8, fest=()):
    """Eigene Sonderfeld-Zeile: die genutzten Eintraege (Gewicht = Texelzahl) so umfaerben, dass das Feldmittel (Lab, texelgewichtet) das Ziel
    trifft. L* ueber eine Potenz (monoton, Rangfolge haelt), a*b* gedreht und skaliert (Farbton und Buntheit des Mittels). Iteriert gegen die
    15-Bit-Rundung. fest = weitere genutzte Eintraege der Zeile, die ihre G4-Farbe behalten (Haut im F1-Feld): dann bleibt L* bis zum hellsten
    festen Eintrag Lk gleich und wird erst darueber linear gestreckt (L' = Lk + s (L - Lk)), damit die Rangfolge aller genutzten Eintraege haelt (H2)."""
    ziel = lab(np.array([ziel_rgb], float))[0]
    rgb = wort_rgb(zeile)
    L0 = lab(rgb)
    e = np.array(sorted(eintraege))
    fest = [i for i in fest if zeile[i] != 0]
    # Runde 2: Eintraege mit Gewicht, die nicht umgefaerbt werden (Fleisch-Texel mit einem Haut-Eintrag), zaehlen mit ihrer G4-Farbe im
    # Feldmittel mit (F1t ueber alle Fleisch-Texel, Zahlen-Pruefer F1t).
    ef = np.array(sorted(i for i in gewichte if i not in set(e.tolist()) and zeile[i] != 0), int)
    Wg = float(sum(gewichte[i] for i in e) + sum(gewichte[i] for i in ef))
    w = np.array([gewichte[i] for i in e], float) / Wg
    wfix = np.array([gewichte[i] for i in ef], float) / Wg
    Lfix = (L0[ef] * wfix[:, None]).sum(0) if len(ef) else np.zeros(3)
    gam, rot, k = 1.0, 0.0, 1.0
    Lk = float(max(L0[i, 0] for i in fest)) if fest else None
    zl = list(zeile)
    for _ in range(iterationen):
        Ls = L0[e].copy()
        if Lk is None:
            Lp = 100.0 * (np.clip(Ls[:, 0], 0, 100) / 100.0) ** gam
        else:
            Lp = np.where(Ls[:, 0] <= Lk, Ls[:, 0], Lk + gam * (Ls[:, 0] - Lk))
        ab = Ls[:, 1:]
        ca, sa = math.cos(rot), math.sin(rot)
        ab2 = np.stack([ca * ab[:, 0] - sa * ab[:, 1], sa * ab[:, 0] + ca * ab[:, 1]], 1) * k
        X = np.column_stack([Lp, ab2])
        r = rgb_aus_lab(X)
        for j, i in enumerate(e):
            zl[i] = wort(r[j])
        ist = (lab(wort_rgb([zl[i] for i in e])) * w[:, None]).sum(0) + Lfix
        # Nachfuehren: L ueber gam, ab ueber Drehung/Skalierung des Mittels
        if Lk is not None:
            ob = (Ls[:, 0] > Lk)
            wo = w[ob].sum()
            if wo > 0:
                mo = (w[ob] * (Ls[ob, 0] - Lk)).sum()
                gam = max(0.05, (ziel[0] - Lfix[0] - (w[~ob] * Ls[~ob, 0]).sum() - wo * Lk) / max(mo, 1e-9))
        elif ist[0] > 0.5 and ziel[0] > 0.5:
            gam *= math.log(max(ziel[0], 1) / 100.0) / math.log(max(ist[0], 1) / 100.0)
        h_ist, h_z = math.atan2(ist[2], ist[1]), math.atan2(ziel[2], ziel[1])
        rot += h_z - h_ist
        c_ist, c_z = math.hypot(ist[1], ist[2]), math.hypot(ziel[1], ziel[2])
        if c_ist > 0.3:
            k *= c_z / c_ist
    zl, rep = rang_reparatur(zl, zeile, list(e) + list(fest) + list(ef))
    # Runde 2 (Zahlen-Pruefer H2 Zeile 7: 31 Vertauschungen nach Runde 1): Reparatur wiederholen, bis kein Eintrag mehr geaendert wird
    while True:
        zl, r2_ = rang_reparatur(zl, zeile, list(e) + list(fest) + list(ef))
        rep += r2_
        if r2_ == 0:
            break
    ist = (lab(wort_rgb([zl[i] for i in e])) * w[:, None]).sum(0) + Lfix
    return zl, dict(rang_repariert=rep, L_fest_bis=None if Lk is None else round(Lk, 2), gamma=round(gam, 4), drehung_grad=round(math.degrees(rot), 2), c_faktor=round(k, 4),
                    feldmittel_lab=[round(float(x), 2) for x in ist], ziel_lab=[round(float(x), 2) for x in ziel],
                    dE=round(float(np.linalg.norm(ist - ziel)), 2))


# ================================================================ Messdaten der Sonderfelder (v3_daten, aus kf_g4_daten.py)
def daten():
    d = json.load(open(os.path.join(DATEN, 'sonderfelder.json'), encoding='utf-8'))
    z = np.load(os.path.join(DATEN, 'lokpunkte.npz'))
    return d, z


def schluessel(k5, mi, nr):
    """Flaechen-Nr. (tf, danach qf; Leser B flaechen_id.py) -> (mesh, teil, fi)."""
    nt = len(k5.meshes[mi].tf)
    return (mi, 't', nr) if nr < nt else (mi, 'q', nr - nt)


def alle_flaechen(k5, meshes=None):
    out = []
    for mi, m in enumerate(k5.meshes):
        if meshes is not None and mi not in meshes:
            continue
        out += [(mi, 't', i) for i in range(len(m.tf))] + [(mi, 'q', i) for i in range(len(m.qf))]
    return out


def flaeche_info(k5, key):
    """(Ecken lokal n x 3, mittlere Normale lokal, K5c-UV-Schwerpunkt Atlas)."""
    mi, teil, fi = key
    m = k5.meshes[mi]
    if teil == 't':
        g, uv, V, N = m.tf[fi], m.tuv[fi], m.tv, m.tn
        tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])]
    else:
        g, uv, V, N = m.qf[fi], m.quv[fi], m.qv, m.qn
        tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])]
    po = (uv[5] & 0xF) * 128
    P = np.array([V[i][:3] for i in g[1::2]], float)
    n = np.array([N[i][:3] for i in g[0::2]], float).mean(0)
    n = n / max(np.linalg.norm(n), 1e-9)
    c = np.array(tx, float).mean(0) + [po, 0]
    return P, n, c


def in_recht(c, r):
    return r[0] <= c[0] < r[0] + r[2] and r[1] <= c[1] < r[1] + r[3]


BAND_MEHRHEIT = 0.5   # Runde 2: Flaeche gehoert zum Band, wenn die Mehrheit (> 50 %) ihrer in Bild 2 sichtbaren Pixel in der Bandmaske des Pruefers
                      # liegt (Messblatt V3 3.2 band2_pruef). Mehrheitsregel statt der Runde-1-Wahl 0,3 (Zahlen-Pruefer (5)); ergibt 34 Flaechen.


def klassen(k5, D):
    """Sonderfeld-Flaechen: F1, Band, Zaehne, Augen, Maulinneres; Rest = Zuordnung."""
    F1 = {(mi, t, i) for mi, l in F1_FLAECHEN.items() for t, i in l}
    zahn, auge, innen = set(), set(), set()
    for key in alle_flaechen(k5, [22, 23]):
        P, n, c = flaeche_info(k5, key)
        if in_recht(c, K5C_ZAHNRECHT):
            zahn.add(key)
        elif in_recht(c, K5C_AUGENRECHT):
            auge.add(key)
    band = set()
    for b, nr, anteil in D['band_flaechen']:
        key = schluessel(k5, b, nr)
        if anteil > BAND_MEHRHEIT and key not in zahn and key not in auge:
            band.add(key)
    for key in alle_flaechen(k5, [22, 23]):
        if key in zahn or key in auge or key in band:
            continue
        P, n, c = flaeche_info(k5, key)
        # Leser A voll.py: Kieferboden (Mesh 23, Normale nach oben N.y < -0,3) und Gaumen (Mesh 22, Normale nach unten N.y > 0,6) -> Maulinneres F2
        if (key[0] == 23 and n[1] < -0.3) or (key[0] == 22 and n[1] > 0.6):
            innen.add(key)
    return dict(F1=F1, band=band, zahn=zahn, auge=auge, innen=innen)


# ================================================================ Sonderfelder auf Seite 3
SEITE3_X = 384
SLOT_F1 = (SEITE3_X + 2, 2)          # PORT-WAHL Bauer: Lage der Sonderfelder auf der freien Seite 3 (2 Texel Abstand zum Rand)
SLOT_BAND = (SEITE3_X + 2, 100)


def dichte_g4(G, meshes):
    """sqrt(Texel / 3D-Flaeche) eines G4-Meshes (wie Leser A dichte_q)."""
    A, n = 0.0, set()
    for mi in meshes:
        for V, T, cw, q in G.fl[mi]:
            A += 0.5 * np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))
            for x, y in texel_im_dreieck(T):
                n.add((x, y))
    return math.sqrt(len(n) / A)


def ebene(P):
    c = P.mean(0)
    w, E = np.linalg.eigh(np.cov((P - c).T))
    return c, E[:, 2], E[:, 1]


def kopie_gefuellt(px, maske, x0, y0, w, h):
    """Indizes eines Feldausschnitts; Texel ausserhalb der Feldmaske (Loecher) bekommen den Index des naechsten Feldtexels."""
    a = px[y0:y0 + h, x0:x0 + w].copy()
    m = maske[y0:y0 + h, x0:x0 + w]
    if (~m).any():
        _d, lab_ = cv2.distanceTransformWithLabels((~m).astype(np.uint8), cv2.DIST_L2, 5, labelType=cv2.DIST_LABEL_PIXEL)
        ys, xs = np.nonzero(m)
        lut = np.zeros(lab_.max() + 1, int)
        lut[lab_[ys, xs]] = np.arange(len(ys))
        j = lut[lab_]
        a = a[ys[j], xs[j]].reshape(a.shape) if a.ndim == 2 else a
    return a


def groesstes_rechteck_in(maske, w, h):
    """Linke obere Ecke eines w x h-Rechtecks ganz in der Maske (erstes gefundenes, nach Abstand zur Maskenmitte)."""
    ys, xs = np.nonzero(maske)
    cy, cx = ys.mean(), xs.mean()
    ii = cv2.integral(maske.astype(np.uint8))
    best = None
    for y in range(maske.shape[0] - h + 1):
        for x in range(maske.shape[1] - w + 1):
            s = ii[y + h, x + w] - ii[y, x + w] - ii[y + h, x] + ii[y, x]
            if s == w * h:
                d = (x + w / 2 - cx) ** 2 + (y + h / 2 - cy) ** 2
                if best is None or d < best[0]:
                    best = (d, x, y)
    return None if best is None else (best[1], best[2])


# ================================================================ Bau
# Runde 2: FLECK_WEITE entfaellt (Sicht-Pruefer: Fleck 977 px = 1,51-fach, gelappt). Der Kern des Flecks sind die Maskenpunkte selbst.
# Runde 3 (Sicht-Pruefer V3 r2: 941 px = 1,46-fach, "gelappter Fleck mit drei dunklen Streifen", "harte, ausgefranste Kante"; Zahlen-Pruefer (5):
# WEICH_RAND schwach belegt, MORPH_CLOSE 3x3 ohne Quelle): WEICH_RAND und MORPH_CLOSE entfallen.
#  - Kern = Texel, die mehr als zur Haelfte von den Maskenpunkten gedeckt sind (Mehrheitsregel wie BAND_MEHRHEIT; volle Deckung = Median der
#    Punktzahl der Texel, deren 8 Nachbarn alle Punkte haben), eingeschlossene Loecher gefuellt (Definition, keine Stellgroesse).
#  - Rand = ein Texelring aussen um den Kern: dort stehen nur die dunklere Haelfte der Fleisch-Texel, sonst Haut. Quelle: Messblatt V3 3.1
#    Machart "Ein dunkler Rand zur Haut ist G4-Machart und erlaubt"; Runde 2 hatte dort die hellere Haelfte (ausgefranste helle Kante).
#    Gemessen (Scratchpad formfarbe3/r3/f1_flaeche.py, Regel des Sicht-Pruefers): Ring innerhalb des Kerns 447 px (0,69-fach), aussen siehe Dossier.
#  - Fleischquelle: unter allen Lagen des Kernrechtecks in G4-F16 ohne Iris (Seite 2, Zeile 2) die mit den wenigsten Texeln, die nicht heller sind
#    als die hellste Haut des Felds (diese lesen sich als dunkle Streifen; Sicht-Pruefer "einen schraegen Streifen statt drei").


ART_G4, ART_F1, ART_BAND = 1, 2, 3


def zaehler():
    import collections
    return collections.Counter()


def kopf_uv(k5, key):
    """K5c-UV-Ecken einer Flaeche im Atlas (mit Seitenversatz)."""
    mi, teil, fi = key
    m = k5.meshes[mi]
    uv = m.tuv[fi] if teil == 't' else m.quv[fi]
    po = (uv[5] & 0xF) * 128
    tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])] + ([(uv[9], uv[10])] if teil == 'q' else [])
    return np.array(tx, float) + [po, 0]


def bau(aus_dir):
    g4, g4tim, g4q = g4_modell()
    emd, tim = k5c_bytes()
    k5 = FE.modell(emd, tim)
    G = G4Quelle(g4)
    D, Z = daten()
    px_g4, cl_g4 = g4_cluts(g4tim)
    H, W = 256, 512
    px = np.zeros((H, W), np.uint8)
    px[:, :384] = px_g4                                         # Seiten 0-2 bytegleich (Messblatt V3 2.2 b2)
    # Herkunftskarte (Runde 2, H1 "Index bytegleich mit dem G4-Atlas an der kopierten Stelle"): je Texel Quelle im G4-Atlas und Art
    hx = np.full((H, W), -1, np.int16)
    hy = np.full((H, W), -1, np.int16)
    hart = np.zeros((H, W), np.uint8)
    hx[:, :384] = np.arange(384)[None, :]
    hy[:, :384] = np.arange(H)[:, None]
    hart[:, :384] = ART_G4
    cl = [list(cl_g4[0]), list(cl_g4[1]), list(cl_g4[2])]       # Zeilen 0-2 bytegleich
    cl += [None] * 6                                            # Runde 4: 3-5 Gruppenzeilen (unten), 6-8 Sonderfelder (unten)
    bericht = dict(g4=g4q, k5c_emd_sha256=K5C_EMD_SHA, zeilen={}, teile={}, sonder={})
    K = klassen(k5, D)
    belegt = set().union(*K.values())
    uv = {}

    # ---- Zuordnung Messblatt 2.3 (b2)
    for nm, zm, qm, art, op in ZUORDNUNG:
        aw = {k for k in alle_flaechen(k5, zm) if k not in belegt}
        st_sum = {}
        if art == 'kralle':
            mk_k = None
            st_sum = dict(flaechen=0, verlegt=0, kleiner=0, je_mesh={})
            for z in zm:
                uvk, stk = krallen_uv(k5, G, z, px_g4, cl_g4)
                mk_k = stk.pop('mk')
                vl = kl = 0
                for k, q in uvk.items():
                    if k not in aw:
                        continue
                    qq, f, _fr = einpassen(q, mk_k, int(q[:, 0].min() // 128))
                    vl += int(f < 1.0 or not np.array_equal(qq, np.round(q).astype(int)))
                    kl += int(f < 1.0)
                    uv[k] = (qq, int(q[:, 0].min() // 128), KRALLE_INSELN[0] if z in (14, 15, 28, 30) else KRALLE_INSELN[2], f, nm, op)
                    st_sum['flaechen'] += 1
                st_sum['verlegt'] += vl
                st_sum['kleiner'] += kl
                st_sum['je_mesh'][str(z)] = dict(stk, verlegt=vl, kleiner=kl)
            bericht['teile'][nm] = dict(k5c=zm, g4=qm, art=art, **st_sum)
            continue
        if art == 'einzeln':
            for z in zm:
                neu, st, _m = projektion(k5, G, [z], qm, 'kette', op, k5.skel.rel, {k for k in aw if k[0] == z})
                for k, v in neu.items():
                    uv[k] = (v[0], v[1], v[2], v[3], nm, op)
                # Runde 3 (Zahlen-Pruefer V3 r2 (5): n_theta, s_T und umfang_k5c_quelle waren Summen ueber die Ziel-Meshes): Zaehler summiert,
                # Masse je Mesh unter 'je_mesh'
                for a_, b_ in st.items():
                    if a_ in ('n_theta', 's_T', 'umfang_k5c_quelle'):
                        st_sum.setdefault('je_mesh', {}).setdefault(str(z), {})[a_] = b_
                    else:
                        st_sum[a_] = st_sum.get(a_, 0) + b_
        else:
            neu, st, _m = projektion(k5, G, zm, qm, art, op, k5.skel.rel, aw)
            for k, v in neu.items():
                uv[k] = (v[0], v[1], v[2], v[3], nm, op)
            st_sum = st
        bericht['teile'][nm] = dict(k5c=zm, g4=qm, art=art, **st_sum)

    # ---- Maulinneres F2 (Leser A voll.py: Kieferboden/Gaumen -> F2), Projektion im Kopfrahmen auf G4-Kopf 26
    neu, st, _m = projektion(k5, G, [22, 23], [26], 'kopf', dict(achse=(1, 0, 0), nur=['F2']), k5.skel.rel, K['innen'])
    for k, v in neu.items():
        uv[k] = (v[0], v[1], v[2], v[3], 'Maulinneres F2', dict(zeilen='g4'))
    bericht['teile']['Maulinneres F2'] = dict(k5c=[22, 23], g4=[26], art='kopf', **st)

    # ---- Zeilenfamilie je Flaeche (Runde 3); Runde 4 ersetzt die Zeile unten durch die Gruppenzeile
    fertig = {}
    quelle_feld = {}
    familie = {}
    for k, (q, seite, herk, f, nm, op) in uv.items():
        z = op.get('zeilen', 'g4')
        if isinstance(z, dict):
            z = z.get(k[0], 'g4')
        if z == 'b1' and k[0] in B1_SICHTBAR_NR:
            nr_ = k[2] if k[1] == 't' else len(k5.meshes[k[0]].tf) + k[2]
            if nr_ not in B1_SICHTBAR_NR[k[0]]:
                z = 'g4'
        zeile = (Z_B1 if z == 'b1' else Z_G4)[seite]
        fertig[k] = (q, seite, zeile, nm)
        quelle_feld[k] = '%s:%d/%d' % (G.feld(*herk) or '-', herk[0], herk[1])
        familie[k] = z if nm != 'Maulinneres F2' else None

    # ---- Augen: K5c-Augenfeld (24 x 10) -> dunkles Rechteck in G4-F10 (Lende, Lab 5,0/1,7/-2,7, Leser A 1), Zeile der Lende (Bild-1-Familie)
    m10 = G.feldmaske('F10')
    ecke = groesstes_rechteck_in(m10, K5C_AUGENRECHT[2], K5C_AUGENRECHT[3])
    for k in K['auge']:
        q = kopf_uv(k5, k) - np.array(K5C_AUGENRECHT[:2]) + np.array(ecke)
        fertig[k] = (np.round(q).astype(int), 0, Z_G4[0], 'Augen F10')   # Runde 4: G4-Zeile 0 (F10 ohne Rot-Eintraege; Zeile 3 ist Gruppenzeile)
    bericht['sonder']['Augen'] = dict(flaechen=len(K['auge']), ziel_rechteck=[int(ecke[0]), int(ecke[1]), K5C_AUGENRECHT[2], K5C_AUGENRECHT[3]])

    # ---- Zaehne: K5c-Zahnfeld (24 x 14) -> G4-F8 (Stachelkranz, oben hellgelb-weiss, Leser A 1), quer gestellt, Zeile 8
    m8 = G.feldmaske('F8')
    r8 = cv2.dilate(m8.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    ys, xs = np.nonzero(m8)
    x8, y8, w8 = xs.min(), ys.min(), xs.max() - xs.min() + 1
    zahn_texel = zaehler()
    zahn_xy = set()      # Runde 2: jedes gelesene Texel einmal (wie Riegel F3t / F2t: "genutzte Texel des Felds")
    for k in K['zahn']:
        r = kopf_uv(k5, k) - np.array(K5C_ZAHNRECHT[:2])         # 0..24 x 0..14
        q = np.stack([x8 + r[:, 1] * (w8 - 1) / K5C_ZAHNRECHT[3], y8 + r[:, 0]], 1)
        q, f, fremd = einpassen(q, m8, 0, r8)
        fertig[k] = (q, 0, Z_ZAHN, 'Zaehne F8')
        zahn_xy.update((int(x), int(y)) for x, y in texel_set(q))

    # ---- F1-Stelle (Runde 3): Becken-Vierecke (Messblatt V3 1.2) -> eigenes Feld auf Seite 3.
    #      Haut: G4-F11 (Becken) als lueckenloses Rechteck, Indizes bytegleich, Quelle je Texel in hx/hy (H1: G4-Kopie, kein Sonderfeld).
    #      Fleck = Sonderfeld: Kern = Texel, die die Maskenpunkte F1 (Bild 1, 646 px, im Knochenraum) mehrheitlich decken; im Texelring um den
    #      Kern nur die dunklere Haelfte des Fleischs (dunkler Rand, Messblatt 3.1). Fleisch = G4-F16 ohne Iris, Lage mit den wenigsten
    #      hautdunklen Texeln. Zeile 6 = G4-Zeile 2, Fleisch-Eintraege so umgefaerbt, dass das Mittel ueber ALLE Fleck-Texel ZIEL_F1 trifft (F1t);
    #      Haut-Eintraege bleiben (H2).
    rho_b = dichte_g4(G, [0])                                   # G4-Becken: Texel je Einheit (gemessen, t4.py: 0,0339)
    Pall = np.vstack([flaeche_info(k5, k)[0] for k in K['F1']])
    c0, e1, e2 = ebene(Pall)
    st_ = np.stack([(Pall - c0) @ e1, (Pall - c0) @ e2], 1) * rho_b
    lo = st_.min(0)
    wf, hf = int(np.ceil(st_[:, 0].max() - lo[0])) + 3, int(np.ceil(st_[:, 1].max() - lo[1])) + 3
    ox, oy = SLOT_F1

    def f1_uv(P):
        s = np.stack([(P - c0) @ e1, (P - c0) @ e2], 1) * rho_b - lo + 1
        return s + np.array([ox, oy])
    for k in K['F1']:
        fertig[k] = (np.round(f1_uv(flaeche_info(k5, k)[0])).astype(int), 3, Z_F1, 'F1-Stelle')
    feld = np.zeros((H, W), bool)
    feld[oy:oy + hf, ox:ox + wf] = True
    # Haut: groesstes lueckenloses F11-Rechteck in Feldhoehe; fehlende Spalten gespiegelt (Spiegelkachel, Quelle bleibt je Texel bekannt)
    m11 = G.feldmaske('F11')
    bw = wf
    while bw > 1 and groesstes_rechteck_in(m11, bw, hf) is None:
        bw -= 1
    e11 = groesstes_rechteck_in(m11, bw, hf)
    if e11 is None:
        raise SystemExit('F11: kein lueckenloses Rechteck der Hoehe %d' % hf)
    for i in range(wf):
        per = i % (2 * bw)
        sx = e11[0] + (per if per < bw else 2 * bw - 1 - per)
        for j in range(hf):
            px[oy + j, ox + i] = px_g4[e11[1] + j, sx]
            hx[oy + j, ox + i], hy[oy + j, ox + i], hart[oy + j, ox + i] = sx, e11[1] + j, ART_G4
    lp = f1_uv(Z['F1_lok'].astype(float))
    anz = np.zeros((H, W), np.int32)
    np.add.at(anz, (np.floor(lp[:, 1]).astype(int), np.floor(lp[:, 0]).astype(int)), 1)
    belegt_ = anz > 0
    innen = cv2.erode(belegt_.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    voll = float(np.median(anz[innen])) if innen.any() else float(np.median(anz[belegt_]))
    kern = (anz > 0.5 * voll) & feld
    import scipy.ndimage as ndi
    kern = ndi.binary_fill_holes(kern) & feld
    rand = cv2.dilate(kern.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & feld & ~kern
    fleck = kern | rand
    m16 = G.feldmaske('F16')
    rgb16 = wort_rgb(np.array(cl_g4[2])[px_g4])
    L16 = lab(rgb16.reshape(-1, 3)).reshape(H, 384, 3)
    h16 = np.degrees(np.arctan2(L16[..., 2], L16[..., 1]))
    iris = m16 & (h16 > 60) & (h16 < 100) & (L16[..., 0] > 30)          # Leser A 1 "gelb-olive Iris"
    iris = cv2.dilate(iris.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool)
    ys_f, xs_f = np.nonzero(fleck)
    fw, fh = int(xs_f.max() - xs_f.min() + 1), int(ys_f.max() - ys_f.min() + 1)
    haut_L = lab(wort_rgb(np.array(cl_g4[2])[px[feld & ~fleck]])).max(0)[0] if (feld & ~fleck).any() else 0.0
    erl16 = m16 & ~iris
    erl16[:, :256] = False                                      # Seite 2 (Zeile 2 = Basis der Zeile 6)
    dunkel16 = (L16[..., 0] <= haut_L) & erl16
    km = fleck[ys_f.min():ys_f.min() + fh, xs_f.min():xs_f.min() + fw]
    kk_ = kern[ys_f.min():ys_f.min() + fh, xs_f.min():xs_f.min() + fw]
    best16 = None
    ii_e = cv2.integral(erl16.astype(np.uint8))
    for y in range(H - fh + 1):
        for x in range(256, 384 - fw + 1):
            if ii_e[y + fh, x + fw] - ii_e[y, x + fw] - ii_e[y + fh, x] + ii_e[y, x] != fw * fh:
                continue
            nd = int(dunkel16[y:y + fh, x:x + fw][kk_].sum())
            if best16 is None or nd < best16[0]:
                best16 = (nd, x, y)
    if best16 is None:
        raise SystemExit('F16 ohne Iris (Seite 2): kein %dx%d-Rechteck' % (fw, fh))
    e16 = (best16[1], best16[2])
    fl_idx = px_g4[e16[1]:e16[1] + fh, e16[0]:e16[0] + fw]
    fl_L = L16[e16[1]:e16[1] + fh, e16[0]:e16[0] + fw, 0]
    med = float(np.median(fl_L[km]))      # Median ueber Kern und Rand
    haut_eintraege = set(np.unique(px[feld & ~fleck]).tolist())
    fleisch_texel = zaehler()
    for y, x in zip(ys_f, xs_f):
        j, i = y - ys_f.min(), x - xs_f.min()
        if rand[y, x] and fl_L[j, i] >= med:
            continue
        px[y, x] = fl_idx[j, i]
        hx[y, x], hy[y, x], hart[y, x] = -1, -1, ART_F1
    # Gewichte = die Texel, die die F1-Flaechen tatsaechlich lesen (texel_psx), damit Bau und Riegel F1t gleich zaehlen
    gelesen = set()
    for k in K['F1']:
        for x, y in texel_psx(fertig[k][0]):
            gelesen.add((int(x), int(y)))
    # Runde 3: Bauziel ZIEL_F1 auf den Kern-Texeln (Gewicht = gelesene Kern-Texel). Gemessen (r3/schnell.py): mit Kern + dunklem Rand im Mittel
    # liest die Kette an der Maske F1 nur den hellen Kern, K14 (54,3 / 16,4 / -2,5), dE 9,3 ROT; mit Kerngewicht (47,9 / 17,5 / -1,2), dE 3,1.
    # Der Riegel F1t mittelt weiter ueber alle Fleck-Texel samt Rand.
    for x, y in gelesen:
        if hart[y, x] == ART_F1 and kern[y, x]:
            fleisch_texel[int(px[y, x])] += 1
    # Runde 4, Baufehler A (Zahlen-Pruefer V3 r3: 12 von 162 Fleck-Texeln mit rohem G4-Blutrot, z. B. (173,16,16)): umgefaerbt werden alle
    # Fleisch-Eintraege der gelesenen Fleck-Texel (Kern UND Rand); das Gewicht des Ziels bleibt auf dem Kern (Runde 3, r3/schnell.py).
    rand_eintraege = sorted({int(px[y, x]) for x, y in gelesen if hart[y, x] == ART_F1} - set(fleisch_texel))
    eintr = [i for i in list(fleisch_texel) + rand_eintraege if i not in haut_eintraege and cl_g4[2][i] != 0]
    z6, info6 = zeile_ziel(cl_g4[2], eintr, fleisch_texel, ZIEL_F1, fest=sorted(haut_eintraege))
    cl[Z_F1] = z6
    bericht['sonder']['F1'] = dict(flaechen=sorted([list(k) for k in K['F1']]), dichte=round(rho_b, 4), feld=[ox, oy, wf, hf], kern_texel=int(kern.sum()),
                                   rand_texel=int(rand.sum()), volle_deckung_punkte=voll, dunkle_quelltexel=int(best16[0]), haut_L_max=round(float(haut_L), 2), fleisch_texel=int(sum(fleisch_texel.values())),
                                   gelesen=len(gelesen), haut_g4_kopie=int(sum(1 for x, y in gelesen if hart[y, x] == ART_G4)),
                                   quelle_fleisch_F16=[int(e16[0]), int(e16[1]), fw, fh], quelle_haut_F11=[int(e11[0]), int(e11[1]), bw, hf],
                                   umgefaerbt=len(eintr), rand_eintraege_r4=len(rand_eintraege), zeile=info6)

    # ---- Maul-Band: Bandflaechen (Bild 2) -> eigenes Feld auf Seite 3: G4-F7 (strahlenfoermiges Fleisch, 175 Farben) mit gefuellten Loechern;
    #      Zeile 7 = G4-Zeile 0 mit umgefaerbten F7-Eintraegen (Ziel F2t)
    m7 = G.feldmaske('F7')
    ys7, xs7 = np.nonzero(m7)
    x7, y7 = int(xs7.min()), int(ys7.min())
    w7, h7 = int(xs7.max() - x7 + 1), int(ys7.max() - y7 + 1)
    bx, by = SLOT_BAND
    px[by:by + h7, bx:bx + w7] = kopie_gefuellt(px_g4, m7, x7, y7, w7, h7)
    hart[by:by + h7, bx:bx + w7] = ART_BAND
    bandfeld = np.zeros((H, W), bool)
    bandfeld[by:by + h7, bx:bx + w7] = True
    Kk = Rahmen(k5, [22, 23], achse=(1, 0, 0))
    rho_r = dichte_g4(G, [10])                                  # G4-Brust (Herkunft F7): Texel je Einheit (gemessen, t4.py: 0,0290)
    pts = {k: Kk.p(k[0], flaeche_info(k5, k)[0]) for k in K['band']}
    th_all = np.concatenate([v[1] for v in pts.values()])
    th0 = math.atan2(np.sin(th_all).mean(), np.cos(th_all).mean())

    def band_st(T, th, s):
        d = (th - th0 + math.pi) % (2 * math.pi) - math.pi
        return np.stack([d * Kk.rad * s, T * Kk.L * s], 1)
    allst = np.vstack([band_st(T, th, rho_r) for T, th in pts.values()])
    ext = allst.max(0) - allst.min(0)
    s_band = rho_r * min(1.0, (w7 - 3) / max(ext[0], 1e-9), (h7 - 3) / max(ext[1], 1e-9))
    allst = np.vstack([band_st(T, th, s_band) for T, th in pts.values()])
    lo_b = allst.min(0)
    band_texel = zaehler()
    band_xy = set()
    for k, (T, th) in pts.items():
        q = band_st(T, th, s_band) - lo_b + 1 + np.array([bx, by])
        q, f, fremd = einpassen(q, bandfeld, 3)
        fertig[k] = (q, 3, Z_BAND, 'Maul-Band')
        band_xy.update((int(x), int(y)) for x, y in texel_set(q))
    for x, y in band_xy:
        band_texel[int(px[y, x])] += 1
    for x, y in zahn_xy:
        zahn_texel[int(px[y, x])] += 1
    eintr7 = [i for i in band_texel if cl_g4[0][i] != 0]
    z7, info7 = zeile_ziel(cl_g4[0], eintr7, band_texel, ZIEL_BAND_KETTE)
    cl[Z_BAND] = z7
    bericht['sonder']['Band'] = dict(flaechen=len(K['band']), dichte_soll=round(rho_r, 4), dichte=round(s_band, 4), feld=[bx, by, w7, h7],
                                     quelle_F7=[x7, y7, w7, h7], umgefaerbt=len(eintr7), zeile=info7)
    eintr8 = [i for i in zahn_texel if cl_g4[0][i] != 0]
    z8, info8 = zeile_ziel(cl_g4[0], eintr8, zahn_texel, ZIEL_ZAHN)
    cl[Z_ZAHN] = z8
    bericht['sonder']['Zaehne'] = dict(flaechen=len(K['zahn']), umgefaerbt=len(eintr8), zeile=info8)

    # ---- Runde 4: Gruppenzeilen (R4_GRUPPEN x Seite) aus den G4-Zeilen, jede Koerperflaeche auf ihre Gruppenzeile
    par4 = r4_par()
    fn = r4_flaechen_normalen(k5)
    gruppe = {}
    for k, fam in familie.items():
        if fam is None or k not in fertig or fertig[k][2] not in Z_G4 + Z_B1:
            continue
        gruppe[k] = r4_gruppe(k, fam, k5, fn)
    paare = sorted({(g, fertig[k][1]) for k, g in gruppe.items()}, key=lambda p: (R4_GRUPPEN.index(p[0]), p[1]))
    if len(paare) > len(R4_ZEILEN_FREI):
        raise SystemExit('Runde 4: %d Gruppenzeilen, frei %d' % (len(paare), len(R4_ZEILEN_FREI)))
    zplan = {p: R4_ZEILEN_FREI[i] for i, p in enumerate(paare)}
    zaehl = {p: np.zeros(256) for p in paare}
    for k, g in gruppe.items():
        q, seite, _z, nm = fertig[k]
        xy = texel_set(q)
        if len(xy):
            np.add.at(zaehl[(g, seite)], px[xy[:, 1], xy[:, 0]], 1)
    while len(cl) <= max(zplan.values()):
        cl.append(None)
    bericht['zeilen']['r4'] = {}
    for (g, seite), z in zplan.items():
        st4 = r4_statistik(cl_g4[seite], zaehl[(g, seite)])
        cl[z], info = r4_zeile(cl_g4[seite], par4[g], st4)
        bericht['zeilen']['r4'][str(z)] = dict(gruppe=g, seite=seite, basis=seite, par=par4[g], **info)
    for z in range(len(cl)):
        if cl[z] is None:                       # ungenutzte Luecke (nur bei weniger als 3 Gruppenzeilen): G4-Zeile 0
            cl[z] = list(cl_g4[0])
    for k, g in gruppe.items():
        q, seite, _z, nm = fertig[k]
        fertig[k] = (q, seite, zplan[(g, seite)], nm)
    # ---- Runde 5: Krallen zurueck auf ihre G4-Zeile (Runde-3-Klauenfarbe), nach der Gruppenstatistik (Zeile 16 bleibt bytegleich)
    r5 = dict(flaechen=0, zeilen={})
    for k, (q, seite, z, nm) in list(fertig.items()):
        if nm in R5_KRALLEN:
            fertig[k] = (q, seite, Z_G4[seite], nm)
            r5['flaechen'] += 1
            r5['zeilen']['%d->%d' % (z, Z_G4[seite])] = r5['zeilen'].get('%d->%d' % (z, Z_G4[seite]), 0) + 1
    bericht['r5'] = dict(teile=list(R5_KRALLEN), **r5)
    bericht['zeilen']['basis'] = {str(z): seite for (g, seite), z in zplan.items()}
    bericht['zeilen']['basis'].update({str(Z_F1): 2, str(Z_BAND): 0, str(Z_ZAHN): 0})
    bericht['zeilen']['gruppe'] = {str(z): g for (g, seite), z in zplan.items()}
    bericht['r4'] = dict(nutzer=R4_NUTZER, par_datei=os.path.relpath(R4_PAR_DATEI, K5).replace(os.sep, '/'), L_HELL=R4_L_HELL, SMAX=R4_SMAX,
                         flaechen_je_gruppe={g: sum(1 for x in gruppe.values() if x == g) for g in R4_GRUPPEN},
                         gruppe_je_flaeche={'%d|%s|%d' % k: g for k, g in gruppe.items()})

    # ---- Vollstaendigkeit
    fehlt = [k for k in alle_flaechen(k5) if k not in fertig]
    if fehlt:
        raise SystemExit('%d Flaechen ohne UV, z. B. %s' % (len(fehlt), fehlt[:5]))

    # ---- EMD (nur uv/clut/page) und TIM schreiben
    form = FE.lese_emd(emd)
    for (mi, teil, fi), (q, seite, zeile, nm) in fertig.items():
        f = form['meshes'][mi]['dreiecke' if teil == 't' else 'vierecke'][fi]
        u = [int(x) - 128 * seite for x in q[:, 0]]
        v = [int(y) for y in q[:, 1]]
        if min(u) < 0 or max(u) > 255 or min(v) < 0 or max(v) > 255:
            raise SystemExit('UV ausserhalb u8: %s %s %s' % ((mi, teil, fi), u, v))
        f['uv'] = [[a_, b_] for a_, b_ in zip(u, v)]
        f['clut'] = ((CLUT_Y + zeile) << 6) | 0
        f['page'] = (int(f['page']) & ~0xF) | seite
    emd_neu = FE.schreibe_emd(form, pruefen=True)
    tim_neu = FE.tim_schreiben(px, cl, clut_x=0, clut_y=CLUT_Y)
    os.makedirs(aus_dir, exist_ok=True)
    open(os.path.join(aus_dir, 'FORM.EMD'), 'wb').write(emd_neu)
    open(os.path.join(aus_dir, 'FORM.TIM'), 'wb').write(tim_neu)
    fj = json.load(open(os.path.join(K5, 'K5c', 'form.json'), encoding='utf-8'))
    fj['name'] = 'K5c_farbe_v3'
    json.dump(fj, open(os.path.join(aus_dir, 'form.json'), 'w', encoding='utf-8'), indent=1)
    bericht['flaechen_herkunft'] = {'%d|%s|%d' % k: [nm, int(zeile), quelle_feld.get(k, nm)] for k, (q, s, zeile, nm) in fertig.items()}
    bericht['zeilen']['anzahl'] = len(cl)
    np.savez_compressed(os.path.join(aus_dir, 'herkunft.npz'), hx=hx, hy=hy, art=hart)
    bericht['herkunft'] = dict(datei='herkunft.npz', art={'1': 'G4-Texel (Seiten 0-2 an Ort, Seite 3 Kopie mit Quelle hx/hy)', '2': 'Sonderfeld F1-Fleisch',
                                                          '3': 'Sonderfeld Maul-Band', '0': 'leer'},
                               g4_kopie_seite3=int(((hart == ART_G4) & (np.arange(W)[None, :] >= 384)).sum()))
    bericht['sha256'] = dict(emd=hashlib.sha256(emd_neu).hexdigest(), tim=hashlib.sha256(tim_neu).hexdigest())
    bericht['basis'] = ('K5c (varianten/K5/K5c): Geometrie, Knochen, Clips unveraendert; UV/CLUT/TPage je Flaeche und FORM.TIM neu '
                        '(Versuch 3, kf_g4.py; Runde 4: Gruppenzeilen, weniger Rot; Runde 5: Krallen auf G4-Zeile)')
    bericht['konstanten'] = dict(R5_KRALLEN=R5_KRALLEN, R4_PAR=r4_par(), R4_L_HELL=R4_L_HELL, R4_SMAX=R4_SMAX, R4_N_GRENZE=R4_N_GRENZE,
                                 G4_KIND=hex(G4_KIND), B1_ROT_LAB=B1_ROT_LAB, G4_ROT_LAB=G4_ROT_LAB, B1_TON=round(B1_TON, 2), G4_TON=round(G4_TON, 2),
                                 B1_C_JE_SEITE=[round(x, 4) for x in B1_C_JE_SEITE], ROT_RAMPE=(ROT_C0, ROT_C1, ROT_H0, ROT_H1), ZIEL_F1=ZIEL_F1, ZIEL_BAND=ZIEL_BAND, ZIEL_BAND_KETTE=ZIEL_BAND_KETTE,
                                 ZIEL_ZAHN=ZIEL_ZAHN, KAPPE_NA=KAPPE_NA, BAND_MEHRHEIT=BAND_MEHRHEIT, B1_C_SEITE1_EICH=B1_C_SEITE1_EICH,
                                 KRALLE_INSELN=KRALLE_INSELN, B1_SICHTBAR_NR={str(k): len(v) for k, v in B1_SICHTBAR_NR.items()})
    json.dump(bericht, open(os.path.join(aus_dir, 'bau.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=str)
    return bericht


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=os.path.join(K5, 'farbe', 'K5c_farbe_v3'))
    x = a.parse_args()
    b = bau(x.aus)
    shutil.copyfile(os.path.join(x.aus, 'FORM.EMD'), os.path.join(K5, 'farbe', 'K5C_FARBE_V3.EMD'))
    shutil.copyfile(os.path.join(x.aus, 'FORM.TIM'), os.path.join(K5, 'farbe', 'K5C_FARBE_V3.TIM'))
    os.makedirs(os.path.join(SCRATCH, 'emd'), exist_ok=True)
    for f in ('FORM.EMD', 'FORM.TIM', 'form.json', 'bau.json', 'herkunft.npz'):
        shutil.copyfile(os.path.join(x.aus, f), os.path.join(SCRATCH, 'emd', f))
    for nm, t in b['teile'].items():
        print('%-32s %s' % (nm, {k: v for k, v in t.items() if k not in ('k5c', 'g4', 'art')}))
    for nm, t in b['sonder'].items():
        print('%-12s %s' % (nm, t))
    print('sha256', b['sha256'])


if __name__ == '__main__':
    main()
