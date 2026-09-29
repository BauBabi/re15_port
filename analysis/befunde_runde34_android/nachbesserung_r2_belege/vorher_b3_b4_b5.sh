#!/usr/bin/env bash
# Nachbesserung R2 - Nachmessung B3 (TOCTOU), B4 (fremder Schluessel), B5 (Ausrichtung) mit der ECHTEN Kette
# (release/build_android.sh --gate-only, schreibt nichts) am unveraenderten Werkzeugstand.
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
PY=/c/Python310/python
W=build/r34a/nb2
F=analysis/befunde_runde34_android/pruefer_umgehung_r2_belege/r2_faelschen.py
BT="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
JB="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin"
SIGN() { "$JB/java" -jar "$(cygpath -m $BT/lib/apksigner.jar)" "$@"; }
cd "$B"
REF=build/r34a/ref_v0.8.19.apk
[[ -f $W/apk/K0.apk ]] || $PY $F $REF $W/apk/K0.apk >/dev/null
# --- B3: Tausch gegen K0 waehrend des Gate-Selbsttests
cp -f $REF $W/apk/toctou.apk; cp -f $W/apk/K0.apk $W/apk/toctou_tausch.apk
LOG=$W/logs/vorher_b3_toctou.log
( bash release/build_android.sh --gate-only $W/apk/toctou.apk --version v0.8.19 > $LOG 2>&1; echo "EXIT=$?" >> $LOG ) &
pid=$!
for i in $(seq 1 600); do
  if grep -q "Volle Asset-Pruefung 1/2" $LOG 2>/dev/null; then mv -f $W/apk/toctou_tausch.apk $W/apk/toctou.apk; echo "B3: $(date +%T) getauscht (waehrend des Selbsttests)"; break; fi
  sleep 0.2
done
wait $pid
echo "B3 Kette: $(grep -E 'Verified using v2|ANDROID-GATES|ABBRUCH|EXIT=' $LOG | tr '\n' ' ')"
SIGN verify "$(cygpath -m $W/apk/toctou.apk)" > $W/logs/vorher_b3_verify_danach.log 2>&1; echo "B3 apksigner verify der Datei unter dem Namen danach: rc=$? $(head -1 $W/logs/vorher_b3_verify_danach.log)"
# --- B4: fremder Schluessel
rm -f $W/apk/fremd.jks
"$JB/keytool" -genkeypair -keystore "$(cygpath -m $W/apk/fremd.jks)" -storepass fremd123 -keypass fremd123 -alias fremd \
   -keyalg RSA -keysize 2048 -validity 365 -dname "CN=Fremd, O=Test" > $W/logs/vorher_b4_keytool.log 2>&1; echo "B4 keytool rc=$?"
cp -f $W/apk/K0.apk $W/apk/K0_fremd.apk
SIGN sign --ks "$(cygpath -m $W/apk/fremd.jks)" --ks-pass pass:fremd123 --ks-key-alias fremd "$(cygpath -m $W/apk/K0_fremd.apk)" > $W/logs/vorher_b4_sign.log 2>&1; echo "B4 sign rc=$?"
bash release/build_android.sh --gate-only $W/apk/K0_fremd.apk --version v0.8.19 > $W/logs/vorher_b4_kette.log 2>&1; echo "B4 Kette fremder Schluessel EXIT=$? | $(grep -E 'Signer #1 certificate SHA-256|ANDROID-GATES|ABBRUCH' $W/logs/vorher_b4_kette.log | tr '\n' ' ')"
SIGN verify --print-certs "$(cygpath -m $REF)" 2>&1 | grep -E "Signer #1 certificate SHA-256" | sed 's/^/B4 Referenz: /'
# --- B5: ohne Ausrichtung, signiert mit --alignment-preserved
$PY $F $W/apk/K0.apk $W/apk/K0_unausgerichtet.apk --ohne-lfh-extra >/dev/null
SIGN sign --ks "$(cygpath -m "$USERPROFILE/.android/debug.keystore")" --ks-pass pass:android --ks-key-alias androiddebugkey \
   --alignment-preserved true "$(cygpath -m $W/apk/K0_unausgerichtet.apk)" > $W/logs/vorher_b5_sign.log 2>&1; echo "B5 sign rc=$?"
SIGN verify "$(cygpath -m $W/apk/K0_unausgerichtet.apk)" > $W/logs/vorher_b5_verify.log 2>&1; echo "B5 verify rc=$?"
"$BT/zipalign.exe" -c -P 16 -v 4 "$(cygpath -m $W/apk/K0_unausgerichtet.apk)" > $W/logs/vorher_b5_zipalign.log 2>&1; echo "B5 zipalign -c -P 16 4 rc=$? | BAD: $(grep -c BAD $W/logs/vorher_b5_zipalign.log) | $(grep -E 'resources.arsc|libmain.so|Verification' $W/logs/vorher_b5_zipalign.log | head -4 | tr '\n' ' ')"
"$BT/zipalign.exe" -c -P 16 4 "$(cygpath -m $REF)" > $W/logs/vorher_b5_zipalign_ref.log 2>&1; echo "B5 zipalign Referenz rc=$? | $(tail -1 $W/logs/vorher_b5_zipalign_ref.log)"
bash release/build_android.sh --gate-only $W/apk/K0_unausgerichtet.apk --version v0.8.19 > $W/logs/vorher_b5_kette.log 2>&1; echo "B5 Kette unausgerichtet EXIT=$? | $(grep -E 'ANDROID-GATES|ABBRUCH' $W/logs/vorher_b5_kette.log | tr '\n' ' ')"
