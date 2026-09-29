#!/usr/bin/env bash
# Nachbesserung R2 - B1, Teil 2: jeder Teil-Mutant gegen seine Faelschung an der echten APK (mit --repo, sonst
# sucht die Mutantenkopie den Quellbaum neben sich). F_M1 zusaetzlich signiert: besteht apksigner?
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
PY=/c/Python310/python
W=$B/build/r34a/nb2
BT="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
JB="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin"
cd "$B"
for paar in "M1_manifest_nur_zu_gross F_M1" "M9_laenge_nicht_gegen_cd F_M9" "M5_eocd_nur_muell_hinten F_M5" "M14_dd_bit_aus_cd F_M14" "M16_nicht_assets_nur_stored F_M16"; do
  set -- $paar
  $PY $W/mut/r2/$1.py --repo "$B" $W/apk/$2.apk > $W/logs/vorher_gate_mutant_$2.log 2>&1; rcm=$?
  echo "FAELSCHUNG $2: Mutant $1 rc=$rcm ($(grep -E -m1 'APK-ASSET-GATE|ABBRUCH' $W/logs/vorher_gate_mutant_$2.log | cut -c1-90))"
done
cp -f $W/apk/F_M1.apk $W/apk/F_M1_sig.apk
"$JB/java" -jar "$(cygpath -m $BT/lib/apksigner.jar)" sign --ks "$(cygpath -m "$USERPROFILE/.android/debug.keystore")" --ks-pass pass:android \
   --ks-key-alias androiddebugkey "$(cygpath -m $W/apk/F_M1_sig.apk)" > $W/logs/vorher_b1_sign_F_M1.log 2>&1; echo "F_M1 signiert rc=$?"
"$JB/java" -jar "$(cygpath -m $BT/lib/apksigner.jar)" verify --print-certs "$(cygpath -m $W/apk/F_M1_sig.apk)" 2>&1 | grep -E "Signer #1 certificate SHA-256" | sed 's/^/F_M1_sig apksigner: /'
$PY $W/mut/r2/M1_manifest_nur_zu_gross.py --repo "$B" $W/apk/F_M1_sig.apk > $W/logs/vorher_gate_mutant_F_M1_sig.log 2>&1; echo "F_M1_sig: Mutant M1 rc=$?"
$PY release/apk_asset_gate.py $W/apk/F_M1_sig.apk > $W/logs/vorher_gate_echt_F_M1_sig.log 2>&1; echo "F_M1_sig: echtes Gate rc=$? ($(grep -m1 'Manifest-Groesse falsch' $W/logs/vorher_gate_echt_F_M1_sig.log | cut -c1-100))"
