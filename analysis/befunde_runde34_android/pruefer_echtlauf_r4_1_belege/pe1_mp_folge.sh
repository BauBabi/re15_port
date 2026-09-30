#!/usr/bin/env bash
# Pruefer echtlauf R4-1: make_package.sh echt - Folge E0a/E0b/E1/E2/E3 (Beschreibung im Dossier, Abschnitt 2).
# PC-Binaries v0.8.19 aus dem Archiv (pe1_binaries.sh, Original-mtime, KEIN touch), APK N aus Abschnitt 1.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
B=analysis/befunde_runde34_android/pruefer_echtlauf_r4_1_belege
W=build/r34a/pruefer_e1; L=$W/logs/mp; mkdir -p "$L"
N=re15_port_v0.8.19
G=$W/git_iso
isogit() { GIT_INDEX_FILE="$G/index" GIT_OBJECT_DIRECTORY="$G/objects" GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects" git "$@"; }
kern() {
    grep -a -E 'Selbsttest|SELBSTTEST-(OK|FEHLER)|QUELLBAUM-OK|APK-PRUEFUNG-OK|gepruefte APK|VERALTET|alten Format|APK-ASSET-GATE|Optimierungs-Gate|glibc|PAKET-OK|Laufzeit-Gate|Volumes:|Zippen: |APK im Split-Satz|SHA256SUMS.txt geschrieben|ohne-android|kein Android|ABBRUCH|Android-Satz|vorgemerkt|Assets kopieren \(|== Fertig|^START|^ENDE|EXIT=' "$1" \
        | grep -a -v '\[ok\]' | tr -d '\r' | cut -c1-230 | sed 's/^/   /'
}
zustand() {
    echo "   Wegwerf-Index gegen HEAD: [$(isogit diff --cached --name-status | tr '\n\t' '  ')]"
    echo "   echter Index: $(git diff --cached --name-only | wc -l) Eintraege"
    echo "   SHA256SUMS.txt: [$(tr -d '\r' < release/SHA256SUMS.txt | awk '{print $2}' | tr '\n' ' ')]"
    echo "   Android-Volumes: [$(ls release/${N}_android.z* 2>/dev/null | xargs -r sha256sum | cut -c1-16 | tr '\n' ' ')]"
    echo "   'Assets kopieren' im Log: $(grep -a -c 'Assets kopieren' "$1")"
}
lauf() {
    local tag="$1"; shift
    local andere
    andere=$(powershell.exe -NoProfile -Command "(Get-CimInstance Win32_Process | Where-Object { \$_.CommandLine -match 'make_package' -and \$_.CommandLine -notmatch 'pe1_mp' -and \$_.CommandLine -notmatch 'pruefer_u1' }).Count" | tr -d '\r')
    echo "== $tag: make_package.sh $* (andere make_package-Prozesse vorher: ${andere:-0})"
    bash $B/pe1_mp.sh "$tag" "$@" > /dev/null
    kern "$L/$tag.log"
    zustand "$L/$tag.log"
}
for d in release/win_out release/linux_out release/pkg-win release/pkg-linux; do
    [[ ! -e "$d" ]] || { echo "$d existiert schon - nicht ueberschreiben"; exit 1; }
done
[[ -f release/${N}_android.apk ]] || { echo "APK N fehlt unter release/"; exit 1; }
git ls-files release/ | while read -r f; do sha256sum "$f"; done > "$L/release_vorher.sha256"
echo "echter Index vorher: $(git diff --cached --name-only | wc -l) Eintraege"
echo "letzter Commit PC-Pfade:  $(git log -1 --format='%h %ci' -- re15_port/engine re15_port/include re15_port/platform ':(exclude)re15_port/platform/android')"
echo "letzter Commit APK-Pfade: $(git log -1 --format='%h %ci' -- re15_port/engine re15_port/include re15_port/platform/pc re15_port/platform/android)"
mkdir -p release/win_out release/linux_out
cp -p $W/bin/win/re15_pc.exe release/win_out/re15_pc.exe
cp -p $W/bin/linux/re15_pc release/linux_out/re15_pc
mv release/${N}_android.apk $W/apk/N_geparkt.apk
ls -la --time-style=full-iso release/win_out/re15_pc.exe release/linux_out/re15_pc $W/apk/N_geparkt.apk | sed 's/^/   /'
echo "--- E0a: keine APK, der VERSIONIERTE Android-Satz v0.8.19 (HEAD, enthaelt die v1-APK) liegt da"
lauf E0a_ohne_apk_alter_satz --version v0.8.19
echo "--- E0b: die ALTE v0.8.19-APK (Liste v1) als frische Kopie unter dem Auslieferungsnamen"
cp $W/ref_v0.8.19.apk release/${N}_android.apk
lauf E0b_alte_apk_v1 --version v0.8.19
echo "   APK nach dem Lauf: $(ls -la release/${N}_android.apk 2>&1 | cut -c1-120)"
rm -f release/${N}_android.apk
echo "--- E1: APK N"
mv $W/apk/N_geparkt.apk release/${N}_android.apk
lauf E1_voll --version v0.8.19
( cd release && tr -d '\r' < SHA256SUMS.txt | sha256sum -c - 2>&1 | sed 's/^/   sums -c: /' )
echo "   unabhaengig: APK aus dem neuen Android-Satz (zip -s 0 + unzip -p) gegen die gebaute APK"
rm -f $W/ganz.zip
( cd release && /c/msys64/usr/bin/zip -q -s 0 ${N}_android.zip --out "$(cygpath -m "$BAUM/$W")/ganz.zip" )
echo "   im Satz: $(/c/msys64/usr/bin/unzip -Z1 $W/ganz.zip | tr '\n' ' ') sha256 $(/c/msys64/usr/bin/unzip -p $W/ganz.zip | sha256sum | cut -c1-64)"
echo "   gebaut:  $(sha256sum release/${N}_android.apk | cut -c1-64)"
rm -f $W/ganz.zip
sha256sum release/${N}_*.z* > "$L/volumes_E1.sha256"
mv release/${N}_android.apk $W/apk/N_geparkt.apk
echo "--- E2: APK weg, Satz aus E1 liegt, nur PC (--only win --zip-only)"
lauf E2_ohne_apk_satz_E1 --version v0.8.19 --only win --zip-only
echo "--- E3: dasselbe + --ohne-android"
lauf E3_ohne_android --version v0.8.19 --only win --zip-only --ohne-android
echo "   noch da: [$(ls release/${N}_android.* 2>/dev/null | tr '\n' ' ')]"
echo "--- Aufraeumen release/"
git restore --source=HEAD --worktree -- release/SHA256SUMS.txt release/SHA256SUMS_android.txt \
    release/${N}_android.z01 release/${N}_android.zip release/${N}_linux_steamdeck_x64.z01 \
    release/${N}_linux_steamdeck_x64.zip release/${N}_win64.z01 release/${N}_win64.zip
rm -rf release/pkg-win release/pkg-linux release/win_out release/linux_out
git ls-files release/ | while read -r f; do sha256sum "$f"; done > "$L/release_nachher.sha256"
cmp -s "$L/release_vorher.sha256" "$L/release_nachher.sha256" && echo "   release/: alle versionierten Dateien = Stand vor den Laeufen" || echo "   release/: ABWEICHUNG"
echo "   git status --short release/ re15_port/: [$(git status --short release/ re15_port/ | tr '\n' ' ')]"
echo "   echter Index: $(git diff --cached --name-only | wc -l) Eintraege"
echo "   APK N geparkt: $W/apk/N_geparkt.apk ($(sha256sum $W/apk/N_geparkt.apk | cut -c1-16)...)"
echo FERTIG
