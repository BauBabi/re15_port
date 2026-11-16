#!/usr/bin/env bash
# Runde 8c NACHEINANDER (hoechstens 2 Rechenprozesse): Asset + Pin, Kette zweimal, Bau + ctest-Gruppe, Szenen- und Markerlaeufe, Riegel.
# Asset = schwanz_r8.py Stufe q (Commit 804c62601) auf 07b6703f, Farbe b; erwartet sha256 d31f9534... (Probe K_RB / NEU_b).
R8=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/r8
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
P=$R8/r8d
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
O=$B/analysis/befunde_runde41/SF_belege/v8/runde8c
mkdir -p $O
export PYTHONIOENCODING=utf-8 PYTHONDONTWRITEBYTECODE=1
cd $B
# 1 Asset + Pin
cp -f $P/NEU_b.EMD $A
H=$(sha256sum $A | cut -d' ' -f1)
printf '%s  re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD\n' "$H" > release/em30_hangar.sha256
echo "asset $H" > $P/ablauf.log
# 2 Kette zweimal
for k in 1 2; do
  rm -rf $P/kette_$k
  bash re15_port/tools/r41_sichel/v8_kette.sh $P/kette_$k > $O/kette_lauf$k.txt 2>&1
  echo "rc_kette=$?" >> $O/kette_lauf$k.txt
done
echo "kette fertig" >> $P/ablauf.log
# 3 Bau + ctest-Gruppe
RE15_BUILD_DIR=$B/re15_port/build_r8 RE15_JOBS=2 bash re15_port/tools/local_build.sh build > $P/bau.txt 2>&1
echo "rc_bau=$?" >> $P/bau.txt
( cd $B/re15_port && export PATH="/c/msys64/mingw64/bin:$PATH" && sha256sum shared_assets/PSX/EMD/EM30_HANGAR.EMD > $O/ctest_gruppe.txt && \
  ctest --test-dir build_r8 -j 1 --timeout 600 -R "^(unit_birkin_ai|unit_birkin_formen|unit_birkin_bahnsteig|unit_birkin_szenen|unit_schwanz_r41|integration_r41_birkin_pistole|integration_r41_sichel_form|unit_birkin_opfer)$" >> $O/ctest_gruppe.txt 2>&1; echo "rc=$?" >> $O/ctest_gruppe.txt )
( cd $B/re15_port && export PATH="/c/msys64/mingw64/bin:$PATH" && build_r8/tests/unit/test_birkin_formen.exe > $O/test_formen_neu.txt 2>&1; echo "rc=$?" >> $O/test_formen_neu.txt
  for g in ALT_07b6703f R8B2_66db2244; do cp -f $R8/emd/$g.EMD $A; build_r8/tests/unit/test_birkin_formen.exe > $O/test_formen_gegenprobe_$g.txt 2>&1; echo "rc=$?" >> $O/test_formen_gegenprobe_$g.txt; done
  cp -f $P/NEU_b.EMD $A; sha256sum $A >> $P/ablauf.log )
echo "bau/test fertig" >> $P/ablauf.log
# 4 Laeufe (eine Spiel-Instanz nacheinander)
export BAUM=$B EXE_SRC=$B/re15_port/build_r8/platform/pc/re15_pc.exe
L=$B/re15_port/tools/r41_sichel/v4/lauf_v4.sh
W=$B/re15_port/tools/r41_sichel/v8
mkdir -p $P/mk
sha256sum $A $EXE_SRC > $P/laeufe_eingang.txt
( cd $W && python szene_marker.py bauen $A $P/mk && python szene_marker.py ko $A $P/mk/KO_NEU.EMD \
  && python riegel_r7c.py lm $A $P/mk/LM_NEU.EMD ) > $P/mk/bauen.txt 2>&1
echo "bauen rc=$?" >> $P/mk/bauen.txt
(time FD=200-1100/1 bash $L $P/szene - 1110) > $P/lauf_szene.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_LM_NEU $P/mk/LM_NEU.EMD 460) > $P/mk/lauf_LM_NEU.txt 2>&1
(time FD=225-450/1 bash $L $P/mk/lauf_KO_NEU $P/mk/KO_NEU.EMD 460) > $P/mk/lauf_KO_NEU.txt 2>&1
for m in MK_A MK_B GR; do (time FD=300-380/1 bash $L $P/mk/lauf_$m $P/mk/$m.EMD 390) > $P/mk/lauf_$m.txt 2>&1; done
echo "laeufe fertig" >> $P/ablauf.log
# 5 Riegel
R=$S/r7c; Q=$S/r6b; P3=$S/p5/pruef_zahlen-r3/laeufe; P2=$S/p5/pruef_zahlen-r2/laeufe
V7L="--r7c-lm-v7 $P3/V7_LM --r7c-plain-v7 $P3/V7 --r7c-lm-ein $R/laeufe/N7B_LM --r7c-plain-ein $R/laeufe/N7B --r7c-plain-r4 $P2/R4"
export RE15_RIEGEL_JOBS=2 RE15_RIEGEL_PICKLE=$P/riegel_v8_r.pkl
cd $W
python riegel_r7c.py teil $A $O/riegel_r7c_NEU.txt --r4 $Q/emd/R4.EMD $V7L --r7c-lm $P/mk/lauf_LM_NEU --r7c-plain $P/szene > $P/riegel_r7c_NEU.out 2>&1
echo "rc_teil=$?" >> $P/riegel_r7c_NEU.out
python riegel_v8.py $A --r4 $Q/emd/R4.EMD $V7L --r7c-lm $P/mk/lauf_LM_NEU --r7c-plain $P/szene \
  --mk-a $P/mk/lauf_MK_A --mk-b $P/mk/lauf_MK_B --gr $P/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $P/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $P/mk/lauf_KO_NEU --szene $P/szene/debug.log --farbe b \
  --aus $O/riegel_v8r8c.txt --json $O/riegel_v8r8c.json > $P/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $P/riegel_neu.out
python riegel_schwanz_r8.py $A --farbe b --szene $S/r7e/szene/debug.log $P/szene/debug.log $S/r7e/szene/state.log $P/szene/state.log \
  --aus $O/r8_b.txt --json $O/r8_b.json > $P/r8_b.out 2>&1; echo "rc_r8b=$?" >> $P/r8_b.out
python riegel_schwanz_r8.py $R8/emd/ALT_07b6703f.EMD --farbe b --aus $O/r8_gegenprobe_07b6703f.txt > $P/r8_g1.out 2>&1; echo "rc=$?" >> $P/r8_g1.out
python riegel_schwanz_r8.py $R8/emd/R8B2_66db2244.EMD --farbe b --aus $O/r8_gegenprobe_66db2244.txt > $P/r8_g2.out 2>&1; echo "rc=$?" >> $P/r8_g2.out
echo "riegel fertig" >> $P/ablauf.log
echo FERTIG > $P/ablauf_fertig.txt
