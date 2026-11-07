#!/bin/bash
# Dritter Versuch: Ausgang g2 Start 3, Pose 2 feiner verankert (Knie, Knoechel, Klauen am Boden), Kiefer vorn, Dicken der Arme zurueck
cd "$(dirname "$0")/../gerippe"
PK="--punkt 2,unterarm_r,start,326,332,5 --punkt 2,hand_r,start,262,398,4 --punkt 2,klaue_r1,ende,235,468,7 --punkt 2,klaue_r2,ende,200,462,7 --punkt 2,klaue_r3,ende,168,425,7"
PL="--punkt 2,klaue_l1,ende,460,392,6 --punkt 2,klaue_l2,ende,468,417,6 --punkt 2,klaue_l3,ende,437,418,6"
PF="--punkt 2,unterschenkel_r,start,254,313,10 --punkt 2,fuss_r,ende,249,368,8 --punkt 2,fuss_l,start,352,330,8"
python fit.py --weiter lauf/g2.pt --wahl 3,3,3,3 --gier2start=-5,-12,-20,-28 \
 --setze kiefer_x=380,kiefer_y=190,kiefer=420,schenkel_x=-480,huefte_hinten=60 \
 --setzed "oberarm=340/320/300;unterarm=320/285/265;klaue=200/140/70;hand=230/260/230:290/340/330" \
 --anker3d ../gegenprobe/anker_b.json --vorpunkte 900 --anatomie --kopfrand --iter 1600 --stride 3 --lr 0.012 --zwang kopfx2=325,kopfy2=245 $PK $PL $PF --aus lauf/g3.pt
