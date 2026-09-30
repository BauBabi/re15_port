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
# (gemessen in analysis/befunde_runde33/tuer1120_werkzeug/lauf.sh). Das Skript beginnt bei Bild 200,
# der Weg endet bei Bild 605 = erster Druck am Schalter.
#
#   lauf.sh <modus> <zielverzeichnis>
#     ist      : (Ermittlung, Stand master) Aktionstaste, "Yes" (Viereck bestaetigt), Tor faehrt
#     nein     : (Ermittlung) dasselbe, aber "No" (Rechts + Viereck)
#     ohne     : (Bau) keine Sicherung: Druck -> Ja -> Nahansicht Cut 7 + msg 2 -> schliessen -> Cut 3;
#                zweiter Druck -> Frage -> Nein (Schalter bleibt bedienbar, keine Doppelausloesung)
#     mit      : (Bau) RE15_GIVE=0x40:1: Druck -> Ja -> msg 2 -> msg 20 Ja -> Cut 8 + msg 21 -> Cut 3;
#                Statusschirm auf/zu (Sicherung weg?); zweiter Druck -> ausgelieferter sub02, Tor faehrt
#     mit_nein : (Bau) wie mit, aber "No" auf msg 20; Statusschirm (Sicherung noch da)
#     danach   : (Bau) RE15_SET_FLAG_AT=9:63 -> ausgelieferter sub02 (Frage, Tor, Ton)
#     elza_ohne / elza_mit : wie ohne / mit, aber Elza (RE15_PSELECT_AUTO_SWITCH) -> ROOM1051
# Umgebung: SKRIPT (Eingabe ab START), START (Bild), SERIE (Framedump), ENDE (Exit-Bild),
#           GIVE (RE15_GIVE), FLAGS (RE15_SET_FLAG_AT ohne @), SEK (Wanduhr-Notbremse),
#           TON=1 (Audio an, fuer das SE-Protokoll).
set -uo pipefail
MODUS="${1:?ist|nein|ohne|mit|mit_nein|danach|elza_ohne|elza_mit}"; ZIEL="${2:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r34n_a.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/state.log

WEG="U1.8,R0.3667,U0.9,L0.3667,U8.5,L0.3667,U0.7,W0.5"
JA_OHNE="A0.1,W5,A0.1,W4,A0.1,W4"                 # Druck, Frage steht, Ja, msg 2 steht, schliessen
JA_MIT="A0.1,W5,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4"  # ... msg 20 steht, Ja, msg 21 steht, schliessen
NEIN_MIT="A0.1,W5,A0.1,W4,A0.1,W4,R0.1,W0.5,A0.1,W4"
STATUS="S0.1,W3,S0.1,W2"                          # Statusschirm auf, 3 s, zu
TOR="A0.1,W5,A0.1,W14"                            # ausgelieferter sub02: Frage, Ja, Fahrt
NOCHMAL_NEIN="A0.1,W5,R0.1,W0.5,A0.1,W3"
DEF_ENDE=1400; DEF_SERIE="200-1400/10"
case "$MODUS" in
ist)       ANTWORT="A0.1,W5,A0.1,W14" ;;
nein)      ANTWORT="A0.1,W5,R0.1,W0.5,A0.1,W6" ;;
ohne|elza_ohne)
           ANTWORT="$JA_OHNE,$NOCHMAL_NEIN"; DEF_ENDE=1500; DEF_SERIE="580-1500/10" ;;
mit|elza_mit)
           GIVE="${GIVE:-0x40:1}"
           ANTWORT="$JA_MIT,$STATUS,$TOR"; DEF_ENDE=2060; DEF_SERIE="580-2060/10" ;;
mit_nein)  GIVE="${GIVE:-0x40:1}"
           ANTWORT="$NEIN_MIT,$STATUS"; DEF_ENDE=1460; DEF_SERIE="580-1460/10" ;;
danach)    FLAGS="${FLAGS:-9:63}"
           ANTWORT="$TOR"; DEF_ENDE=1400; DEF_SERIE="580-1400/10" ;;
*) echo "Modus?"; exit 2 ;;
esac

export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3
[ -z "${TON:-}" ] && export RE15_NOAUDIO=1
# Leon: RE15_TITLE_SHOT = Auto-Vorlauf, ruft re15_gameflow_new_game(0) (main.c) = immer Leon;
#       Debug-JUMP 1050@120, Spawn per RE15_PLAYER_POS in die Zone 0->1.
# Elza: der Debug-JUMP laedt IMMER die Basis-Datei (gemessen: "PC loaded room1050.rdt" auch mit Elza),
#       also der echte Weg: Titel -> NEW GAME per RE15_TITLE_CONFIRM_MS -> Charakterwahl
#       RE15_PSELECT_AUTO(+_SWITCH = RECHTS = Elza, main.c pselect auto_drive) -> Vorspann ROOM1241,
#       Abbruch im Tastenfenster (square@352..364, wie integration_elza_vollstart) -> ROOM1031 ->
#       Tuer Slot 0 (main00 @0x01CD8, Ziel Raum 0x05, Ankunft (20600,0,12350) Blick 2048 = West)
#       per RE15_FIRE_AOT -> ROOM1051. Der Bildzaehler beginnt je Raum bei 0 (main.c), das Skript
#       startet deshalb in jedem Raum bei START; 400 liegt hinter dem Abbruchfenster.
#       Weg: West bis x~17000 (U1.62), links auf Sued, 350 mehr nach Sueden als Leon (U1.96), dann
#       Leons Weg. (Eine 11-Bilder-Drehung sind 1056 statt 1024 — Richtung 992 statt 1024.)
case "$MODUS" in
elza_*) export RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
        export RE15_PRESS="square@352,square@353,square@354,square@355,square@356,square@357,square@358,square@359,square@360,square@361,square@362,square@363,square@364"
        export RE15_FIRE_AOT="0@60#1031"
        # Der lange Suedschenkel laeuft 2/3 mit Richtung 992 (driftet Ost) und nach EINER
        # 1-Bild-Drehung (+96) 1/3 mit 1088 (driftet doppelt so stark West) — Drift ~0; ohne
        # das lief Elza bei (15735,-5400) auf die Zelle i12 (gemessen, Lauf 1).
        WEG="U1.62,L0.3667,U1.96,R0.3667,U0.9,L0.3667,U5.667,R0.0334,U2.9,L0.3667,U0.7,W0.5"
        START="${START:-400}"; DEF_ENDE=$((DEF_ENDE + 268))
        DEF_SERIE="$((${DEF_SERIE%%-*} + 268))-$DEF_ENDE/10" ;;
*)      export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
        export RE15_DEBUG_JUMP="1050@120"
        export RE15_PLAYER_POS="17000,12000,1024" ;;
esac
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-200}"
export RE15_INPUT_SCRIPT="${SKRIPT:-$WEG,$ANTWORT}"
export RE15_STATE_LOG=state.log RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_SE_DEBUG=1
export RE15_FRAMEDUMP="${SERIE:-$DEF_SERIE}:f_"
RAUM=1050; case "$MODUS" in elza_*) RAUM=1051 ;; esac
export RE15_EXIT_AT="${ENDE:-$DEF_ENDE}#$RAUM"
[ -n "${GIVE:-}" ]  && export RE15_GIVE="$GIVE"
[ -n "${FLAGS:-}" ] && export RE15_SET_FLAG_AT="$FLAGS@150"
cd "$ZIEL"
timeout -k 5 "${SEK:-300}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|room\] PC loaded|EXIT_AT|\[msg\]|\[rolltor\]|Cut_chg|\[evt\]|give\]|setflag|\[se\] SCD|input-script\] RE15" "$ZIEL/debug.log" 2>/dev/null | head -80
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
