#!/bin/bash
# Feinlauf 2: Ausgang g7. Ellbogen und Handgelenk auf die Achse des Streifens (rechte Kante (335,326)-(274,408), halbe Breite 20 px nach links oben), Finger an die Luecke
cd "$(dirname "$0")/../gerippe"
PK="--punkt 2,unterarm_r,start,312,322,5 --punkt 2,hand_r,start,257,398,4 --punkt 2,klaue_r1,ende,232,468,8 --punkt 2,klaue_r2,ende,190,462,8 --punkt 2,klaue_r3,ende,165,425,8"
PL="--punkt 2,klaue_l1,ende,460,392,6 --punkt 2,klaue_l2,ende,468,417,6 --punkt 2,klaue_l3,ende,437,418,6"
PF="--punkt 2,unterschenkel_r,start,254,313,10 --punkt 2,fuss_r,ende,249,368,8 --punkt 2,fuss_l,start,352,330,8"
python fit.py --weiter lauf/g7.pt --wahl 0 --anatomie --kopfrand --kante --iter 1100 --stride 2 --lr 0.009 --zwang kopfx2=325,kopfy2=245 $PK $PL $PF --aus lauf/g8.pt
