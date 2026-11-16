#!/usr/bin/env bash
# Runde 8d: Bezuege sfv8/r4 und sfv8/r6b fehlen seit der Runde 8c (Ordner geloescht) -> R4.EMD aus sfv8/p5/staende (sha 10d02173, gleich), Marker-
# Laeufe GR_V7 / KO_V7 mit V7 (319bbd17, Kette) und der heutigen exe neu; riegel_r7c teil und riegel_v8 fuer 8d UND zur Kontrolle fuer 8c (d31f9534,
# Laeufe r8d) mit denselben neuen Bezuegen (Summenzeile 8c muss gleich bleiben).
R8=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
P=$R8/r8e
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
O=$B/analysis/befunde_runde41/SF_belege/v8/runde8d
export PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1 BAUM=$B EXE_SRC=$B/re15_port/build_r8/platform/pc/re15_pc.exe
L=$B/re15_port/tools/r41_sichel/v4/lauf_v4.sh
W=$B/re15_port/tools/r41_sichel/v8
V7=$P/kette_1/emd/EM30_V7.EMD
mkdir -p $P/mk_v7
sha256sum $V7 $S/p5/staende/R4.EMD $EXE_SRC > $P/mk_v7/eingang.txt
( cd $W && python szene_marker.py bauen $V7 $P/mk_v7 && python szene_marker.py ko $V7 $P/mk_v7/KO_V7.EMD ) > $P/mk_v7/bauen.txt 2>&1
(time FD=300-380/1 bash $L $P/mk_v7/lauf_GR_V7 $P/mk_v7/GR.EMD 390) > $P/mk_v7/lauf_GR_V7.txt 2>&1
(time FD=225-450/1 bash $L $P/mk_v7/lauf_KO_V7 $P/mk_v7/KO_V7.EMD 460) > $P/mk_v7/lauf_KO_V7.txt 2>&1
R=$S/r7c; P3=$S/p5/pruef_zahlen-r3/laeufe; P2=$S/p5/pruef_zahlen-r2/laeufe; R4=$S/p5/staende/R4.EMD
V7L="--r7c-lm-v7 $P3/V7_LM --r7c-plain-v7 $P3/V7 --r7c-lm-ein $R/laeufe/N7B_LM --r7c-plain-ein $R/laeufe/N7B --r7c-plain-r4 $P2/R4"
export RE15_RIEGEL_JOBS=2
cd $W
for st in 8d 8c; do
  if [ $st = 8d ]; then E=$A; Q=$P; AUS=$O/riegel_v8r8d; T=$O/riegel_r7c_NEU.txt; else E=$R8/emd/R8C_d31f9534.EMD; Q=$R8/r8d; AUS=$O/kontrolle_8c_riegel_v8; T=$O/kontrolle_8c_riegel_r7c.txt; fi
  export RE15_RIEGEL_PICKLE=$P/riegel_v8_$st.pkl
  python riegel_r7c.py teil $E $T --r4 $R4 $V7L --r7c-lm $Q/mk/lauf_LM_NEU --r7c-plain $Q/szene > $P/riegel_r7c_$st.out 2>&1; echo "rc=$?" >> $P/riegel_r7c_$st.out
  python riegel_v8.py $E --r4 $R4 $V7L --r7c-lm $Q/mk/lauf_LM_NEU --r7c-plain $Q/szene \
    --mk-a $Q/mk/lauf_MK_A --mk-b $Q/mk/lauf_MK_B --gr $Q/mk/lauf_GR --gr-v7 $P/mk_v7/lauf_GR_V7 --fd $Q/szene/fd \
    --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $P/mk_v7/lauf_KO_V7 --ko $Q/mk/lauf_KO_NEU --szene $Q/szene/debug.log --farbe b \
    --aus $AUS.txt --json $AUS.json > $P/riegel_v8_$st.out 2>&1; echo "rc=$?" >> $P/riegel_v8_$st.out
done
echo FERTIG > $P/riegel_nach_fertig.txt
