# Runde 41 Spur Q Punkt 1 (Abnahme M 5 Mangel 1): Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei) an Teil 8 ROOM6010/6011 Cut 5.
# Stellen und Fenster gemessen (Dossier Q_restpunkte.md Punkt 1; Laeufe Q41/runs/op1, op2, op4, vorher ap1, ap2):
#   Z = Figur HINTER der Wange, im Fenster ist kein Figurpunkt zu sehen. q8h2 faellt im Stand NB5 durch (18 Punkte im Fenster sichtbar, Spaltentiefe).
#       Elza ist schmaler und steht weiter vorn: Fenster aus ihren Laeufen ep1/ep2 (die Punkte ausserhalb liegen vor der Westflaeche, wie bei Leon an der Abnahme-Stelle).
#   L = Figur VOR der Wange (ragt in das Teil): kein Punkt der Spielfigur uebermalt (Fenster ohne die 2 Punkte x 168/169 der Ferse des Kopfs an der Wange).
#   A0 = (20000,20000): 6010 C5 hat keinen erreichbaren Platz ausserhalb des Regionsvierecks (test_r37_masken36 aussen: "-"), Figur-Cull wie Lauf o6010_05 der Abnahme.
#   name raum cut x z band art x0 y0 x1 y1 [E]
set(R41M_SZENEN
    "q8h1 6010 5 -19612 -19720 0 Z 167 228 182 238 A0=20000,20000"
    "q8h2 6010 5 -19712 -19820 0 Z 160 200 178 232 A0=20000,20000 SMIN=30"
    "q8v 6010 5 -20112 -19620 0 L 154 230 167 239 A0=20000,20000 SMIN=10"
    "q8h1e 6010 5 -19612 -19720 0 Z 171 222 177 239 E A0=20000,20000 SMIN=30"
    "q8h2e 6010 5 -19712 -19820 0 Z 173 222 177 239 E A0=20000,20000 SMIN=20"
    "q8ve 6010 5 -20112 -19620 0 L 154 230 167 239 E A0=20000,20000 SMIN=10"
)
