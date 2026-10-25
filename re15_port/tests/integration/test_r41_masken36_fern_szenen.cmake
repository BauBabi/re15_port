# Runde 41 Spur M, Nachbesserung 3 (Abnahme 2, Mangel 1) - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei):
# die Buero-Trennwand ROOM30A0/30A1 Cut 0/1 (Zelle 6 @RDT+0x0045C, obere Felder von Hand zugeordnet, Teilrechtecke kacheln_r41.py).
# Z = Figur HINTER der Trennwand: im Fenster (Trennwandflaeche unter ihrer Oberkante, Spalten der Figur) ist KEIN Figurpunkt im
#     Endbild zu sehen (V == 0; A0 = Lauf mit Masken, Leon bei (-19800,19763) ausserhalb des Regionsvierecks von Cut 0 und 1 =
#     Akteur-Cull FUN_8002c18c, gemessen: keine Figur-Dreiecke). Stellen = die Spielstellen der Abnahme 2 (Gehlauf w30A0b, Cut 0
#     durchgehend; vorher sichtbar ueber dem oberen Feld s30A0_00 1072, u30A0_00 986, t30A0_00 858, Elza e30A1_00 704, h30A0_01 988).
# V = Figur VOR der Trennwand (d30A0_00/01 der Abnahme 2, FREI-Stelle der R37 f30A1_00): im Fenster 0 Punkte uebermalt (die
#     R37-Stelle f30A0_00 zeigt im Trennwand-Fenster nur 43 Figurpunkte - sie bleibt bei unit/integration_r37_masken36_verdeckung).
# Fenster = groesstes Rechteck der Trennwand-Region in den Spalten der Figur (aus den Dreiecken der Laeufe gerechnet, RE15_TRILOG).
#   name raum cut x z band art x0 y0 x1 y1 [E] [A0=x,z]
set(R41M_SZENEN
    "s30A0_00 30A0 0 -21743 16263 0 Z 269 86 301 169 A0=-19800,19763"
    "u30A0_00 30A0 0 -23000 16300 0 Z 243 83 270 154 A0=-19800,19763"
    "t30A0_00 30A0 0 -24300 16400 0 Z 224 82 249 143 A0=-19800,19763"
    "h30A0_00 30A0 0 -22343 16063 0 Z 251 84 281 162 A0=-19800,19763"
    "e30A1_00 30A0 0 -21743 16263 0 Z 270 86 298 167 E A0=-19800,19763"
    "h30A0_01 30A0 1 -19143 15863 0 Z 50 71 80 156 A0=-19800,19763"
    "d30A0_00 30A0 0 -21943 13663 0 V 203 81 239 154"
    "d30A0_01 30A0 0 -21343 13663 0 V 214 82 253 161"
    "f30A1_00 30A0 0 -24284 13657 0 V 176 78 197 136 E"
)
