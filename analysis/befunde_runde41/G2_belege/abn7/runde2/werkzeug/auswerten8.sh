#!/usr/bin/env bash
# Abnahme 8: alle Auswertungen der eigenen exe-Laeufe in eine Datei (messung_abnahme8.txt). Eigene Werkzeuge a8.py / a8biss.py / a8schub.py / a8bahn.py /
# erst8.py; dazu die Riegel-Auswerter des Testprogramms (Kopie test_r41_gatorfix_g2abn8.exe): gtrlog, aussenlog, haenglog, obenlog.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn8"
T="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/unit/test_r41_gatorfix_g2abn8.exe"
A7="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7/runs"
A6="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6/runs"
B2="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/r2/runs"
PY=/c/Python310/python; export PATH="/c/msys64/mingw64/bin:$PATH"   # Auswerter mit der Windows-Python (matplotlib), Testprogramm mit mingw64-DLLs
W="$SP/werkzeug"
LAEUFE="a8_tod_leon a8_tod_elza a8_oben_leon a8_oben_elza a8_frei4_leon a8_frei4_elza a8_west4_leon a8_west4_elza a8_west5_elza a8_ohne_frei4_leon a8_ohne_oben_elza a8_aufn_elza a8_nische2 a8_k_leon_w4 a8_k_leon_w5 a8_k_leon_w7 a8_k_leon_w8 a8_k_elza_w4 a8_k_elza_w5 a8_k_elza_w7 a8_k_elza_w8 a8_k_leon_eingabe_elza a8_k_elza_eingabe_leon a8_spawn_nord_elza_r1024 a8_spawn_sued_leon_r1024 a8_spawn_ost_leon_r3072 a8_spawn_mitte_elza_r2048 a8_west5_elza_kletter a8_frei4_elza_schub"
echo "# Abnahme 8 - Messung $(date '+%F %T'), Messkopie re15_pc_g2abn8.exe md5 $(md5sum "C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/platform/pc/re15_pc_g2abn8.exe" | cut -c1-32)"
echo
echo "## 1. Laeufe (pid.txt / exe_md5 / Figur)"
for r in $LAEUFE; do echo "$r: $(cat $SP/runs/$r/pid.txt | tr '\n' ' ') md5 $(cat $SP/runs/$r/exe_md5.txt) $(grep -m1 -o 'Spieler-Familie PL0[04]' $SP/runs/$r/debug.log)"; done
echo
echo "## 2. Treffer und Reaktionen (a8.py: hp-Flanken, Abstand, gleicher Clip-Lauf, Sub/Dauer/Clips, Weg, Luftlinie, Sturz)"
for r in $LAEUFE; do "$PY" "$W/a8.py" "$SP/runs/$r" -v; done
echo
echo "## 3. Biss-Fenster (a8biss.py: Bissbedingung ohne Sperre erfuellt, nach dem Treffer bzw. ohne Treffer)"
for r in $LAEUFE; do "$PY" "$W/a8biss.py" "$SP/runs/$r" -v; done
echo
echo "## 4. Schub waehrend der Reaktion (a8schub.py) - eigene Laeufe und Abnahme-7-Laeufe (Bau 6f4314fb) mit gleicher Eingabe"
for r in $LAEUFE; do "$PY" "$W/a8schub.py" "$SP/runs/$r"; done
for r in a7_frei4_leon a7_frei4_elza a7_west4_leon a7_west4_elza a7_west5_elza a7_oben_leon a7_oben_elza; do "$PY" "$W/a8schub.py" "$A7/$r"; done
echo
echo "## 5. Kanister (a8bahn.py: Liegestelle, Stufe 1, Naehe Gator/Figur)"
for r in $LAEUFE; do "$PY" "$W/a8bahn.py" "$SP/runs/$r" "$SP/aus/bahn_$r.png" "$r" 2>/dev/null; done
echo
echo "## 6. Riegel-Auswerter des Testprogramms (gtrlog / aussenlog; haenglog mit; obenlog ab0 bzw. kampf)"
for r in $LAEUFE; do echo "== $r"; "$T" gtrlog "$SP/runs/$r" 2>&1 | tail -2; "$T" aussenlog "$SP/runs/$r" 2>&1 | tail -1;
  case $r in a8_tod_*|a8_nische2|a8_spawn_*|a8_west5_elza_kletter|a8_frei4_elza_schub) ;; *) "$T" haenglog "$SP/runs/$r" mit 2>&1 | tail -1;; esac
  case $r in a8_oben_*|a8_west5_elza|a8_ohne_oben_elza) "$T" obenlog "$SP/runs/$r" ab0 2>&1 | tail -1;; a8_spawn_*) "$T" obenlog "$SP/runs/$r" kampf 2>&1 | tail -1;; esac; done
echo
echo "## 7. Vergleich mit Abnahme 7 (Bau 6f4314fb), Abnahme 6 (Bau 5910fc24) und den Bauer-Laeufen Runde 2 (erst8.py: erste abweichende state.log-Zeile)"
for n in tod_leon oben_leon oben_elza frei4_leon frei4_elza west4_leon west4_elza west5_elza ohne_frei4_leon ohne_oben_elza; do echo "== a8_$n gegen Abnahme 7 a7_$n"; "$PY" "$W/erst8.py" "$SP/runs/a8_$n" "$A7/a7_$n" | tail -3; done
for n in ohne_frei4_leon ohne_oben_elza; do echo "== a8_$n gegen Abnahme 6 $n"; "$PY" "$W/erst8.py" "$SP/runs/a8_$n" "$A6/$n" | tail -3; done
for n in tod_leon tod_elza oben_leon oben_elza frei4_leon frei4_elza aufn_elza; do if [ -d "$B2/r2_$n" ]; then echo "== a8_$n gegen Bauer r2_$n: state.log $(cmp -s $SP/runs/a8_$n/state.log $B2/r2_$n/state.log && echo bytegleich || echo VERSCHIEDEN), gator_boss.log $(cmp -s $SP/runs/a8_$n/gator_boss.log $B2/r2_$n/gator_boss.log && echo bytegleich || echo VERSCHIEDEN)"; fi; done
