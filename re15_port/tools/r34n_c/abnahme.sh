#!/usr/bin/env bash
# Spur C (Runde 34 Nacht, BAU-Abnahme) — die Messlaeufe an der ECHTEN exe (Kopie re15_r34nc.exe,
# lauf.sh). Panel offen ab F500 (lauf.sh: DEBUG_JUMP 11F0, FIRE_AOT Slot 1 = sub16, Meldungen
# blaettern, "Ja"); Cursor startet in der Zelle von Schalter 8 (rechts, Zeile 3).
# Tempo: 200 Welteinheiten je Bild (sub02..05 @0x012F6.. Speed_set +-200); Zeilenabstand 2200
# (~11 Bilder), Spaltenabstand 7600 (~38 Bilder). A0.07 = Quadrat 2 Bilder (eine Kippung, 16 Bilder).
#
# Aufruf: abnahme.sh <lauf> [framedump-spec]   lauf = s1|s2|s3|s4
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator
L="$BAUM/re15_port/tools/r34n_c/lauf.sh"
OUT="$BAUM/build/r34n_c/bau"
FD="${2:-}"
case "$1" in
  # S1 = Abnahme 1 (Nutzersymptom): wie Ist-Lauf 2.2 — Schalter 7, dann 3 s UNTEN gehalten.
  s1) bash "$L" "$OUT/s1" 640 "U0.3,W0.2,A0.07,D3,W2" "${FD:-505-640/3:fd_}" ;;
  # S2 = Abnahme 2: 7, 9, 3, 1 ohne auf den Zeiger zu warten, dann 5 (= Loesung), danach 3 s
  # HOCH+QUADRAT gehalten (Endsperre, Auflage 7b) — erst NACH b* (W0.7), sonst kippte das gehaltene
  # Quadrat auf dem Weg nach oben Schalter 4 um —, dann warten bis nach "OK" / Cut 8 (Leck-Test 7a).
  s2) bash "$L" "$OUT/s2" 960 "U0.3,W0.2,A0.07,D0.75,W0.2,A0.07,L1.17,U0.4,W0.2,A0.07,U0.6,W0.2,A0.07,D1.35,W0.2,A0.07,W0.7,UA3,W6" "$FD" ;;
  # S3 = Abnahme 5: 3, 1, 5 (oben an) ; +2 (oben aus) ; -2 (oben an) ; +7 ; +9 (= Loesung, beide an).
  # Start Schalter 8 (z 22684) -> 3: L1.17 (35 Bilder, x -26554) ; 3 -> 1: U0.65 (20, z 26684) ;
  # 1 -> 5: D1.47 (44, z 17884) ; 5 -> 2: U1.1 (33, z 24484) ; 2 bleibt ; 2 -> 7: R1.3 (39, x -18754) ;
  # 7 -> 9: D0.75 (23, z 19884). Zwischen den Schaltern W0.8/W1.2 (Kippung 16 Bilder, Zelle wieder scharf).
  s3) bash "$L" "$OUT/s3" 1110 "L1.17,W0.2,A0.07,W0.8,U0.65,W0.2,A0.07,W0.8,D1.47,W0.2,A0.07,W1.2,U1.1,W0.2,A0.07,W1.2,A0.07,W1.2,R1.3,W0.2,A0.07,W0.8,D0.75,W0.2,A0.07,W6" "$FD" ;;
  # S4 = Abnahme 3 (Zielwechsel waehrend der Fahrt, Bildpaar): Schalter 9 (+30), gleich danach 10
  # (-60): Ziel 30 -> 0 mitten in der Fahrt; Schalter 8 -> 9: D0.37 ; 9 -> 10: D0.37.
  s4) bash "$L" "$OUT/s4" 640 "D0.37,W0.2,A0.07,D0.37,W0.2,A0.07,W4" "$FD" ;;
  *) echo "lauf?"; exit 2 ;;
esac
