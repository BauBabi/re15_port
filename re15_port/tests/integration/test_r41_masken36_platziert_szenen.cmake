# Runde 41 Spur M, Nachbesserung 4 (Abnahme 3, Mangel 2 "Restliste: Gelaender ohne Maske ..."): Szenen fuer test_r41_masken36_stage2.cmake
# (SZENEN=diese Datei) an den neu platzierten Kuenstler-Komponenten (tools/maske/platzieren_r41.py, Tabelle tests/unit/gen_r41_masken36_platziert.inc).
# Stellen = test_r37_masken36 suche 5 <stage> (staerkste HINTER-/DAVOR-Stelle je Cut nach dem Ausschluss), gemessen an der NB4-exe
# (Dossier NB4, eigene Laeufe m41nb4/runs): Fenster = Kasten der Leon-Punkte unter NEUEN Rechtecken (V) bzw. der von neuen Rechtecken
# verdeckten Leon-Punkte (M), je +3.
#   V = Figur VOR dem neuen Gegenstand (Gelaender, Kisten, Lampen): 0 Figurpunkte uebermalt.
#   M = Figur HINTER ihm: die neue Maske verdeckt sie (H > 0) - vorher (NB3) gab es dort keine Maske.
#   name raum cut x z band art x0 y0 x1 y1 [E] [W=gier] [A0=x,z]
set(R41M_SZENEN
    "d3040_03 3040 3 -18700 -4400 1 V 0 92 67 207"
    "d3040_09 3040 9 -7800 -15100 0 V 227 67 264 204"
    "d30E0_06 30E0 6 13800 4800 0 V 218 125 313 201"
    "d30E0_09 30E0 9 25300 12100 0 V 116 141 158 200"
    "d3050_11 3050 11 -12800 -18868 4 V 112 160 138 239"
    "d5090_12 5090 12 16417 -23365 0 V 186 63 226 98"
    "d6000_02 6000 2 14233 -484 2 V 99 144 168 216"
    "h3040_03 3040 3 -14000 -8200 1 M 72 106 84 131"
    "h3040_07 3040 7 -18900 -12800 1 M 292 143 309 166"
    "h30E0_08 30E0 8 24800 8300 0 M 136 118 144 154"
    "h50B0_03 50B0 3 -15400 -6628 0 M 211 156 218 199"
    "h6010_02 6010 2 -17412 -7920 0 M 133 92 146 160"
)
