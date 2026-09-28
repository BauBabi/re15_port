#!/usr/bin/env bash
# birkin_sonden_lauf.sh <ziel> [bauen] - baut (optional) und faehrt alle Birkin-/G5-/5090-Sonden,
# je Sonde eine Ausgabedatei <ziel>/<sonde>.out, dazu <ziel>/rc.txt.
# Dossier nachschliff-room5080.md 9.4: zwei Laeufe (Engine vor/nach dem Eingriff, z.B. per git stash
# der Engine-Dateien) und dann `cmp` je Datei = Vorher/Nachher-Vergleich des Endkampfs.
set -u
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"; mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
export PATH="/c/msys64/mingw64/bin:$PATH"
SONDEN="probe_r30_birkin_frost probe_r30_n_room5080 probe_5090_birkin probe_5090_birkin_verify probe_5090_lang probe_g5_boss probe_g5_tentakel probe_p2_birkin_g5 probe_p3_birkin_rest probe_r16_birkin_g5 probe_r16_sk_birkin_g5 probe_r17_birkin_1zu1 probe_r18_birkin_rest probe_r20_birkin_push probe_r21_tentakel_schub"
if [ "${2:-}" = "bauen" ]; then
  cmake --build "$BAUM/re15_port/build" --target $SONDEN > "$ZIEL/bau.log" 2>&1; echo "BAU=$?"
fi
rm -f "$ZIEL/rc.txt"
for s in $SONDEN; do
  ( cd "$BAUM/re15_port/build/tests/unit" && ./$s.exe > "$ZIEL/$s.out" 2> "$ZIEL/$s.err"; echo "$s rc=$?" ) >> "$ZIEL/rc.txt"
done
cat "$ZIEL/rc.txt"
