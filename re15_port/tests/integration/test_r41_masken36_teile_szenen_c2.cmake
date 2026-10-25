# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe c2.
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
    "w5020_02 5020 2 -8233 -25525 0 L 189 149 207 211 A0=-1333,-13325"   # erwartet 734
    "w5020_09 5020 9 -12233 -13325 0 L 133 64 199 198 A0=3267,-18925"   # erwartet 495
    "t5090_11 5090 11 2217 -20365 0 M 238 117 256 168 SMIN=0"   # erwartet 341
    "w5090_11 5090 11 4817 -22365 0 L 236 117 282 189 A0=11417,11335"   # erwartet 1227
    "w5090_14 5090 14 18817 -22965 0 L 75 71 95 100 A0=-3383,11435"   # erwartet 233
    "w5100_05 5100 5 -9130 -9000 0 L 75 99 91 154 A0=-26430,-15600"   # erwartet 565
    "w30A0_01 30A0 1 -21243 10063 0 L 204 118 249 173 A0=-21943,17763"   # erwartet 501
    "t30A0_04 30A0 4 -24743 17663 0 M 192 111 217 153 SMIN=0"   # erwartet 634
    "w30A0_04 30A0 4 -22343 17763 0 L 191 81 240 206 A0=-23743,9563"   # erwartet 2399
    "t30E0_07 30E0 7 25800 8200 0 M 182 123 195 145 SMIN=0"   # erwartet 89
    "w30E0_07 30E0 7 7500 4400 0 L 32 126 97 188 A0=-30200,29600"   # erwartet 776
    "w5090_12 5090 12 16317 -22165 0 L 269 84 319 225 A0=11417,11335"   # erwartet 2859
)
