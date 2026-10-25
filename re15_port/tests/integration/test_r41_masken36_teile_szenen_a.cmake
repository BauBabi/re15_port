# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe a.
# Stellen = test_r37_masken36 neustellen <raum> <cut> <erstes> (dieselben erreichbaren Stehplaetze, dasselbe Figurraster und dieselbe
# Engine-Tabelle wie der Ausschluss): Fenster = Kasten der Figurpunkte, die NUR neue Rechtecke verdecken (M) bzw. der Figurpunkte auf neuen
# Rechtecken, von denen keiner verdeckt ist (V), je +3. "erwartet" = Zahl dieser Figurpunkte im Kasten-Modell (Hoehe 3000, +-350).
#   M = Figur HINTER dem Teil: die neue Maske verdeckt sie (H > 0) - vorher (Stand NB3) gab es dort keine Maske.
#   L = Figur VOR dem Teil (ueberlappt es im Bild): 0 Punkte der SPIELFIGUR uebermalt (Lauf D ohne Masken mit der Figur an A0 =
#       test_r37_masken36 aussen: erreichbarer Platz ausserhalb des Regionsvierecks; Gegner/3D-Objekte zaehlen nicht). V = dasselbe fuer
#       alle Figuren, wo es keinen Platz ausserhalb gibt (50B0 C2/C3). SMIN = Haelfte der erwarteten Punkte, wenn diese < 100 (L/V);
#       M: SMIN=0 (H > 0 setzt Punkte der Figur im Fenster voraus).
#   E = Elza (Raumvariante xxx1, Masken bytegleich). Kameras ohne Zeile: es gibt keine erreichbare Stelle (Dossier NB5).
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "t3040_03 3040 3 -14000 -6700 1 M 25 120 53 148 SMIN=0"   # erwartet 257
    "w3040_03 3040 3 -18500 -4300 1 L 30 91 103 201 A0=-14700,-26400"   # erwartet 2923
    "t3040_06 3040 6 -12700 -2800 1 M 64 128 81 137 SMIN=0"   # erwartet 39
    "w3040_06 3040 6 -13500 -9800 1 L 118 93 184 193 A0=-11400,-26400"   # erwartet 1668
    "t3040_13 3040 13 -8300 -13500 0 M 81 91 110 119 SMIN=0"   # erwartet 170
    "w3040_13 3040 13 -13000 -11700 1 L 96 125 143 174 A0=-26300,-300"   # erwartet 1022
    "w3040_01 3040 1 -25400 -2300 0 L 146 128 180 181 A0=-5600,-15000"   # erwartet 140
    "t3040_04 3040 4 -12700 -2800 1 M 226 83 248 104 SMIN=0"   # erwartet 129
    "w3040_04 3040 4 -19100 -6200 1 L 232 55 319 189 A0=-10300,-26400"   # erwartet 2276
    "t3040_07 3040 7 -18600 -12200 1 M 289 140 314 169 SMIN=0"   # erwartet 199
    "w3040_07 3040 7 -13200 -7600 1 L 43 113 94 225 A0=-14700,-26400"   # erwartet 919
    "t3040_11 3040 11 -21300 -13700 1 M 121 114 134 129 SMIN=0"   # erwartet 24
    "w3040_11 3040 11 -24200 -10100 0 L 110 101 154 205 A0=-5600,-26400"   # erwartet 908
    "t3040_02 3040 2 -22300 -11400 0 M 236 131 276 168 SMIN=0"   # erwartet 785
    "w3040_02 3040 2 -18700 -11400 1 L 208 130 272 175 A0=-5600,-26400"   # erwartet 1617
    "t3040_05 3040 5 -11700 -5800 1 M 189 101 208 147 SMIN=0"   # erwartet 265
    "w3040_05 3040 5 -16000 -2200 1 L 144 101 208 168 A0=-12500,-26400"   # erwartet 912
    "w3040_09 3040 9 -7800 -15200 0 L 224 66 253 204 A0=-26300,-300"   # erwartet 2384
    "t3040_12 3040 12 -9300 -5200 1 M 201 138 211 149 SMIN=0"   # erwartet 34
    "w3040_12 3040 12 -11100 -6700 1 L 260 91 305 146 A0=-11000,-26400"   # erwartet 337
    "t3040_02e 3040 2 -22300 -11400 0 M 236 131 276 168 E SMIN=0"   # erwartet 785
    "w3040_03e 3040 3 -18500 -4300 1 L 30 91 103 201 E A0=-14700,-26400"   # erwartet 2923
)
