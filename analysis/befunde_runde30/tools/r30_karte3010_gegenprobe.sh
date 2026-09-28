#!/usr/bin/env bash
# r30_karte3010_gegenprobe.sh - fallen die Riegel unit_r30_hinweis_* am falschen Code?
# Je Mutation: Quelle aendern, test_r30_hinweis bauen, den zustaendigen Riegel fahren,
# Quelle per git checkout zurueck. Erwartet: jeder Riegel FAELLT (rc != 0).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
cd "$ROOT" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
T="$ROOT/re15_port/build/tests/unit/test_r30_hinweis.exe"

mut() {   # $1 Name, $2 Datei, $3 alt, $4 neu, $5 Riegel
    python - "$2" "$3" "$4" <<'PY'
import sys
p, a, b = sys.argv[1], sys.argv[2], sys.argv[3]
s = open(p, encoding="utf-8").read()
assert s.count(a) == 1, (p, s.count(a))
open(p, "w", encoding="utf-8", newline="\n").write(s.replace(a, b))
PY
    cmake --build re15_port/build --target test_r30_hinweis > /dev/null 2>&1 || { echo "[$1] BAU FEHLGESCHLAGEN"; git checkout -- "$2"; return; }
    "$T" "$5" > /dev/null 2>&1; rc=$?
    git checkout -- "$2"
    echo "[$1] Riegel $5 rc=$rc $( [ $rc -ne 0 ] && echo FAELLT || echo 'FAELLT NICHT' )"
}

mut "Takt halbiert"     re15_port/engine/src/map_hint_common.c \
    "(uint64_t)RE15_HINT_VBLANKS_JE_SCHRITT * 1000ull" "(uint64_t)2 * 1000ull" fsm
mut "Anker-Haken weg"   re15_port/engine/src/scd_vm.c \
    "if (g_re15_map_hint_anchor_n) re15_map_hint_pc(t->pc);" "(void)0;" anker
mut "Raum-Gatter weg"   re15_port/engine/src/map_hint_common.c \
    "if (room_id != (unsigned)e->quelle) continue;" "(void)room_id;" anker
mut "Zielkachel weg"    re15_port/engine/src/re15_inv_screen.c \
    "if (st->hint_aktiv && st->map_page == st->hint_page && i == st->hint_rect) {" "if (0) {" zeichner
mut "Marker bleibt"     re15_port/engine/src/re15_inv_screen.c \
    "if (st->substate == 1 && st->item_state == 1 && !st->hint_aktiv) {" "if (st->substate == 1 && st->item_state == 1) {" zeichner
mut "map_entry statt Blatt" re15_port/engine/src/menu_common.c \
    "    g_inv_screen.map_page = (uint8_t)page;   /* RE2 @0x8006F738: Blatt fest je Hinweis */" "    map_entry();" spurlos
mut "Oeffnen-Ton"       re15_port/engine/src/menu_common.c \
    "    if (s_hint_target) se4(9);" "    if (s_hint_target) { se4(6); se4(9); }" fsm
mut "L1 schliesst"      re15_port/engine/src/menu_common.c \
    "if ((pressed & RE15_PAD_BIT_START) || (re15_pad_virtual_word(pressed) & 0x8000)) {
                se4(5);" "if ((pressed & (RE15_PAD_BIT_START | RE15_PAD_BIT_L1)) || (re15_pad_virtual_word(pressed) & 0x8000)) {
                se4(5);" fsm
cmake --build re15_port/build --target test_r30_hinweis > /dev/null 2>&1
for r in anker fsm spurlos zeichner bank; do "$T" $r > /dev/null 2>&1; echo "[zurueck] $r rc=$?"; done
