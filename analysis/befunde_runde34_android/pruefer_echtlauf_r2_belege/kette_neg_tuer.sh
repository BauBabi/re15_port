#!/usr/bin/env bash
# Pruefer echtlauf r2: der urspruengliche offene Punkt ("nur eine der 30 Tuerdateien automatisch geprueft")
# in der ECHTEN Kette: Kopie der frisch gebauten APK OHNE assets/shared_assets/RE15DOOR/P2DS.DO2 (nicht die
# fruehere Stichprobe P07G.DO2), zipalign + mit dem Debug-Schluessel neu signiert (wie ein normaler Bau:
# apksigner verify gruen), dann build_android.sh --gate-only. Muss am Asset-Gate scheitern.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
O=$BAUM/build/r34a/pruefer_echtlauf_r2/kette_neg
BT=/c/Users/mjoedicke/AppData/Local/Android/Sdk/build-tools/35.0.0
JAVA="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java"
ZIP=/c/msys64/usr/bin/zip   # absolut: /c/msys64/usr/bin NICHT in den PATH (zweite MSYS-Laufzeit, bash wuerde MSYS2-bash)
cd "$BAUM" || exit 99
rm -rf "$O"; mkdir -p "$O"
cp "$BAUM/build/r34a/pruefer_echtlauf_r2/neu_v0.8.19.apk" "$O/roh.apk"
"$ZIP" -q -d "$O/roh.apk" "assets/shared_assets/RE15DOOR/P2DS.DO2" || exit 91
unzip -Z1 "$O/roh.apk" | grep -c '^assets/shared_assets/RE15DOOR/' | sed 's/^/RE15DOOR-Eintraege nach zip -d: /'
"$BT/zipalign.exe" -f -p 4 "$(cygpath -m "$O/roh.apk")" "$(cygpath -m "$O/aligned.apk")" || exit 92
"$JAVA" -jar "$(cygpath -m "$BT/lib/apksigner.jar")" sign --ks "$(cygpath -m ~/.android/debug.keystore)" \
    --ks-pass pass:android --key-pass pass:android --ks-key-alias androiddebugkey \
    --out "$(cygpath -m "$O/neg_p2ds_signiert.apk")" "$(cygpath -m "$O/aligned.apk")" || exit 93
"$JAVA" -jar "$(cygpath -m "$BT/lib/apksigner.jar")" verify -v "$(cygpath -m "$O/neg_p2ds_signiert.apk")" | grep -E "^Verifies|v2 scheme"
rc=0
LOG=$O/kette_neg_p2ds.log bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r2_belege/lauf_mit_zeit.sh" \
    bash release/build_android.sh --gate-only "$O/neg_p2ds_signiert.apk" --version v0.8.19 || rc=$?
echo "Kette rc=$rc"
grep -v "\[ok\]" "$O/kette_neg_p2ds.log" | grep -E "Stichproben|versionName|v2 scheme|SELBSTTEST|RE15DOOR|fehlt|Manifest nennt|ABBRUCH|EXIT" | cut -c1-220
