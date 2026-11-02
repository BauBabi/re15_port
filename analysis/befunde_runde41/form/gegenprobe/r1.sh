#!/bin/bash
# Erster Versuch: Ausgang Abgabestand, rechter Arm auf den Streifen, Anatomie-Kosten an, Gier Bild 2 frei mit vier Starts
cd "$(dirname "$0")/../gerippe"
PK="--punkt 2,unterarm_r,start,325,331,5 --punkt 2,hand_r,start,262,398,4 --punkt 2,klaue_r1,ende,240,470,6 --punkt 2,klaue_r2,ende,200,470,6 --punkt 2,klaue_r3,ende,165,425,6"
PL="--punkt 2,klaue_l1,ende,460,392,5 --punkt 2,klaue_l2,ende,468,417,5 --punkt 2,klaue_l3,ende,437,418,5"
PF="--punkt 2,fuss_r,start,247,352,6 --punkt 2,fuss_l,start,352,330,6"
python fit.py --weiter lauf/ende3.pt --wahl 0,0,0,0 --gier2start 21,0,-20,40 --vorpunkte 400 --anatomie --iter 1400 --stride 3 --lr 0.015 --zwang kopfx2=325,kopfy2=240 $PK $PL $PF --aus lauf/g1.pt
