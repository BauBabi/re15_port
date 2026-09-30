#!/usr/bin/env bash
# Spur B (Runde 34 Nacht): local_build.sh aufrufen, OHNE dass sein Rueckfall
# `taskkill //F //IM re15_pc.exe` (local_build.sh:298-299) jede re15_pc.exe der Maschine beendet —
# also die Mess- und Testlaeufe der parallelen Spuren. local_build.sh prueft `command -v powershell`
# unter seinem CLEAN_PATH (:147), in dem System32\WindowsPowerShell\v1.0 fehlt, und faellt dann auf den
# Namens-Kill zurueck. Eine EXPORTIERTE Shell-Funktion `powershell` ist fuer `command -v` sichtbar ->
# das Skript nimmt seinen Pfadfilter-Zweig (:293-297: nur re15_pc.exe DIESES Bauverzeichnisses).
# Vorbild: r34g_a/build/r34g_a/lb.sh (Parallelsitzung). local_build.sh selbst bleibt unveraendert.
set -u
powershell() { /c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe "$@"; }
export -f powershell
zweig="$(PATH=/usr/bin:/c/Windows/System32 bash -c 'if command -v powershell >/dev/null 2>&1 && command -v cygpath >/dev/null 2>&1; then echo powershell; else echo taskkill; fi')"
if [ "$zweig" != "powershell" ]; then
    echo "!!! lb.sh: Kill-Zweig waere '$zweig' - ABBRUCH (kein Fremd-Kill)" >&2
    exit 97
fi
echo "=== lb.sh: Kill-Zweig = $zweig (nur dieses Bauverzeichnis)"
cd /c/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch || exit 98
exec bash re15_port/tools/local_build.sh "$@"
