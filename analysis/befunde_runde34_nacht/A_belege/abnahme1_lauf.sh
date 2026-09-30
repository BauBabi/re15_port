#!/usr/bin/env bash
# abnahme1_lauf.sh — Spur A, ABNAHME 1 (unabhaengig): Gegenproben an der ECHTEN re15_pc.exe.
# Eigene exe-Kopie (re15_pc_abn1.exe) — local_build.sh beendet nur Prozesse namens re15_pc.
#
#   abnahme1_lauf.sh <modus> <zielverzeichnis>
#     tuer_mash_mit : Leon ueber den ECHTEN Tuerweg (Debug-JUMP 1030, Tuer Slot 0 @0x01C6A ->
#                     ROOM1050 (20600,0,12350) Blick West), Sicherung per RE15_GIVE, am Schalter
#                     30 s lang Viereck-Hammern (A0.1,W0.1): Doppelausloesen? haengt etwas?
#     ohne_mash     : Leon per Debug-JUMP 1050, KEINE Sicherung, 25 s Viereck-Hammern: jede Runde
#                     zurueck in Cut 3, Tor bleibt zu?
#     mit_status    : Leon per Debug-JUMP 1050, Sicherung, Viereck und START im Wechsel waehrend
#                     der ganzen Nahansicht: geht der Statusschirm mitten in der Nahansicht auf?
#     mit_erst_nein : Sicherung, auf die SCHALTERFRAGE "No" (Nahansicht darf nicht kommen), dann
#                     nochmal: Ja, auf "Will you use the Fuse?" Ja; dann gleich weiter Ja -> Tor.
set -uo pipefail
MODUS="${1:?modus}"; ZIEL="${2:?zielverzeichnis}"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r34n_rolltor"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_abn1.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/state.log

# Leons Weg von (17000,12000) Blick Sued (lauf.sh von Spur A, dort vermessen)
WEG_JUMP="U1.8,R0.3667,U0.9,L0.3667,U8.5,L0.3667,U0.7,W0.5"
# Weg von der Tuer-Ankunft (20600,12350) Blick West (lauf.sh, Elza-Fassung mit Driftausgleich)
WEG_TUER="U1.62,L0.3667,U1.96,R0.3667,U0.9,L0.3667,U5.667,R0.0334,U2.9,L0.3667,U0.7,W0.5"
mash() { local n="$1" tok="$2" s=""; for ((i=0;i<n;i++)); do s="$s,$tok"; done; echo "${s#,}"; }

export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=3 RE15_NOAUDIO=1
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_INPUT_SCRIPT_BASIS=spiel
case "$MODUS" in
tuer_mash_mit)
    export RE15_GIVE=0x40:1 RE15_DEBUG_JUMP="1030@120" RE15_FIRE_AOT="0@60#1030"
    export RE15_INPUT_SCRIPT_START=400
    export RE15_INPUT_SCRIPT="$WEG_TUER,$(mash 150 A0.1,W0.1),W3"
    export RE15_FRAMEDUMP="860-1900/6:f_" RE15_EXIT_AT="1900#1050" ;;
ohne_mash)
    export RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024"
    export RE15_INPUT_SCRIPT_START=200
    export RE15_INPUT_SCRIPT="$WEG_JUMP,$(mash 125 A0.1,W0.1),W3"
    export RE15_FRAMEDUMP="590-1450/6:f_" RE15_EXIT_AT="1450#1050" ;;
mit_status)
    export RE15_GIVE=0x40:1 RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024"
    export RE15_INPUT_SCRIPT_START=200
    export RE15_INPUT_SCRIPT="$WEG_JUMP,$(mash 40 A0.1,W0.2,S0.1,W0.2),W3"
    export RE15_FRAMEDUMP="590-1500/6:f_" RE15_EXIT_AT="1500#1050" ;;
mit_erst_nein)
    export RE15_GIVE=0x40:1 RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024"
    export RE15_INPUT_SCRIPT_START=200
    # Druck, Frage steht (5 s), Rechts = No, Viereck; 1 s; Druck, Frage, Ja, msg 2 schliessen,
    # msg 20 Ja, msg 21 schliessen; 1 s; Druck, Frage, Ja -> Tor
    export RE15_INPUT_SCRIPT="$WEG_JUMP,A0.1,W5,R0.1,W0.5,A0.1,W1,A0.1,W5,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,W1,A0.1,W5,A0.1,W12"
    export RE15_FRAMEDUMP="590-2300/10:f_" RE15_EXIT_AT="2300#1050" ;;
wieder)
    # WIEDERBETRETEN an der exe: Einsetzen (Ja/Ja) wie lauf.sh "mit", dann Debug-Menue von Hand
    # (E = SELECT oeffnet, A = Viereck laedt die JUMP-Zeile, Cursor steht noch auf ROOM1050) ->
    # derselbe Raumaufbau-Weg wie eine Tuer (re15_room_request_change -> re15_room_apply_pending).
    # Das Skript laeuft je Raum neu (BASIS=spiel, Bildzaehler je Raum ab 0): im 2. Durchgang
    # fragt der Schalter AUSGELIEFERT, das Ja faehrt das Tor. Ende per Wanduhr (SEK).
    export RE15_GIVE=0x40:1 RE15_DEBUG_JUMP="1050@120" RE15_PLAYER_POS="17000,12000,1024"
    export RE15_INPUT_SCRIPT_START=200
    export RE15_INPUT_SCRIPT="$WEG_JUMP,A0.1,W5,A0.1,W4,A0.1,W4,A0.1,W4,A0.1,W4,E0.1,W1,A0.1,W2"
    export RE15_FRAMEDUMP="1000:f_1000.ppm" ;;
*) echo "Modus?"; exit 2 ;;
esac
export RE15_STATE_LOG=state.log RE15_MSG_LOG=1 RE15_EVT_TRACE=1 RE15_SE_DEBUG=1
cd "$ZIEL"
timeout -k 5 "${SEK:-420}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|room\] PC loaded|fire-aot|EXIT_AT|\[msg\]|\[rolltor\]|Cut_chg|give\]|\[se\] SCD|status|inv\]" "$ZIEL/debug.log" 2>/dev/null | head -120
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
