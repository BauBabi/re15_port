#!/bin/bash
# Messkopie mit RE15_RNG_ENTROPY starten, bis ein dicker Zombie (Typ 11) eine Arm-Variante wuerfelt (oder ALWAYS),
# dann das EIGENE Fenster (PID) per PrintWindow abgreifen. Nur eigene PIDs werden beendet.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r41_zombiearm/re15_port/build/platform/pc || exit 1
export PATH="/c/msys64/mingw64/bin:$PATH"
Z=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/za
EXE=${EXE:-./re15_pc_za_vorher.exe}; TAG=${TAG:-vorher}; EXTRA=${EXTRA:-}
NAME=$(basename "$EXE" .exe)
MUSTER=${MUSTER:-"ZA-MESS.*typ 11 idx (0|2|3) "}
for i in $(seq 1 ${N:-10}); do
  env SDL_ASSERT=always_ignore RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
      RE15_AI_MODELS=re15 RE15_DEBUG_JUMP=${RAUM:-1140}@240 RE15_WINDOW_SCALE=3 ${ENTROPY-RE15_RNG_ENTROPY=1} $EXTRA "$EXE" > /dev/null 2>&1 &
  GP=$!
  for w in $(seq 1 60); do sleep 1; grep -q -E "Hybrid EM11|EM011 loaded|EM11 loaded" debug.log 2>/dev/null && break; done
  sleep ${NACH:-5}
  if [ -n "$ALWAYS" ] || grep -E "$MUSTER" debug.log >/dev/null 2>&1; then
    echo "Lauf $i: Treffer"; grep "ZA-MESS" debug.log
    WPID=$(powershell -NoProfile -Command "(Get-Process $NAME | Select -First 1).Id")
    for t in $(seq 1 ${BILDER:-6}); do
      /c/Python310/python.exe "$Z/fenster_bild.py" "$WPID" "$Z/${TAG}_$t.png"
      sleep ${TAKT:-2}
    done
    kill $GP 2>/dev/null; wait $GP 2>/dev/null
    cp debug.log "$Z/debug_${TAG}.log"
    exit 0
  fi
  echo "Lauf $i: $(grep -c ZA-MESS debug.log) Varianten: $(grep ZA-MESS debug.log | tr '\n' ' ')"
  kill $GP 2>/dev/null; wait $GP 2>/dev/null
done
exit 1
