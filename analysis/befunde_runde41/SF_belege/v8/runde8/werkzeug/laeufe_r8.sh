#!/usr/bin/env bash
# Runde 8 (Schwanz, Stufe q): Marker-Assets aus dem Spiel-Asset 4cf28242 bauen, dann Szenenlauf, Farbproben-Laeufe a / c und Marker-Laeufe
# NACHEINANDER (eine Spiel-Instanz; Muster runde7e/laeufe.sh). Spiel-Asset unberuehrt (Proben und Marker per Overlay-Wurzel von lauf_v4.sh).
# exe = eigener Bau re15_port/build_r8 (local_build.sh, RE15_BUILD_DIR), Quelltext der Engine = Stand 7e (nur Werkzeuge/Test/Asset geaendert).
P=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8 PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1
export EXE_SRC=$BAUM/re15_port/build_r8/platform/pc/re15_pc.exe
L=$BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh
A=$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
W=$BAUM/re15_port/tools/r41_sichel/v8
mkdir -p $P/mk
sha256sum $A $EXE_SRC > $P/laeufe_eingang.txt
( cd $W && python szene_marker.py bauen $A $P/mk && python szene_marker.py ko $A $P/mk/KO_NEU.EMD \
  && python riegel_r7c.py lm $A $P/mk/LM_NEU.EMD ) > $P/mk/bauen.txt 2>&1
echo "bauen rc=$?" >> $P/mk/bauen.txt
sha256sum $P/mk/*.EMD >> $P/mk/bauen.txt
(time FD=200-1100/1 bash $L $P/szene - 1110) > $P/lauf_szene.txt 2>&1
(time FD=255-320/1 bash $L $P/probe_a $P/proben/EM30_R8_a.EMD 330) > $P/lauf_probe_a.txt 2>&1
(time FD=255-320/1 bash $L $P/probe_c $P/proben/EM30_R8_c.EMD 330) > $P/lauf_probe_c.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_LM_NEU $P/mk/LM_NEU.EMD 460) > $P/mk/lauf_LM_NEU.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_KO_NEU $P/mk/KO_NEU.EMD 460) > $P/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do (time FD=300-380/1 bash $L $P/mk/lauf_$m $P/mk/$m.EMD 390) > $P/mk/lauf_$m.txt 2>&1; done
echo ALLE-FERTIG > $P/laeufe_fertig.txt
