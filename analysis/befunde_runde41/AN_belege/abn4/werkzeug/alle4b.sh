#!/usr/bin/env bash
# alle4b.sh - Abnahme 4 Spur AN, Nachlauf: Leiche im Kampf genau auf die Schusslinie Leon -> Form/G5 gelegt (Messhaken
# RE15_GEGNER_POS, Akteur 1 = Annette ab Bild 1790 auf (9000,-23550)); Rest wie Lauf "kampf" (Kette ab dem Tor, Pistole).
W=$(dirname "$0"); L=$W/lauf4.sh
BASIS="RE15_DEBUG_JUMP=6000@250 RE15_PLAYER_POS=1763,-9700,1024,2 RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23,3:89,3:22"
bash $L kampf_linie 700 $W/eingabe_kette_kampf.txt $BASIS RE15_EQUIP=3 RE15_GIVE=3:400,21:250 "RE15_GEGNER_POS=1,9000,-23550,1170@1790#5090" RE15_EXIT_AT=3400#5090 RE15_FRAMEDUMP=1800-3400/40:f
# ROOM6030 Fuehrerstand vor dem Tor ((4,23)=1, (3,89)=0: sub07 @0x010F8 setzt die 0x4D ans Pult, RE1.5-Inhalt): Leon 810 westlich
# von ihr, Blick +x, geht 2 s in sie hinein. Gemessen wird der Abstand, bei dem der Koerperschub ihn haelt (Port: Spieler 450 + 0x4D).
bash $L an6030_pult 120 /dev/stdin RE15_DEBUG_JUMP=6030@250 RE15_PLAYER_POS=-2600,-22272,0,0 RE15_SET_FLAG=3:48,3:49,3:100,3:239,4:23 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_EXIT_AT=400#6030 RE15_FRAMEDUMP=90-400/10:f <<<"U2,W4"
echo ALLE-B-FERTIG
