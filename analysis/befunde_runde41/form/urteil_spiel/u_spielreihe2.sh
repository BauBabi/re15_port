#!/usr/bin/env bash
# Pruefer Spielmodell: neutrale Posen im echten Spiel, gleiche Lage, gleiche Gier. Wartet, bis die erste Reihe fertig ist. Beendet nur eigene PIDs (form_spiel.sh).
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$BAUM/analysis/befunde_runde41/form
A=$F/urteil_spiel/spiel
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/biege"
L1=$F/urteil_spiel/spiel_reihe.log
L=$F/urteil_spiel/spiel_reihe2.log
until grep -a -q FERTIG "$L1"; do sleep 3; done
: > "$L"
lauf() {
    bash "$BAUM/re15_port/tools/form_spiel.sh" --emd "$2" --tim "$3" --pose "$4" --raum "$5" --aus "$A/$1.png" >>"$L" 2>&1
    echo "== $1 rc=$?" >>"$L"
    grep -a "Laufverzeichnis" "$L" | tail -1 | sed 's/Laufverzeichnis: //' | tr -d '\r' >>"$F/urteil_spiel/spiel_reihe.ordner"
}
X5080=-18545,-18368
for K in A B C; do
    E=$SP/neutral_$K.EMD; T=$SP/neutral_$K.TIM
    # Clip 0 bind, 1 kauer, 2 sprung, 3 kurve
    lauf ${K}_kauer_g3172 "$E" "$T" $X5080,3172,1,0,1 5080
    lauf ${K}_kauer_g1100 "$E" "$T" $X5080,1100,1,0,1 5080
    lauf ${K}_kurve_g2145 "$E" "$T" $X5080,2145,3,0,1 5080
done
echo FERTIG >>"$L"
