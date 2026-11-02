#!/bin/bash
# Profile neu, Ausgang Abgabestand der Gegenprobe. Aufruf: profile2.sh lauf/x.pt START
cd "$(dirname "$0")/../gerippe"
P=$1; S=$2
Z="kopfx2=325,kopfy2=245"
PK="--punkt 2,unterarm_r,start,312,322,5 --punkt 2,hand_r,start,257,398,4 --punkt 2,klaue_r1,ende,232,468,8 --punkt 2,klaue_r2,ende,190,462,8 --punkt 2,klaue_r3,ende,165,425,8"
PL="--punkt 2,klaue_l1,ende,460,392,6 --punkt 2,klaue_l2,ende,468,417,6 --punkt 2,klaue_l3,ende,437,418,6"
PF="--punkt 2,unterschenkel_r,start,254,313,10 --punkt 2,fuss_r,ende,249,368,8 --punkt 2,fuss_l,start,352,330,8"
lauf() { python fit.py --weiter $P --wahl $S --anatomie --kopfrand --kante --iter 500 --stride 3 --lr 0.012 --zwang "$3" --profil "$1" $PK $PL $PF --aus lauf/t_$2.pt > lauf/t_$2.log 2>&1; }
lauf "gier2=-40,-25,-15,0,21,45" gier2 "$Z"
lauf "rumpf=1500,1800,2100,2400,2800" rumpf "$Z"
lauf "arm=1500,1700,1900,2100,2300" arm "$Z"
lauf "bein=1400,1600,1800,2000,2200" bein "$Z"
lauf "schwanz=3000,3400,3800,4200" schwanz "$Z"
lauf "tiefe2=6000,6500,7000,7500,8000" tiefe2 "$Z"
lauf "tiefe1=4800,5200,5600,6000" tiefe1 "$Z"
lauf "gier1=-20,-5,10,25" gier1 "$Z"
lauf "kopfx2=290,310,325,345,365" kopfx2 "kopfy2=245"
lauf "kopfy2=215,230,245,260,280" kopfy2 "kopfx2=325"
echo PROFILE2_FERTIG
