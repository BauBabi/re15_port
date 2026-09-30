#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1, H1/H4: Selbsttest jedes Mutanten DIREKT - Exit-Code (das, was make_package.sh/apk_pruefen.sh
# allein auswerten) neben der gedruckten Aussage. Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python; M=build/r34a/pruefer_u1/mut; L=build/r34a/pruefer_u1/logs/selbst; mkdir -p "$L"
lauf() {   # $1 = Name, $2 = Gate-Datei
    local rc=0 t0 t1
    t0=$(date +%s)
    "$PY" "$2" --selbsttest > "$L/$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-28s EXIT=%s %4ss  Bytes-Ausgabe=%-6s OK-Zeile=%s  FEHLER-Zeile=%s  [FEHLER]-Faelle=%s\n' "$1" "$rc" "$((t1 - t0))" \
        "$(wc -c < "$L/$1.log")" \
        "$(grep -a -c 'SELBSTTEST-OK' "$L/$1.log")" "$(grep -a -c 'SELBSTTEST-FEHLER' "$L/$1.log")" \
        "$(grep -a -c '\[FEHLER\]' "$L/$1.log")"
    grep -a -E 'SELBSTTEST-(OK|FEHLER)' "$L/$1.log" | tr -d '\r' | sed 's/^/      /' | cut -c1-160
}
echo "Gate Arbeitsbaum sha256 $(sha256sum release/apk_asset_gate.py | cut -c1-16)..."
lauf ECHT release/apk_asset_gate.py
for n in "$@"; do lauf "$n" "$M/$n/apk_asset_gate.py"; done
echo FERTIG
