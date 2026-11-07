#!/usr/bin/env bash
# Abnahme 5 G5A: lauf5.sh NAME RAUM EXITAT MODUS [FRAMEDUMP-SPEC] [GDB 0|1] [VIDEO_SECS]
# Eigene exe-Kopie bin/re15_pc_g5abn5_<NAME>.exe der Integrations-exe (Messkopie), Assets per RE15_CD_ROOT aus dem Integrationsbaum,
# kein AUTOSHOT/SOFTWARE_RENDER/NOAUDIO. MODUS rakete = Rakete bis zur Szene (wie integration_r41_g5_tod), danach Rueckzug + Raketen;
# rakete_steh = Rakete bis zur Szene, danach keine Taste; pistole = Eingabe integration_r41_birkin_pistole (mit Messhaken HP 100).
# GDB 1: Lauf unter gdb, Haltepunkt nach re15_rumble_tick in pad_rumble_tick (+0x20) -> je Bild a0/a1 (nur eigene PID).
# VIDEO_SECS > 0: gdigrab auf das hwnd der eigenen PID, Fenster vorher per SetWindowPos nach (40,40) + TOPMOST gestellt.
set -u
NAME=$1; RAUM=$2; EXITAT=$3; MODUS=$4; FD=${5:-}; GDB=${6:-0}; SECS=${7:-0}
B="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/testint/g5abn5"
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration
EXE=re15_pc_g5abn5_$NAME
WD="$B/lauf/$NAME"; rm -rf "$WD"; mkdir -p "$WD/fd" "$WD/exe"
cp -f "$B/bin/re15_pc_g5abn5_quelle.exe" "$WD/exe/$EXE.exe"
SKRIPT="A0.1,W1,U8.5"
GIVE="RE15_GIVE=18:200"; EQ="RE15_EQUIP=18"; HAKEN=""
if [ "$MODUS" = "rakete" ]; then SKRIPT="$SKRIPT,MA9,W43.3,D4.5"; for i in $(seq 1 40); do SKRIPT="$SKRIPT,MA4,W0.2"; done
elif [ "$MODUS" = "rakete_steh" ]; then SKRIPT="$SKRIPT,MA9,W260"
elif [ "$MODUS" = "pistole" ]; then for i in $(seq 1 34); do SKRIPT="$SKRIPT,MA4"; done; SKRIPT="$SKRIPT,W2.4,D12"; for i in $(seq 1 60); do SKRIPT="$SKRIPT,MA4,W0.2"; done
     GIVE="RE15_GIVE=3:255,21:250"; EQ="RE15_EQUIP=3"; HAKEN="RE15_R39_MESS_SPIELER_HP=100"
fi
EXTRA=""
[ "$RAUM" = "5091" ] && EXTRA="RE15_TITLE_SHOT_CHAR=1"
[ -n "$FD" ] && EXTRA="$EXTRA RE15_FRAMEDUMP=$FD"
EXTRA="$EXTRA $HAKEN ${EXTRA2:-}"
cd "$WD"
export PATH="/c/msys64/mingw64/bin:$PATH"
ENVV="SDL_ASSERT=always_ignore RE15_CD_ROOT=$W/re15_port/shared_assets/PSX RE15_NO_INTRO=1 RE15_AUDIO_CAP_SYNC=ton.raw RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 $EXTRA RE15_DEBUG_JUMP=${RAUM}@250 RE15_PLAYER_POS=500,-9175,2048,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=$SKRIPT RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log $GIVE $EQ RE15_G5A_LAMPE_LOG=1 RE15_GATOR_FX_LOG=1 RE15_EXIT_AT=${EXITAT}#${RAUM}"
echo "$ENVV" > env.txt
if [ "$GDB" = "1" ]; then
  cat > gdb_cmds.txt <<'G'
set pagination off
set confirm off
set print thread-events off
set $bis = 0
break *re15_rumble_ramp
commands
silent
printf "RAMP F%u dauer %d von %d bis %d vorlauf %d\n", *(unsigned int*)&g_engine, $rcx, $rdx, $r8, $r9
set $bis = *(unsigned int*)&g_engine + 40
enable 4
continue
end
break *re15_rumble_large
commands
silent
printf "LARGE F%u dauer %d amp %d vorlauf %d\n", *(unsigned int*)&g_engine, $rcx, $rdx, $r8
set $bis = *(unsigned int*)&g_engine + 40
enable 4
continue
end
break *re15_rumble_small
commands
silent
printf "SMALL F%u dauer %d vorlauf %d\n", *(unsigned int*)&g_engine, $rcx, $rdx
set $bis = *(unsigned int*)&g_engine + 40
enable 4
continue
end
break *(pad_rumble_tick+32)
commands
silent
printf "RUMBLE F%u a0 %u a1 %u\n", *(unsigned int*)&g_engine, *(unsigned char*)($rbp-1), *(unsigned char*)($rbp-2)
if *(unsigned int*)&g_engine > $bis
disable 4
end
continue
end
disable 4
run
quit
G
  env $ENVV gdb -q -batch -x gdb_cmds.txt "$WD/exe/$EXE.exe" > gdb_out.txt 2> stdout.txt &
else
  env $ENVV "$WD/exe/$EXE.exe" > stdout.txt 2>&1 &
fi
LP=$!
sleep 3
WPID=$(powershell.exe -NoProfile -Command "(Get-Process $EXE -ErrorAction SilentlyContinue | Select-Object -First 1).Id" 2>/dev/null | tr -d '\r\n ')
echo "pid=$LP winpid=$WPID" | tee pid.txt
if [ "$SECS" != "0" ]; then
  # Aufnahme erst ab Spielbild VIDEO_AB (state.log), damit die Szene sicher im Video liegt
  if [ -n "${VIDEO_AB:-}" ]; then
    for i in $(seq 1 2400); do
      F=$(tail -c 600 state.log 2>/dev/null | grep -ao '^F[0-9]*' | tail -1 | tr -d F)
      [ -n "$F" ] && [ "$F" -ge "$VIDEO_AB" ] && break
      kill -0 $LP 2>/dev/null || break
      sleep 0.25
    done
    echo "Aufnahme ab Bild ${F:-?}"
  fi
  H=""
  for i in $(seq 1 120); do
    H=$(powershell.exe -NoProfile -Command "(Get-Process -Id $WPID -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 }).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
    [ -n "$H" ] && [ "$H" != "0" ] && break
    sleep 0.5
  done
  echo "hwnd=$H"
  powershell.exe -NoProfile -Command "Add-Type -Name W -Namespace U -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int cx, int cy, uint f);'; [U.W]::SetWindowPos([System.IntPtr]$H, [System.IntPtr](-1), 40, 0, 0, 0, 0x0001 -bor 0x0040)" > setwindowpos.txt 2>&1
  HX=$(printf '0x%x' "$H")
  if [ "${CAPTURE:-gdigrab}" = "pw" ]; then
    /c/Python310/python.exe "$B/werkzeug/printwindow5.py" "$HX" "$WD/pw" "$SECS" > printwindow.txt 2>&1; cat printwindow.txt
  else
  /c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 20 -draw_mouse 0 -t "$SECS" -i "hwnd=$HX" \
    -c:v libx264 -crf 16 -pix_fmt yuv420p "$WD/fenster.mp4"
  echo "ffmpeg rc=$?"
  fi
fi
for i in $(seq 1 ${WAIT_N:-1600}); do kill -0 $LP 2>/dev/null || break; sleep 0.5; done
if kill -0 $LP 2>/dev/null; then powershell.exe -NoProfile -Command "Stop-Process -Id $WPID -Force" >/dev/null 2>&1; echo "beendet (eigene PID $WPID)"; fi
wait $LP
echo "exe rc=$?"
rm -f "$WD/exe/$EXE.exe"
grep -a "g5auftritt\]" "$WD/stdout.txt" "$WD/debug.log" 2>/dev/null | head -8
ls "$WD/fd" | wc -l
