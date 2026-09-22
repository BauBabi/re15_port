#!/usr/bin/env bash
# LIVE-ABZUG der "Discard it?"-Abfrage aus dem LAUFENDEN Spiel — ROOM1100, der
# Minidisc-Player (Gegenstand 0x44). Das ist die Stelle mit dem GROESSTEN Loch des
# Runde-22-Modells (80 Bilder mit freiem Pad, Riegel TEIL I), deshalb wird hier
# abgezogen und nicht mehr in ROOM10D0.
#
# ECHTER RENDERPFAD: RE15_FRAMEDUMP liest das KOMPLETT komponierte Bild INNERHALB von
# re15_render_end_frame() unmittelbar VOR SDL_RenderPresent zurueck — kein RE15_AUTOSHOT,
# kein Softwarerenderer.
#
# Weg an die Stelle:
#   RE15_NO_INTRO / RE15_TITLE_SHOT  - Vorspann + Titel durchklicken
#   RE15_DEBUG_JUMP=1100@120         - Debug-Menue-Sprung (Original-UTILITY-MENU-Pfad)
#   RE15_GIVE=0x44:1                 - Minidisc Player w/ Disc ins Inventar
#   RE15_FORCE_EVENT=2@300           - ROOM1100 sub02 ueber den ECHTEN Ereignis-Dispatcher
#                                      (scd_event_fire) zuenden.
#     ⛔ EHRLICH: die ECHTE Zuendung steht in sub01 @0x00C4A
#        `Evt_exec cond=0xFF type=0x18 sub=2` und haengt an drei Bedingungen —
#        `Ck(5,4,1)` @0x00C30, `Member_cmp member=0x0F == 5` @0x00C3C (die Stellung des
#        Zahlenschlosses) und `Sce_key_ck 0x40` @0x00C46 (SQUARE). Ein Abzug, der das
#        Schloss per Steuerkreuz auf Kerbe 5 dreht, ist nicht deterministisch fahrbar;
#        gefahren wird deshalb das AUSGELIEFERTE sub02 durch denselben Dispatcher, den
#        das Skript benutzt. Das Skript selbst ist unangetastet.
#   RE15_INPUT_SCRIPT                - siehe die Kalibrierung unten.
#   RE15_DISCARD_LOG                 - Messchiene, JEDE Zeile: belegt/frost/pausepad/px/pz
#
# ⛔ KALIBRIERUNG, gemessen und nicht geraten: RE15_INPUT_SCRIPT_START zaehlt GERENDERTE
# Bilder ALLER Modi (input_pc.c s_input_ticks, Titel eingeschlossen), die Messchiene und
# RE15_FORCE_EVENT zaehlen g_engine.frame_count. Eigener Kalibrierlauf (START=150,
# Skript "U8"): der Spieler setzt sich in Bild F27 in Bewegung -> der Versatz ist
# 150-27 = 123 Bilder. Fuer "Vorwaerts ab dem Zuendbild 300" steht START deshalb auf
# 300+123 = 423.
#
# Aufruf: abzug_1100.sh <ziel> [sekunden] [von] [bis] [schritt]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-90}"
VON="${3:-300}"
BIS="${4:-620}"
SCHRITT="${5:-4}"
EXE_DIR="${RE15_EXE_DIR:-re15_port/build/platform/pc}"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/bild*.ppm "$ZIEL"/discard.log debug.log

# Phase 1 (F300..F420): 4 s VORWAERTS, WAEHREND die Abfrage vorgemerkt und unsichtbar ist.
#   Das ist der Beweis am laufenden Spiel: px/pz duerfen sich nicht ruehren.
# Phase 2 (ab F420): Quadrat-Stoesse (0,2 s / 0,3 s) reden die Raum-Nachricht msg 4 aus,
#   danach geht die Abfrage auf und tippt. In der Tipp-Phase nimmt sie keine Eingabe an
#   (re15_discard_tick kehrt vor dem Auswahl-Block um), also bleibt sie unbeantwortet
#   stehen und "Yes" bleibt vorbelegt.
STOSS=""
for i in $(seq 1 5); do STOSS="${STOSS}A0.2,W0.3,"; done

export RE15_NO_INTRO=1
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1100@120"
export RE15_GIVE="0x44:1"
export RE15_FORCE_EVENT="2@300"
export RE15_INPUT_SCRIPT="U4,${STOSS}W40"
export RE15_INPUT_SCRIPT_START=423
export RE15_DISCARD_LOG="$ZIEL/discard.log"
export RE15_FRAMEDUMP="$VON-$BIS/$SCHRITT:$ZIEL/bild"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" || true

echo "--- Abzuege"
ls "$ZIEL"/bild*.ppm 2>/dev/null | wc -l
echo "--- erste Zeile mit vorgemerkter Abfrage (abfrage=1)"
grep -n "abfrage=1" "$ZIEL/discard.log" 2>/dev/null | head -1 || echo "KEINE"
echo "--- erste/letzte Zeile mit stehender Abfrage (frage=8)"
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | head -1 || echo "KEINE"
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | tail -1 || true
echo "--- Wartebilder INSGESAMT / davon mit FREIEM Pad (muss 0 sein)"
grep -c "abfrage=1 frage=0" "$ZIEL/discard.log" 2>/dev/null || echo 0
grep -c "abfrage=1 frage=0 .*pausepad=0" "$ZIEL/discard.log" 2>/dev/null || echo 0
echo "--- px/pz ueber das Wartefenster (muss EIN Paar sein)"
grep "abfrage=1 frage=0" "$ZIEL/discard.log" 2>/dev/null \
  | grep -o "px=-\?[0-9]* pz=-\?[0-9]*" | sort -u | head -5 || true
