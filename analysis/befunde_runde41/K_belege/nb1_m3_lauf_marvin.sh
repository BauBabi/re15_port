#!/usr/bin/env bash
# NB1 M2/M3 Messlauf: Leon-Neuspiel (Pistole, Gift bei Bild 100), Sprung ROOM3020, work_vars[0x10] = 0x0D
# (= ROOM3020 sub02 @0x00D7A) und Tuer Slot 2 (= Aot_on 2 @0x00D7E, Ziel 30C0) -> Raumlader bindet Marvin.
# Bilder + Knochen (Bild 60) + Zustand in 30C0, dann ein Stueck zu Fuss.
# Aufruf: lauf_marvin.sh <ordner> <exe-name-im-build>
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/K41nb1"
B="C:/workspace/git/reAi_v2/.claude/worktrees/r41_stage3szenen_b/re15_port/build/platform/pc"
QUELLE="$B/${2:-re15_pc.exe}"
EXE="$B/re15_pc_k41nb1m3.exe"
W="$SP/$1"
mkdir -p "$W"; cd "$W"; rm -f *.ppm debug.log anim.txt state.txt
cp "$QUELLE" "$EXE"
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset $v; done
P=""
for f in $(seq 120 150); do P="${P}${f}:U,"; done
for f in $(seq 170 190); do P="${P}${f}:L,"; done
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 \
  "RE15_EQUIP=3" "RE15_R39_GIFT_AT=100" \
  "RE15_DEBUG_JUMP=3020@250" "RE15_WORKVAR_AT=0x10:13@30#3020" "RE15_FIRE_AOT=2@60#3020" \
  "RE15_PAD_AT=${P%,}" RE15_STATE_LOG=state.txt RE15_ANIM_TRACE=anim.txt \
  "RE15_EXIT_AT=220#30C0" "RE15_FRAMEDUMP=20-220/20:fd_" \
  timeout 400 "$EXE" > stdout.txt 2>&1
echo "exit=$?" > exit.txt
rm -f "$EXE"
