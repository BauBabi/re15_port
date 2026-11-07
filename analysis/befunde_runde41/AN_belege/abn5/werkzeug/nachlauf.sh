#!/usr/bin/env bash
# nachlauf.sh - Abnahme 5 (zweiter Abnehmer), nach Kette 2: (a) Asset-Selbsttest der Messkopie, (b) an6000_bahn: Leon steht auf der
# eigenen Bahn der 0x4D (Spawn @0x00F68, Zustand 1, Bahnpunkt F21 (5131,-38) aus an6000_direkt), keine Eingabe; sie laeuft in ihn
# hinein, der Koerperschub muss ihn auf 900 (RE1.5) halten.
W=$(dirname "$0"); L=$W/lauf.sh
M=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6/abn5/mein
mkdir -p "$M/laeufe/selftest"; cd "$M/laeufe/selftest" && cp "$M/exe/re15_pc_anabn5.exe" "$M/exe/re15_pc_anabn5_selftest_$$.exe" && \
  RE15_ASSET_SELFTEST=1 SDL_ASSERT=always_ignore PATH=/c/msys64/mingw64/bin:$PATH timeout 60 "$M/exe/re15_pc_anabn5_selftest_$$.exe" > "$M/belege/selftest_abn5.txt" 2>&1; echo "selftest exit=$?" >> "$M/belege/selftest_abn5.txt"; rm -f "$M/exe/re15_pc_anabn5_selftest_$$.exe"
grep "cd-root\|RESULT\|exit=" "$M/belege/selftest_abn5.txt"
bash $L an6000_bahn 120 /dev/stdin RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=5131,-38,2048,2 RE15_SET_FLAG=3:48,3:49,3:100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=200#6000 RE15_FRAMEDUMP=10-80/2:f <<<"W6"
echo "NACHLAUF-FERTIG $(date '+%F %T')"
