# Runde 41 Spur M, Nachbesserung 2 (Abnahme 1, Mangel 1, Weg (a)) - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei):
# Generator-Objekte der Stages 3..6, die das 3D-Urteil des Ausschlusses jetzt zeichnet (Koerper der Modellzelle vor dem verdeckten
# Figurpunkt; tests/unit/gen_r41_masken36_zellen.inc, Dossier "Nachbesserung 2"). Lagen: Python-Figurraster auf den erreichbaren
# Plaetzen (test_r37_masken36 plaetze) - HINTER = meiste NEU verdeckte Punkte, DAVOR = staerkste Ueberdeckung ohne neu verdeckten
# Punkt; Fenster = Figurkasten 3000 x +-350 (+4), ans Bild geschnitten. An der exe gemessen (Dossier NB2, RE15_TRILOG, Leon =
# vorderstes Dreieck): DAVOR sichtbar uebermalt 0 -> 0; HINTER sichtbar verdeckt vorher -> nachher 6000 C5 0 -> 1387, 6000 C9
# 0 -> 437. Die 6000-HINTER-Stellen liegen auf Band 0 (Gleisebene) im Grundriss der Lok. (5050 C1 und 3040 C3 sind seit der
# Nahfeld-Sperre - Kamera ueber dem Grundriss der Zelle - wieder aus.)
# Zuordnung von Hand (S/H gemessen): X-Kiste 5020 C6 DAVOR 1903/0, HINTER 1081/63; Trennwand 30A0 C1 HINTER 1675/889; Schaltkasten
# 3040 C0 DAVOR 1489/0; Treppenraum 5020 C0 HINTER 751/606; Kistenstapel 5090 C13 DAVOR 945/0, HINTER 739/732.
#   name raum cut x z band art x0 y0 x1 y1 [E] [G=...]
set(R41M_SZENEN
    "d6000_05 6000 5 15433 -4184 2 V 0 91 58 171"
    "d6000_05t 6000 5 14933 -4484 2 V 3 98 63 179"
    "d6000_07 6000 7 15133 -18684 2 V 56 114 128 221"
    "d6000_08 6000 8 12733 -18684 2 V 62 133 116 213"
    "h6000_05 6000 5 14233 -15484 0 H 278 151 320 203"
    "h6000_09 6000 9 14033 -14984 0 H 101 117 121 161"
    "e6001_05 6000 5 15433 -4184 2 V 0 91 58 171 E"
    # Zuordnung von Hand (tools/maske/zuordnung_r41.json): gemessen S/H (Spurbaum-exe, Kette k2)
    "d5020_06 5020 6 -1733 -14625 0 V 187 49 223 174"
    "h5020_06 5020 6 -4333 -15925 0 M 169 71 196 158"
    "h30A0_01 30A0 1 -19443 15363 0 H 53 71 91 164"
    "d3040_00 3040 0 -22600 -2000 0 V 173 84 205 186"
    "h5020_00 5020 0 -13633 -21625 0 H 181 69 207 147"
    "d5090_13 5090 13 20717 -21365 0 V 137 64 162 139"
    "h5090_13 5090 13 24417 -26365 0 H 247 63 275 128"
)
