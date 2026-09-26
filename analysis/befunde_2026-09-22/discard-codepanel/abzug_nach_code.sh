#!/usr/bin/env bash
# LIVE-ABZUG: die Wegwerf-Abfrage NACH erfolgreicher Code-Eingabe — ROOM10D0, Kartenleser.
#
# ECHTER RENDERPFAD: RE15_FRAMEDUMP liest das KOMPLETT komponierte Bild INNERHALB von
# re15_render_end_frame() unmittelbar VOR SDL_RenderPresent zurueck — kein RE15_AUTOSHOT,
# kein Softwarerenderer.
#
# WIE DER ZUSTAND "CODE EINMAL RICHTIG EINGEGEBEN" HERGESTELLT WIRD:
#   Die Erfolgs-Schranke des Panels steht in ROOM10D0 sub01 (sub01 laeuft jedes Bild).
#   Nachgemessen an ROOM10D0.RDT (re15_port/shared_assets/PSX/STAGE1/):
#     @0x01512  21 03 32 00   Ck(3,50,0)   Schloss noch zu
#     @0x01516  21 05 0d 01   Ck(5,13,1)   Ziffer 1 richtig
#     @0x0151A  21 05 0e 01   Ck(5,14,1)   Ziffer 2 richtig
#     @0x0151E  21 05 0f 01   Ck(5,15,1)   Ziffer 3 richtig
#     @0x01522  21 05 10 01   Ck(5,16,1)   Ziffer 4 richtig
#     @0x01526  04 ff 18 13   Evt_exec sub19
#     @0x0152A  22 03 32 01   Set(3,50,1)  ERFOLG
#   sub19 gibt @0x0199E `2b 05 ff ff` = Message_on 5 = "You've opened the lock." aus, und
#   DORT haengt seit Runde 25 die Wegwerf-Abfrage. (`Evt_exec` gibt die Kontrolle nicht ab,
#   op_evt_exec byte-true @0x8003f2b8 — das Erfolgs-Bit steht also, wenn sub19 seine
#   Nachricht oeffnet. Genau das prueft das Gate.)
#   Die vier Riegel flag(5,13..16) setzen im Spiel die Ziffern-Subs (z.B. ROOM10D0 sub07
#   @0x015C6). Der Messhaken setzt GENAU DIESE VIER Bits — denselben Zustand, den eine
#   richtige Eingabe hinterlaesst. NICHT nachgestellt wird die Dreh-Eingabe selbst
#   (Member[15] + Quadrat); sie ist fuer dieses Bild ohne Belang, weil die Abfrage hinter
#   der Schranke haengt, nicht an der Tastatur.
#
# ⛔ WARUM RE15_SET_FLAG_AT UND NICHT RE15_SET_FLAG (gemessen, nicht gewaehlt):
#   Bank 5 ist die RAUM-LOKALE Bank und wird beim Raum-Aufbau geloescht — byte-true
#   FUN_8003ecec @0x8003ed74 `sw zero,0x800b1028` (Port: scd_room_setup.c:276). Der erste
#   Versuch dieses Abzugs (2026-09-22) setzte die vier Bits mit RE15_SET_FLAG beim
#   SPIELSTART, also VOR dem Sprung nach 10D0; sie waren beim Raum-Aufbau wieder 0. Ergebnis
#   damals: 1959 Bilder in ROOM10D0, msg_aktiv=0, kein einziges frage=8. RE15_SET_FLAG_AT
#   setzt sie NACH dem Raum-Aufbau.
#
#   RE15_GIVE=0x38:1      Blaue Keycard ins Inventar (die Besitzpruefung @0x80051628)
#   RE15_SET_FLAG 9:52    das "genommen"-Bit der Blauen Keycard, Bank 9 = persistent
#   RE15_SET_FLAG_AT      die vier Ziffern-Bits, Bank 5, NACH dem Raum-Aufbau
#   RE15_DEBUG_JUMP       Debug-Menue-Sprung, derselbe Pfad wie ein Quadrat-Druck des Nutzers
#   RE15_DISCARD_LOG      Messchiene: je Bild eine Zeile, damit ein Fehllauf nicht als leere
#                         Datei endet
#
# Aufruf: abzug_nach_code.sh <ziel> [sekunden] [von] [bis] [schritt] [setzbild]
set -euo pipefail
cd "$(dirname "$0")/../../.."
ZIEL="${1:?Zielverzeichnis fehlt}"
SEK="${2:-70}"
VON="${3:-190}"
BIS="${4:-420}"
SCHRITT="${5:-5}"
SETZ="${6:-200}"
EXE_DIR="${RE15_EXE_DIR:-re15_port/build/platform/pc}"
mkdir -p "$ZIEL"
rm -f "$ZIEL"/bild*.ppm "$ZIEL"/discard.log

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"   # auch AUTO-VORLAUF des Titelmenues (main.c:2833) — ohne ihn bleibt das Spiel im Titel stehen
export RE15_TITLE_SHOT_AF=2
export RE15_NOAUDIO=1
export RE15_DEBUG_JUMP="10D0@120"
export RE15_GIVE="0x38:1"
export RE15_SET_FLAG="9:52"
export RE15_SET_FLAG_AT="5:13,5:14,5:15,5:16@${SETZ}"
export RE15_DISCARD_LOG="$ZIEL/discard.log"
# Die Nachricht "You've opened the lock." haelt den Freeze, bis der Spieler bestaetigt;
# erst danach macht re15_discard_tick die Abfrage auf (Warte-Schranke = RE2s Belegt-Bit
# @0x800517f0 / @0x800517f4). Der Pruefstand drueckt deshalb selbst - ohne das blieb die
# Abfrage im Lauf vom 26.09. ueber 2000 Bilder auf "vorgemerkt" stehen (abfrage=1, frage=0).
export RE15_PRESS="${RE15_PRESS:-square@250,square@290,square@330,square@430,square@470}"
export RE15_FRAMEDUMP="$VON-$BIS/$SCHRITT:$ZIEL/bild"
timeout -k 5 "$SEK" "$EXE_DIR/re15_pc.exe" > "$ZIEL/lauf.out" 2> "$ZIEL/lauf.err" || true

echo "--- setflag-at Quittung"
grep -c "setflag-at" "$ZIEL/lauf.err" || echo 0
echo "--- Abzuege"
ls "$ZIEL"/bild*.ppm 2>/dev/null | wc -l
echo "--- Zeilen mit stehender Abfrage (frage=8)"
grep -c "frage=8" "$ZIEL/discard.log" 2>/dev/null || echo 0
echo "--- erste Zeile, in der die Abfrage vorgemerkt ist"
grep -n "abfrage=1" "$ZIEL/discard.log" 2>/dev/null | head -3 || echo "KEINE"
echo "--- erste Zeile mit frage=8"
grep -n "frage=8" "$ZIEL/discard.log" 2>/dev/null | head -3 || echo "KEINE"
echo "--- erste Zeile mit offener Nachricht"
grep -n "msg_aktiv=1" "$ZIEL/discard.log" 2>/dev/null | head -3 || echo "KEINE"
