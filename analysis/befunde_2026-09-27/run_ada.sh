#!/bin/bash
# Messlauf ROOM1090 -> ROOM1050 (echter Tuerweg, Tuer-Slot 0 = Ziel 1050).
# $1 = Ausgabe-Unterordner unter $SP/ada
SP="$HOME/../MJOEDI~1/AppData/Local/Temp"
OUT="${OUTDIR:?OUTDIR fehlt}"
mkdir -p "$OUT"
export RE15_NOAUDIO=1
export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$OUT/t.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP=1090@gp
export RE15_SET_FLAG=3:0x6e
export RE15_FIRE_AOT="${FIRE:-0@90#1090}"
export RE15_ANIM_TRACE="$OUT/tr.txt"
export RE15_AP_CUTSCENE_CLICK=1
[ -n "$EXTRA_STATE" ] && export RE15_STATE_LOG="$OUT/st.txt"
[ -n "$SCDTRACE" ] && export RE15_SCD_TRACE="$OUT/scd.txt"
timeout "${SECS:-75}" ./re15_port/build/platform/pc/re15_pc.exe 2>"$OUT/err.txt"
echo "exit=$?"
