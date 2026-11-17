#!/usr/bin/env bash
# Runde 7e (Handfarbe, Asset 07b6703f): Voll-Riegel S1-S21 (riegel_v8.py mit denselben Optionen wie runde7d/riegel_alle.sh, Laeufe sfv8/r7e),
# Teil-Riegel riegel_r7c.py NEU, Riegel Handfarbe NEU und Gegenprobe Stand 7d 5c902d49 (Laeufe r7d/szene + HF-Marker), dazu Voll-Riegel am
# Stand 7d mit den Werkzeugen von 7e (S17 f nach dem Nutzerentscheid). NACHEINANDER.
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
R=$S/r7c
D=$S/r7d
E=$S/r7e
Q=$S/r6b
P3=$S/p5/pruef_zahlen-r3/laeufe
P2=$S/p5/pruef_zahlen-r2/laeufe
B=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
W=$B/re15_port/tools/r41_sichel/v8
O=$B/analysis/befunde_runde41/SF_belege/v8/runde7e
A=$B/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
V7L="--r7c-lm-v7 $P3/V7_LM --r7c-plain-v7 $P3/V7 --r7c-lm-ein $R/laeufe/N7B_LM --r7c-plain-ein $R/laeufe/N7B --r7c-plain-r4 $P2/R4"
export PYTHONIOENCODING=utf-8
cd $W
git -C $B show 639132b77:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD > $E/emd/ALT_5c902d49.EMD
python riegel_handfarbe.py messen $A --plain $E/szene --hf $E/mk/lauf_HF_NEU --name "NEU 07b6703f" \
  --aus $O/riegel_handfarbe_NEU.txt --json $O/riegel_handfarbe_NEU.json > $E/riegel_hf_neu.out 2>&1; echo "rc_hf_neu=$?" >> $E/riegel_hf_neu.out
python riegel_handfarbe.py messen $E/emd/ALT_5c902d49.EMD --plain $D/szene --hf $E/mk/lauf_HF_ALT --name "Gegenprobe Stand 7d 5c902d49" \
  --aus $O/riegel_handfarbe_ALT_gegenprobe.txt --json $O/riegel_handfarbe_ALT_gegenprobe.json > $E/riegel_hf_alt.out 2>&1
echo "rc_hf_alt=$?" >> $E/riegel_hf_alt.out
python riegel_r7c.py teil $A $O/riegel_r7c_NEU.txt --r4 $Q/emd/R4.EMD $V7L --r7c-lm $E/mk/lauf_LM_NEU --r7c-plain $E/szene > $E/riegel_r7c_NEU.out 2>&1
echo "rc_teil=$?" >> $E/riegel_r7c_NEU.out
python riegel_v8.py $A --r4 $Q/emd/R4.EMD $V7L --r7c-lm $E/mk/lauf_LM_NEU --r7c-plain $E/szene \
  --mk-a $E/mk/lauf_MK_A --mk-b $E/mk/lauf_MK_B --gr $E/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $E/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $E/mk/lauf_KO_NEU \
  --aus $O/riegel_v8r7e.txt --json $O/riegel_v8r7e.json > $E/riegel_neu.out 2>&1
echo "rc_neu=$?" >> $E/riegel_neu.out
python riegel_v8.py $E/emd/ALT_5c902d49.EMD --r4 $Q/emd/R4.EMD $V7L --r7c-lm $D/mk/lauf_LM_NEU --r7c-plain $D/szene \
  --mk-a $D/mk/lauf_MK_A --mk-b $D/mk/lauf_MK_B --gr $D/mk/lauf_GR --gr-v7 $S/r4/mk/lauf_GR_V7 --fd $D/szene/fd \
  --fd-v7 $S/../sfv4/r/V7_szene/fd --ko-v7 $Q/mk/lauf_KO_V7 --ko $D/mk/lauf_KO_NEU \
  --aus $O/riegel_v8_ALT5c902d49_gegenprobe.txt --json $O/riegel_v8_ALT5c902d49_gegenprobe.json > $E/riegel_alt.out 2>&1
echo "rc_alt=$?" >> $E/riegel_alt.out
echo FERTIG > $E/riegel_alle_fertig.txt
# Ergebnis 2026-11-14: der letzte Aufruf (Voll-Riegel am Stand 7d) brach in S16 mit numpy MemoryError ab (luecke_r5.messen_parallel, Pool
# mit 12 Prozessen, gleichzeitig lief die Unit-Gruppe). Nicht wiederholt (Auftrag: hoechstens 2 Rechenprozesse); ersetzt durch
# riegel_s17f_gegenprobe.txt (S17 f allein: Stand 7d und V7 ROT, NEU GRUEN) - alle uebrigen Riegel aendern sich zwischen 7d und 7e
# nicht (riegel_v8r7e.txt gegen runde7d/riegel_v8r7d.txt: nur sha256-Zeile und S17 f anders).
