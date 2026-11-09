#!/usr/bin/env bash
# Einbau K5e v0.8.30 (E32): Spiellauf am ECHTEN Asset. re15_port/tools/form_spiel.sh: eigene exe-Kopie, echtes Fenster (gdigrab bzw.
# PrintWindow, Zeile "Verfahren:"), nur eigene PID; kein RE15_AUTOSHOT, kein RE15_SOFTWARE_RENDER (ausser im eigenen Softrender-Lauf).
#   bash kf_k5e_f1_spiel.sh [aus-Ordner]   (Standard ../ruhe/einbau_spiel)
# Teil a (Framedump-Serie RE15_FRAMEDUMP=<von>-<bis>/<schritt>:dump_ im Laufverzeichnis, Spielbild-Zaehler g_engine.frame_count): 5080 Schnitt 1
#   Bild-1-Lage und 5090 Schnitt 11 Bild-2-Pose, je einmal mit dem Asset (ohne Umleitung) und einmal mit RE15_RE2_ASSET_ROOT=ruhe/K5e_f1
#   -> <raum>_{asset,var}/dump_NNNNNN.ppm (gleiche Bildnummern muessen byte-gleich sein). Lauf 1 mit Einzelbild 700 schrieb nichts: der
#   Zaehler stand beim Fensterbild erst bei ~F420 (debug.log "[walk] F420"); darum Serie 360-440 und --nach 10.
# Teil b (gdigrab): Asset ohne Umleitung in Ruhe/Bild-1-Lage/Bild-2-Pose (5080, 5090) und Seitenansicht mit F1 (5090 Gier 2850, Clip 2);
#   die Seitenansicht zusaetzlich fuer K5c (einbau_k5c) und K5e (ruhe/K5e) ueber RE15_RE2_ASSET_ROOT (Tafel 09).
# Posen im 28-Clip-Asset (FARBE.md E.5): Bild 1 = Clip 0 Bild 11 (Nachstellung -20711,-15984, Gier 756, 5080 Schnitt 1),
# Bild 2 = Clip 12 Bild 0 (941,-23776, Gier 3874, 5090 Schnitt 11), Ruhe = Clip 2 Bild 0.
W=$(cd "$(dirname "$0")/../../../../../.." && pwd -W)
K5=$W/analysis/befunde_runde41/form/varianten/K5
A=${1:-$K5/ruhe/einbau_spiel}
DUMP_SERIE=${DUMP_SERIE:-360-440/20}
mkdir -p "$A"
cd "$W" || exit 2
FILTER="Verfahren\|Bahnsteig-Form\|typ 0x30\|unbrauchbar\|kein Bild\|Laufverzeichnis\|FAILED\|not loaded\|WARNUNG\|\[tim\]"
lauf() { ziel=$1; shift; echo "=== $ziel $*"; bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 2>&1 | grep -a "$FILTER" | cut -c1-220; echo "rc=${PIPESTATUS[0]}"; }
dump() {   # dump <ordner> <raum-args...> -- kopiert dump_*.ppm aus dem Laufverzeichnis nach <aus>/<ordner>/
    ziel=$1; shift; echo "=== $ziel $*"
    out=$(bash re15_port/tools/form_spiel.sh "$@" --aus "$A/_$ziel.png" --nach 10 2>&1); rc=$?
    echo "$out" | grep -a "$FILTER" | cut -c1-220; echo "rc=$rc"
    d=$(echo "$out" | grep -a "^Laufverzeichnis:" | sed 's/^Laufverzeichnis: //')
    mkdir -p "$A/$ziel"; cp "$d"/dump_*.ppm "$A/$ziel/" 2>/dev/null
    echo "dump: $(ls "$A/$ziel" | tr '\n' ' ')"; grep -a "\[walk\] F" "$d/debug.log" | tail -1
}
P1="-20711,-15984,756,0,11,1"; P2="941,-23776,3874,12,0,11"
# Teil a
if [ "${NUR_TEIL:-}" != b ]; then
dump 5080_asset --raum 5080 --pose "$P1" --env "RE15_FRAMEDUMP=$DUMP_SERIE:dump_"
dump 5080_var   --raum 5080 --pose "$P1" --env "RE15_FRAMEDUMP=$DUMP_SERIE:dump_ RE15_RE2_ASSET_ROOT=$K5/ruhe/K5e_f1"
dump 5090_asset --pose "$P2" --env "RE15_FRAMEDUMP=$DUMP_SERIE:dump_"
dump 5090_var   --pose "$P2" --env "RE15_FRAMEDUMP=$DUMP_SERIE:dump_ RE15_RE2_ASSET_ROOT=$K5/ruhe/K5e_f1"
fi
[ "${NUR_TEIL:-}" = a ] && { echo SPIEL-FERTIG; exit 0; }
# Teil b
lauf f1_pose1_nachstellung_r5080.png --raum 5080 --pose "$P1"
lauf f1_ruhe_r5080_s1.png --raum 5080 --pose "-20711,-15984,756,2,0,1"
lauf f1_pose2_r5090_s11.png --pose "$P2"
lauf f1_ruhe_r5090_s11.png --pose "941,-23776,3874,2,0,11"
lauf f1_seite_r5090_s11.png --pose "941,-23776,2850,2,0,11" --env "RE15_TIM_LOG=1"
lauf k5c_seite_r5090_s11.png --pose "941,-23776,2850,2,0,11" --env "RE15_RE2_ASSET_ROOT=$K5/einbau_k5c"
lauf k5e_seite_r5090_s11.png --pose "941,-23776,2850,2,0,11" --env "RE15_RE2_ASSET_ROOT=$K5/ruhe/K5e"
echo SPIEL-FERTIG
