#!/usr/bin/env bash
# TOCTOU in apk_pruefen: Signatur/Version werden an Datei A geprueft, Assets an Datei B.
# Die gueltige APK wird WAEHREND des Gate-Selbsttests (Schritt 4.1) gegen K0 getauscht
# (unsigniert, Assets identisch). Echte release/build_android.sh --gate-only (schreibt nichts).
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_umgehung_r2
BT="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
J="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java"
cp -f $W/ref.apk $W/apk/toctou.apk
cp -f $W/apk/K0.apk $W/apk/toctou_tausch.apk
LOG=$W/logs/kette_toctou.log
( bash release/build_android.sh --gate-only $W/apk/toctou.apk --version v0.8.19 > $LOG 2>&1; echo "EXIT=$?" >> $LOG ) &
pid=$!
for i in $(seq 1 600); do
    if grep -q "Volle Asset-Pruefung 1/2" $LOG 2>/dev/null; then
        mv -f $W/apk/toctou_tausch.apk $W/apk/toctou.apk
        echo "$(date +%T.%N | cut -c1-12) getauscht (nach apksigner, waehrend des Selbsttests)"
        break
    fi
    sleep 0.2
done
wait $pid
echo "--- Kette:"
grep -E "Verified using v2|Signer #1|Volle Asset|SELBSTTEST|GATE-OK|ANDROID-GATES|ABBRUCH|EXIT=" $LOG
echo "--- apksigner verify auf die Datei, die jetzt unter dem Namen liegt:"
"$J" -jar "$(cygpath -m $BT/lib/apksigner.jar)" verify "$(cygpath -m $W/apk/toctou.apk)" 2>&1 | head -3
echo "verify rc=${PIPESTATUS[0]}"
sha256sum $W/apk/toctou.apk $W/apk/K0.apk $W/ref.apk
