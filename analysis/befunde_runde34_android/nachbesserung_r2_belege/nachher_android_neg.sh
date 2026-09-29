#!/usr/bin/env bash
# Nachbesserung R2 - echte Android-Baue, die scheitern MUESSEN:
#  A  Quellbaum ohne RE15DOOR/P2DS.DO2 (B2): Gradle baut (doFirst verlangt nur >= 1 *.DO2), das Gate bricht am
#     Tuer-Soll ab; danach keine APK unter dem Auslieferungsnamen, keine .ungeprueft, keine Pruefkopie
#  K  RE15_KEYSTORE zeigt auf eine fehlende Datei (B4): Gradle bricht ab, statt mit dem Debug-Schluessel zu signieren
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
BEL=$B/analysis/befunde_runde34_android
W=$B/build/r34a/nb2
cd "$B"
OUT=release/re15_port_v0.8.19_android.apk
[[ -f $OUT ]] && cp -p $OUT $W/apk/vor_neg.apk
mv re15_port/shared_assets/RE15DOOR/P2DS.DO2 $W/P2DS.geparkt
trap 'mv -f $W/P2DS.geparkt re15_port/shared_assets/RE15DOOR/P2DS.DO2 2>/dev/null' EXIT
LOG=$W/logs/android_neg_p2ds.log bash $BEL/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh bash release/build_android.sh --version v0.8.19 --no-toolchain > /dev/null 2>&1
mv -f $W/P2DS.geparkt re15_port/shared_assets/RE15DOOR/P2DS.DO2; trap - EXIT
echo "A  $(grep -E '^EXIT=' $W/logs/android_neg_p2ds.log) | $(grep -m1 -E 'ABBRUCH' $W/logs/android_neg_p2ds.log | cut -d' ' -f2- | cut -c1-120)"
grep -E "stageAssets: re15_port/shared_assets/RE15DOOR|BUILD SUCCESSFUL|RE15DOOR:  Quelle|Port-Tuerarchiv fehlt|Gate-Abbruch" $W/logs/android_neg_p2ds.log | cut -d' ' -f2- | cut -c1-170 | sed 's/^/     /'
echo "     danach: APK=$(ls $OUT 2>/dev/null | wc -l) ungeprueft=$(ls release/*.ungeprueft 2>/dev/null | wc -l) Pruefkopien=$(ls -d /tmp/re15_apk_pruefen.* 2>/dev/null | wc -l) | P2DS zurueck $(sha256sum re15_port/shared_assets/RE15DOOR/P2DS.DO2 | cut -c1-16)... | git status re15_port: '$(git status --short re15_port/ | tr '\n' ' ')'"
[[ -f $W/apk/vor_neg.apk ]] && cp -p $W/apk/vor_neg.apk $OUT
LOG=$W/logs/android_neg_keystore.log RE15_KEYSTORE=C:/gibt/es/nicht.jks bash $BEL/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh bash release/build_android.sh --version v0.8.19 --no-toolchain > /dev/null 2>&1
echo "K  $(grep -E '^EXIT=' $W/logs/android_neg_keystore.log) | $(grep -m1 -E 'RE15_KEYSTORE=|ABBRUCH' $W/logs/android_neg_keystore.log | cut -d' ' -f2- | cut -c1-160)"
grep -E "BUILD FAILED|existiert nicht" $W/logs/android_neg_keystore.log | cut -d' ' -f2- | cut -c1-170 | head -3 | sed 's/^/     /'
echo "     danach: APK=$(ls $OUT 2>/dev/null | wc -l) (die vorige wurde VOR Gradle entfernt)"
[[ -f $W/apk/vor_neg.apk ]] && cp -p $W/apk/vor_neg.apk $OUT
true
