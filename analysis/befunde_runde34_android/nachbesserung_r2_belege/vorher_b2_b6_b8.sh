#!/usr/bin/env bash
# Nachbesserung R2 - Nachmessung B2 (29/30 bzw. 0-Byte-Tuerarchiv), B6 (Baum in keiner Liste), B8 (Praefix, arabische Kopfziffern)
# am unveraenderten Gate. Schattenbaum: PSX/synchro/RE2/extracted_fx als Hardlinks, RE15DOOR echte Kopie.
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
PY=/c/Python310/python
W=$B/build/r34a/nb2
S=$W/schatten/repo
F=$B/analysis/befunde_runde34_android/pruefer_umgehung_r2_belege/r2_faelschen.py
REF=$B/build/r34a/ref_v0.8.19.apk
cd "$B"
rm -rf "$W/schatten"; mkdir -p "$S/re15_port/shared_assets" "$S/re15_port/platform/android/app"
cp -p re15_port/platform/android/app/build.gradle "$S/re15_port/platform/android/app/"
for t in PSX extracted_fx RE2; do cp -al "re15_port/shared_assets/$t" "$S/re15_port/shared_assets/$t"; done
cp -a re15_port/shared_assets/RE15DOOR "$S/re15_port/shared_assets/RE15DOOR"
cp -al synchro "$S/synchro"
echo "Schatten: $(find "$S" -type f | wc -l) Dateien; Hardlink-Probe: $(stat -c %h "$S/re15_port/shared_assets/PSX/DATA/TEX.TIM") Links auf TEX.TIM"
$PY release/apk_asset_gate.py --repo "$S" "$REF" > $W/logs/vorher_schatten_ref.log 2>&1; echo "KONTROLLE Schatten vs Referenz rc=$?"
# B2a: P2DS fehlt in Quelle UND APK
mv "$S/re15_port/shared_assets/RE15DOOR/P2DS.DO2" "$W/schatten/P2DS.DO2.geparkt"
$PY $F "$REF" $W/apk/F_ohne_P2DS.apk --ohne assets/shared_assets/RE15DOOR/P2DS.DO2 --manifest-weg shared_assets/RE15DOOR/P2DS.DO2 >/dev/null
$PY release/apk_asset_gate.py --repo "$S" $W/apk/F_ohne_P2DS.apk > $W/logs/vorher_b2_ohne_P2DS.log 2>&1; echo "B2a 29/30 (Quelle+APK ohne P2DS) rc=$? | $(grep -E 'RE15DOOR:' $W/logs/vorher_b2_ohne_P2DS.log)"
mv "$W/schatten/P2DS.DO2.geparkt" "$S/re15_port/shared_assets/RE15DOOR/P2DS.DO2"
# B2b: P07G mit 0 Byte in Quelle UND APK
cp -p "$S/re15_port/shared_assets/RE15DOOR/P07G.DO2" "$W/schatten/P07G.DO2.geparkt"
: > "$S/re15_port/shared_assets/RE15DOOR/P07G.DO2"; : > $W/schatten/leer.bin
$PY $F "$REF" $W/apk/F_P07G_leer.apk --daten assets/shared_assets/RE15DOOR/P07G.DO2=$W/schatten/leer.bin --manifest-groesse shared_assets/RE15DOOR/P07G.DO2=0 >/dev/null
$PY release/apk_asset_gate.py --repo "$S" $W/apk/F_P07G_leer.apk > $W/logs/vorher_b2_P07G_leer.log 2>&1; echo "B2b P07G 0 B (Quelle+APK) rc=$? | $(grep -E 'RE15DOOR:' $W/logs/vorher_b2_P07G_leer.log)"
cp -p "$W/schatten/P07G.DO2.geparkt" "$S/re15_port/shared_assets/RE15DOOR/P07G.DO2"
# B6: neuer Baum unter shared_assets, in keiner Liste
mkdir -p "$S/re15_port/shared_assets/RE15NEU"; echo neu > "$S/re15_port/shared_assets/RE15NEU/NEU.DAT"
$PY release/apk_asset_gate.py --repo "$S" "$REF" > $W/logs/vorher_b6_neuer_baum.log 2>&1; echo "B6 neuer Baum shared_assets/RE15NEU (keine Liste) rc=$?"
rm -rf "$S/re15_port/shared_assets/RE15NEU"
# B8a: Praefix vor dem ersten Local Header (alle Offsets verschoben)
$PY $F "$REF" $W/apk/F_praefix.apk --praefix 16 >/dev/null
$PY release/apk_asset_gate.py $W/apk/F_praefix.apk > $W/logs/vorher_b8_praefix.log 2>&1; echo "B8a Praefix 16 B: Gate rc=$? | $(grep -E 'APK-ASSET-GATE|ABBRUCH' $W/logs/vorher_b8_praefix.log)"
AAPT2=$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0/aapt2.exe
"$AAPT2" dump badging "$(cygpath -m $W/apk/F_praefix.apk)" > $W/logs/vorher_b8_praefix_aapt2.log 2>&1; echo "B8a aapt2 (libziparchive) rc=$? | $(head -2 $W/logs/vorher_b8_praefix_aapt2.log | tr '\n' ' ' | cut -c1-200)"
# B8b: Kopfzeile mit arabisch-indischen Ziffern
$PY $F "$REF" $W/apk/F_kopf_arabisch.apk --manifest-kopf-arabisch >/dev/null
$PY release/apk_asset_gate.py $W/apk/F_kopf_arabisch.apk > $W/logs/vorher_b8_kopf_arabisch.log 2>&1; echo "B8b Kopf arabisch: Gate rc=$? | $(grep -E 'APK-ASSET-GATE|Kopfzeile' $W/logs/vorher_b8_kopf_arabisch.log | head -2 | tr '\n' ' ')"
