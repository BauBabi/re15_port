# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe b2.
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
    "w3050_13 3050 13 -19300 -18568 4 L 116 194 148 238 A0=-1400,1532"   # erwartet 489
    "t3050_01 3050 1 -25100 -12568 0 M 184 121 205 163 SMIN=0"   # erwartet 227
    "w3050_01 3050 1 -24400 -5768 0 L 245 156 303 227 A0=-1000,-9468"   # erwartet 1412
    "t3050_06 3050 6 -2900 -4368 4 M 191 2 206 21 SMIN=0"   # erwartet 124
    "w3050_06 3050 6 -7700 -11768 0 L 178 108 199 144 A0=-25100,-12568"   # erwartet 247
    "t3050_09 3050 9 -4300 -7668 4 M 150 143 167 175 SMIN=0"   # erwartet 189
    "w3050_09 3050 9 -6100 -3968 4 L 134 112 180 183 A0=-25100,-10168"   # erwartet 499
    "t4040_12 4040 12 -4000 4678 0 M 72 70 88 96 SMIN=0"   # erwartet 72
    "w4040_12 4040 12 -1800 5078 0 L 73 72 108 109 A0=-6700,-24822"   # erwartet 630
    "t3050_04e 3050 4 -16800 -19268 0 M 39 158 60 200 E SMIN=0"   # erwartet 494
    "w3050_04e 3050 4 -13500 -19268 0 L 0 157 60 224 E A0=-1400,1532"   # erwartet 2909
    "t4040_12e 4040 12 -4000 4678 0 M 72 70 88 96 E SMIN=0"   # erwartet 72
    "w4040_12e 4040 12 -1800 5078 0 L 73 72 108 109 E A0=-6700,-24822"   # erwartet 630
)
