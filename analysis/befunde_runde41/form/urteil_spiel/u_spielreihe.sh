#!/usr/bin/env bash
# Pruefer Spielmodell: gleiche Lage, gleiche Gier fuer A, B, C im echten Spiel, abgegebene EMD und Biegeposen. Beendet nur eigene PIDs (form_spiel.sh).
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$BAUM/analysis/befunde_runde41/form
A=$F/urteil_spiel/spiel
EICH="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/biege"
L=$F/urteil_spiel/spiel_reihe.log
mkdir -p "$A"
: > "$L"
: > "$F/urteil_spiel/spiel_reihe.ordner"
lauf() {  # name emd tim pose raum
    bash "$BAUM/re15_port/tools/form_spiel.sh" --emd "$2" --tim "$3" --pose "$4" --raum "$5" --aus "$A/$1.png" >>"$L" 2>&1
    echo "== $1 rc=$?" >>"$L"
    grep -a "Laufverzeichnis" "$L" | tail -1 | sed 's/Laufverzeichnis: //' | tr -d '\r' >>"$F/urteil_spiel/spiel_reihe.ordner"
}
X5080=-18545,-18368
X5090=776,-23713
for k in a b c; do
    K=$(echo $k | tr a-z A-Z)
    E=$F/rohbau_$k/FORM.EMD; T=$F/rohbau_$k/FORM.TIM
    # Abgabe: Pose 1, Pose 2, Ruhe, je wie im Dossier des Rohbaus; A traegt eigene Lagen
    if [ $k = a ]; then
        P1=-19141,-18630,83; P2=745,-23923,3917
    else
        P1=$X5080,83; P2=$X5090,3917
    fi
    lauf ${K}_abgabe_pose1_r5080 "$E" "$T" $P1,0,0,1 5080
    lauf ${K}_abgabe_pose2_r5090 "$E" "$T" $P2,1,0,11 5090
    lauf ${K}_abgabe_ruhe_r5080 "$E" "$T" $P1,2,0,1 5080
    # gleiche Lage, gleiche Gier
    lauf ${K}_ruhe_g2145 "$E" "$T" $X5080,2145,2,0,1 5080
    lauf ${K}_ruhe_g3172 "$E" "$T" $X5080,3172,2,0,1 5080
    lauf ${K}_ruhe_g1166_r5090 "$E" "$T" $X5090,1166,2,0,11 5090
    # Biegeposen: Clip 0 ruhe, 1 gehen, 2 hieb, 3 biss, 4 drohen, 5 seitenhieb, 6 schwanzhieb
    E=$EICH/biegepose_$K.EMD; T=$EICH/biegepose_$K.TIM
    lauf ${K}_hieb_g3172 "$E" "$T" $X5080,3172,2,0,1 5080
    lauf ${K}_biss_g3172 "$E" "$T" $X5080,3172,3,0,1 5080
    lauf ${K}_schwanzhieb_g1100 "$E" "$T" $X5080,1100,6,0,1 5080
done
echo FERTIG >>"$L"
