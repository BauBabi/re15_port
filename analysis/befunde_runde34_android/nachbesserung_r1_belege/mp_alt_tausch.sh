#!/usr/bin/env bash
# Nachbesserung R1: Befund B5 der Gegenpruefung ("nur aus dem Code") am ALTEN make_package.sh (568e9b88) GEMESSEN:
# APK waehrend der Kopierminuten gegen eine gueltig signierte Faelschung (F1_sig, gleiche Version) tauschen.
# Das alte Skript liegt dafuer kurz als release/make_package_alt.sh (HERE = release/), wird danach geloescht.
# Die PC-Binaries bekommen per touch die aktuelle Zeit (das alte check_binary_fresh zaehlt build.gradle mit).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
NB=build/r34a/nb; APK=release/re15_port_v0.8.19_android.apk; SICHER=$NB/apk_positiv_sicher.apk
LOG=$NB/mp_alt_tausch.log; ERG=$NB/mp_alt_tausch_ergebnis.txt
git show 568e9b88:release/make_package.sh > release/make_package_alt.sh
cp -p "$SICHER" "$APK"; touch "$APK" release/win_out/re15_pc.exe release/linux_out/re15_pc
G=$NB/git_iso_alt; rm -rf "$G"; mkdir -p "$G/objects"; cp "$(git rev-parse --git-path index)" "$G/index"
( export GIT_INDEX_FILE="$G/index" GIT_OBJECT_DIRECTORY="$G/objects" GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
  LOG="$LOG" bash analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh bash release/make_package_alt.sh --version v0.8.19 > /dev/null 2>&1
  echo "RC=$?" > "$NB/mp_alt_tausch.rc" ) &
pid=$!
until grep -q "Assets kopieren (shared_assets/PSX" "$LOG" 2>/dev/null || ! kill -0 "$pid" 2>/dev/null; do sleep 0.5; done
cp "$NB/apk/F1_crc_koll_sig.apk" "$APK"; echo "Eingriff $(date +%T): F1_crc_koll_sig -> $APK" > "$ERG"
wait "$pid"
echo "altes make_package.sh $(cat "$NB/mp_alt_tausch.rc")" >> "$ERG"
grep -E "APK-ASSET-GATE-OK|Zippen: .*android|Volumes: .*Katalog: 1$|Fertig|ABBRUCH" "$LOG" | sed 's/^[0-9.]* /   /' >> "$ERG"
rm -f release/make_package_alt.sh
echo "Katalog des gezippten Android-Satzes (verify_apk_im_zip aus dem NEUEN make_package, gegen die Kennung der Faelschung und der gepruefte APK):" >> "$ERG"
{
  die() { echo "ABBRUCH: $*"; }
  source release/python_finden.sh 2>/dev/null; source release/apk_pruefen.sh
  eval "$(awk '/^apk_kennung\(\) \{/{f=1} f{print} f&&/^}$/{exit}' release/make_package.sh)"
  eval "$(awk '/^verify_apk_im_zip\(\) \{/{f=1} f{print} f&&/^}$/{exit}' release/make_package.sh)"
  k_f=$(apk_kennung "$NB/apk/F1_crc_koll_sig.apk"); k_p=$(apk_kennung "$SICHER")
  echo "   Faelschung F1_sig: $k_f"; ( cd release && verify_apk_im_zip re15_port_v0.8.19_android.zip re15_port_v0.8.19_android.apk "$k_f" ); echo "   -> rc $?"
  echo "   gepruefte APK:     $k_p"; ( cd release && verify_apk_im_zip re15_port_v0.8.19_android.zip re15_port_v0.8.19_android.apk "$k_p" ); echo "   -> rc $?"
} >> "$ERG" 2>&1
cp -p "$SICHER" "$APK"; touch "$APK"
cat "$ERG"
