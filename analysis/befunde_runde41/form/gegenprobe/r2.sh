#!/bin/bash
# Zweiter Versuch: Pose 2 ueber Anker im Raum umgelegt (gerader Rumpf, Gier etwa -18), Oberarm laenger, Kopf auf Zug Q
cd "$(dirname "$0")/../gerippe"
PK="--punkt 2,unterarm_r,start,326,332,6 --punkt 2,hand_r,start,262,398,5"
PL="--punkt 2,klaue_l1,ende,460,392,6 --punkt 2,klaue_l2,ende,468,417,6 --punkt 2,klaue_l3,ende,437,418,6"
PF="--punkt 2,fuss_r,start,247,352,8 --punkt 2,fuss_l,start,352,330,8"
python fit.py --weiter lauf/ende3.pt --wahl 0,0,0,0 --gier2start=-18,-12,-25,-5 --setze oberarm=850,unterarm=1130,kopf=1000,kiefer=620 --anker3d ../gegenprobe/anker_a.json --vorpunkte 700 --anatomie --kopfrand --iter 1500 --stride 3 --lr 0.012 --zwang kopfx2=325,kopfy2=245 $PK $PL $PF --aus lauf/g2.pt
