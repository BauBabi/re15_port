# Runde 41 Spur M, Nachbesserung 1, Mangel 1 - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei): Objekte der
# Stages 3..6, deren Rechtecke der Ausschluss der Spur P (R37) bisher GANZ wegnahm und die jetzt die Tiefe ihrer Kollisionszelle
# tragen (re15_port/tools/maske/tiefe_zelle_r41.py, Dossier NB1 Mangel 1). Lagen: Python-Figurraster wie test_r37_masken36 auf den
# erreichbaren Plaetzen (test_r37_masken36 plaetze) - HINTER = meiste NEU verdeckte Punkte, DAVOR = staerkste Ueberdeckung der
# wieder aufgenommenen Rechtecke ohne verdeckten Figurpunkt; Fenster = Figurkasten 3000 x +-350 (+4), ans Bild geschnitten.
# An der exe gemessen (Dossier NB1 Mangel 1; RE15_TRILOG, Leon = vorderstes Dreieck, verdeckt = Maske mit Schluessel <= z):
# 88 DAVOR-Szenen neu uebermalt 0; HINTER sichtbar verdeckt vor -> nach NB1: 3050 C5 2 -> 740, 5100 C7 0 -> 1085,
# 5020 C7 98 -> 770, 4040 C6 0 -> 124, 30A0 C3 185 -> 336 (Schritt 1); 4040 C9 0 -> 278, 5100 C2 34 -> 756, 4040 C7 8 -> 201
# (Schritt 2, Fehltreffer erben).
#   name raum cut x z band art x0 y0 x1 y1 [E] [G=...]
set(R41M_SZENEN
    "h3050_05 3050 5 -11600 -11668 0 H 89 98 112 165"
    "d3050_05 3050 5 -18100 -16868 0 V 41 29 106 240"
    "h5100_07 5100 7 -14030 -2500 0 M 63 100 94 195"
    "d5100_07 5100 7 -12030 -5200 0 V 37 97 86 240"
    "h5020_07 5020 7 -13533 -14125 0 H 90 56 119 134"
    "d5020_07 5020 7 -10033 -20025 0 V 43 14 129 240"
    "h4040_06 4040 6 -23600 -422 0 H 138 76 152 109"
    "d30A0_03 30A0 3 -23543 16463 0 V 186 50 263 226"
    "h30A0_03 30A0 3 -19443 14663 0 M 191 35 227 128"
    # Schritt 2 (--erben)
    "h4040_09 4040 9 -14800 -12722 0 H 108 71 130 123"
    "d4040_09 4040 9 -9800 -5122 0 V 63 92 109 205"
    "h5100_02 5100 2 -23730 -21100 0 H 196 108 222 190"
    "d5100_02 5100 2 -19930 -23800 0 V 203 97 249 240"
    "h4040_07 4040 7 -15800 -7322 0 H 135 71 152 110"
    "d4040_07 4040 7 -6600 -21122 0 V 140 89 181 218"
    "d30A0_00 30A0 0 -20243 11863 0 V 183 91 233 230"
)
