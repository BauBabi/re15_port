#!/usr/bin/env bash
# Nachbesserung R2 - Stolperdraht vorn im PATH (python3/python/py protokollieren + rc 97), dahinter
# /usr/bin, /mingw64/bin, WindowsApps (Langname) VOR dem Rest: jeder Aufruf ausser den Proben des Finders
# waere ein Aufruf am Finder vorbei und traefe ohne Draht den WindowsApps-Alias.
#  (1) Direktaufruf ./release/apk_asset_gate.py --quellbaum (echtlauf B1 - vorher landete er beim Draht)
#  (2) Direktaufruf ./release/zip_exec_bit.py (ohne Argumente: Hilfe)
#  (3) build_android.sh --gate-only <referenz>
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$B"
L=build/r34a/nb2/logs/stolperdraht.log; : > $L
D=/c/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/nb2/stolperdraht   # POSIX: "C:/..." zerlegte PATH am ':' (Lauf 1: Draht nie im PATH)
WA=/c/Users/mjoedicke/AppData/Local/Microsoft/WindowsApps
P="$D:/usr/bin:/mingw64/bin:$WA:$PATH"
echo "--- (1) ./release/apk_asset_gate.py --quellbaum"
env PATH="$P" /usr/bin/bash -c './release/apk_asset_gate.py --quellbaum' > build/r34a/nb2/logs/stolper_gate_direkt.log 2>&1; echo "    rc=$? | $(grep -E '^   Python:|QUELLBAUM-OK|FEHLER' build/r34a/nb2/logs/stolper_gate_direkt.log | tr '\n' ' ')"
echo "--- (2) ./release/zip_exec_bit.py"
env PATH="$P" /usr/bin/bash -c './release/zip_exec_bit.py' > build/r34a/nb2/logs/stolper_zipbit_direkt.log 2>&1; echo "    rc=$? | $(grep -E '^   Python:|Ausfuehrungsbit' build/r34a/nb2/logs/stolper_zipbit_direkt.log | head -2 | tr '\n' ' ')"
echo "--- (3) build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19"
env PATH="$P" /usr/bin/bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19 > build/r34a/nb2/logs/stolper_gateonly.log 2>&1; echo "    EXIT=$? | $(grep -E 'verworfen|^   Python:|ANDROID-GATES-OK|^ABBRUCH' build/r34a/nb2/logs/stolper_gateonly.log | cut -c1-120 | tr '\n' '|')"
echo "--- Drahtprotokoll ($(grep -c '' $L) Eintraege):"
sed 's/^/    /' $L
