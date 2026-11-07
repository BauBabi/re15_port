#!/bin/bash
# Sechster Versuch: Ausgang g4 (Rumpf ohne Wespentaille), gelesene Kante einseitig
cd "$(dirname "$0")/../gerippe"
PK="--punkt 2,unterarm_r,start,326,332,5 --punkt 2,hand_r,start,262,398,4 --punkt 2,klaue_r1,ende,235,468,8 --punkt 2,klaue_r2,ende,200,462,8 --punkt 2,klaue_r3,ende,168,425,8"
PL="--punkt 2,klaue_l1,ende,460,392,6 --punkt 2,klaue_l2,ende,468,417,6 --punkt 2,klaue_l3,ende,437,418,6"
PF="--punkt 2,unterschenkel_r,start,254,313,10 --punkt 2,fuss_r,ende,249,368,8 --punkt 2,fuss_l,start,352,330,8"
python fit.py --weiter lauf/g4.pt --wahl 0,0 --gier2start=-15,-10 --anatomie --kopfrand --kante --iter 1500 --stride 3 --lr 0.012 --zwang kopfx2=325,kopfy2=245 $PK $PL $PF --aus lauf/g6.pt
