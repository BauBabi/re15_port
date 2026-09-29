#!/usr/bin/env bash
# Selbsttest des echten Gates und jedes Mutanten; Rueckgabe je Lauf selbst abgefangen (keine Pipe).
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_umgehung_r2
PY=/c/Python310/python
for g in release/apk_asset_gate.py $W/mutanten/M1_manifest_nur_zu_gross.py $W/mutanten/M9_laenge_nicht_gegen_cd.py \
         $W/mutanten/M5_eocd_nur_muell_hinten.py $W/mutanten/M14_dd_bit_aus_cd.py $W/mutanten/M16_nicht_assets_nur_stored.py; do
    n=$(basename "$g" .py)
    t0=$(date +%s)
    "$PY" "$g" --selbsttest > "$W/logs/selbsttest_$n.log" 2>&1
    rc=$?
    echo "$(date +%T) $n selbsttest rc=$rc ($(( $(date +%s) - t0 )) s) $(grep -E 'SELBSTTEST-(OK|FEHLER)' "$W/logs/selbsttest_$n.log")"
done
echo "ENDE"
