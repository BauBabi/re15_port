#!/bin/bash
EXE="/c/Users/mjoedicke/AppData/Local/Programs/DuckStation/duckstation-qt-x64-ReleaseLTCG.exe"
LOG="/c/Users/mjoedicke/AppData/Local/DuckStation/duckstation.log"
ISO="/c/workspace/git/reAi/psx_dev/re15_reborn/build/re15_reborn.cue"
"$EXE" -batch "$ISO" >/dev/null 2>&1 &
ping -n 46 127.0.0.1 >/dev/null 2>&1
taskkill //IM duckstation-qt-x64-ReleaseLTCG.exe >/dev/null 2>&1
ping -n 4 127.0.0.1 >/dev/null 2>&1
C=$(grep -ciE "psxspu: timeout|PC=0x00000000|Failed to read block" "$LOG")
B=$(grep -cE "psxcd/iso: Loc|VSync|main window close" "$LOG")
if [ "$C" -gt 0 ]; then echo "RESULT: CRASH ($C markers)"; else echo "RESULT: BOOT-OK (0 crash, $B progress markers)"; fi
tail -3 "$LOG"
