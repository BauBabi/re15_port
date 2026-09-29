#!/usr/bin/env bash
# (Lauf 1 am 2026-09-30: die Mutanten-Gate-Laeufe standen noch OHNE --repo -> rc 2 "build.gradle fehlt"; korrigiert
#  und nachgefahren in vorher_b1_mutanten_gegen_faelschungen.sh)
# Nachbesserung R2 - Nachmessung B1 (Teil-Mutanten) am unveraenderten Gate (HEAD e1640cd0 = Werkzeugstand 35d25455)
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
PY=/c/Python310/python
W=$B/build/r34a/nb2
BEL=$B/analysis/befunde_runde34_android/pruefer_umgehung_r2_belege
cd "$B"
mkdir -p $W/mut/r2 $W/apk
$PY $BEL/r2_mutanten.py release/apk_asset_gate.py $W/mut/r2
for m in M1_manifest_nur_zu_gross M9_laenge_nicht_gegen_cd M5_eocd_nur_muell_hinten M14_dd_bit_aus_cd M16_nicht_assets_nur_stored; do

  $PY $W/mut/r2/$m.py --selbsttest > $W/logs/vorher_selbsttest_$m.log 2>&1; rc=$?

  echo "SELBSTTEST $m rc=$rc $(grep -E '^== SELBSTTEST' $W/logs/vorher_selbsttest_$m.log)"
done
REF=$B/build/r34a/ref_v0.8.19.apk
F=$BEL/r2_faelschen.py
$PY $F $REF $W/apk/F_M1.apk --manifest-groesse shared_assets/RE15DOOR/P2DS.DO2=78243 >/dev/null
$PY $F $REF $W/apk/F_M9.apk --usize-plus classes.dex=100 >/dev/null
$PY $F $REF $W/apk/F_M5.apk --kommentar-plus 10 >/dev/null
$PY $F $REF $W/apk/F_M14.apk --cd-dd assets/shared_assets/RE15DOOR/P07G.DO2 --lfh-crc-kippen assets/shared_assets/RE15DOOR/P07G.DO2 >/dev/null
$PY $F $REF $W/apk/F_M16.apk --daten-kippen classes.dex=5000 >/dev/null
$PY $F $REF $W/apk/K0.apk >/dev/null
for paar in "M1_manifest_nur_zu_gross F_M1" "M9_laenge_nicht_gegen_cd F_M9" "M5_eocd_nur_muell_hinten F_M5" "M14_dd_bit_aus_cd F_M14" "M16_nicht_assets_nur_stored F_M16"; do
  set -- $paar
  $PY release/apk_asset_gate.py $W/apk/$2.apk > $W/logs/vorher_gate_echt_$2.log 2>&1; rce=$?
  $PY $W/mut/r2/$1.py --repo "$B" $W/apk/$2.apk > $W/logs/vorher_gate_mutant_$2.log 2>&1; rcm=$?
  echo "FAELSCHUNG $2: echtes Gate rc=$rce | Mutant $1 rc=$rcm | echt: $(grep -m1 -E 'Manifest-Groesse falsch|beschaedigt|Bytes hinter|Local Header weichen|ABBRUCH' $W/logs/vorher_gate_echt_$2.log | cut -c1-150)"
done
$PY release/apk_asset_gate.py $W/apk/K0.apk > $W/logs/vorher_gate_K0.log 2>&1; echo "KONTROLLE K0 (unsigniert, Inhalt gleich) echtes Gate rc=$?"
