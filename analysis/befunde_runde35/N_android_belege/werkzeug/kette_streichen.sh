#!/usr/bin/env bash
# Runde 35 Spur N, Nachbesserung 1 (M2): Streich-Messung der bash-Urteilslogik (siehe kette_streichen.py).
# Aufruf: bash kette_streichen.sh <repo-wurzel> <ziel> [parallel=4]
# Junctions (re15_port, synchro; MSYS_NO_PATHCONV=1, sonst wird /J zum Pfad) werden NUR mit "cmd /c rmdir" entfernt - nie rm -rf (das liefe in den Baum hinein).
set -u
REPO="$1"; ZIEL="$2"; PAR="${3:-4}"
TEST="$REPO/re15_port/tests/unit/r35_android/test_r35_android_pruefkette.sh"
source "$REPO/release/python_finden.sh" > /dev/null 2>&1 || { echo "kein Python"; exit 9; }
mkdir -p "$ZIEL"
ids="$("$PY" "$(cygpath -w "$REPO/analysis/befunde_runde35/N_android_belege/werkzeug/kette_streichen.py")" \
       "$(cygpath -w "$REPO")" "$(cygpath -w "$ZIEL")" | tr -d '\r')" || exit 9
for id in $ids; do
    for j in re15_port synchro; do
        [[ -e "$ZIEL/$id/repo/$j" ]] || MSYS_NO_PATHCONV=1 cmd /c mklink /J "$(cygpath -w "$ZIEL/$id/repo/$j")"             "$(cygpath -w "$REPO/$j")" > /dev/null || { echo "Junction $id/$j nicht anlegbar"; exit 9; }
        [[ -d "$ZIEL/$id/repo/$j" ]] || { echo "Junction $id/$j fehlt"; exit 9; }
    done
done
lauf_eine() {
    local id="$1"
    bash "$TEST" "$ZIEL/$id/repo" "$ZIEL/$id/arbeit" > "$ZIEL/$id/kette.log" 2>&1
    echo "$?" > "$ZIEL/$id/rc.txt"
}
n=0
for id in $ids; do
    lauf_eine "$id" &
    n=$((n + 1))
    (( n % PAR == 0 )) && wait
done
wait
for id in $ids; do
    for j in re15_port synchro; do MSYS_NO_PATHCONV=1 cmd /c rmdir "$(cygpath -w "$ZIEL/$id/repo/$j")" > /dev/null 2>&1; done
done
bemerkt=0; gesamt=0
for id in $ids; do
    rc="$(cat "$ZIEL/$id/rc.txt")"
    f="$(grep -a -c '^FALSCH' "$ZIEL/$id/kette.log")"
    besch="$(cat "$ZIEL/$id/beschreibung.txt")"
    if [[ "$id" == B0 ]]; then
        printf '%-4s %-9s rc=%s FALSCH=%s  %s\n' "$id" "$([[ $rc == 0 ]] && echo KONTROLLE-OK || echo KONTROLLE-ROT)" "$rc" "$f" "$besch"
    else
        gesamt=$((gesamt + 1)); [[ "$rc" != 0 ]] && bemerkt=$((bemerkt + 1))
        printf '%-4s %-9s rc=%s FALSCH=%s  %s\n' "$id" "$([[ $rc != 0 ]] && echo BEMERKT || echo NICHT)" "$rc" "$f" "$besch"
    fi
    grep -a '^FALSCH' "$ZIEL/$id/kette.log" | head -3 | cut -c1-150 | sed 's/^/        /'
done
echo "SUMME: $bemerkt von $gesamt Aenderungen bemerkt"
