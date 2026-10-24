# Runde 41 Spur M, Nachbesserung 2 (Abnahme 1, Mangel 1, Weg (a)) - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei):
# Generator-Objekte der Stages 3..6, die das 3D-Urteil des Ausschlusses jetzt zeichnet (Koerper der Modellzelle vor dem verdeckten
# Figurpunkt; tests/unit/gen_r41_masken36_zellen.inc, Dossier "Nachbesserung 2"). Lagen: Python-Figurraster auf den erreichbaren
# Plaetzen (test_r37_masken36 plaetze) - HINTER = meiste NEU verdeckte Punkte, DAVOR = staerkste Ueberdeckung ohne neu verdeckten
# Punkt; Fenster = Figurkasten 3000 x +-350 (+4), ans Bild geschnitten. An der exe gemessen (Dossier NB2, RE15_TRILOG, Leon =
# vorderstes Dreieck): DAVOR sichtbar uebermalt 0 -> 0; HINTER sichtbar verdeckt vorher -> nachher 6000 C5 0 -> 1387, 6000 C9
# 0 -> 437, 5050 C1 0 -> 76, 3040 C3 0 -> 20. Die 6000-HINTER-Stellen liegen auf Band 0 (Gleisebene) im Grundriss der Lok.
#   name raum cut x z band art x0 y0 x1 y1 [E] [G=...]
set(R41M_SZENEN
    "d6000_05 6000 5 15433 -4184 2 V 0 91 58 171"
    "d6000_05t 6000 5 14933 -4484 2 V 3 98 63 179"
    "d6000_07 6000 7 15133 -18684 2 V 56 114 128 221"
    "d6000_08 6000 8 12733 -18684 2 V 62 133 116 213"
    "h6000_05 6000 5 14233 -15484 0 H 278 151 320 203"
    "h6000_09 6000 9 14033 -14984 0 H 101 117 121 161"
    "h5050_01 5050 1 -21600 -24450 0 M 48 68 91 166"
    "h3040_03 3040 3 -21200 -12600 1 M 270 49 309 133"
    "e6001_05 6000 5 15433 -4184 2 V 0 91 58 171 E"
)
