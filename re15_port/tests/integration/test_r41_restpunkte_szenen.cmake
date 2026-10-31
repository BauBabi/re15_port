# Runde 41 Spur Q Punkt 1 (Abnahme M 5 Mangel 1, Nachbesserung 1 = Abnahme 0 Mangel M1): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an Teil 8 ROOM6010/6011 Cut 5.
# Teil 8 ist die Aussenflaeche der Westwange (Ebene x -20591, tools/maske/wange_r41.py). Alle Stellen liegen oestlich davon, also HINTER der Wange: Z = im Fenster ist kein Figurpunkt zu sehen.
# Figur VOR der Wange auf dem Teil gibt es nicht (Riegel unit_r41_restpunkte_teil8_davor, test_r37_masken36 neustellen: V -), die frueheren Szenen q8v/q8ve standen hinter der Wange.
# Fenster gemessen (Dossier Abschnitt "Nachbesserung 1", Laeufe Q41n1/runs, tools/fenster.py): groesstes Rechteck, in dem jeder Figurpunkt auf dem Teil liegt, SMIN darunter.
# q8a = Stelle des Mangels (-19912,-19720): vorher 63 von 96 sichtbar, q8b 50 von 141, q8c 58 von 61.
#   A0 = (20000,20000): 6010 C5 hat keinen erreichbaren Platz ausserhalb des Regionsvierecks (test_r37_masken36 aussen: "-"), Figur-Cull wie Lauf o6010_05 der Abnahme.
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "q8a 6010 5 -19912 -19720 0 Z 167 228 175 239 A0=20000,20000 SMIN=40"
    "q8b 6010 5 -19812 -19620 0 Z 167 225 178 239 A0=20000,20000 SMIN=60"
    "q8c 6010 5 -20012 -19820 0 Z 167 231 174 239 A0=20000,20000 SMIN=30"
    "q8d 6010 5 -20112 -19920 0 Z 168 235 175 240 A0=20000,20000 SMIN=15"
    "q8h1 6010 5 -19612 -19720 0 Z 167 228 183 239 A0=20000,20000"
    "q8h2 6010 5 -19712 -19820 0 Z 167 226 179 239 A0=20000,20000"
    "q8ea 6010 5 -19812 -19620 0 Z 166 228 172 238 E A0=20000,20000 SMIN=25"
    "q8eb 6010 5 -19612 -19720 0 Z 168 227 178 240 E A0=20000,20000 SMIN=50"
    "q8ec 6010 5 -19712 -19820 0 Z 168 230 178 240 E A0=20000,20000 SMIN=40"
    "q8ed 6010 5 -20012 -19820 0 Z 168 235 173 240 E A0=20000,20000 SMIN=10"
)
