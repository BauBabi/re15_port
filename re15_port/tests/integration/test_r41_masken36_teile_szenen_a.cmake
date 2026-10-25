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
    "t3040_01 3040 1 -21300 -13500 1 M 131 123 148 151"   # erwartet 86
    "w3040_01 3040 1 -25200 -2400 0 V 131 123 180 184"   # erwartet 506
    "t3040_02 3040 2 -22300 -11400 0 M 236 131 276 168"   # erwartet 785
    "w3040_02 3040 2 -18700 -11400 1 V 208 130 272 175"   # erwartet 1617
    "t3040_03 3040 3 -14000 -8100 1 M 67 106 84 135"   # erwartet 208
    "w3040_03 3040 3 -18600 -4400 1 V 39 88 115 201"   # erwartet 2773
    "t3040_04 3040 4 -12700 -2800 1 M 226 83 248 104"   # erwartet 129
    "w3040_04 3040 4 -19100 -6200 1 V 232 55 319 189"   # erwartet 2276
    "t3040_05 3040 5 -11700 -5800 1 M 189 101 208 147"   # erwartet 265
    "w3040_05 3040 5 -16000 -2200 1 V 144 101 208 168"   # erwartet 912
    "t3040_06 3040 6 -12700 -2800 1 M 64 128 81 137"   # erwartet 39
    "w3040_06 3040 6 -13500 -9800 1 V 118 93 184 193"   # erwartet 1668
    "t3040_07 3040 7 -18500 -12000 1 M 285 140 314 172"   # erwartet 201
    "w3040_07 3040 7 -13200 -7600 1 V 43 113 94 225"   # erwartet 919
    "w3040_09 3040 9 -7800 -15200 0 V 224 66 253 204"   # erwartet 2384
    "w3040_11 3040 11 -23600 -9900 0 V 80 101 121 141"   # erwartet 351
    "t3040_12 3040 12 -9300 -5200 1 M 201 138 211 149"   # erwartet 34
    "w3040_12 3040 12 -11100 -6700 1 V 260 91 305 146"   # erwartet 337
    "t3040_13 3040 13 -8300 -13500 0 M 81 91 110 119"   # erwartet 170
    "w3040_13 3040 13 -13000 -11700 1 V 96 125 143 174"   # erwartet 1022
    "t3040_02e 3040 2 -22300 -11400 0 M 236 131 276 168 E"   # erwartet 785
    "w3040_03e 3040 3 -18600 -4400 1 V 39 88 115 201 E"   # erwartet 2773
)
