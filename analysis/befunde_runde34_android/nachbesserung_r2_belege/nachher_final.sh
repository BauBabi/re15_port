#!/usr/bin/env bash
# Nachbesserung R2 - Endlauf mit dem ENDSTAND der Werkzeuge (Gate v5): Kette --gate-only (Kontrollen),
# Android-Baue die scheitern muessen, voller Android-Bau positiv, make_package.sh positiv (echt, Wegwerf-Index).
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
BEL=$B/analysis/befunde_runde34_android/nachbesserung_r2_belege
L=$B/build/r34a/nb2/logs
cd "$B"
echo "=== 1 Kette --gate-only ($(date +%T))"
bash $BEL/nachher_kette.sh > $L/final_kette.out 2>&1; cat $L/final_kette.out
echo "=== 2 Android-Baue, die scheitern muessen ($(date +%T))"
bash $BEL/nachher_android_neg.sh 2>&1
echo "=== 3 voller Android-Bau positiv ($(date +%T))"
LOG=$L/final_android_voll.log bash $BEL/../pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh bash release/build_android.sh --version v0.8.19 --no-toolchain > /dev/null 2>&1
echo "    $(grep -E '^EXIT=' $L/final_android_voll.log) | $(grep -E 'ANDROID-BUILD-OK|ABBRUCH' $L/final_android_voll.log | cut -d' ' -f2- | cut -c1-120)"
grep -E "BUILD SUCCESSFUL|SELBSTTEST-OK|Innere Proben|Tuer-Soll|APK-ASSET-GATE-OK|zipalign -c|Signer #1" $L/final_android_voll.log | cut -d' ' -f2- | cut -c1-150 | sed 's/^/    /'
echo "=== 4 make_package.sh positiv ($(date +%T))"
LOG=$L/final_mp_positiv.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 > /dev/null 2>&1
echo "    $(grep -E '^EXIT=' $L/final_mp_positiv.log) | $(grep -E '== Fertig ==|ABBRUCH' $L/final_mp_positiv.log | cut -d' ' -f2- | cut -c1-120)"
grep -E "QUELLBAUM-OK|APK-PRUEFUNG-OK|PAKET-OK|APK im Split-Satz|Innere Proben|SELBSTTEST-OK" $L/final_mp_positiv.log | cut -d' ' -f2- | cut -c1-150 | sed 's/^/    /'
echo "=== 5 release/ zuruecksetzen ($(date +%T))"
git restore --source=HEAD -- release/SHA256SUMS.txt release/SHA256SUMS_android.txt release/re15_port_v0.8.19_android.z01 \
    release/re15_port_v0.8.19_android.zip release/re15_port_v0.8.19_linux_steamdeck_x64.z01 release/re15_port_v0.8.19_linux_steamdeck_x64.zip \
    release/re15_port_v0.8.19_win64.z01 release/re15_port_v0.8.19_win64.zip
echo "    git status release/ re15_port/: '$(git status --short release/ re15_port/ | tr '\n' ' ')'"
echo "=== ENDE ($(date +%T))"
