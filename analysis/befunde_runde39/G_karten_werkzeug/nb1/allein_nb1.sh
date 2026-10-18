#!/bin/bash
# Nachbesserung 1 M3: jeder korrigierte Ort ALLEIN besucht (und aktuell) - nur die NEUE exe; VORHER-Abzuege der
# Abnahme liegen in ../G39abn0/al_<tag>_alt (nicht wiederholt). Liste wie G39abn0/allein.sh.
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
WT="C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten"
NEU="$WT/re15_port/build/platform/pc/re15_pc.exe"
SUF="${1:-nb1}"
while read tag r x z; do
  [ -z "$tag" ] && continue
  bash "$SP/G39abn0/lauf.sh" "$NEU" "$SP/G39nb1/al_${tag}_$SUF" $r 300 $r $x 0 $z 0 "besucht:$r:$x:$z" -- RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=60 "RE15_EXIT_AT=300#$r" > /dev/null
  [ -f "$SP/G39nb1/al_${tag}_$SUF/karte.bmp" ] || echo "FEHLT al_${tag}_$SUF"
done << 'LISTE'
1050 1050 20600 12350
1020 1020 -4000 -18000
1040 1040 -20083 -3559
1070 1070 160 6145
1030 1030 1172 -12558
1010a 1010 3650 -3950
1010b 1010 3650 6900
1080 1080 -13650 -900
1000o 1000 21850 -13400
1000n 1000 650 50
1000s 1000 650 3250
1110 1110 -2400 -5000
1210 1210 -26400 -2200
1220a 1220 -22400 -6500
1220c 1220 -22400 -14000
LISTE
echo FERTIG
