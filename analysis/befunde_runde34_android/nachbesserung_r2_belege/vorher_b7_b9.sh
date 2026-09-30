#!/usr/bin/env bash
# Nachbesserung R2 - Nachmessung B7 (8.3-Pfad ohne readlink; instrumentierte Kopie des Pruefers, startet NICHTS)
# und B9 (check_binary_fresh stumm) am unveraenderten Werkzeugstand.
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
P=$B/analysis/befunde_runde34_android/pruefer_umgehung_r2_belege
W=$B/build/r34a/nb2
KURZ=/c/Users/MJOEDI~1/AppData/Local/MICROS~1/WINDOW~1
echo "B7 Sonde = python_finden.sh bis auf die zwei Startzeilen: $(diff $B/release/python_finden.sh $P/python_finden_SONDE.sh | grep -c '^[<>]') geaenderte Zeilen"
echo "--- B7 (a) PATH=<8.3 WindowsApps>, RE15_PYTHON_NUR_PATH=1:"
env -i PATH="$KURZ" RE15_PYTHON_NUR_PATH=1 OSTYPE=msys /usr/bin/bash $P/python_finden_SONDE.sh 2>&1 | sed 's/^/   /'
echo "--- B7 (d) RE15_PYTHON=<8.3>/python3.exe, PATH leer:"
env -i PATH="" RE15_PYTHON="$KURZ/python3.exe" OSTYPE=msys /usr/bin/bash $P/python_finden_SONDE.sh 2>&1 | sed 's/^/   /'
echo "--- B7 (b) wie (a), aber /usr/bin dahinter (readlink erreichbar):"
env -i PATH="$KURZ:/usr/bin" RE15_PYTHON_NUR_PATH=1 OSTYPE=msys /usr/bin/bash $P/python_finden_SONDE.sh 2>&1 | sed 's/^/   /'
echo "--- B9: check_binary_fresh ohne passenden Commit (Mini-Repo):"
rm -rf $W/mini_repo; mkdir -p $W/mini_repo/release; ( cd $W/mini_repo && git init -q && echo x > a.txt && git add a.txt && git -c user.name=t -c user.email=t@t commit -qm x ); touch $W/mini_repo/release/x.apk
/usr/bin/bash $P/frische_stumm_sonde.sh $W/mini_repo > $W/logs/vorher_b9.log 2>&1; echo "   EXIT=$? Ausgabe: $(tr '\n' '|' < $W/logs/vorher_b9.log)"
