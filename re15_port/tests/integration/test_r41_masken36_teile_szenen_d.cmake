# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe d.
# Stellen = test_r37_masken36 neustellen <raum> <cut> <erstes> (dieselben erreichbaren Stehplaetze, dasselbe Figurraster und dieselbe
# Engine-Tabelle wie der Ausschluss): Fenster = Kasten der Figurpunkte, die NUR neue Rechtecke verdecken (M) bzw. der Figurpunkte auf neuen
# Rechtecken, von denen keiner verdeckt ist (V), je +3. "erwartet" = Zahl dieser Figurpunkte im Kasten-Modell (Hoehe 3000, +-350).
#   M = Figur HINTER dem Teil: die neue Maske verdeckt sie (H > 0) - vorher (Stand NB3) gab es dort keine Maske.
#   V = Figur VOR dem Teil (ueberlappt es im Bild): 0 Figurpunkte uebermalt.
#   E = Elza (Raumvariante xxx1, Masken bytegleich). Kameras ohne Zeile: es gibt keine erreichbare Stelle (Dossier NB5).
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "w40A0_07 40A0 7 -17700 -24301 0 V 6 151 56 184"   # erwartet 1260
    "t6000_05 6000 5 16933 -8484 2 M 147 118 157 133"   # erwartet 57
    "w6000_05 6000 5 13133 -9884 2 V 200 114 228 172"   # erwartet 586
    "w6000_09 6000 9 6833 -20584 2 V 160 92 186 113"   # erwartet 77
    "t6010_05 6010 5 -13512 -9520 0 M 127 117 142 143"   # erwartet 85
    "t6010_12 6010 12 -26312 -7120 0 M 243 138 255 147"   # erwartet 31
    "w6010_12 6010 12 -14612 -6120 0 V 218 130 275 202"   # erwartet 556
    "w6000_07 6000 7 17633 -20684 2 V 262 157 301 211"   # erwartet 380
    "t6010_02 6010 2 -18312 -7920 0 M 134 92 153 155"   # erwartet 715
    "w6010_02 6010 2 -11312 -4520 0 V 130 82 175 173"   # erwartet 1569
    "t6010_06 6010 6 -17512 -20820 0 M 185 138 203 160"   # erwartet 223
    "w6010_06 6010 6 -10712 -24620 0 V 161 15 222 174"   # erwartet 1674
    "t6010_13 6010 13 -23812 -21020 0 M 81 121 100 152"   # erwartet 375
    "w6010_13 6010 13 -14412 -22820 0 V 46 120 101 203"   # erwartet 1442
    "w6030_10 6030 10 -21861 -24006 0 V 52 133 109 193"   # erwartet 708
    "w6000_04 6000 4 2933 -7384 2 V 118 144 155 166"   # erwartet 108
    "w6000_08 6000 8 14133 -20584 2 V 157 124 188 158"   # erwartet 504
    "t6010_03 6010 3 -22112 -7920 0 M 71 150 84 160"   # erwartet 40
    "t6010_07 6010 7 -23512 -25220 0 M 130 183 143 192"   # erwartet 30
    "w40A0_07e 40A0 7 -17700 -24301 0 V 6 151 56 184 E"   # erwartet 1260
    "t6000_05e 6000 5 16933 -8484 2 M 147 118 157 133 E"   # erwartet 57
    "w6000_05e 6000 5 13133 -9884 2 V 200 114 228 172 E"   # erwartet 586
    "t6010_02e 6010 2 -18312 -7920 0 M 134 92 153 155 E"   # erwartet 715
    "w6010_06e 6010 6 -10712 -24620 0 V 161 15 222 174 E"   # erwartet 1674
    "w6030_10e 6030 10 -21861 -24006 0 V 52 133 109 193 E"   # erwartet 708
)
