#!/usr/bin/env bash
# =============================================================================
# release/apk_pruefen.sh — EINE Pruefkette fuer die Android-APK (Runde 34a, Nachbesserung R1)
# =============================================================================
# Per `source` aus release/build_android.sh (nach dem Bau und --gate-only) UND aus
# release/make_package.sh (vor dem Zippen): beide pruefen damit genau dasselbe.
# Vorher prueften die beiden Skripte Verschiedenes, und make_package.sh zippte eine APK, deren
# versionName, Signatur und Codestand niemand angesehen hatte (Gegenpruefung R1, B2/B4/B5).
#
# BRAUCHT vom Aufrufer: die() (bricht ab), PY (release/python_finden.sh), set -euo pipefail.
#
# apk_werkzeuge_finden
#     setzt APK_SDK, APK_AAPT, APK_SIGNER_JAR, APK_JAVA - oder bricht ab. KEIN stilles
#     Ueberspringen mehr: bis zur Nachbesserung lief `--gate-only <apk> --version v9.9.9` ohne
#     aapt mit Rueckgabe 0 durch (Versionsname, Paketname und ABIs wurden gar nicht geprueft).
#     SDK:  ANDROID_SDK_ROOT / ANDROID_HOME, sonst %LOCALAPPDATA%/Android/Sdk bzw. ~/Android/Sdk
#     build-tools: APK_BUILD_TOOLS (Standard 35.0.0 = BUILD_TOOLS_PKG in build_android.sh)
#     Java: JAVA_HOME, sonst das Adoptium-JDK 17 unter C:/Program Files, sonst java im PATH
#
# apk_pruefen <apk> <version> <repo>   - jeder Befund bricht ab (die)
#   1. Stichproben (unzip -Z1): native libs beider ABIs, Manifest, je ein Asset je Baum
#   2. aapt dump badging: package 'de.re15.port', versionName = <version>, arm64-v8a + x86_64
#   3. apksigner verify: Signatur gueltig UND per v2 oder v3 (targetSdk 35 - ohne v2+ ist die APK
#      auf Android 11+ nicht installierbar). Bis zur Nachbesserung gingen unsignierte und nach dem
#      Signieren veraenderte APKs durch jede Pruefung (Gegenpruefung R1, B2).
#   4. release/apk_asset_gate.py --selbsttest, dann --repo <repo> <apk> (JEDE Datei der
#      Asset-Baeume, Manifest, ZIP-Struktur so, wie Android sie liest)
# =============================================================================

apk_nativ() {           # Pfad fuer Windows-Programme (aapt.exe, java.exe, python.exe) als C:/...
    if command -v cygpath >/dev/null 2>&1; then cygpath -m "$1"; else printf '%s\n' "$1"; fi
}

apk_werkzeuge_finden() {
    local sdk bt kand j
    sdk="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-}}"
    if [[ -z "$sdk" ]]; then
        if command -v cygpath >/dev/null 2>&1 && [[ -n "${LOCALAPPDATA:-}" ]]; then
            sdk="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk"
        else
            sdk="$HOME/Android/Sdk"
        fi
    elif command -v cygpath >/dev/null 2>&1; then
        sdk="$(cygpath -u "$sdk")"
    fi
    [[ -d "$sdk" ]] || die "APK-Pruefung: Android-SDK-Ordner fehlt: $sdk (ANDROID_SDK_ROOT setzen) - ohne aapt und
        apksigner gibt es keine Pruefung von Version, Paketname, ABIs und Signatur. Nur die Assets:
        \"\$PY\" release/apk_asset_gate.py --repo . <apk>"
    APK_SDK="$sdk"

    bt="$sdk/build-tools/${APK_BUILD_TOOLS:-35.0.0}"
    if [[ ! -d "$bt" ]]; then
        # Rueckfall: die neueste vorhandene build-tools-Version mit aapt UND apksigner.jar
        bt=""
        while IFS= read -r kand; do
            if [[ ( -f "$kand/aapt" || -f "$kand/aapt.exe" ) && -f "$kand/lib/apksigner.jar" ]]; then bt="$kand"; fi
        done < <(ls -d "$sdk"/build-tools/*/ 2>/dev/null | sed 's#/$##' | sort -V)
        [[ -n "$bt" ]] || die "APK-Pruefung: keine build-tools mit aapt + lib/apksigner.jar unter $sdk/build-tools"
        echo "   (build-tools ${APK_BUILD_TOOLS:-35.0.0} fehlt - nehme $(basename "$bt"))"
    fi
    if [[ -f "$bt/aapt.exe" ]]; then APK_AAPT="$bt/aapt.exe"; else APK_AAPT="$bt/aapt"; fi
    [[ -f "$APK_AAPT" ]] || die "APK-Pruefung: aapt fehlt in $bt"
    APK_SIGNER_JAR="$bt/lib/apksigner.jar"
    [[ -f "$APK_SIGNER_JAR" ]] || die "APK-Pruefung: apksigner.jar fehlt: $APK_SIGNER_JAR"

    APK_JAVA=""
    if [[ -n "${JAVA_HOME:-}" ]]; then
        j="$JAVA_HOME"
        command -v cygpath >/dev/null 2>&1 && j="$(cygpath -u "$j")"
        j="${j%/}/bin/java"
        [[ -f "$j" || -f "$j.exe" ]] && APK_JAVA="$j"
    fi
    if [[ -z "$APK_JAVA" && -f "/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java.exe" ]]; then
        APK_JAVA="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java"
    fi
    if [[ -z "$APK_JAVA" ]] && command -v java >/dev/null 2>&1; then
        APK_JAVA="$(command -v java)"
    fi
    [[ -n "$APK_JAVA" ]] || die "APK-Pruefung: kein Java fuer apksigner (JAVA_HOME setzen)"
    echo "   APK-Werkzeuge: aapt + apksigner aus $(basename "$bt"), Java $APK_JAVA"
}

apk_pruefen() {
    local apk="$1" version="$2" repo="$3"
    local list f n_assets badging signer rc gate
    [[ -n "$version" ]] || die "apk_pruefen: Version fehlt (versionName wird immer geprueft)"
    [[ -n "${APK_AAPT:-}" && -n "${APK_SIGNER_JAR:-}" && -n "${APK_JAVA:-}" ]] \
        || die "apk_pruefen: erst apk_werkzeuge_finden aufrufen"
    [[ -n "${PY:-}" ]] || die "apk_pruefen: kein Python (release/python_finden.sh)"
    echo "== APK-Pruefung: $apk (Version $version) =="

    # 1. Stichproben (die volle Asset-Pruefung folgt in 4.)
    list="$(unzip -Z1 "$apk")" || die "APK nicht lesbar (unzip -Z1): $apk"
    for f in lib/arm64-v8a/libmain.so lib/arm64-v8a/libSDL2.so lib/x86_64/libmain.so lib/x86_64/libSDL2.so \
             assets/re15_assets.txt assets/shared_assets/PSX/DATA/TEX.TIM assets/shared_assets/PSX/STAGE1/ROOM1240.RDT \
             assets/shared_assets/extracted_fx/effect0_blood.tim assets/shared_assets/RE2/CDEMD0.EMS \
             assets/synchro/STAGE1/room1170/main00.wav; do
        grep -qxF "$f" <<<"$list" || die "APK unvollstaendig: $f fehlt"
    done
    n_assets="$(grep -c '^assets/' <<<"$list" || true)"
    echo "   Stichproben ok; unzip zaehlt $n_assets Asset-Eintraege (Pruefung jedes Eintrags: Schritt 4)"

    # 2. aapt badging - Paketname, Version, ABIs (fehlt aapt: apk_werkzeuge_finden ist schon abgebrochen)
    rc=0; badging="$("$APK_AAPT" dump badging "$(apk_nativ "$apk")" 2>&1)" || rc=$?
    grep -E "^package:|^native-code:|^sdkVersion|^targetSdkVersion" <<<"$badging" | sed 's/^/   /' || true
    (( rc == 0 )) || die "aapt dump badging fehlgeschlagen (rc=$rc): $(head -3 <<<"$badging")"
    grep -q "^package: name='de.re15.port'" <<<"$badging" || die "aapt: Paketname ist nicht de.re15.port"
    grep -qF "versionName='${version}'" <<<"$badging" || die "aapt: versionName ist nicht '${version}'"
    grep -q "^native-code:.*'arm64-v8a'" <<<"$badging" || die "aapt: arm64-v8a fehlt"
    grep -q "^native-code:.*'x86_64'"    <<<"$badging" || die "aapt: x86_64 fehlt"

    # 3. Signatur (apksigner.jar direkt ueber java: dieselbe Klasse wie apksigner(.bat), ohne cmd.exe)
    rc=0; signer="$("$APK_JAVA" -jar "$(apk_nativ "$APK_SIGNER_JAR")" verify -v --print-certs "$(apk_nativ "$apk")" 2>&1)" || rc=$?
    if (( rc != 0 )); then
        grep -E "ERROR|DOES NOT VERIFY" <<<"$signer" | head -5 | sed 's/^/   /' >&2 || true
        die "apksigner: Signatur ungueltig (rc=$rc) - die APK waere nicht installierbar oder nach dem Signieren veraendert"
    fi
    grep -qE "^Verified using v(2|3) scheme \(APK Signature Scheme v(2|3)\): true" <<<"$signer" \
        || die "apksigner: weder v2- noch v3-Signatur - mit targetSdk 35 auf Android 11+ nicht installierbar"
    grep -E "^Verified using v[1-4]|^Signer #1 certificate SHA-256" <<<"$signer" | sed 's/^/   /' || true

    # 4. volle Asset-Pruefung: erst beweist das Gate an Faelschungen, dass es sie erkennt, dann
    #    JEDE Datei der Asset-Baeume gegen die APK (roh gelesen wie Android) und das Manifest.
    #    Rueckgaben getrennt abfangen - kein Pipe, keine Subshell, die sie verschlucken koennte.
    gate="$(apk_nativ "$(dirname "${BASH_SOURCE[0]}")/apk_asset_gate.py")"
    echo "== Volle Asset-Pruefung 1/2: Selbsttest des Gates ($PY) =="
    rc=0; "$PY" "$gate" --selbsttest || rc=$?
    (( rc == 0 )) || die "Selbsttest des APK-Asset-Gates fehlgeschlagen (rc=$rc) - dem Gate ist nicht zu trauen"
    echo "== Volle Asset-Pruefung 2/2: APK gegen den Quellbaum =="
    rc=0; "$PY" "$gate" --repo "$(apk_nativ "$repo")" "$(apk_nativ "$apk")" || rc=$?
    case "$rc" in
        0) ;;
        1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab (Befunde oben)" ;;
        *) die "APK-Asset-Gate: keine Aussage moeglich (rc=$rc, Meldung oben)" ;;
    esac
    echo "== APK-PRUEFUNG-OK: $apk =="
}
