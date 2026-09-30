#!/usr/bin/env bash
# Gegenpruefung R3: --selbsttest des echten Gates und jedes Mutanten (build/r34a/pruefer_r3/mutanten/*).
# Rueckgabe je Lauf selbst abgefangen (kein Pipe um den Aufruf). Ergebnis: logs/selbsttest_<name>.log +
# Uebersicht auf stdout. Interpreter ausdruecklich /c/Python310/python (nie der WindowsApps-Alias).
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; PY=/c/Python310/python; L=$W/logs; mkdir -p "$L"
lauf() {   # $1 = Name, $2 = Gate-Datei
    local rc=0 t0 t1
    t0=$(date +%s)
    "$PY" "$2" --selbsttest > "$L/selbsttest_$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-30s rc=%s %4ss | %s\n' "$1" "$rc" "$((t1 - t0))" \
        "$(grep -E '^== SELBSTTEST' "$L/selbsttest_$1.log" | tail -1)"
}
namen=("${@}")
[[ ${#namen[@]} -gt 0 ]] || { namen=(ECHT); for d in "$W"/mutanten/*/; do namen+=("$(basename "$d")"); done; }
for n in "${namen[@]}"; do
    if [[ "$n" == ECHT ]]; then lauf ECHT release/apk_asset_gate.py; else lauf "$n" "$W/mutanten/$n/apk_asset_gate.py"; fi
done
echo "FERTIG"
