#!/usr/bin/env bash
# Pruefer echtlauf r3: make_package.sh im ENDSTAND (eff38fc1) voll laufen lassen, mit einem STOLPERDRAHT vorn im PATH
# (python3/python/py: protokollieren jeden Aufruf samt Eltern und scheitern mit rc 97 - KEIN Shim, sie lassen nichts
# gelingen) und dem WindowsApps-Ordner VOR /c/Python310. Jeder Python-Aufruf am Finder vorbei traefe den Draht (und ohne
# Draht den WindowsApps-Alias); erwartet sind NUR die Probe-Aufrufe von python_finden.sh. git-Schreibzugriffe isoliert
# (mp_isoliert_r3.sh). Aufruf: bash stolperdraht_r3.sh; Rueckgabe = die von make_package.sh.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=$BAUM/build/r34a/pruefer_echtlauf_r3
D=/c/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_echtlauf_r3/stolperdraht
WA=/c/Users/mjoedicke/AppData/Local/Microsoft/WindowsApps
rm -rf "$D"; mkdir -p "$D"; : > "$W/stolperdraht.log"
for n in python3 python py; do
    cat > "$D/$n" <<'DRAHT'
#!/usr/bin/env bash
eltern="$(tr '\0\n' '  ' < /proc/$PPID/cmdline 2>/dev/null | cut -c1-200)"
printf '%s %s ARGS=[%.100s] ELTERN=[%s]\n' "$(date '+%T.%N' | cut -c1-12)" "$0" "$*" "$eltern" \
    >> C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_echtlauf_r3/stolperdraht.log
exit 97
DRAHT
    chmod +x "$D/$n"
done
rest="$(printf '%s' "$PATH" | tr ':' '\n' | grep -vxF -e "$D" -e /usr/bin -e /mingw64/bin -e "$WA" | paste -sd: -)"
export PATH="$D:/usr/bin:/mingw64/bin:$WA:$rest"
echo "PATH (Anfang): $(printf '%s' "$PATH" | tr ':' '\n' | head -8 | paste -sd' ' -)"
echo "type -a python3: $(type -a python3 2>&1 | head -3 | paste -sd';' -)"
LOG=$W/mp_stolper.log bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r3_belege/mp_isoliert_r3.sh" --version v0.8.19
rc=$?
echo "EXIT make_package: $rc"
echo "Drahtprotokoll ($(wc -l < "$W/stolperdraht.log") Eintraege):"
cat "$W/stolperdraht.log"
exit $rc
