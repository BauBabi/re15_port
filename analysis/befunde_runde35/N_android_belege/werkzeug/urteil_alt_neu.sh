#!/usr/bin/env bash
# Runde 35 Spur N: Gleichheit des ausgelagerten Urteils (release/gate_urteil.py) mit dem Heredoc bis Runde 34
# (apk_pruefen.sh gate_urteil @154a73c1) - auf allen festen Faellen des Urteils-Selbsttests und auf echten Gate-Ausgaben.
# Aufruf aus der Repo-Wurzel: bash <dies> <arbeitsordner> [echte-gate-ausgaben...]  (je Datei: modus:rc:pfad)
set -u
W="$1"; shift
rm -rf "$W" && mkdir -p "$W"
source release/python_finden.sh > /dev/null || exit 9
die() { echo "DIE: $*" >&2; exit 1; }
git show 154a73c1:release/apk_pruefen.sh > "$W/alt_apk_pruefen.sh"
source "$W/alt_apk_pruefen.sh"                 # gate_urteil = alter Heredoc, Mindestzahlen 258/132 (alter Stand)
"$PY" - "$W" <<'PY'
import os, sys
sys.path.insert(0, "release")
import importlib.util
spec = importlib.util.spec_from_file_location("gu", "release/gate_urteil.py")
gu = importlib.util.module_from_spec(spec); spec.loader.exec_module(gu)
w = sys.argv[1]
with open(os.path.join(w, "faelle.tsv"), "w", encoding="utf-8") as idx:
    for nr, (titel, modus, text, rc, ein, soll) in enumerate(gu._faelle(), 1):
        p = os.path.join(w, "f%03d.log" % nr)
        open(p, "w", encoding="utf-8", newline="").write(text)
        idx.write("%d|%s|%d|%s|%s\n" % (nr, modus, rc, ein or "-", titel))
PY
gleich=0; anders=0
while IFS='|' read -r nr modus rc ein titel; do
    [[ "$ein" == - ]] && ein=""
    a=0; GATE_SELBSTTEST_MIN_FAELLE=5 GATE_SELBSTTEST_MIN_INNEN=3 GATE_APK_EINTRAEGE="$ein" \
        gate_urteil "$modus" "$(apk_nativ "$W/$(printf 'f%03d.log' "$nr")")" "$rc" > /dev/null 2>&1 || a=$?
    n=0; "$PY" release/gate_urteil.py "$modus" "$(apk_nativ "$W/$(printf 'f%03d.log' "$nr")")" "$rc" 5 3 $ein > /dev/null 2>&1 || n=$?
    if [[ "$a" == "$n" ]]; then gleich=$((gleich + 1)); else anders=$((anders + 1)); echo "ANDERS f$nr $modus rc=$rc: alt $a, neu $n  ($titel)"; fi
done < "$W/faelle.tsv"
for spec in "$@"; do
    modus="${spec%%:*}"; rest="${spec#*:}"; rc="${rest%%:*}"; p="${rest#*:}"
    a=0; GATE_SELBSTTEST_MIN_FAELLE=258 GATE_SELBSTTEST_MIN_INNEN=132 gate_urteil "$modus" "$p" "$rc" > /dev/null 2>&1 || a=$?
    n=0; "$PY" release/gate_urteil.py "$modus" "$p" "$rc" 258 132 > /dev/null 2>&1 || n=$?
    if [[ "$a" == "$n" ]]; then gleich=$((gleich + 1)); echo "gleich echte Ausgabe $modus rc=$rc -> $a ($(basename "$p"))"; else anders=$((anders + 1)); echo "ANDERS echte Ausgabe $modus rc=$rc: alt $a, neu $n ($p)"; fi
done
echo "gleich $gleich, anders $anders"
