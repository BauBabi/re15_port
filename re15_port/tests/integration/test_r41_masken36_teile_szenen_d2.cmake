# Runde 41 Spur M, Nachbesserung 5 (Abnahme 4 Mangel 1 = Entscheid M-E2): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an den
# eingesetzten Teilen des Original-Vordergrundatlas (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc), Gruppe d2.
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
    "w6010_13 6010 13 -14412 -22820 0 L 46 120 101 203 A0=-14712,-2720"   # erwartet 1442
    "w6030_10 6030 10 -21861 -24006 0 L 52 133 109 193 A0=-1861,-22706"   # erwartet 708
    "w6000_04 6000 4 2933 -7384 2 L 118 144 155 166 A0=7533,-22584"   # erwartet 108
    "w6000_08 6000 8 14133 -20584 2 L 157 124 188 158 A0=4033,2816"   # erwartet 504
    "t6010_03 6010 3 -22112 -7920 0 M 71 150 84 160 SMIN=0"   # erwartet 40
    "t6010_07 6010 7 -23512 -25220 0 M 130 183 143 192 SMIN=0"   # erwartet 30
    "t6000_05e 6000 5 16933 -8484 2 M 147 118 157 133 E SMIN=0"   # erwartet 57
    "w6000_05e 6000 5 13133 -9884 2 L 200 114 228 172 E A0=5833,-22584"   # erwartet 586
    "t6010_02e 6010 2 -18312 -7920 0 M 134 92 153 155 E SMIN=0"   # erwartet 715
    "w6010_06e 6010 6 -10712 -24620 0 L 161 15 222 174 E A0=-14712,-2720"   # erwartet 1674
    "w6030_10e 6030 10 -21861 -24006 0 L 52 133 109 193 E A0=-1861,-22706"   # erwartet 708
    # Rueckschritt-Suche NB5 (rueckschritt_r41.py): an (15433,-4584) Band 2 Cut 5 meldet der 450-Zylinder 3 neu verdeckte Punkte ohne Zelle davor
    # (Rand x 15883, Fusshoehe) unter den ALTEN Rechtecken 26/97/100 (Tiefe 129), die das Tuer-Band der Saaten wieder zeichnet - an der exe nur
    # die Spielfigur gezaehlt (Art L), Fenster um die gemeldeten Bildpunkte (50..57, 153..158)
    # r6000_05 (15433,-4584) Band 2 L: gemessen NB5 - 17 uebermalte Punkte der Spielfigur, ALLE unter dem alten Rechteck 22 (Tiefe 63, seit R37
    # gezeichnet) an der Wandecke (Leon steht hinter dem Wandblock Zelle 12 @RDT+0x007EC, Abnahme 3); die durch das Tuer-Band wieder
    # gezeichneten Rechtecke 26/97/100 uebermalen dort 0 Punkte - keine DAVOR-Stelle, Dossier NB5
)
