#!/bin/bash
# Messlauf ROOM1050: Shutter-Schalter (AOT-Slot 7) ausloesen und den Zustand mitschreiben.
#   OUT=<verzeichnis>  FIRE=<slot@frame#raum>  SCRIPT=<eingabeskript>  SECS=<laufzeit>
set -u
OUT="${OUT:?OUT fehlt}"
mkdir -p "$OUT"
export RE15_NOAUDIO=1
export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$OUT/t.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${ROOM:-1050}@gp"
export RE15_STATE_LOG="$OUT/st.txt"
export RE15_AOT_DUMP=1
export RE15_AP_CUTSCENE_CLICK=1
[ -n "${FIRE:-}" ]   && export RE15_FIRE_AOT="$FIRE"
[ -n "${SCRIPT:-}" ] && export RE15_INPUT_SCRIPT="$SCRIPT"
[ -n "${SSTART:-}" ] && export RE15_INPUT_SCRIPT_START="$SSTART"
[ -n "${SHOT:-}" ]   && export RE15_FRAMEDUMP="$SHOT"
[ -n "${PRESS:-}" ] && export RE15_PRESS="$PRESS"
[ -n "${SCD:-}" ]    && export RE15_SCD_TRACE="$OUT/scd.txt"
timeout "${SECS:-40}" ./re15_port/build/platform/pc/re15_pc.exe 2>"$OUT/err.txt"
echo "exit=$?"
