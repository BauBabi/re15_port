#!/usr/bin/env bash
# NB1 M1 Bildlauf: Weg des Riegels integration_r41_stage3szenen_b_3091 (Elza-Neuspiel, work_vars[0x10]=15, Sprung 3091,
# Viereck an der Riegeltuer, Kreuz alle 30 Bilder) + RE15_FRAMEDUMP an den Original-Takten B306/B706/.../B2706
# (Port-Bild = 362 + B; erster Clip-13-Takt F363 = Original B1).
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/K41nb1"
BB="C:/workspace/git/reAi_v2/.claude/worktrees/r41_stage3szenen_b/re15_port/build"
B="$BB/platform/pc"
EXE="$B/re15_pc_k41nb1bild.exe"
W="$SP/${1:-bild3091}"
mkdir -p "$W"; cd "$W"; rm -f *.ppm debug.log anim.txt
cp "$B/re15_pc.exe" "$EXE"
P="300:A,310:A,320:A"
for f in $(seq 360 30 3700); do P="${P},${f}:X"; done
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset $v; done
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 \
  "RE15_SET_FLAG=4:8,9:21" "RE15_WORKVAR_AT=0x10:15@200" "RE15_DEBUG_JUMP=3090@250" "RE15_PLAYER_POS=-24665,1000,3072" \
  "RE15_PAD_AT=$P" RE15_ANIM_TRACE=anim.txt "RE15_EXIT_AT=30#30C1" "RE15_FRAMEDUMP=${FD:-668-3068/400}:fd_" \
  timeout 600 "$EXE" > stdout.txt 2>&1
echo "exit=$?" > exit.txt
rm -f "$EXE"
