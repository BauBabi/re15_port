# Runde 41 Spur M, Nachbesserung 1, Mangel 1 - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei): Objekte der
# Stages 3..6, deren Rechtecke der Ausschluss der Spur P (R37) bisher GANZ wegnahm und die jetzt die Tiefe ihrer Kollisionszelle
# tragen (re15_port/tools/maske/tiefe_zelle_r41.py, Dossier NB1 Mangel 1). Lagen: Python-Figurraster wie test_r37_masken36 auf den
# erreichbaren Plaetzen (test_r37_masken36 plaetze) - HINTER = meiste NEU verdeckte Punkte, DAVOR = staerkste Ueberdeckung der
# wieder aufgenommenen Rechtecke ohne verdeckten Figurpunkt; Fenster = Figurkasten 3000 x +-350 (+4), ans Bild geschnitten.
# An der exe gemessen (Dossier NB1 Mangel 1, Leon-Dreiecke per RE15_TRILOG): DAVOR neu uebermalt 0; HINTER neu verdeckt
# 3050 C5 893, 5100 C7 982, 5020 C7 668, 4040 C6 124, 30A0 C3 151 (vor NB1: 2 / 0 / 98 / 0 / 185 verdeckt).
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
)
