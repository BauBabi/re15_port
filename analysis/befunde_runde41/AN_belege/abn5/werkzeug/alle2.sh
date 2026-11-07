#!/usr/bin/env bash
# alle2.sh - Abnahme 5 Spur AN (zweiter Abnehmer): Rest der Fensterlaeufe NACHEINANDER (Kette 1 brach bei z6030 am Eingabepfad ab,
# der Lauf z6030 aus Kette 1 ist verworfen). Eingaben = Original der Abnahme 3 (abnahme3/werkzeug, bitgleich mit abn5/werkzeug).
W=$(dirname "$0"); L=$W/lauf.sh; K=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/abnahme3/werkzeug
BASIS="RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=1763,-9700,1024,2 RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23,3:89,3:22"
J5090="RE15_DEBUG_JUMP=5090@250 RE15_PLAYER_POS=500,-9175,2048,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100"
echo "START2 $(date '+%F %T')"
bash $L z6030 300 $K/eingabe_kette.txt $BASIS RE15_SE_DEBUG=1 RE15_EXIT_AT=1700#6030 RE15_FRAMEDUMP=1440-1600/4:f
# (2) RE2-Szene und Leiche: RE2-Segment 350
bash $L s5090 420 $K/eingabe_kette.txt $BASIS RE15_SE_DEBUG=1 RE15_EXIT_AT=1965#5090 RE15_FRAMEDUMP=1150-1960/15:f
bash $L s5090_leiche750 420 $K/eingabe_kette.txt $BASIS RE15_SE_DEBUG=1 "RE15_GEGNER_POS=1,13025,-23550,1170@1860#5090" RE15_EXIT_AT=1905#5090 RE15_FRAMEDUMP=1850-1905/5:f
bash $L leiche_west 150 /dev/stdin $J5090 RE15_SET_FLAG=3:89,9:133,3:42 RE15_EXIT_AT=1100#5090 RE15_FRAMEDUMP=300-700/5:f <<<"A0.1,W1,U16,W10"
# (3) NB5: Schuesse durch die lebende 0x4D und durch die Leiche
bash $L an_durch 150 /dev/stdin $J5090 RE15_SET_FLAG=3:89,9:133 RE15_EQUIP=3 RE15_GIVE=3:100 "RE15_GEGNER_POS=1,600,-23550,2048@120#5090" RE15_EXIT_AT=400#5090 RE15_FRAMEDUMP=130-400/10:f <<<"A0.1,W1,U2,MA4,W2"
bash $L an_vorszene 300 /dev/stdin $J5090 RE15_SET_FLAG=3:89,9:133 RE15_EQUIP=3 RE15_GIVE=3:100 RE15_EXIT_AT=1300#5090 RE15_FRAMEDUMP=130-1300/10:f <<<"A0.1,W1,U2,MA3,U8.5,W45"
bash $L kampf_linie 700 $K/eingabe_kette_kampf.txt $BASIS RE15_EQUIP=3 RE15_GIVE=3:400,21:250 "RE15_GEGNER_POS=1,9000,-23550,1170@1790#5090" RE15_EXIT_AT=3400#5090 RE15_FRAMEDUMP=1800-3400/40:f
# (4) Elza
bash $L elza5091 300 /dev/stdin RE15_DEBUG_JUMP=5091@250 RE15_TITLE_SHOT_CHAR=1 RE15_PLAYER_POS=500,-9175,2048,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_SET_FLAG=3:89,9:133 RE15_EQUIP=3 RE15_GIVE=3:100 RE15_EXIT_AT=1300#5091 RE15_FRAMEDUMP=100-1300/20:f <<<"A0.1,W1,U2,MA3,U8.5,W30"
# (5) 6000 direkt, ohne Messhaken: Spawn @0x00F68 ((3,100)=1, (3,239)=0), die 0x4D laeuft selbst nach (5198,1622) und bleibt dort;
#     Leon steht oestlich bei (6400,1600) mit Blick -x (rot 2048) und geht ab Bild 100 fuer 2 s auf sie zu.
bash $L an6000_direkt 150 /dev/stdin RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=6400,1600,2048,2 RE15_SET_FLAG=3:48,3:49,3:100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=300#6000 RE15_FRAMEDUMP=90-300/10:f <<<"U2,W3"
# (6) Kampf-Referenz (44 hp-Wechsel der G5)
bash $L kampf 700 $K/eingabe_kette_kampf.txt $BASIS RE15_SE_DEBUG=1 RE15_EQUIP=3 RE15_GIVE=3:400,21:250 RE15_EXIT_AT=5400#5090 RE15_FRAMEDUMP=1700-5400/50:f
echo "ALLE2-FERTIG $(date '+%F %T')"
