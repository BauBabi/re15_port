#!/usr/bin/env bash
# lauf.sh — Spur A (Runde 34 Nacht): Messlauf am Rolltor-Schalter ROOM1050 im ECHTEN Spiel.
#
# re15_pc.exe unter eigenem Namen (parallele Agenten, local_build.sh beendet nur die eigene exe),
# beschleunigter Renderer, Bilder per RE15_FRAMEDUMP (komponierter Frame VOR dem Present; kein
# AUTOSHOT / SOFTWARE_RENDER). Neues Spiel ueber den Titel (RE15_TITLE_SHOT = Auto-Vorlauf),
# dann Debug-JUMP nach ROOM1050 — der Sprung setzt Cut 0 (@0x8001d818-20, main.c JUMP-Zweig).
# Damit die Kamera wie im Spiel laeuft, beginnt der Spieler IN der Uebergangszone 0->1
# (RVD @0x1C4: x 16700..17700, z 9700..14700) und GEHT durch 1->2 (@0x200, z 4200..5200) und
# 2->3 (@0x23C, z -2300..-1300) zum Schalter. Weg an der Kollision vorbei (SCA @0x550, Zellen
# i21 x 13650..16650 z 10850..14350 / i12 x 15700..17200 z -7850..-5950, Ostwand x >= 17200):
#   (17000,12000) Blick Sued 1024 -> 4000 Sued -> West bis x~15000 -> Sued bis z~-8550 (U8.5 = 255 Bilder) ->
#   Ost bis x~16550 -> Aktionstaste (Vorwaerts-Tastpunkt 620 @0x80042bac-Pfad, landet bei
#   x~17170 im Schalter-Rechteck x 16800..17600 z -8950..-8150, sub00 @0x0C22).
# Zeitachse = Spielbilder (RE15_INPUT_SCRIPT_BASIS=spiel); Gehen ~74/Bild, Drehen 96/Bild
# (gemessen in analysis/befunde_runde33/tuer1120_werkzeug/lauf.sh).
#
#   lauf.sh <modus> <zielverzeichnis>
#     ist     : Aktionstaste am Schalter, "Yes" (Viereck bestaetigt — wie r30 nachschliff), Tor faehrt
#     nein    : dasselbe, aber "No" (Rechts + Viereck)
# Umgebung: SKRIPT (Eingabe ab START), START (Bild), SERIE (Framedump), ENDE (Exit-Bild),
#           GIVE (RE15_GIVE), FLAGS (RE15_SET_FLAG_AT ohne @), SEK (Wanduhr-Notbremse).
set -uo pipefail
MODUS="${1:?ist|nein}"; ZIEL="${2:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r34n_a.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/state.log

WEG="U1.8,R0.3667,U0.9,L0.3667,U8.5,L0.3667,U0.7,W0.5"
case "$MODUS" in
ist)  ANTWORT="A0.1,W5,A0.1,W14" ;;
nein) ANTWORT="A0.1,W5,R0.1,W0.5,A0.1,W6" ;;
*) echo "Modus?"; exit 2 ;;
esac

export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=3
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1050@120"
export RE15_PLAYER_POS="17000,12000,1024"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-200}"
export RE15_INPUT_SCRIPT="${SKRIPT:-$WEG,$ANTWORT}"
export RE15_STATE_LOG=state.log RE15_MSG_LOG=1 RE15_EVT_TRACE=1
export RE15_FRAMEDUMP="${SERIE:-200-1400/10}:f_"
export RE15_EXIT_AT="${ENDE:-1400}#1050"
[ -n "${GIVE:-}" ]  && export RE15_GIVE="$GIVE"
[ -n "${FLAGS:-}" ] && export RE15_SET_FLAG_AT="$FLAGS@150"
cd "$ZIEL"
timeout -k 5 "${SEK:-240}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|room\] PC loaded|EXIT_AT|\[msg\]|Cut_chg|\[evt\]|input-script\] RE15" "$ZIEL/debug.log" 2>/dev/null | head -60
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
