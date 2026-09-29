#!/usr/bin/env bash
# Gegenproben Runde 33 / Thema K: je eine Mutation am Bau, Build, die drei Riegel; danach
# wird die Datei aus git zurueckgeholt. Erwartet: jede Mutation faellt an ihrem Riegel.
set -u
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
cd "$ROOT"
T="$ROOT/re15_port/build/tests/unit/test_r33_karte.exe"
mut() {  # $1 Name, $2 Datei, $3 python-Ersetzung (alt), $4 (neu)
    python - "$2" "$3" "$4" <<'PY'
import sys
p, a, b = sys.argv[1], sys.argv[2], sys.argv[3]
s = open(p, 'rb').read().decode('utf-8')
assert s.count(a) == 1, (p, a, s.count(a))
open(p, 'wb').write(s.replace(a, b).encode('utf-8'))
PY
    bash re15_port/tools/local_build.sh build > /dev/null 2>&1 || { echo "$1: BUILD FEHLER"; git checkout -- "$2"; return; }
    r=""
    for m in etage markierung speicher; do
        PATH="/c/msys64/mingw64/bin:$PATH" "$T" $m > /dev/null 2>&1; r="$r $m=$?"
    done
    echo "$1:$r"
    git checkout -- "$2"
}
mut "M1 Freigabe weg"      re15_port/engine/src/map_hint_common.c \
    "return re15_map_page_known(page) || re15_map_ziel_blatt_frei(page);" "return re15_map_page_known(page);"
mut "M2 Zielkachel weg"    re15_port/engine/src/re15_inv_screen.c \
    "if (!st->hint_aktiv && st->ziel_aktiv &&" "if (0 && !st->hint_aktiv && st->ziel_aktiv &&"
mut "M3 Hinweis-Ton"       re15_port/engine/src/map_hint_common.c \
    "while (due--) { (void)zaehl_schritt(&s_zb_zaehler, &s_zb_richtung); s_zb_schritte++; }" \
    "while (due--) { if (zaehl_schritt(&s_zb_zaehler, &s_zb_richtung)) re15_audio_re2_hint_se(RE2_HINT_SE); s_zb_schritte++; }"
mut "M4 Besuch egal"       re15_port/engine/src/map_hint_common.c \
    "if (!zn || re15_map_zone_visited(zn)) continue;   /* erreicht -> aus */" "if (!zn) continue;"
bash re15_port/tools/local_build.sh build > /dev/null 2>&1
r=""; for m in etage markierung speicher; do PATH="/c/msys64/mingw64/bin:$PATH" "$T" $m > /dev/null 2>&1; r="$r $m=$?"; done
echo "unveraendert:$r"
