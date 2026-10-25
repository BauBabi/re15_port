# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe a.
# Stellen = test_r37_masken36 neustellen <raum> <cut> <erstes> (dieselben erreichbaren Stehplaetze, dasselbe Figurraster und dieselbe
# Engine-Tabelle wie der Ausschluss): Fenster = Kasten der Figurpunkte, die NUR neue Rechtecke verdecken (M) bzw. der Figurpunkte auf neuen
# Rechtecken, von denen keiner verdeckt ist (V), je +3. "erwartet" = Zahl dieser Figurpunkte im Kasten-Modell (Hoehe 3000, +-350).
#   M = Figur HINTER dem Teil: die neue Maske verdeckt sie (H > 0) - vorher (Stand NB3) gab es dort keine Maske.
#   V = Figur VOR dem Teil (ueberlappt es im Bild): 0 Figurpunkte uebermalt.
#   E = Elza (Raumvariante xxx1, Masken bytegleich). Kameras ohne Zeile: es gibt keine erreichbare Stelle (Dossier NB5).
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "t3040_03 3040 3 -14000 -6700 1 M 25 120 53 148"   # erwartet 257
    "w3040_03 3040 3 -18500 -4300 1 V 30 91 103 201"   # erwartet 2923
    "t3040_06 3040 6 -12700 -2800 1 M 64 128 81 137"   # erwartet 39
    "w3040_06 3040 6 -13500 -9800 1 V 118 93 184 193"   # erwartet 1668
    "t3040_13 3040 13 -8300 -13500 0 M 81 91 110 119"   # erwartet 170
    "w3040_13 3040 13 -13000 -11700 1 V 96 125 143 174"   # erwartet 1022
    "w3040_01 3040 1 -25400 -2300 0 V 146 128 180 181"   # erwartet 140
    "t3040_04 3040 4 -12700 -2800 1 M 226 83 248 104"   # erwartet 129
    "w3040_04 3040 4 -19100 -6200 1 V 232 55 319 189"   # erwartet 2276
    "t3040_07 3040 7 -18600 -12200 1 M 289 140 314 169"   # erwartet 199
    "w3040_07 3040 7 -13200 -7600 1 V 43 113 94 225"   # erwartet 919
    "t3040_11 3040 11 -21300 -13700 1 M 121 114 134 129"   # erwartet 24
    "w3040_11 3040 11 -24200 -10100 0 V 110 101 154 205"   # erwartet 908
    "t3040_02 3040 2 -22300 -11400 0 M 236 131 276 168"   # erwartet 785
    "w3040_02 3040 2 -18700 -11400 1 V 208 130 272 175"   # erwartet 1617
    "t3040_05 3040 5 -11700 -5800 1 M 189 101 208 147"   # erwartet 265
    "w3040_05 3040 5 -16000 -2200 1 V 144 101 208 168"   # erwartet 912
    "w3040_09 3040 9 -7800 -15200 0 V 224 66 253 204"   # erwartet 2384
    "t3040_12 3040 12 -9300 -5200 1 M 201 138 211 149"   # erwartet 34
    "w3040_12 3040 12 -11100 -6700 1 V 260 91 305 146"   # erwartet 337
    "t3040_02e 3040 2 -22300 -11400 0 M 236 131 276 168 E"   # erwartet 785
    "w3040_03e 3040 3 -18500 -4300 1 V 30 91 103 201 E"   # erwartet 2923
)
