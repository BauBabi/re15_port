#!/usr/bin/env bash
# =============================================================================
# Fingerabdruck eines ctest-Laufs aus Testing/Temporary/LastTest.log
# =============================================================================
# Zweck: zwei Laeufe derselben Suite vergleichbar machen (Bau auf dem Windows-
# Bindmount /src gegen Bau auf einer Container-Kopie, Dossier
# analysis/befunde_runde30/nachtrag-linux-bau.md). ctest zaehlt einen Test, der
# eine fehlende Datei mit "SKIP: ... fehlt" quittiert und 0 zurueckgibt, als
# BESTANDEN — die Summenzeile sieht ein stilles Ueberspringen nicht. Deshalb je Test:
#   <name> <Status> <Zahl der Ausgabezeilen mit Ueberspringen-/Fehlt-Muster> <Sekunden>
# und in <prefix>.skip jede dieser Zeilen woertlich (fuer diff).
# Aufruf: ctest_fingerprint.sh <LastTest.log> <prefix>
#   -> <prefix>.tsv, <prefix>.skip
# =============================================================================
set -euo pipefail
LOG="$1"; OUT="$2"
[[ -s "$LOG" ]] || { echo "ctest_fingerprint: $LOG fehlt oder ist leer" >&2; exit 1; }
awk -v skipf="$OUT.skip" '
    /^[0-9]+\/[0-9]+ Test: / { name = $3; n = 0; t = "?"; inout = 0; next }
    /^Output:$/              { inout = 1; next }
    /^<end of output>$/      { inout = 0; next }
    inout && /SKIP|[Ss]kip|fehlt|uebersprungen|übersprungen|nicht gefunden|nicht lesbar|not found|missing|No such file/ {
        n++; print name "\t" $0 > skipf; next }
    /^Test time = /          { t = $4; next }
    /^Test [A-Z][A-Za-z ]*\.$/ && name != "" {
        st = $0; sub(/^Test /, "", st); sub(/\.$/, "", st); gsub(/ /, "_", st)
        printf "%s\t%s\t%d\t%s\n", name, st, n, t; name = "" }
' "$LOG" | sort > "$OUT.tsv"
touch "$OUT.skip"
sort -o "$OUT.skip" "$OUT.skip"
echo "ctest_fingerprint: $(wc -l < "$OUT.tsv") Tests, $(awk -F'\t' '$3>0' "$OUT.tsv" | wc -l) mit Ueberspringen-/Fehlt-Zeilen ($(wc -l < "$OUT.skip") Zeilen), Status: $(cut -f2 "$OUT.tsv" | sort | uniq -c | tr -s ' ' | tr '\n' ' ')"
