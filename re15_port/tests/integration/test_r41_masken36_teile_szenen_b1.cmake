# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe b1.
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
    "t3050_04 3050 4 -16800 -19268 0 M 39 158 60 200 SMIN=0"   # erwartet 494
    "w3050_04 3050 4 -13500 -19268 0 L 0 157 60 224 A0=-1400,1532"   # erwartet 2909
    "t3050_07 3050 7 -3400 -9068 0 M 147 112 167 160 SMIN=0"   # erwartet 202
    "w3050_07 3050 7 -8300 -11768 0 L 172 109 219 179 A0=-24500,-22768"   # erwartet 1384
    "t3050_12 3050 12 -23700 -16068 0 M 282 197 298 239 SMIN=0"   # erwartet 429
    "w3050_12 3050 12 -22700 -18568 4 L 258 112 265 142 A0=-7600,1532 SMIN=4"   # erwartet 9
    "t3050_00 3050 0 -25100 -1568 0 M 164 162 180 173 SMIN=0"   # erwartet 42
    "w3050_00 3050 0 -24500 -8568 0 L 88 168 137 223 A0=-1400,-4268"   # erwartet 941
    "t3050_05 3050 5 -10800 -19268 0 M 264 145 276 173 SMIN=0"   # erwartet 156
    "w3050_05 3050 5 -16900 -19268 0 L 264 56 305 179 A0=-25100,1532"   # erwartet 612
    "t3050_08 3050 8 -3500 -4468 4 M 216 172 229 189 SMIN=0"   # erwartet 83
    "w3050_08 3050 8 -5500 -6868 4 L 193 136 240 229 A0=-24500,-22768"   # erwartet 599
    "t3050_13 3050 13 -22000 -20868 0 M 128 199 148 232 SMIN=0"   # erwartet 190
)
