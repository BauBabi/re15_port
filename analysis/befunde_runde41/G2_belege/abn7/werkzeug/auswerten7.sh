#!/usr/bin/env bash
# Abnahme 7: alle Auswertungen in eine Datei (messung_abnahme7.txt): je eigenem Lauf gtrlog/aussenlog/obenlog (exe-Auswerter der Riegel, eigene Kopie der
# test_r41_gatorfix.exe), t7 (eigener Auswerter), dbl7 (Doppelbisse), kanister7 (Kanister-Lebenslauf); dazu die juengsten Arbeitsordner der exe-Riegel.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb4/abn7"
T="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/unit/test_r41_gatorfix_g2abn7.exe"
B="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/integration"
export PATH="/c/msys64/mingw64/bin:$PATH"
OUT="$SP/messung_abnahme7.txt"
{
echo "# Abnahme 7 - Messung (eigene Laeufe, Messkopie re15_pc_g2abn7.exe = Bau 6f4314fb; Auswerter-Kopie test_r41_gatorfix_g2abn7.exe) $(date '+%F %T')"
echo
echo "## 1. Eigene exe-Laeufe: Umgebung, Ende"
for d in "$SP"/runs/a7_*/; do n=$(basename "$d"); printf "%-26s exe %s | %s | Bilder %s\n" "$n" "$(cat "$d/exe_md5.txt" | cut -c1-8)" "$(cat "$d/pid.txt" | tr '\n' ' ')" "$(tail -1 "$d/state.log" 2>/dev/null | cut -d' ' -f1)"; done
echo
echo "## 2. exe-Auswerter der Riegel (gtrlog / aussenlog / obenlog ab0) je eigenem Lauf"
for d in "$SP"/runs/a7_*/; do n=$(basename "$d"); [ -f "$d/state.log" ] || continue; echo "== $n"; "$T" gtrlog "$d"; "$T" aussenlog "$d"; "$T" obenlog "$d" ab0 | tail -1; done
echo
echo "## 3. Eigener Auswerter t7 (state.log + climb.log): je Treffer Unterzustand, Dauer, Luftlinie, Sturz"
cd "$SP/werkzeug" && /c/Python310/python t7.py "$SP"/runs/a7_*/
echo
echo "## 4. Doppelbisse im selben Gator-Clip (dbl7) - eigene Laeufe und Bauer-Laeufe c4_*"
/c/Python310/python dbl7.py "$SP"/runs/a7_*/ "$SP"/../runs/c4_*/
echo
echo "## 5. Kanister-Lebenslauf (kanister7) - eigene Laeufe mit Szene"
/c/Python310/python kanister7.py "$SP"/runs/a7_*/
echo
echo "## 6. Juengste Arbeitsordner der exe-Riegel (integration_r41_gatorfix_*): gtrlog, dbl7, kanister7"
for wd in "$B"/r41_gatorfix_*_wd; do
  neu=$(ls -dt "$wd"/lauf_* 2>/dev/null | grep -v _ohne | head -1); [ -n "$neu" ] || continue
  for d in "$neu" "${neu}_ohne"; do [ -d "$d" ] || continue; echo "== $(basename "$wd") / $(basename "$d")"; "$T" gtrlog "$d" | cut -c1-420; /c/Python310/python dbl7.py "$d" | head -1 | cut -c1-200; /c/Python310/python kanister7.py "$d" | cut -c1-260; done
done
} > "$OUT" 2>&1
echo "geschrieben $OUT ($(wc -l < "$OUT") Zeilen)"
