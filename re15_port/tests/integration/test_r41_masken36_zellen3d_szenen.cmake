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
# Nachbesserung 3 (Abnahme 2): die NB2-Szenen d6000_05 / d6000_05t / e6001_05 ("DAVOR") sind HINTER-Stellen -> m6000_05 /
# m6000_05t / m6001_05, Art M (Figur HINTER dem Gegenstand, Maske wirkt: H > 0). Beleg: Zelle 12 @RDT+0x007EC =
# d8 4f c8 0f 4a e1 d6 e9 01 ff 00 23 (x -7862..12578 z -5674..-1634 Typ 1 Band 2) ist ein Wandblock bis unter die Decke (Cut 2: die
# linke Kante der Wandflaeche ist die Senkrechte (12578,-5674) vom Boden (230,189) bis zum oberen Bildrand (223,4) = 7000 ueber dem
# Band-2-Boden y = -2 * 0x708, Bild M_bilder/NB3/NB3_6000_05_ecke.png). Leon (15433,-4184) / (14933,-4484) steht im Gang zwischen
# Zelle 12 und Zelle 13 @RDT+0x007F8 (x >= 15903); der Sehstrahl Kamera C5 (8794,-9403,-7278) -> Leon kreuzt den Grundriss der Zelle 12
# bei x 12235..12578 / z -5674..-5514, 2500..3700 ueber dem Boden: 1574 von 1613 Leon-Punkten liegen HINTER dem Block (RE15_TRILOG,
# Dossier NB3). NB2 hatte die Plaetze als DAVOR gewaehlt, weil sein Modell des Traegers falsch war (R23 Zelle @0x007D4 IoU 0.08 Tiefe
# 187, R33/R35 Zelle 13 @0x007F8 Hoehe 150). Fuer die Traeger-Masken gibt es auf Band 2 keinen Platz VOR dem Block (0 von 3302
# sichtbaren Plaetzen, test_r37_masken36 plaetze 6000 5); DAVOR fuer Cut 5 = die eigene Stelle der Abnahme 2 (M_abnahme_2.md 1b
# d6000_05 (12233,-9984) Band 2: Leon 2500, unter Masken 1547, uebermalt 0 -> an der NB3-exe 0). Fenster d6000_05a = Leon-Dreiecke
# (RE15_TRILOG x 203..262 y 132..233) + 4, oben ohne Rand: in y 128..131 liegt die Kante von Slot 5 (z 16535; das 3D-Tor
# Obj_model_set[1] (20916,0,-18540) hat in Cut 5 vz ~17 100, Abnahme 2: hinter der Lokfront), die die Lokmaske dort verdeckt
# (gemessen: Fenster ab y 128 S 2704 / H 3, ab y 132 S 2648 / H 0; Leon selbst 0 uebermalt).
#   name raum cut x z band art x0 y0 x1 y1 [E] [G=...]
set(R41M_SZENEN
    "m6000_05 6000 5 15433 -4184 2 M 0 91 58 171"
    "m6000_05t 6000 5 14933 -4484 2 M 3 98 63 179"
    "d6000_05a 6000 5 12233 -9984 2 V 199 132 267 238"
    "d6000_07 6000 7 15133 -18684 2 V 56 114 128 221"
    "d6000_08 6000 8 12733 -18684 2 V 62 133 116 213"
    "h6000_05 6000 5 14233 -15484 0 H 278 151 320 203"
    "h6000_09 6000 9 14033 -14984 0 H 101 117 121 161"
    "m6001_05 6000 5 15433 -4184 2 M 0 91 58 171 E"
    # Zuordnung von Hand (tools/maske/zuordnung_r41.json): gemessen S/H (Spurbaum-exe, Kette k2)
    "d5020_06 5020 6 -1733 -14625 0 V 187 49 223 174"
    "h5020_06 5020 6 -4333 -15925 0 M 169 71 196 158"
    "h30A0_01 30A0 1 -19443 15363 0 H 53 71 91 164"
    "d3040_00 3040 0 -22600 -2000 0 V 173 84 205 186"
    "h5020_00 5020 0 -13633 -21625 0 H 181 69 207 147"
    "d5090_13 5090 13 20717 -21365 0 V 137 64 162 139"
    "h5090_13 5090 13 24417 -26365 0 H 247 63 275 128"
)
