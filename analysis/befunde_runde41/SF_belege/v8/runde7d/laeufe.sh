#!/usr/bin/env bash
# Runde 7d Abgabe: Marker-Assets aus dem Spiel-Asset 5c902d49 neu bauen (Vergleich mit r7c/mk), dann Szenenlauf und Marker-Laeufe NACHEINANDER
# (eine Spiel-Instanz). Spiel-Asset unberuehrt (Marker per Overlay-Wurzel von lauf_v4.sh).
P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r7d
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 PYTHONIOENCODING=utf-8
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
A=$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
W=$BAUM/re15_port/tools/r41_sichel/v8
( cd $W && python szene_marker.py bauen $A $P/mk && python szene_marker.py ko $A $P/mk/KO_NEU.EMD \
  && python riegel_r7c.py lm $A $P/mk/LM_NEU.EMD && python glaette_r7b.py marker $A $P/mk/KL_NEU.EMD ) > $P/mk/bauen.txt 2>&1
for m in MK_A MK_B GR KO_NEU LM_NEU; do cmp -s $P/mk/$m.EMD $P/../r7c/mk/$m.EMD && echo "$m bytegleich r7c" || echo "$m ANDERS als r7c"; done >> $P/mk/bauen.txt
sha256sum $P/mk/*.EMD >> $P/mk/bauen.txt
(time FD=200-1100/1 bash $L $P/szene - 1110) > $P/lauf_szene.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_LM_NEU $P/mk/LM_NEU.EMD 460) > $P/mk/lauf_LM_NEU.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_KO_NEU $P/mk/KO_NEU.EMD 460) > $P/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do (time FD=300-380/1 bash $L $P/mk/lauf_$m $P/mk/$m.EMD 390) > $P/mk/lauf_$m.txt 2>&1; done
(time FD=200-1100/1 bash $L $P/mk/lauf_KL_NEU $P/mk/KL_NEU.EMD 1110) > $P/mk/lauf_KL_NEU.txt 2>&1
echo ALLE-FERTIG > $P/laeufe_fertig.txt
