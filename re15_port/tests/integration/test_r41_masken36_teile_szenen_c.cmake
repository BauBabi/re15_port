# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe c.
# Stellen = test_r37_masken36 neustellen <raum> <cut> <erstes> (dieselben erreichbaren Stehplaetze, dasselbe Figurraster und dieselbe
# Engine-Tabelle wie der Ausschluss): Fenster = Kasten der Figurpunkte, die NUR neue Rechtecke verdecken (M) bzw. der Figurpunkte auf neuen
# Rechtecken, von denen keiner verdeckt ist (V), je +3. "erwartet" = Zahl dieser Figurpunkte im Kasten-Modell (Hoehe 3000, +-350).
#   M = Figur HINTER dem Teil: die neue Maske verdeckt sie (H > 0) - vorher (Stand NB3) gab es dort keine Maske.
#   V = Figur VOR dem Teil (ueberlappt es im Bild): 0 Figurpunkte uebermalt.
#   E = Elza (Raumvariante xxx1, Masken bytegleich). Kameras ohne Zeile: es gibt keine erreichbare Stelle (Dossier NB5).
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "w30A0_02 30A0 2 -19443 14563 0 V 1 100 19 152"   # erwartet 30
    "w30A0_05 30A0 5 -25743 17763 0 V 181 140 214 186"   # erwartet 373
    "t30E0_08 30E0 8 25300 10700 0 M 100 119 118 150"   # erwartet 239
    "w30E0_08 30E0 8 18300 4700 0 V 87 118 144 170"   # erwartet 810
    "w5020_07 5020 7 -10433 -19825 0 V 57 93 101 144"   # erwartet 907
    "w5050_03 5050 3 -17800 -22950 0 V 238 178 266 215"   # erwartet 238
    "w5090_13 5090 13 13917 -22565 0 V 52 136 103 200"   # erwartet 1649
    "w50B0_03 50B0 3 -18100 -11928 0 V 58 141 95 188"   # erwartet 851
    "t30A0_03 30A0 3 -22943 17163 0 M 143 166 164 206"   # erwartet 540
    "w30E0_06 30E0 6 10000 5200 0 V 150 121 177 155"   # erwartet 242
    "w30E0_09 30E0 9 25000 12100 0 V 107 141 148 222"   # erwartet 592
    "w5020_02 5020 2 -8233 -25525 0 V 189 149 207 211"   # erwartet 734
    "w5020_09 5020 9 -12233 -13325 0 V 133 64 199 198"   # erwartet 495
    "t5090_11 5090 11 2217 -20365 0 M 238 117 256 168"   # erwartet 341
    "w5090_11 5090 11 4817 -22365 0 V 236 117 282 189"   # erwartet 1227
    "w5090_14 5090 14 18817 -22965 0 V 75 71 95 100"   # erwartet 233
    "w5100_05 5100 5 -9130 -9000 0 V 75 99 91 154"   # erwartet 565
    "w30A0_01 30A0 1 -21243 10063 0 V 204 118 249 173"   # erwartet 501
    "t30A0_04 30A0 4 -24743 17663 0 M 192 111 217 153"   # erwartet 634
    "w30A0_04 30A0 4 -22343 17763 0 V 191 81 240 206"   # erwartet 2399
    "t30E0_07 30E0 7 25800 8200 0 M 182 123 195 145"   # erwartet 89
    "w30E0_07 30E0 7 7500 4400 0 V 32 126 97 188"   # erwartet 776
    "w5090_12 5090 12 16317 -22165 0 V 269 84 319 225"   # erwartet 2859
    "w50B0_02 50B0 2 -14600 -18728 0 V 101 147 145 185"   # erwartet 1028
    "w30A0_04e 30A0 4 -22343 17763 0 V 191 81 240 206 E"   # erwartet 2399
    "t30E0_08e 30E0 8 25300 10700 0 M 100 119 118 150 E"   # erwartet 239
    "w30E0_08e 30E0 8 18300 4700 0 V 87 118 144 170 E"   # erwartet 810
    "w5020_07e 5020 7 -10433 -19825 0 V 57 93 101 144 E"   # erwartet 907
    "w5050_03e 5050 3 -17800 -22950 0 V 238 178 266 215 E"   # erwartet 238
    "w5090_12e 5090 12 16317 -22165 0 V 269 84 319 225 E"   # erwartet 2859
    "w50B0_02e 50B0 2 -14600 -18728 0 V 101 147 145 185 E"   # erwartet 1028
    "t30A0_04e 30A0 4 -24743 17663 0 M 192 111 217 153 E"   # erwartet 634
    "t5090_11e 5090 11 2217 -20365 0 M 238 117 256 168 E"   # erwartet 341
    "w5100_05e 5100 5 -9130 -9000 0 V 75 99 91 154 E"   # erwartet 565
)
