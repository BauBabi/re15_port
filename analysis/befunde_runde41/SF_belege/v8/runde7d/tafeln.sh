#!/usr/bin/env bash
# Runde 7d (Abgabe): ALLE Tafeln 01-19 neu am Endstand 5c902d49 (Spiel-Asset), NACHEINANDER, in dieser Reihenfolge (spaetere Werkzeuge
# ueberschreiben die aelteren Fassungen derselben Nummer wie in den Runden 6d/7a/7b):
#   tafeln_v8.py   01-08, 10-13 (09/14/15 werden unten ersetzt), 11 aus den zugeordneten Fensterbildern runde7d/gdi_fenster_F*.png
#   tafeln_r6.py   09, 15, 17 (Spalten V7 | V8 r4 | V8 r5d | NEU bzw. V7 | V8 r5d | NEU)
#   tafel_r7b.py   14, 14b aus echten Framedumps (Video | V7 | Stand 6d | NEU)
#   tafel_r7a.py   16 (hand), 18 (stellung), 19_armhaltung_r7a (arm)
#   tafel_r7d.py   19_nacken_p5 (neu: Vorlage | V7 | V8 r4 | V8 r6 | NEU)
# Laeufe: NEU = sfv8/r7d/szene (+ mk/lauf_KL_NEU, mk/lauf_GR); V7 = sfv4/r/V7_szene; V8 r4 / V8 r5d / V8 r6 = Laeufe des Zahlen-Pruefers P5 R2
# (gleiche Exe 8f15f6c5); Stand 6d fuer 14/14b/16/18/19a = sfv8/r6d/szene (wie in 7a/7b).
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
D=$S/r7d
P=$S/p5/pruef_zahlen-r2/laeufe
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
T=$B/analysis/befunde_runde41/SF_belege/v8
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
V7=$S/emd/EM30_V7.EMD; R4=$S/r6b/emd/R4.EMD; R5D=$S/r6b/emd/R5D.EMD; R6=$S/r7a/emd/V6d.EMD
V7S=$S/../sfv4/r/V7_szene
export PYTHONIOENCODING=utf-8
cd $W
python tafeln_v8.py --v7 $V7 --v8 $A --szene7 $V7S --szene8 $D/szene --v8r2 $R4 --alt-name "V8 r4 10d02173" --szene8r3 $P/R4 --v8r4 $R4 \
  --gdi-bilder $T/runde7d > $D/tafeln_v8.txt 2>&1; echo "rc=$?" >> $D/tafeln_v8.txt
python tafeln_r6.py --v7 $V7 --r4 $R4 --r5d $R5D --neu $A --fd-v7 $V7S --fd-r5d $P/R5D --fd-neu $D/szene --ko-v7 $S/r6b/mk/lauf_KO_V7 \
  --gr $D/mk/lauf_GR --nur 09,15,17 > $D/tafeln_r6.txt 2>&1; echo "rc=$?" >> $D/tafeln_r6.txt
python tafel_r7b.py --v7 $V7S/fd --r6d $S/r6d/szene/fd --neu $D/szene/fd --kl $D/mk/lauf_KL_NEU/fd --emd-6d $R6 --emd-neu $A --aus $T \
  > $D/tafel_r7b.txt 2>&1; echo "rc=$?" >> $D/tafel_r7b.txt
python tafel_r7a.py hand --s6d $R6 --neu $A --v7 $V7 --aus $T/16_hand.png > $D/tafel_r7a_hand.txt 2>&1; echo "rc=$?" >> $D/tafel_r7a_hand.txt
python tafel_r7a.py stellung --fd-v7 $V7S/fd --fd-6d $S/r6d/szene/fd --fd-neu $D/szene/fd --s6d $R6 --neu $A --v7 $V7 --aus $T/18_handstellung.png \
  > $D/tafel_r7a_stellung.txt 2>&1; echo "rc=$?" >> $D/tafel_r7a_stellung.txt
python tafel_r7a.py arm --fd-v7 $V7S/fd --fd-6d $S/r6d/szene/fd --fd-neu $D/szene/fd --aus $T/19_armhaltung_r7a.png > $D/tafel_r7a_arm.txt 2>&1
echo "rc=$?" >> $D/tafel_r7a_arm.txt
python tafel_r7d.py --v7 $V7 --r4 $R4 --r6 $R6 --neu $A --fd-v7 $P/V7/fd --fd-r4 $P/R4/fd --fd-r6 $P/R6/fd --fd-neu $D/szene/fd \
  --ko $D/mk/lauf_KO_NEU/fd --aus $T/19_nacken_p5.png > $D/tafel_r7d.txt 2>&1; echo "rc=$?" >> $D/tafel_r7d.txt
echo TAFELN-FERTIG > $D/tafeln_fertig.txt
