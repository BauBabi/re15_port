#!/usr/bin/env bash
# Stolperdraht (Pruefer echtlauf r2): jeder Aufruf wird protokolliert und scheitert mit rc 97.
# Erwartet sind NUR die Probe-Aufrufe von release/python_finden.sh; jeder andere Aufruf = ein
# Skriptteil ruft python3/python/py am Finder vorbei (unter Git-Bash waere das der WindowsApps-Alias).
LOGF="C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_echtlauf_r2/stolperdraht.log"
eltern="$(tr '\0\n' '  ' < /proc/$PPID/cmdline 2>/dev/null | cut -c1-160)"
printf '%s %s ARGS=[%.80s] ELTERN=[%s]\n' "$(date '+%T.%N' | cut -c1-12)" "$0" "$*" "$eltern" >> "$LOGF"
exit 97
