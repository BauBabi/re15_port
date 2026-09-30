#!/usr/bin/env bash
# Pruefer echtlauf R4-2: make_package.sh ECHT, Folge E0a E0b E1 E2 E3 (Binaries v0.8.19 aus dem Archiv, APK N aus Abschnitt 1).
set -u
T=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=$T/build/r34a/pruefer_e2
B=$T/analysis/befunde_runde34_android/pruefer_echtlauf_r4_2_belege
R=$T/release
APK=$R/re15_port_v0.8.19_android.apk
CD="$(cd "$T" && git rev-parse --path-format=absolute --git-common-dir)"
mkdir -p "$W/apk/geparkt"
zustand() {   # $1 = tag
    echo "--- Zustand nach $1 ($(date '+%T')):"
    ( cd "$R" && for f in re15_port_v0.8.19_*.z* re15_port_v0.8.19_*.apk; do [[ -e "$f" ]] && echo "   $(stat -c '%y %s' "$f" | cut -c12-19,36-) $(sha256sum "$f" | cut -c1-16).. $f"; done
      echo "   SHA256SUMS.txt:"; sed 's/^/      /' SHA256SUMS.txt )
    echo "   Wegwerf-Index gegen HEAD (release/):"
    GIT_INDEX_FILE="$W/git/index_$1" GIT_OBJECT_DIRECTORY="$W/git/objects_$1" GIT_ALTERNATE_OBJECT_DIRECTORIES="$CD/objects" \
        git -C "$T" diff --cached --no-renames --name-status HEAD -- release/ | sed 's/^/      /'
    echo "   Schluesselzeilen:"
    grep -nE 'ABBRUCH|EXIT=|SHA256SUMS.txt geschrieben|vorgemerkt|Android-Satz|ohne-android|Gate-Urteil|APK-PRUEFUNG-OK|PAKET|Laufzeit-Gate|Satz re15|APK-Satz|APK im Split|== Zippen|kopieren \(|fremd|Frische|VERALTET|Manifest im alten' "$W/logs/mp/$1.log" | cut -c1-240 | sed 's/^/      /'
}
laeuft_fremd() {
    powershell.exe -NoProfile -Command "Get-CimInstance Win32_Process | Where-Object { \$_.CommandLine -match 'make_package' -and \$_.CommandLine -notmatch 'pe2_mp|Get-CimInstance' } | ForEach-Object { \$_.ProcessId }" | tr -d '\r'
}
{
echo "== pe2_mp_folge Start $(date '+%F %T'), HEAD $(git -C "$T" rev-parse --short HEAD)"
echo "-- versionierte release/-Dateien vorher (sha256):"
( cd "$T" && git ls-files release/ | xargs sha256sum ) > "$W/logs/mp_release_vorher.txt"; wc -l < "$W/logs/mp_release_vorher.txt"
echo "-- Binaries (cp -p aus build/r34a/pruefer_e2/bin, Archiv-mtime):"
mkdir -p "$R/linux_out" "$R/win_out"
cp -p "$W/bin/linux/re15_pc" "$R/linux_out/re15_pc"; cp -p "$W/bin/win/re15_pc.exe" "$R/win_out/re15_pc.exe"
ls -la --time-style=full-iso "$R/linux_out/re15_pc" "$R/win_out/re15_pc.exe" | sed 's/^/   /'
sha256sum "$R/linux_out/re15_pc" "$R/win_out/re15_pc.exe" | sed 's/^/   /'
echo "-- fremde make_package-Prozesse: [$(laeuft_fremd)]"

echo; echo "===== E0a: keine APK, versionierter Android-Satz v0.8.19 (HEAD) liegt da ====="
mv "$APK" "$W/apk/geparkt/"
bash "$B/pe2_mp.sh" E0a --version v0.8.19; zustand E0a

echo; echo "===== E0b: alte v0.8.19-APK (Liste v1) als frische Kopie unter dem Auslieferungsnamen ====="
cp "$W/apk/REF_v0.8.19.apk" "$APK"; echo "   $(sha256sum "$APK" | cut -c1-64) mtime $(stat -c %y "$APK")"
echo "-- fremde make_package-Prozesse: [$(laeuft_fremd)]"
bash "$B/pe2_mp.sh" E0b --version v0.8.19; zustand E0b
rm -f "$APK"

echo; echo "===== E1: APK N, beide Plattformen ====="
mv "$W/apk/geparkt/re15_port_v0.8.19_android.apk" "$APK"; echo "   APK $(sha256sum "$APK" | cut -c1-64)"
echo "-- fremde make_package-Prozesse: [$(laeuft_fremd)]"
bash "$B/pe2_mp.sh" E1 --version v0.8.19; zustand E1
echo "-- unabhaengig: sha256sum -c SHA256SUMS.txt"
( cd "$R" && sha256sum -c SHA256SUMS.txt 2>&1 | sed 's/^/   /' )
echo "-- unabhaengig: APK aus dem neuen Android-Satz (zip -s 0 + unzip)"
rm -rf "$W/satz"; mkdir -p "$W/satz"
( export PATH="/c/msys64/usr/bin:$PATH"; zip -q -s 0 "$(cygpath -m "$R/re15_port_v0.8.19_android.zip")" --out "$(cygpath -m "$W/satz/ganz.zip")" && unzip -Z1 "$(cygpath -m "$W/satz/ganz.zip")" | sed 's/^/   Eintrag: /' && echo "   sha256 aus dem Satz: $(unzip -p "$(cygpath -m "$W/satz/ganz.zip")" | sha256sum | cut -c1-64)" )
echo "   sha256 APK N:        $(sha256sum "$W/apk/N.apk" | cut -c1-64)"
rm -rf "$W/satz"

echo; echo "===== E2: nur PC (--only linux), APK weg, Android-Satz aus E1 liegt da ====="
mv "$APK" "$W/apk/geparkt/"
echo "-- fremde make_package-Prozesse: [$(laeuft_fremd)]"
bash "$B/pe2_mp.sh" E2 --version v0.8.19 --only linux; zustand E2

echo; echo "===== E3: dasselbe + --ohne-android ====="
echo "-- fremde make_package-Prozesse: [$(laeuft_fremd)]"
bash "$B/pe2_mp.sh" E3 --version v0.8.19 --only linux --ohne-android; zustand E3
echo "-- unabhaengig: sha256sum -c SHA256SUMS.txt"
( cd "$R" && sha256sum -c SHA256SUMS.txt 2>&1 | sed 's/^/   /' )
echo "== pe2_mp_folge Ende $(date '+%F %T')"
} > "$W/logs/mp_folge.txt" 2>&1
echo "FOLGE-ENDE"
