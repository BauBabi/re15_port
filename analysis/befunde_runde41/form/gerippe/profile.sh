#!/bin/bash
# Profile: je Groesse ein Wert erzwungen, Rest frei nachgerechnet, Ausgang ist der Abgabestand. Aufruf: profile.sh lauf/x.pt START
cd "$(dirname "$0")"
P=$1; S=$2
Z="gier2=21,kopfx2=325,kopfy2=230"
lauf() { python fit.py --weiter $P --wahl $S --hand2 boden --iter 500 --stride 3 --lr 0.012 --zwang "$3" --profil "$1" --aus lauf/q_$2.pt > lauf/q_$2.log 2>&1; }
lauf "rumpf=1200,1500,1800,2100,2400,2800" rumpf "$Z"
lauf "arm=1400,1600,1800,2000,2300,2600" arm "$Z"
lauf "bein=1200,1400,1600,1800,2100,2500" bein "$Z"
lauf "schwanz=2400,2900,3400,3900,4400" schwanz "$Z"
lauf "tiefe2=5500,6000,6500,7000,7500,8000" tiefe2 "$Z"
lauf "tiefe1=4400,4800,5200,5600,6000" tiefe1 "$Z"
lauf "gier2=-30,0,21,45,70,95" gier2 "kopfx2=325,kopfy2=230"
lauf "gier1=-35,-20,-5,10,25,40" gier1 "$Z"
lauf "kopfx2=260,290,325,360,390" kopfx2 "gier2=21,kopfy2=230"
lauf "kopfy2=190,230,270,310" kopfy2 "gier2=21,kopfx2=325"
echo PROFILE_FERTIG
