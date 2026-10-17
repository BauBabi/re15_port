#!/usr/bin/env bash
# nachher_laeufe.sh — Messung NACHHER an der echten exe (Runde 38 Spur M), dieselben Wege wie die Vorher-Messung:
#   n1261  Elza: Spielstand ROOM10B1 (-21370,0,3000) Cut 7, (3,0x41) -> CONTINUE -> Tuer Slot 1 (@0x016F2) -> ROOM1261
#   n1260  Leon: Spielstand ROOM11B0 (-60,0,-18900), (4,0xF4)(4,0xCD) -> CONTINUE -> Tuer Slot 5 (@0x01034) -> ROOM1260
#   n10b1  Elza: Spielstand ROOM10B1 (0,-9000,-12000) Band 5 Cut 2, (3,0x80)(3,0x82) -> CONTINUE (sub02 laeuft)
#   n2000  Leon: Spielstand ROOM1260 (-8750,-1800,-8400) Band 1 Cut 0, (4,0xF4) -> CONTINUE -> Tuer Slot 1 (@0x0114A,
#          Ziel Stage 2 Raum 00) -> ROOM2000 sub02 (Szene mit 12 Nachrichten: Viereck = Bestaetigen alle 25 Bilder)
# Ausgabe: <scratchpad>/mess_<name>/{npckante.log,stderr.txt,f*.ppm}; Auswertung luft_auswertung.py.
HIER="$(cd "$(dirname "$0")" && pwd)"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
nur="${1:-alle}"
if [ "$nur" = alle ] || [ "$nur" = n1261 ]; then
bash "$HIER/messlauf.sh" n1261 10B1 -21370 0 3000 1024 7 3:0x41 -- RE15_FIRE_AOT="1@40#10B1" RE15_EXIT_AT="1200#1261" \
    RE15_FRAMEDUMP="150-190/4:$SP/mess_n1261/f"
fi
if [ "$nur" = alle ] || [ "$nur" = n1260 ]; then
bash "$HIER/messlauf.sh" n1260 11B0 -60 0 -18900 2048 0 4:0xF4 4:0xCD -- RE15_FIRE_AOT="5@40#11B0" RE15_EXIT_AT="1200#1260" \
    RE15_FRAMEDUMP="55-135/4:$SP/mess_n1260/f"
fi
if [ "$nur" = alle ] || [ "$nur" = n10b1 ]; then
bash "$HIER/messlauf.sh" n10b1 10B1 0 -9000 -12000 2048 2 3:0x80 3:0x82 -- RE15_EXIT_AT="900#10B1" \
    RE15_FRAMEDUMP="110-302/6:$SP/mess_n10b1/f"
fi
if [ "$nur" = alle ] || [ "$nur" = n2000 ]; then
pad=""; for f in $(seq 60 25 2400); do pad="$pad$f:A,"; done
bash "$HIER/messlauf.sh" n2000 1260 -8750 -1800 -8400 0 0 4:0xF4 -- RE15_FIRE_AOT="1@40#1260" RE15_EXIT_AT="2400#2000"     RE15_PAD_AT="${pad%,}" ${RE15_N2000_DUMP:+RE15_FRAMEDUMP="$RE15_N2000_DUMP"}
fi
# Gegenprobe VORHER fuer ROOM2000 (gleicher Weg, exe aus re15_port/build_bisect = Haken-AUS-Stand 0bcf46ac:
# enemy_ai_common.c/scd_vm.c/main.c ohne Runde-38-Haken, nur Messhaken)
if [ "$nur" = v2000 ]; then
pad=""; for f in $(seq 60 25 2400); do pad="$pad$f:A,"; done
MESS_BUILD="$(cd "$HIER/../../.." && pwd)/re15_port/build_bisect" bash "$HIER/messlauf.sh" v2000 1260 -8750 -1800 -8400 0 0 4:0xF4 -- \
    RE15_FIRE_AOT="1@40#1260" RE15_EXIT_AT="2400#2000" RE15_PAD_AT="${pad%,}"
fi
