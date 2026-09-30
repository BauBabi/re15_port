#!/usr/bin/env bash
# =============================================================================
# release/apk_pruefen.sh — EINE Pruefkette fuer die Android-APK (Runde 34a, Nachbesserung R1/R2)
# =============================================================================
# Per `source` aus release/build_android.sh (nach dem Bau und --gate-only) UND aus
# release/make_package.sh (vor dem Zippen): beide pruefen damit genau dasselbe.
# Vorher prueften die beiden Skripte Verschiedenes, und make_package.sh zippte eine APK, deren
# versionName, Signatur und Codestand niemand angesehen hatte (Gegenpruefung R1, B2/B4/B5).
#
# BRAUCHT vom Aufrufer: die() (bricht ab), PY (release/python_finden.sh), set -euo pipefail.
# Der Aufrufer ruft apk_pruefen_aufraeumen in seiner EXIT-Falle (die Pruefkopie, ~360 MB).
#
# apk_werkzeuge_finden
#     setzt APK_SDK, APK_AAPT, APK_ZIPALIGN, APK_SIGNER_JAR, APK_JAVA, APK_SIGNER_SOLL - oder bricht
#     ab. KEIN stilles Ueberspringen: bis zur Nachbesserung R1 lief `--gate-only <apk> --version
#     v9.9.9` ohne aapt mit Rueckgabe 0 durch.
#     SDK:  ANDROID_SDK_ROOT / ANDROID_HOME, sonst %LOCALAPPDATA%/Android/Sdk bzw. ~/Android/Sdk
#     build-tools: APK_BUILD_TOOLS (Standard 35.0.0 = BUILD_TOOLS_PKG in build_android.sh)
#     Java: JAVA_HOME, sonst das Adoptium-JDK 17 unter C:/Program Files, sonst java im PATH
#     Signer: RE15_APK_SIGNER_SHA256, sonst release/apk_signer.sha256 (Nachbesserung R2, B4)
#
# apk_pruefen <apk> <version> <repo>   - jeder Befund bricht ab (die)
#   0. PRUEFKOPIE (Nachbesserung R2, Gegenpruefung B3): die APK wird EINMAL in einen privaten
#      Temp-Ordner kopiert (sha256/CRC32/Groesse im selben Lesedurchgang); JEDER folgende Schritt
#      liest nur diese Kopie. Vorher las jeder Schritt den Pfad neu - zwischen apksigner und
#      Asset-Gate liegt der Selbsttest (6-55 s), und eine in der Zeit getauschte unsignierte APK
#      galt als "geprueft" (EXIT 0) und waere gezippt worden.
#   1. Stichproben (unzip -Z1): native libs beider ABIs, Manifest, je ein Asset je Baum
#   2. aapt dump badging: package 'de.re15.port', versionName = <version>, arm64-v8a + x86_64
#   3. zipalign -c -P 16 4 (Nachbesserung R2, B5): Stored-Daten 4-Byte-, .so 16-KiB-ausgerichtet -
#      mit targetSdk 35 installiert Android 11+ keine APK mit unausgerichteter resources.arsc, und
#      useLegacyPackaging=false (build.gradle) verlangt seitenausgerichtete .so
#   4. apksigner verify: gueltig, per v2 oder v3 (ohne v2+ auf Android 11+ nicht installierbar),
#      genau EIN Signer, und dessen Zertifikat = der festgehaltene (Nachbesserung R2, B4: sonst
#      installiert sich die APK nicht als Update, und Deinstallieren loescht die Spielstaende)
#   5. release/apk_asset_gate.py --selbsttest, dann --repo <repo> <kopie> (JEDE Datei der
#      Asset-Baeume, Tuer-Soll aus den Engine-Tabellen, Manifest, ZIP-Struktur wie Android sie liest);
#      ist APK_GATE_DATEI gesetzt, diese Gate-Datei (make_package.sh: seine private Kopie, Runde 4 B4)
#   6. die Kopie ist unveraendert UND die APK unter dem Pfad hat noch dieselbe Kennung - sonst
#      Abbruch (eine waehrend der Pruefung getauschte Datei gilt nicht als geprueft)
#   Ergebnis: APK_GEPRUEFT_KENNUNG ("sha256 crc32 bytes"), APK_GEPRUEFT_KOPIE (die gepruefte Kopie -
#   build_android.sh legt genau DIESE Bytes unter dem Auslieferungsnamen ab).
# =============================================================================

APK_PRUEF_TMP=""
APK_GEPRUEFT_KENNUNG=""
APK_GEPRUEFT_KOPIE=""
APK_SIGNER_SOLL_DATEI="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/apk_signer.sha256"

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
    [[ -d "$sdk" ]] || die "APK-Pruefung: Android-SDK-Ordner fehlt: $sdk (ANDROID_SDK_ROOT setzen) - ohne aapt,
        zipalign und apksigner gibt es keine Pruefung von Version, Paketname, ABIs, Ausrichtung und Signatur.
        Nur die Assets: \"\$PY\" release/apk_asset_gate.py --repo . <apk>"
    APK_SDK="$sdk"

    bt="$sdk/build-tools/${APK_BUILD_TOOLS:-35.0.0}"
    if [[ ! -d "$bt" ]]; then
        # Rueckfall: die neueste vorhandene build-tools-Version mit aapt, zipalign UND apksigner.jar
        bt=""
        while IFS= read -r kand; do
            if [[ ( -f "$kand/aapt" || -f "$kand/aapt.exe" ) && ( -f "$kand/zipalign" || -f "$kand/zipalign.exe" )
                  && -f "$kand/lib/apksigner.jar" ]]; then bt="$kand"; fi
        done < <(ls -d "$sdk"/build-tools/*/ 2>/dev/null | sed 's#/$##' | sort -V)
        [[ -n "$bt" ]] || die "APK-Pruefung: keine build-tools mit aapt + zipalign + lib/apksigner.jar unter $sdk/build-tools"
        echo "   (build-tools ${APK_BUILD_TOOLS:-35.0.0} fehlt - nehme $(basename "$bt"))"
    fi
    if [[ -f "$bt/aapt.exe" ]]; then APK_AAPT="$bt/aapt.exe"; else APK_AAPT="$bt/aapt"; fi
    [[ -f "$APK_AAPT" ]] || die "APK-Pruefung: aapt fehlt in $bt"
    if [[ -f "$bt/zipalign.exe" ]]; then APK_ZIPALIGN="$bt/zipalign.exe"; else APK_ZIPALIGN="$bt/zipalign"; fi
    [[ -f "$APK_ZIPALIGN" ]] || die "APK-Pruefung: zipalign fehlt in $bt"
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

    # Erwarteter Signer (Nachbesserung R2, B4): Umgebung vor Datei; nur Kommentar-/Leerzeilen erlaubt
    local quelle="RE15_APK_SIGNER_SHA256"
    APK_SIGNER_SOLL="${RE15_APK_SIGNER_SHA256:-}"
    if [[ -z "$APK_SIGNER_SOLL" ]]; then
        quelle="release/apk_signer.sha256"
        [[ -f "$APK_SIGNER_SOLL_DATEI" ]] || die "APK-Pruefung: $APK_SIGNER_SOLL_DATEI fehlt (erwarteter Signer der APK)"
        APK_SIGNER_SOLL="$(sed -e 's/#.*//' -e 's/[[:space:]]//g' "$APK_SIGNER_SOLL_DATEI" | tr -d '\n')"
    fi
    APK_SIGNER_SOLL="${APK_SIGNER_SOLL,,}"
    [[ "$APK_SIGNER_SOLL" =~ ^[0-9a-f]{64}$ ]] \
        || die "APK-Pruefung: erwarteter Signer aus $quelle ist kein SHA-256 (64 Hexziffern): '$APK_SIGNER_SOLL'"
    echo "   APK-Werkzeuge: aapt + zipalign + apksigner aus $(basename "$bt"), Java $APK_JAVA"
    echo "   erwarteter Signer: ${APK_SIGNER_SOLL:0:16}... ($quelle)"
}

apk_kennung() {          # $1 = Datei -> "sha256 crc32 groesse" in EINEM Lesedurchgang
    "$PY" - "$(apk_nativ "$1")" <<'PY'
import hashlib, sys, zlib
h, c, n = hashlib.sha256(), 0, 0
with open(sys.argv[1], "rb") as f:
    while True:
        b = f.read(1 << 20)
        if not b:
            break
        h.update(b)
        c = zlib.crc32(b, c)
        n += len(b)
print("%s %08x %d" % (h.hexdigest(), c & 0xFFFFFFFF, n))
PY
}

apk_kopie_mit_kennung() {   # $1 = Quelle, $2 = Ziel (wird neu angelegt) -> Kennung der geschriebenen Bytes
    "$PY" - "$(apk_nativ "$1")" "$(apk_nativ "$2")" <<'PY'
import hashlib, sys, zlib
h, c, n = hashlib.sha256(), 0, 0
with open(sys.argv[1], "rb") as f, open(sys.argv[2], "xb") as g:
    while True:
        b = f.read(1 << 20)
        if not b:
            break
        g.write(b)
        h.update(b)
        c = zlib.crc32(b, c)
        n += len(b)
print("%s %08x %d" % (h.hexdigest(), c & 0xFFFFFFFF, n))
PY
}

apk_pruefen_aufraeumen() {  # Pruefkopie weg (EXIT-Falle der Aufrufer, und nach Gebrauch)
    if [[ -n "${APK_PRUEF_TMP:-}" && -d "$APK_PRUEF_TMP" ]]; then
        rm -rf "$APK_PRUEF_TMP"
    fi
    APK_PRUEF_TMP=""
    APK_GEPRUEFT_KOPIE=""
}

apk_pruefen() {
    local apk="$1" version="$2" repo="$3"
    local kopie kennung jetzt list f n_assets badging za signer n_signer digest rc gate
    [[ -n "$version" ]] || die "apk_pruefen: Version fehlt (versionName wird immer geprueft)"
    [[ -n "${APK_AAPT:-}" && -n "${APK_ZIPALIGN:-}" && -n "${APK_SIGNER_JAR:-}" && -n "${APK_JAVA:-}" \
       && -n "${APK_SIGNER_SOLL:-}" ]] || die "apk_pruefen: erst apk_werkzeuge_finden aufrufen"
    [[ -n "${PY:-}" ]] || die "apk_pruefen: kein Python (release/python_finden.sh)"
    [[ -f "$apk" ]] || die "apk_pruefen: APK fehlt: $apk"
    echo "== APK-Pruefung: $apk (Version $version) =="

    # 0. Pruefkopie - ab hier liest KEIN Schritt mehr den Pfad $apk (bis auf den Vergleich in 6.)
    apk_pruefen_aufraeumen
    APK_GEPRUEFT_KENNUNG=""
    APK_PRUEF_TMP="$(mktemp -d "${TMPDIR:-/tmp}/re15_apk_pruefen.XXXXXX")" \
        || die "apk_pruefen: kein Temp-Ordner fuer die Pruefkopie"
    # Windows-Pfad (Runde 4): make_package.sh stellt spaeter /c/msys64/usr/bin vorn in den PATH - dessen rm kennt /tmp
    # als C:/msys64/tmp; mit dem C:/...-Pfad raeumt apk_pruefen_aufraeumen in jeder Lage den richtigen Ordner ab
    APK_PRUEF_TMP="$(apk_nativ "$APK_PRUEF_TMP")"
    kopie="$APK_PRUEF_TMP/$(basename "$apk")"
    kennung="$(apk_kopie_mit_kennung "$apk" "$kopie")" || die "apk_pruefen: APK nicht lesbar/kopierbar: $apk"
    [[ "$kennung" =~ ^[0-9a-f]{64}\ [0-9a-f]{8}\ [0-9]+$ ]] || die "apk_pruefen: Kennung der Pruefkopie unlesbar: '$kennung'"
    echo "   Pruefkopie (sha256 crc32 Bytes: $kennung) - alle Schritte lesen nur sie: $kopie"

    # 1. Stichproben (die volle Asset-Pruefung folgt in 5.)
    list="$(unzip -Z1 "$kopie")" || die "APK nicht lesbar (unzip -Z1): $apk"
    for f in lib/arm64-v8a/libmain.so lib/arm64-v8a/libSDL2.so lib/x86_64/libmain.so lib/x86_64/libSDL2.so \
             assets/re15_assets.txt assets/shared_assets/PSX/DATA/TEX.TIM assets/shared_assets/PSX/STAGE1/ROOM1240.RDT \
             assets/shared_assets/extracted_fx/effect0_blood.tim assets/shared_assets/RE2/CDEMD0.EMS \
             assets/synchro/STAGE1/room1170/main00.wav; do
        grep -qxF "$f" <<<"$list" || die "APK unvollstaendig: $f fehlt"
    done
    n_assets="$(grep -c '^assets/' <<<"$list" || true)"
    echo "   Stichproben ok; unzip zaehlt $n_assets Asset-Eintraege (Pruefung jedes Eintrags: Schritt 5)"

    # 2. aapt badging - Paketname, Version, ABIs (fehlt aapt: apk_werkzeuge_finden ist schon abgebrochen)
    rc=0; badging="$("$APK_AAPT" dump badging "$(apk_nativ "$kopie")" 2>&1)" || rc=$?
    grep -E "^package:|^native-code:|^sdkVersion|^targetSdkVersion" <<<"$badging" | sed 's/^/   /' || true
    (( rc == 0 )) || die "aapt dump badging fehlgeschlagen (rc=$rc): $(head -3 <<<"$badging")"
    grep -q "^package: name='de.re15.port'" <<<"$badging" || die "aapt: Paketname ist nicht de.re15.port"
    grep -qF "versionName='${version}'" <<<"$badging" || die "aapt: versionName ist nicht '${version}'"
    grep -q "^native-code:.*'arm64-v8a'" <<<"$badging" || die "aapt: arm64-v8a fehlt"
    grep -q "^native-code:.*'x86_64'"    <<<"$badging" || die "aapt: x86_64 fehlt"

    # 3. Ausrichtung (Nachbesserung R2, B5) - wie AGP sie herstellt: Stored 4 B, .so 16 KiB (-P 16)
    rc=0; za="$("$APK_ZIPALIGN" -c -P 16 4 "$(apk_nativ "$kopie")" 2>&1)" || rc=$?
    if (( rc != 0 )); then
        "$APK_ZIPALIGN" -c -v -P 16 4 "$(apk_nativ "$kopie")" 2>&1 | grep -E "BAD|FAILED" | head -8 | sed 's/^/   /' >&2 || true
        die "zipalign: Ausrichtung falsch (rc=$rc${za:+, $(tail -1 <<<"$za")}) - mit targetSdk 35 installiert Android 11+
        keine APK mit unausgerichteter resources.arsc bzw. (useLegacyPackaging=false) nicht seitenausgerichteten .so"
    fi
    echo "   zipalign -c -P 16 4: Ausrichtung ok (Stored-Daten 4 B, .so 16 KiB)"

    # 4. Signatur (apksigner.jar direkt ueber java: dieselbe Klasse wie apksigner(.bat), ohne cmd.exe)
    rc=0; signer="$("$APK_JAVA" -jar "$(apk_nativ "$APK_SIGNER_JAR")" verify -v --print-certs "$(apk_nativ "$kopie")" 2>&1)" || rc=$?
    if (( rc != 0 )); then
        grep -E "ERROR|DOES NOT VERIFY" <<<"$signer" | head -5 | sed 's/^/   /' >&2 || true
        die "apksigner: Signatur ungueltig (rc=$rc) - die APK waere nicht installierbar oder nach dem Signieren veraendert"
    fi
    grep -qE "^Verified using v(2|3) scheme \(APK Signature Scheme v(2|3)\): true" <<<"$signer" \
        || die "apksigner: weder v2- noch v3-Signatur - mit targetSdk 35 auf Android 11+ nicht installierbar"
    grep -E "^Verified using v[1-4]|^Number of signers|^Signer #1 certificate SHA-256" <<<"$signer" | sed 's/^/   /' || true
    n_signer="$(sed -n 's/^Number of signers: \([0-9]*\).*/\1/p' <<<"$signer" | head -1)"
    [[ "$n_signer" == 1 ]] || die "apksigner: ${n_signer:-?} Signer statt genau einem"
    digest="$(sed -n 's/^Signer #1 certificate SHA-256 digest: \([0-9a-fA-F]*\).*/\1/p' <<<"$signer" | head -1)"
    [[ "${digest,,}" == "$APK_SIGNER_SOLL" ]] || die "apksigner: Signer-Zertifikat ${digest:-?}, erwartet $APK_SIGNER_SOLL
        (release/apk_signer.sha256 bzw. RE15_APK_SIGNER_SHA256). Mit einem anderen Schluessel installiert sich die
        APK nicht als Update ueber die vorige Version; wer deshalb deinstalliert, verliert Android/data/de.re15.port/files
        (Spielstaende). Ist der neue Schluessel Absicht, seinen Wert in release/apk_signer.sha256 festhalten."

    # 5. volle Asset-Pruefung: erst beweist das Gate an Faelschungen, dass es sie erkennt, dann
    #    JEDE Datei der Asset-Baeume gegen die APK (roh gelesen wie Android), Tuer-Soll, Manifest.
    #    Rueckgaben getrennt abfangen - kein Pipe, keine Subshell, die sie verschlucken koennte.
    #    APK_GATE_DATEI (Runde 4, Gegenpruefung R3 B4): make_package.sh uebergibt seine private, schon
    #    selbstgetestete Kopie des Gates - dann pruefen Quellbaum, APK und Pakete mit DEMSELBEN Code.
    gate="$(apk_nativ "${APK_GATE_DATEI:-$(dirname "${BASH_SOURCE[0]}")/apk_asset_gate.py}")"
    echo "== Volle Asset-Pruefung 1/2: Selbsttest des Gates ($PY) =="
    rc=0; "$PY" "$gate" --selbsttest || rc=$?
    (( rc == 0 )) || die "Selbsttest des APK-Asset-Gates fehlgeschlagen (rc=$rc) - dem Gate ist nicht zu trauen"
    echo "== Volle Asset-Pruefung 2/2: APK gegen den Quellbaum =="
    rc=0; "$PY" "$gate" --repo "$(apk_nativ "$repo")" "$(apk_nativ "$kopie")" || rc=$?
    case "$rc" in
        0) ;;
        1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab (Befunde oben)" ;;
        *) die "APK-Asset-Gate: keine Aussage moeglich (rc=$rc, Meldung oben)" ;;
    esac

    # 6. dieselben Bytes? Kopie unveraendert, und unter dem Pfad liegt noch dieselbe Datei
    jetzt="$(apk_kennung "$kopie")" || die "apk_pruefen: Pruefkopie nicht mehr lesbar"
    [[ "$jetzt" == "$kennung" ]] || die "apk_pruefen: Pruefkopie waehrend der Pruefung veraendert ($kennung -> $jetzt)"
    jetzt="$(apk_kennung "$apk")" || die "apk_pruefen: APK nach der Pruefung nicht mehr lesbar: $apk"
    [[ "$jetzt" == "$kennung" ]] || die "APK wurde WAEHREND der Pruefung veraendert oder ersetzt: $apk
        geprueft (Kopie vom Anfang): $kennung
        jetzt unter dem Pfad:        $jetzt
        Die jetzige Datei ist ungeprueft - neu starten."
    APK_GEPRUEFT_KENNUNG="$kennung"
    APK_GEPRUEFT_KOPIE="$kopie"
    echo "== APK-PRUEFUNG-OK: $apk (sha256 crc32 Bytes: $kennung) =="
}
