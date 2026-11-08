#!/usr/bin/env bash
# Abnahme 7 Schlussschritte nach dem exe-Riegel-Lauf: Gesamtauswertung neu (juengste Riegel-Ordner), Belege kopieren, ctest-Summen und Einzelzeiten ausgeben.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7"
B="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/analysis/befunde_runde41/G2_belege/abn7"
bash "$SP/werkzeug/auswerten7.sh"
cp "$SP/messung_abnahme7.txt" "$SP/ctest_exe.txt" "$B/"
cp "$SP/werkzeug/"*.sh "$SP/werkzeug/"*.py "$B/werkzeug/"
cp "$SP/riegel_wave"*.txt "$SP/haenglog_wave12.txt" "$SP/t7_"*.txt "$SP/dbl7_"*.txt "$SP/kanister7_ctest.txt" "$SP/riegel_ctest_elza_fertig.txt" "$B/laeufe/" 2>/dev/null
cp "$SP/welle"*_ausgabe.txt "$B/laeufe/" 2>/dev/null
echo "=== ctest exe Summen ==="
grep -E "tests passed|Total Test time|^exit=|^20[0-9][0-9]-" "$SP/ctest_exe.txt"
echo "=== Einzelzeiten ==="
grep -E "^ *[0-9]+/20 Test" "$SP/ctest_exe.txt" | sed -E 's/ +/ /g'
echo "=== rote Zeilen? ==="
grep -c "VERFEHLT" "$SP/ctest_exe.txt"; grep -E "\*\*\*Failed|Timeout|FATAL" "$SP/ctest_exe.txt" | head -5
echo "=== Statuszeilen r41_gatorfix (gtrlog-Teil) ==="
grep -E "r41_gatorfix [a-z0-9_]+: " "$SP/ctest_exe.txt" | sed -E 's/^[0-9]+: //' | grep -oE "^r41_gatorfix [a-z0-9_]+|NB4 mit Kanister: gtrlog: [0-9]+ RE2-Reaktionen \(vollstaendig [0-9]+: Sub 2 [0-9]+, Sub 3 [0-9]+; abgebrochen [0-9]+\); Abweichungen [0-9]+|-> ok|VERFEHLT" | paste -sd' ' | sed 's/r41_gatorfix /\nr41_gatorfix /g'
echo "=== re2gator ==="
grep -E "r41_re2gator: " "$SP/ctest_exe.txt" | sed -E 's/^[0-9]+: //' | cut -c1-300
echo "=== Doppelbisse in den Riegel-Ordnern (Abschnitt 6 der Messung) ==="
sed -n '/^## 6/,$p' "$SP/messung_abnahme7.txt" | grep -E "Treffer +[0-9]+, Paare" | grep -v "_ohne " | sed -E 's/.*Treffer +([0-9]+), Paare <= 25 Bilder: +([0-9]+), davon im selben Gator-Clip: +([0-9]+).*/\1 \3/' | awk '{t+=$1; p+=$2; n++} END {print n" Riegel-Ordner (mit): Treffer "t", Zweitbisse im selben Clip "p}'
du -sh "$B"
