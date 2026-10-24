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
)
