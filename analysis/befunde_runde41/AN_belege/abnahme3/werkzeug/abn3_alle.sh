#!/usr/bin/env bash
# Abnahme 3: alle Fensterlaeufe nacheinander (keine Parallel-Last). Basis = Torstand wie Abnahme 2.
L=$(dirname "$0")/abn3_lauf.sh
BASIS="RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=1763,-9700,1024,2 RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23,3:89,3:22"
bash $L z6030 300 $(dirname "$0")/eingabe_kette.txt $BASIS RE15_SE_DEBUG=1 RE15_EXIT_AT=1700#6030 RE15_FRAMEDUMP=1340-1700/1:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/z6030/f RE15_AUDIO_CAP_SYNC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/z6030/ton.raw
bash $L s5090 420 $(dirname "$0")/eingabe_kette.txt $BASIS RE15_SE_DEBUG=1 RE15_EXIT_AT=1965#5090 RE15_FRAMEDUMP=1150-1960/1:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/s5090/f RE15_AUDIO_CAP_SYNC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/s5090/ton.raw
bash $L kampf 700 $(dirname "$0")/eingabe_kette_kampf.txt $BASIS RE15_SE_DEBUG=1 RE15_EQUIP=3 RE15_GIVE=3:400,21:250 RE15_EXIT_AT=5400#5090 RE15_FRAMEDUMP=1700-5400/10:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/kampf/f RE15_AUDIO_CAP_SYNC=C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/kampf/ton.raw
bash $L tor 150 /dev/stdin RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=18200,-22404,0,2 RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23 RE15_FLAG_CENSUS=1 RE15_EXIT_AT=420#6000 RE15_FRAMEDUMP=40-410/10:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/tor/f <<<"W5.5,W1.8,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W0.4,A0.1,W12"
bash $L leiche 150 /dev/stdin RE15_DEBUG_JUMP=5090@250 RE15_PLAYER_POS=500,-9175,2048,0 RE15_SET_FLAG=3:89,9:133,3:42 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=900#5090 RE15_FRAMEDUMP=250-900/5:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/leiche/f <<<"A0.1,W1,U8.5,W12"

# Zusatz: Wiedereintritt, Leon geht im Wagen C an der Leiche vorbei nach Westen (Cut 11 von hinten)
bash $L leiche_west 150 /dev/stdin RE15_DEBUG_JUMP=5090@250 RE15_PLAYER_POS=500,-9175,2048,0 RE15_SET_FLAG=3:89,9:133,3:42 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=1100#5090 RE15_FRAMEDUMP=560-1100/10:C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/leiche_west/f <<<"A0.1,W1,U16,W10"
echo ALLE-FERTIG
# Sonden (gegen libre15_engine.a des Baums; npc_schuss zusaetzlich gegen re15_damage.c aus b646c72a6^):
#   gcc -DRE15_ASSETS_PATH=... -DRE15_ASSET_PSX_DIR=\"...\" -DRE15_PLATFORM_PC=1 -I<re15_port>/include -std=c11 npc_schuss.c [dmg_vor.o] engine/libre15_engine.a tests/libre15_test_support.a -lm
#   git show b646c72a6^:re15_port/engine/src/re15_damage.c > re15_damage_vor_nb4.c ; gcc <flags> -c re15_damage_vor_nb4.c -o dmg_vor.o
