#!/bin/sh
# ============================================================================
# Sichtpruefung Runde 34: DER LIEGENDE HUND UNTER BESCHUSS (ROOM1190)
# ----------------------------------------------------------------------------
# Der Hund geht nach dem ersten Treffer nieder. Waehrend er liegt, traegt seine
# Haltungsklasse (word0>>26)&7 den Wert 1 — `and 0xE7FFFFFF` @0x80104090-B4 plus
# `lui v1,0x400` @0x801040AC in FUN_80104088(0) — und die EBEN-Zeile 3 der
# Maskentabelle @0x800A6DB4 lautet `2,0,0`, fragt also allein Bit 1 ab. Leon
# schiesst weiter, es faellt kein Schaden. Steht der Hund wieder, setzt
# FUN_80104088(1) `lui v1,0xc00` @0x801040CC das Bit zurueck und er ist treffbar.
#
# ZWEI MESSHAKEN sind noetig, weil ein Raum-Sprung den Ereignis-Vorlauf ueberspringt:
#   RE15_DEBUG_SUB     startet ROOM1190s sub 13 (dort steht das Sce_em_set der Hunde) —
#                      genau das, was `Evt_exec` (0x04, scd_vm.c:1076) im Skript tut.
#   RE15_DEBUG_DOGWAKE zuendet die SCD-Marke grid 0x43, auf die der Skript-Spawn
#                      wartet (@0x801113E4-EC; derselbe Weg wie probe_dog_attack_live.c:266).
# Beide sind env-gegatet und aendern ohne die Variablen keine einzige Instruktion.
#
# KEIN RE15_AUTOSHOT (maskiert Bugs), kein Softwarerenderer.
# KEIN PATH-Prepend auf msys64: gemessen 2026-09-27 haengt die exe damit vor dem
# Titelbild (2x 140 s Zeitlimit, keine einzige geschriebene Datei).
# ============================================================================
mkdir -p shots/r34
rm -f debug_sub.log
export RE15_NOAUDIO=1
export RE15_NO_INTRO=1
export RE15_TITLE_SHOT=shots/r34/title.ppm
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="1190@240"
export RE15_DEBUG_SUB="13@450"
export RE15_DEBUG_DOGWAKE=500

# 6 s stehen, links drehen, in den Schiessstand laufen; danach R1 halten und
# SQUARE in Press-Flanken (MA gedrueckt / M0 wieder los — ein Dauerhalten waere
# genau EINE Flanke).
SCRIPT="W2,L1,XU3"
i=0
while [ $i -lt 40 ]; do
    SCRIPT="$SCRIPT,MA0.15,M0.25"
    i=$((i + 1))
done
export RE15_INPUT_SCRIPT="$SCRIPT"
export RE15_INPUT_SCRIPT_START=340
export RE15_FRAMEDUMP="500-760/4:shots/r34/hund_"
timeout -k 5 140 ./re15_port/build/platform/pc/re15_pc.exe >shots/r34/run.log 2>&1
echo "RC=$?"
cat debug_sub.log 2>/dev/null
ls shots/r34 | wc -l
