# -*- coding: utf-8 -*-
"""NB3-Bau: exe-Riegel rampe_oben - Eingabe: Klettern und Gang auf die Rampe VOR den Schuessen (Auto-Aim dreht die Figur sonst vom Rampenrand weg), Kletterpose 650 vor der Kante."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/integration/test_r41_gatorfix.cmake"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
old = """    if(TEIL MATCHES "^rampe_oben")   # G2-RE2 NB3 (Vorgabe A): Messblatt NB3 1.3 / batch3.sh oben_leon|oben_elza
        set(_weg "W6.0,M0.6,MA0.1,M0.3,MA0.1,W14.0,U0.1,A0.2,W2.0,U1.2,W170")
        if(TEIL MATCHES "elza$")
            set(_extra "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" RE15_CLIMB_TRACE=climb.log)
        else()
            set(_extra "RE15_K2080_RUECK_LAGE=4200,-18800,3072,2" RE15_CLIMB_TRACE=climb.log)
        endif()
        set(_ende 4400)"""
new = """    if(TEIL MATCHES "^rampe_oben")   # G2-RE2 NB3 (Vorgabe A): Figur oben auf der Rampe, Messblatt NB3 1.3 (batch3.sh oben_leon|oben_elza), Eingabe angepasst:
        # Das Skript startet je Raum neu; im Rueckkehr-Besuch kommt der Alligator mit Vorgabe B schon ab ~F100 an die Kante (erster Biss F122-125) und das Zielen (M)
        # dreht die Figur per Auto-Aim vom Rampenrand weg (rot 3072 -> 1699, Probelaufe 1/2) - darum Klettern (U0.1,A0.2) und Gang auf die Rampe (U1.2) VOR die
        # Schuesse, nach der Szenen-Haltung der Rueckkehr (Cut 12, pst=1: Eingaben bis mindestens F9 verschluckt, Probelauf 3; ab F48 gemessen frei); die Zeit bis
        # zum ersten Schuss bleibt 180 Bilder (48 + 3 + 6 + 36 + 36 + 51 = 180 wie W6.0), damit die Kanister-Szene im ersten Besuch wie bisher ausloest.
        # Kletterpose 650 vor der Kante (z -18750; bei -18800 kein Klettern: Kletterzellen z >= -18000, test_r41_gatorfix kletterkarte 2090).
        set(_weg "W1.6,U0.1,A0.2,W1.2,U1.2,W1.7,M0.6,MA0.1,M0.3,MA0.1,W170")
        if(TEIL MATCHES "elza$")
            set(_extra "RE15_K2080_RUECK_LAGE=3600,-18750,3072,2" RE15_CLIMB_TRACE=climb.log)
        else()
            set(_extra "RE15_K2080_RUECK_LAGE=4200,-18750,3072,2" RE15_CLIMB_TRACE=climb.log)
        endif()
        set(_ende 4400)"""
assert s.count(old) == 1
s = s.replace(old, new)
old2 = """# TEIL=rampe_oben|rampe_oben_elza  G2-RE2 NB3 (Vorgabe A, Riegel A2): Eingabe der Abnahme-4-Laeufe west4_leon / west5_elza bis zum Klettern (A0.2 bei ~F639),
#     dann W2.0,U1.2 auf die Rampe und W170 stehen (RE15_K2080_RUECK_LAGE 4200,-18800 / 3600,-18750, Cut 2), 4400 Bilder, mit und ohne Kanister - obenlog:"""
new2 = """# TEIL=rampe_oben|rampe_oben_elza  G2-RE2 NB3 (Vorgabe A, Riegel A2): Rueckkehr 650 vor der Rampen-Suedkante (RE15_K2080_RUECK_LAGE 4200,-18750 / 3600,-18750,
#     Cut 2), nach der Szenen-Haltung Klettern (U0.1,A0.2 ab F48) und U1.2 auf die Rampe, dann die Schuesse wie in allen Raeumen (F180) und W170 stehen; 4400 Bilder, mit und ohne Kanister - obenlog:"""
assert s.count(old2) == 1
s = s.replace(old2, new2)
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
