#!/usr/bin/env bash
# Runde 4 (Kette): make_package.sh ECHT im Arbeitsbaum (Vorbild pruefer_echtlauf_r3.md 3a): PC-Binaries v0.8.19 aus
# dem Archiv mit Original-mtime (r4_binaries.sh, KEIN touch), APK = frischer Bau dieses Standes (build_android.sh,
# android_voll_r4_auszug.txt). git-Schreibzugriffe nur in einen Wegwerf-Index (mp_isoliert_r4.sh).
#   E1  --version v0.8.19 (beide Plattformen)                       -> soll EXIT 0, Android-Satz aus der geprueften APK
#   --  APK weg (nach build/r34a/r4/apk_echt/)
#   E2  --version v0.8.19 --only win --zip-only (Satz aus E1 liegt)  -> soll ABBRUCH (B1) vor den Kopierminuten
#   E3  dasselbe mit --ohne-android                                 -> soll EXIT 0, Satz weg, SUMS ohne Android,
#                                                                      im Wegwerf-Index als geloescht vorgemerkt
# Danach: release/ auf HEAD (nur versionierte Paket-/SUMS-Dateien), Laufreste weg, echter Index leer.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; L=$W/logs/echt; mkdir -p "$L" "$W/apk_echt"
N=re15_port_v0.8.19
[[ -f release/${N}_android.apk ]] || { echo "APK fehlt (build_android.sh zuerst)"; exit 1; }
for d in release/win_out release/linux_out release/pkg-win release/pkg-linux; do
    [[ ! -e "$d" ]] || { echo "$d existiert schon - nicht ueberschreiben"; exit 1; }
done
git -C . ls-files release/ | while read -r f; do sha256sum "$f"; done > "$L/release_vorher.sha256"
echo "echter Index vorher: $(git diff --cached --name-only | wc -l) Eintraege"
mkdir -p release/win_out release/linux_out
cp -p $W/bin/win/re15_pc.exe release/win_out/re15_pc.exe
cp -p $W/bin/linux/re15_pc release/linux_out/re15_pc
ls -la --time-style=full-iso release/win_out/re15_pc.exe release/linux_out/re15_pc release/${N}_android.apk | sed 's/^/   /'
echo "letzter Commit an den PC-Pfaden:  $(git log -1 --format='%h %ci' -- re15_port/engine re15_port/include re15_port/platform ':(exclude)re15_port/platform/android')"
echo "letzter Commit an den APK-Pfaden: $(git log -1 --format='%h %ci' -- re15_port/engine re15_port/include re15_port/platform/pc re15_port/platform/android)"
kern() {    # $1 = Log
    grep -a -E ' Python:|Asset-Gate: Selbsttest|Gate: |SELBSTTEST-(OK|FEHLER)|QUELLBAUM-OK|APK-PRUEFUNG-OK|gepruefte APK|VERALTET|Optimierungs-Gate|glibc|Tuerarchive im Paket|PAKET-OK|Laufzeit-Gate|Zeilenenden|x-Bit ok|Volumes:|Zippen: |APK-Satz|APK im Split-Satz|SHA256SUMS.txt geschrieben|ohne-android|kein Android|^[0-9.]+ ABBRUCH|Android-Satz|vorgemerkt|== Fertig|EXIT=' "$1" \
        | grep -v '\[ok\]' | cut -d' ' -f2- | tr -d '\r' | cut -c1-200 | sed 's/^/   /'
}
lauf() {    # $1 = Tag, Rest = make_package-Argumente
    local tag="$1" rc=0; shift
    LOG="$L/$tag.log" bash $B/mp_isoliert_r4.sh --version v0.8.19 "$@" > /dev/null 2>&1 || rc=$?
    echo "== $tag ($*): EXIT=$rc"
    kern "$L/$tag.log"
    echo "   Wegwerf-Index gegen HEAD: $(GIT_INDEX_FILE=$W/git_iso/index git diff --cached --name-status | tr '\n\t' '  ')"
    echo "   echter Index: $(git diff --cached --name-only | wc -l) Eintraege"
    echo "   SHA256SUMS.txt: $(tr -d '\r' < release/SHA256SUMS.txt | awk '{print $2}' | tr '\n' ' ')"
}
lauf E1_voll
sha256sum release/${N}_*.z* > "$L/volumes_E1.sha256"
echo "--- unabhaengig: APK aus dem neuen Android-Satz (zip -s 0 + unzip -p) gegen die gebaute APK"
( cd release && /c/msys64/usr/bin/zip -q -s 0 ${N}_android.zip --out "$(cygpath -m "$OLDPWD/$W/apk_echt")/ganz.zip" )
echo "   im Satz: $(unzip -p $W/apk_echt/ganz.zip | sha256sum | cut -c1-64)"
echo "   gebaut:  $(sha256sum release/${N}_android.apk | cut -c1-64)"
rm -f $W/apk_echt/ganz.zip
mv release/${N}_android.apk $W/apk_echt/
lauf E2_ohne_apk_satz_da --only win --zip-only
lauf E3_ohne_android --only win --zip-only --ohne-android
ls release/${N}_android.* 2>/dev/null | sed 's/^/   noch da: /' || true
echo "--- Aufraeumen release/"
git restore --source=HEAD --worktree -- release/SHA256SUMS.txt release/SHA256SUMS_android.txt \
    release/${N}_android.z01 release/${N}_android.zip release/${N}_linux_steamdeck_x64.z01 \
    release/${N}_linux_steamdeck_x64.zip release/${N}_win64.z01 release/${N}_win64.zip
rm -rf release/pkg-win release/pkg-linux release/win_out release/linux_out release/__pycache__
git -C . ls-files release/ | while read -r f; do sha256sum "$f"; done > "$L/release_nachher.sha256"
cmp -s "$L/release_vorher.sha256" "$L/release_nachher.sha256" && echo "   release/: alle versionierten Dateien = Stand vor den Laeufen" || echo "   release/: ABWEICHUNG"
echo "   git status --short release/ re15_port/: [$(git status --short release/ re15_port/ | tr '\n' ' ')]"
echo "   git status --short --ignored release/: [$(git status --short --ignored release/ | tr '\n' ' ')]"
echo "   echter Index: $(git diff --cached --name-only | wc -l) Eintraege"
echo FERTIG
