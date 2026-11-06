#!/usr/bin/env bash
# alle Laeufe des Messblatts NB3, je 3 parallel
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
W="$SP/werkzeug"
L=("rampe_leon ohne" "rampe_leon neu" "rampe_elza ohne" "oben_leon neu" "oben_leon ohne" "oben_elza neu" "oben_elza ohne" "wand_elza ohne")
i=0
for l in "${L[@]}"; do
  bash "$W/batch3.sh" $l > "$SP/batch_${l// /_}.txt" 2>&1 &
  i=$((i+1))
  if [ $((i % 3)) -eq 0 ]; then wait; fi
done
wait
echo ALLE_FERTIG
