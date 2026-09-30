#!/usr/bin/env bash
# Runde 4 (Kette B2): Mutanten MU0-MU8 der Gegenpruefung R3 mit deren UNVERAENDERTEM Werkzeug
# (pruefer_umgehung_r3_belege/r3_mutanten.py) aus dem AKTUELLEN Gate erzeugen, dazu MUP (Paket-sha-Vergleich
# aus, Muster aus r3_mp_ohne_selbsttest.sh), und je --selbsttest fahren (voll, kein Schnellmodus).
# Logik wie r3_selbsttests.sh, nur Arbeitsordner build/r34a/r4. Rueckgabe je Lauf selbst abgefangen.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; PY=/c/Python310/python; L=$W/logs; M=$W/mutanten; mkdir -p "$L"
rm -rf "$M"; mkdir -p "$M/MUP_paket_sha_aus"
"$PY" analysis/befunde_runde34_android/pruefer_umgehung_r3_belege/r3_mutanten.py release/apk_asset_gate.py "$M"
"$PY" - release/apk_asset_gate.py "$M/MUP_paket_sha_aus/apk_asset_gate.py" <<'PY'
import sys
t = open(sys.argv[1], encoding="utf-8", newline="").read()
alt = "        if q_sha != p_sha:\n"
assert t.count(alt) == 1
open(sys.argv[2], "w", encoding="utf-8", newline="").write(t.replace(alt, "        if False:\n"))
print("MUP_paket_sha_aus              geschrieben (paket_pruefen: sha256-Vergleich aus)")
PY
lauf() {   # $1 = Name, $2 = Gate-Datei
    local rc=0 t0 t1
    t0=$(date +%s)
    "$PY" "$2" --selbsttest > "$L/selbsttest_$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-30s rc=%s %4ss | %s\n' "$1" "$rc" "$((t1 - t0))" \
        "$(grep -a -E '^== SELBSTTEST' "$L/selbsttest_$1.log" | tail -1 | tr -d '\r')"
    grep -a -E '^   \[FEHLER\]' "$L/selbsttest_$1.log" | tr -d '\r' | cut -c1-110 | sed 's/^/      /'
}
echo "Gate: sha256 $(sha256sum release/apk_asset_gate.py | cut -c1-16)..."
lauf ECHT release/apk_asset_gate.py
for d in "$M"/*/; do lauf "$(basename "$d")" "$d/apk_asset_gate.py"; done
echo "FERTIG"
