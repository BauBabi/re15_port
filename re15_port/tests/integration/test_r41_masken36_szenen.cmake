# Runde 41 Spur M, Punkt 2 - Szenen fuer test_r41_masken36_stage2.cmake (Leon: Raum xxx0, Elza: Feld E -> xxx1 mit den
# Zwilling-Masken). Lagen: begehbarer Boden (Klemmpfad-Dump R40, probe_p2_floor_dump) mit der staerksten Ueberdeckung der
# Maskenflaeche VOR bzw. HINTER der Standlinie der Freistellung (.STAND), nur begehbare Baender (geom.begehbare_baender +
# Ebene der Freistellung, auswahl.json); Fenster = Projektion des Figurkastens 3000 x +-350 (+4). An der exe gemessen
# (Dossier 2.2, Sweep 98 Szenen): K 2020 C3 86/779, V 0, H 538/629, 440/502, 228/295; 20A0 C12 nach dem Fix 0.
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "k2020_03 2020 3 3650 -2925 1 K 66 78 104 161"
    "k2021_03 2020 3 3650 -2925 1 K 66 78 104 161 E"
    "v11A0_01 11A0 1 2825 18150 1 V 46 93 106 231"
    "v2030_11 2030 11 800 17050 6 V 211 79 254 184"
    "v2040_09 2040 9 5700 -10200 3 V 85 94 141 240"
    "v2050_00 2050 0 8900 0 3 V 144 56 206 240"
    "h11A0_01 11A0 1 3025 15950 1 H 33 81 81 186"
    "h11A1_01 11A0 1 3025 15950 1 H 33 81 81 186 E"
    "h2040_09 2040 9 4500 -16000 3 H 71 92 100 170"
    "h2080_03 2080 3 -9700 -9900 8 H 227 69 276 163"
    # ROOM20A0 Cut 12: Leon vor der Ziegelwand und AUF dem Bodenstreifen 12_02 (Dossier 2.3) - mit 12_02 30 bzw. 50 Leon-Punkte
    # uebermalt, ohne 0
    "v20A0_12 20A0 12 14000 -18900 4 V 83 82 117 154"
    "s20A0_12 20A0 12 14000 -19300 4 V 88 79 121 150"
    "s20A1_12 20A0 12 14000 -19300 4 V 88 79 121 150 E"
    # Nachbesserung 1, Mangel 3: ROOM2030/2031 Cut 15 (4400,3850) Band 3 - Leon/Elza VOR der Reling. Die Abnahme zaehlte dort
    # 12/24 "Figurpunkte" uebermalt: es sind Punkte der Spinne (Akteur 1, EM025) direkt HINTER Leon und HINTER der Reling
    # (RE15_TRILOG: Slot 11, Dreieck-z 9596..9964 >= Masken-Schluessel 8520..8905); Leons Dreiecke (z 6905..8911) liegen
    # nirgends unter einer Maske mit Schluessel <= z. Mit der Spinne weggestellt (G=) zaehlt S nur Leon/Elza: H == 0.
    # Gegenprobe m2030_15: dieselbe Stelle MIT Spinne - die Reling verdeckt die Spinne dahinter (H > 0).
    "v2030_15 2030 15 4400 3850 3 V 7 119 66 189 G=1,-20000,-20000,0"
    "v2031_15 2030 15 4400 3850 3 V 7 119 66 189 E G=1,-20000,-20000,0"
    "m2030_15 2030 15 4400 3850 3 M 7 119 66 189"
    # Nachbesserung 1, Mangel 4: ROOM2030 C7 / ROOM2040 C7 haben keine erreichbare Stelle VOR dem Gelaender mit der Figur im
    # Bild (Dossier NB1 Mangel 4: 3D-Sehstrahl gegen die Gelaenderzellen, 767 bzw. 2378 Plaetze, 0 VOR). Die Stellen, die das
    # Standlinien-Modell (.STAND) als VOR nennt, liegen HINTER dem Gelaender: dort verdeckt die Maske Leon (H > 0).
    "m2030_07 2030 7 4500 9150 3 M 145 79 167 139"
    "m2040_07 2040 7 -17500 -5600 3 M 63 28 120 222"
)
