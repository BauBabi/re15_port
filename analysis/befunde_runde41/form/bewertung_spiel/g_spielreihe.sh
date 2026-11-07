#!/usr/bin/env bash
# Pruefer Spielmodell: gleiche Lage, gleiche Gier, gleicher Zeichenweg fuer A, B, C im echten Spiel. Beendet nur eigene PIDs (form_spiel.sh).
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
F=$BAUM/analysis/befunde_runde41/form
A=$F/bewertung_spiel
L=$A/spiel_reihe.log
: > "$L"
lauf() {  # name emd tim pose raum
    bash "$BAUM/re15_port/tools/form_spiel.sh" --emd "$2" --tim "$3" --pose "$4" --raum "$5" --aus "$A/$1.png" >>"$L" 2>&1
    echo "== $1 rc=$?" >>"$L"
    grep -a "Laufverzeichnis" "$L" | tail -1 | sed 's/Laufverzeichnis: //' | tr -d '\r' >>"$A/spiel_reihe.ordner"
}
: > "$A/spiel_reihe.ordner"
for k in a b c; do
    K=$(echo $k | tr a-z A-Z)
    E=$F/kandidat_$k/FORM.EMD; T=$F/kandidat_$k/FORM.TIM
    lauf spiel_${K}_r5080_ruhe_g2145 "$E" "$T" -18514,-18726,2145,2,0,1 5080
    lauf spiel_${K}_r5080_ruhe_g3172 "$E" "$T" -18514,-18726,3172,2,0,1 5080
    lauf spiel_${K}_r5090_ruhe_g1166 "$E" "$T" 2625,-23625,1166,2,0,11 5090
    E=$A/zw/biege_$K.EMD; T=$A/zw/biege_$K.TIM
    lauf spiel_${K}_r5080_biss_g3172 "$E" "$T" -18514,-18726,3172,2,0,1 5080
    lauf spiel_${K}_r5080_seitenhieb_g3172 "$E" "$T" -18514,-18726,3172,4,0,1 5080
done
echo FERTIG >>"$L"
