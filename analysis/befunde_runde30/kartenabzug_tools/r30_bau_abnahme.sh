#!/usr/bin/env bash
# Runde 30, Thema F (karten-marken), BAU: die Abnahme-Laeufe aus karten-marken.md §9.5.
#
#   $1 = Arbeitsbaum (Wurzel des Repos, dessen re15_port/build/platform/pc/re15_pc.exe gefahren wird)
#   $2 = Ausgabeordner
#
# Laeufe (nacheinander, alle im Verzeichnis der exe - dort liegt re15_card.mcr):
#   n_1F / n_2F / n_ROOF  Titel -> LOAD GAME -> Platz 2 mit der UNVERAENDERTEN Karte des Nutzers
#                         (analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr)
#   sweep1150_NN.bmp      13 Blaetter, alles aufgedeckt, Spieler in ROOM1150
#   s1_10C0 / l1_10C0     alles aufdecken, in ROOM10C0 speichern (frische Karte) ->
#                         frischer Prozess, LOAD GAME, 2F abziehen
# Auswertung: r30_abzug_vergleich.py, r30_blau_zensus.py, r30_bau_diff_klassen.py.
#
# ⛔ Neues Spiel braucht RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_INPUT_SCRIPT_START=30:
# ohne sie faellt der START-Druck des Skripts (Standard-Startbild 90) in den Vorspann, der
# Titel wartet danach ewig (gemessen: 240 s ohne "[fps] target"-Zeile, 0 Abzuege).
# ⛔ Die exe wird ueber timeout beendet (eigener Kindprozess), nie ueber den Bildnamen.
set -uo pipefail
WT="${1:?Arbeitsbaum}"; OUT="${2:?Ausgabeordner}"
EXE="$WT/re15_port/build/platform/pc"
KARTE_NUTZER="$WT/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr"
mkdir -p "$OUT"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*' RE15_NOAUDIO=1

lauf() {   # $1 Marke, $2 Karte ("-" = liegende Karte behalten, "LEER" = keine), $3 Sekunden
    ( cd "$EXE" || exit 9
      rm -f debug.log befund.log
      case "$2" in -) ;; LEER) rm -f re15_card.mcr ;; *) cp -f "$2" re15_card.mcr ;; esac
      timeout -k 5 "$3" ./re15_pc.exe > "$OUT/$1.stdout.log" 2> "$OUT/$1.stderr.log"
      echo "[lauf] $1 rc=$?"
      cp -f debug.log "$OUT/$1.debug.log" 2>/dev/null
      cp -f re15_card.mcr "$OUT/$1.karte_nachher.mcr" 2>/dev/null
      true )
}

( export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=2 RE15_INV_OPEN_AT="60#1070"
  RE15_INV_FB_SHOT="$OUT/n_1F.bmp" RE15_INV_FB_SHOT_AT=90 lauf n_1F "$KARTE_NUTZER" 150
  RE15_INPUT_SCRIPT="W40,U0.1,W60" RE15_INV_FB_SHOT="$OUT/n_2F.bmp" RE15_INV_FB_SHOT_AT=1100 \
      lauf n_2F "$KARTE_NUTZER" 150
  RE15_INPUT_SCRIPT="W40,U0.1,W2,U0.1,W2,U0.1,W60" RE15_INV_FB_SHOT="$OUT/n_ROOF.bmp" \
      RE15_INV_FB_SHOT_AT=1100 lauf n_ROOF "$KARTE_NUTZER" 150 )

( export RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_INPUT_SCRIPT_START=30 RE15_INPUT_SCRIPT="W2,S1,W900"
  RE15_GOTO_ROOM=1150 RE15_INV_OPEN_AT="60#1150" RE15_MAP_SHOT_SWEEP="$OUT/sweep1150_" \
      lauf sweep1150 LEER 240
  RE15_GOTO_ROOM=10C0 RE15_CARD_AUTO=1 RE15_SAVE_TEST_AGAIN=200 RE15_INV_OPEN_AT="500#10C0" \
      RE15_MAP_SHOT_SWEEP="$OUT/s1_10C0_vor_" lauf s1_10C0 LEER 240 )
cp -f "$EXE/re15_card.mcr" "$OUT/s1_10C0_karte.mcr"
( export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_INV_OPEN_AT="60#10C0"
  RE15_INV_FB_SHOT="$OUT/l1_10C0_nach_laden.bmp" RE15_INV_FB_SHOT_AT=90 \
      lauf l1_10C0 "$OUT/s1_10C0_karte.mcr" 150 )
rm -f "$EXE/re15_card.mcr"
echo FERTIG
