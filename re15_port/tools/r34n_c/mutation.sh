#!/usr/bin/env bash
# Spur C (Runde 34 Nacht) — RUECKBAU-NACHWEIS der Riegel unit_r34n_c_generator + unit_r31_generator.
# Je Mutation: Quelldatei per Python-Ersetzung aendern (Pruefung, dass die Stelle genau EINMAL
# vorkommt), NUR die zwei Riegel-Ziele bauen, beide fahren, Zusammenfassung ausgeben, Datei per
# `git checkout` zuruecksetzen. Ergebnis: build/r34n_c/mutation.log.
# Aufruf: mutation.sh [m1 m2 ...]   (ohne Argument alle)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator
cd "$BAUM" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
LOG="$BAUM/build/r34n_c/mutation.log"
PY=C:/Python310/python.exe
ENG=re15_port/engine/src/panel_zeiger_common.c
ZEI=re15_port/platform/pc/src/panel_lampen_pc.c

ersetze() {   # $1 datei, $2 alt, $3 neu  (Python, genau ein Treffer)
    "$PY" - "$1" "$2" "$3" <<'EOF'
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
    echo "=== $1: $2" | tee -a "$LOG"
    if ! cmake --build re15_port/build --target probe_r34n_c_generator r31_generator > "$BAUM/build/r34n_c/mutation_$1_bau.log" 2>&1; then
        echo "  BAU FEHLGESCHLAGEN (s. mutation_$1_bau.log)" | tee -a "$LOG"; return
    fi
    for t in probe_r34n_c_generator r31_generator; do
        ( cd re15_port/build/tests/unit && timeout 300 "./$t.exe" > "$BAUM/build/r34n_c/mutation_$1_$t.txt" 2>&1 )
        rc=$?
        echo "  $t: rc=$rc  $(grep -c '\[FAIL\]' "$BAUM/build/r34n_c/mutation_$1_$t.txt") FAIL" | tee -a "$LOG"
        grep '\[FAIL\]' "$BAUM/build/r34n_c/mutation_$1_$t.txt" | sed 's/^/     /' | tee -a "$LOG"
    done
}

zurueck() { git checkout -- "$ENG" "$ZEI"; }

m1() { ersetze "$ENG" "    return panel_maske() == RE15_PANEL_LOESUNGSMASKE;
}" "    return 0 && panel_maske() == RE15_PANEL_LOESUNGSMASKE;   /* MUTATION m1 */
}" && lauf m1 "Endsperre aus (sperrt() liefert immer 0)"; zurueck; }

m2() { ersetze "$ENG" "    return panel_maske() == RE15_PANEL_LOESUNGSMASKE;
}" "    { unsigned maske = panel_maske();                          /* MUTATION m2: Runde-31-Regel */
      if (maske != s_maske) return 1;
      if (s_wert != re15_panel_zeiger_ziel_aus_maske(maske)) return 1; }
    if (s_ruhe < RE15_PANEL_RUHE_BILDER) return 1;
    return (s_wert == RE15_PANEL_ZIEL && s_ziel == RE15_PANEL_ZIEL) ? 1 : 0;
}" && lauf m2 "alte Runde-31-Zwischensperre wieder an"; zurueck; }

m3() { ersetze "$ENG" "    if (nr == 0) return (maske & RE15_PANEL_LAMPE_OBEN_MASKE)  == RE15_PANEL_LAMPE_OBEN_SOLL;
    if (nr == 1) return (maske & RE15_PANEL_LAMPE_UNTEN_MASKE) == RE15_PANEL_LAMPE_UNTEN_SOLL;" \
"    if (nr == 0) return (maske & 0x015u) == 0x015u;   /* MUTATION m3: nur EIN-Schalter */
    if (nr == 1) return (maske & 0x140u) == 0x140u;" && lauf m3 "Lampenregel lax (nur die EIN-Schalter der Seite)"; zurueck; }

m4() { ersetze "$ZEI" "            if (t & 0x8000u) {                                       /* ABR 1: B + F, gesaettigt */" \
"            if (0 && (t & 0x8000u)) {                                /* MUTATION m4: deckend */" && lauf m4 "Zeichner deckend statt additiv"; zurueck; }

m5() { ersetze "$ZEI" "            if (t == 0) continue;                                   /* durchsichtig */" \
"            /* MUTATION m5: Texel 0 nicht durchsichtig */" && lauf m5 "Texel 0 nicht durchsichtig"; zurueck; }

m6() { ersetze "$ENG" "    if (zelle) *zelle = RE15_PANEL_LAMPE_ZELLE_A + (s_lampe_takt[nr] & 1);" \
"    if (zelle) *zelle = RE15_PANEL_LAMPE_ZELLE_A + ((s_lampe_takt[nr] + 1) & 1);   /* MUTATION m6 */" && lauf m6 "Takt beginnt mit Zelle 4 statt 3"; zurueck; }

m7() { ersetze "$ENG" "        for (int i = 0; i < 2; i++) {
            int an = re15_panel_lampe_an_aus_maske(i, maske);" \
"        for (int i = 0; i < 2; i++) {
            int an = re15_panel_lampe_an_aus_maske(i, maske) || s_lampe_an[i];   /* MUTATION m7: einrasten */" && lauf m7 "Lampe rastet ein (klebriges Bit)"; zurueck; }

: > "$LOG"
if [ $# -eq 0 ]; then set -- m1 m2 m3 m4 m5 m6 m7; fi
for m in "$@"; do "$m"; done
# Ausgangszustand wiederherstellen und die Ziele sauber neu bauen
zurueck
cmake --build re15_port/build --target probe_r34n_c_generator r31_generator > /dev/null 2>&1
echo "=== Rueckbau fertig, Quellen zurueckgesetzt ($(git status --short "$ENG" "$ZEI" | wc -l) geaenderte Dateien)" | tee -a "$LOG"
