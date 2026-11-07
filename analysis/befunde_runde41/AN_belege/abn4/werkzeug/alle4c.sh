#!/usr/bin/env bash
# alle4c.sh - Abnahme 4 Spur AN, Elza-Nachlauf: echter Elza-Raum ROOM5031, sub00 @0x010B2 Ck(3,47)=0 setzt Sherry 0x4B
# (Sce_em_set @0x010B6, (14100,0,17050)) und den kleinen Affen 0x29 (Sce_em_set @0x010CA, (18200,0,17050)) auf dieselbe Linie z 17050.
# Elza steht westlich (11000,17050) mit Blick +x, Sherry 3100 vor ihr, der Affe 7200 (per RE15_GEGNER_POS festgehalten).
# Pistole (RE15_EQUIP=3 wie integration_r41_birkin_pistole), Zielen + Feuer 6 s.
W=$(dirname "$0"); L=$W/lauf4.sh
bash $L elza5031 150 /dev/stdin RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=5031@250 RE15_PLAYER_POS=11000,17050,0,0 "RE15_GEGNER_POS=2,18200,17050,2048@1#5031" RE15_EQUIP=3 RE15_GIVE=3:100 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=520#5031 RE15_FRAMEDUMP=270-520/10:f <<<"W6,MA6,W1"
echo ALLE-C-FERTIG
