#!/usr/bin/env bash
# G2 Abnahme 4: Messung aller eigenen Fensterlaeufe (runs/<neu|ohne>_<lauf>): mass_v4.py (neu gegen ohne), tragvergleich/traglog (Werkzeug des Bauers,
# test_r41_gatorfix.exe des eigenen Baus), Ereignisse aus gator_boss.log, Determinismus gegen die Bauer-Laeufe (scratchpad g2v4/runs, gleiche Eingabe).
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
A="$SP/g2abn4"; B="$SP/g2v4/runs"
TOOL="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/build/tests/unit/test_r41_gatorfix.exe"
export PATH="/c/msys64/mingw64/bin:$PATH"
cd "$A"
for L in west4_leon west5_elza west4_elza frei4_leon frei4_elza; do
  echo "== $L"
  for P in neu ohne; do
    R="runs/${P}_$L"
    echo "-- $P: $(cat $R/pid.txt | tr '\n' ' ') exe_md5=$(cat $R/exe_md5.txt) Framedumps=$(ls $R | grep -c '^fd_')"
    echo "   Ereignisse: $(grep -hE 'RAMMEN F|G2RE2 RUECKWAERTS an|G2RE2 TAUCHEN an|G2RE2 HAENGT|KANISTER angehaengt|MESSHAKEN' $R/gator_boss.log | cut -c1-70 | tr '\n' '|')"
    echo "   Kanister-Sitz-Zeilen: $(grep -c 'Kanister-Sitz' $R/debug.log), AUSSERHALB: $(grep -c 'Kanister F[0-9]* AUSSERHALB' $R/debug.log), Zeilen error/assert/abort: $(grep -ciE 'error|assert|abort' $R/debug.log)"
    BB="$B/f_${P}_$L"; [ -d "$BB" ] || BB="$B/${P}_$L"
    if [ -d "$BB" ]; then
      if diff <(grep '^TRAG F' $R/gator_trag.log | cut -d' ' -f2-7) <(grep '^TRAG F' $BB/gator_trag.log | cut -d' ' -f2-7) > /dev/null; then echo "   Determinismus: TRAG-Zeilen bitgleich mit dem Bauer-Lauf $(basename $BB) (exe $(cat $BB/exe_md5.txt 2>/dev/null))"; else echo "   Determinismus: WEICHT AB vom Bauer-Lauf $(basename $BB)"; fi
    fi
  done
  python -I werkzeug/mass_v4.py runs/neu_$L runs/ohne_$L
  python -I werkzeug/mass_v4.py runs/ohne_$L
  "$TOOL" tragvergleich runs/neu_$L runs/ohne_$L | grep tragvergleich; echo "   tragvergleich exit=${PIPESTATUS[0]}"
  for P in neu ohne; do "$TOOL" traglog runs/${P}_$L | grep traglog | sed "s/^/   $P /"; done
done
