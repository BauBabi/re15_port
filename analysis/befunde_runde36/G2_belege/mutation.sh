#!/usr/bin/env bash
# Runde 36 G2 — RUECKBAU-NACHWEIS der angepassten/neuen Riegel (unit_r31_generator D+G, unit_r34n_c_generator A,
# unit_r36_g2_cursor_rahmen). Je Mutation: Quelldatei per Python-Ersetzung aendern (genau EIN Treffer), Bau ueber
# local_build.sh build (inkrementell), die drei Riegel fahren, FAIL-Zeilen sammeln, Datei per git checkout zurueck.
# Am Ende ein sauberer Bau. Ergebnis: <ausgabe>/mutation.log
# Aufruf: bash mutation.sh <ausgabeordner> [g1 g2 g3]   (ohne Liste alle)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r36_generator
cd "$BAUM" || exit 2
OUT="$1"; shift
mkdir -p "$OUT"
LOG="$OUT/mutation.log"
ZS=re15_port/engine/src/zellschub_r36.c
PZ=re15_port/engine/src/panel_zeiger_common.c

ersetze() {   # $1 datei, $2 alt, $3 neu  (genau ein Treffer)
    python - "$1" "$2" "$3" <<'EOF'
import sys
p, alt, neu = sys.argv[1], sys.argv[2], sys.argv[3]
s = open(p, encoding="utf-8").read()
n = s.count(alt)
if n != 1:
    print("MUTATION-FEHLER: %d Treffer fuer %r in %s" % (n, alt[:60], p)); sys.exit(3)
open(p, "w", encoding="utf-8", newline="").write(s.replace(alt, neu))
EOF
}

lauf() {   # $1 name, $2 beschreibung
    local m="$1"
    echo "=== $m: $2" | tee -a "$LOG"
    if ! bash re15_port/tools/local_build.sh build > "$OUT/mutation_${m}_bau.log" 2>&1; then
        echo "  BAU FEHLGESCHLAGEN (s. mutation_${m}_bau.log)" | tee -a "$LOG"; return
    fi
    local exe arg
    for t in "r31_generator:" "probe_r34n_c_generator:" "probe_r36_g2_cursor_rahmen:pin"; do
        exe="${t%%:*}"; arg="${t#*:}"
        ( cd re15_port/build/tests/unit && PATH="/c/msys64/mingw64/bin:$PATH" SDL_ASSERT=always_ignore \
          timeout 600 "./$exe.exe" $arg > "$OUT/mutation_${m}_$exe.txt" 2>&1 )
        rc=$?
        echo "  $exe: rc=$rc  $(grep -c '\[FAIL\]' "$OUT/mutation_${m}_$exe.txt") FAIL" | tee -a "$LOG"
        grep '\[FAIL\]' "$OUT/mutation_${m}_$exe.txt" | sed 's/^/     /' | tee -a "$LOG"
    done
}

zurueck() { git checkout -- "$ZS" "$PZ"; }

g1() { ersetze "$ZS" "    if (p < 0 || p >= RE15_SCD_MAX_PROPS || !g_scd.props[p].active) return;
    int32_t x = g_scd.props[p].x, z = g_scd.props[p].z;" "    if (p < 0 || p >= RE15_SCD_MAX_PROPS || !g_scd.props[p].active) return;
    return;   /* MUTATION g1: Ruhe-Zweig LAB_8002c0b8 aus (Stand vor Spur R) */
    int32_t x = g_scd.props[p].x, z = g_scd.props[p].z;" && lauf g1 "Ruhe-Zweig aus (re15_zellschub_ruhe kehrt sofort zurueck)"; zurueck; }

g2() { ersetze "$PZ" "    return panel_maske() == RE15_PANEL_LOESUNGSMASKE;
}" "    { unsigned maske = panel_maske();                          /* MUTATION g2: Runde-31-Regel */
      if (maske != s_maske) return 1;
      if (s_wert != re15_panel_zeiger_ziel_aus_maske(maske)) return 1; }
    if (s_ruhe < RE15_PANEL_RUHE_BILDER) return 1;
    return (s_wert == RE15_PANEL_ZIEL && s_ziel == RE15_PANEL_ZIEL) ? 1 : 0;
}" && lauf g2 "alte Runde-31-Zwischensperre wieder an"; zurueck; }

g3() { ersetze "$PZ" "    return panel_maske() == RE15_PANEL_LOESUNGSMASKE;
}" "    return 0 && panel_maske() == RE15_PANEL_LOESUNGSMASKE;   /* MUTATION g3 */
}" && lauf g3 "Endsperre aus (sperrt() liefert immer 0)"; zurueck; }

: > "$LOG"
LISTE="${*:-g1 g2 g3}"
for m in $LISTE; do "$m"; done
echo "=== sauberer Bau" | tee -a "$LOG"
git status --short -- "$ZS" "$PZ" | tee -a "$LOG"
bash re15_port/tools/local_build.sh build > "$OUT/mutation_sauber_bau.log" 2>&1 && echo "  LOCAL-BUILD-OK" | tee -a "$LOG"
