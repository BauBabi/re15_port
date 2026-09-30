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
#      Asset-Baeume, Tuer-Soll aus den Engine-Tabellen, Manifest, ZIP-Struktur wie Android sie liest) -
#      immer mit einer privaten, gegen release/apk_asset_gate.sha256 gepruefte Kopie des Gates
#      (make_package.sh: seine Kopie in APK_GATE_KOPIE, Runde 4 B4), Urteil ueber gate_laufen (unten)
#   6. die Kopie ist unveraendert UND die APK unter dem Pfad hat noch dieselbe Kennung - sonst
#      Abbruch (eine waehrend der Pruefung getauschte Datei gilt nicht als geprueft)
#   Ergebnis: APK_GEPRUEFT_KENNUNG ("sha256 crc32 bytes"), APK_GEPRUEFT_KOPIE (die gepruefte Kopie -
#   build_android.sh legt genau DIESE Bytes unter dem Auslieferungsnamen ab).
#
# ASSET-GATE: FESTHALTEN UND URTEIL (Nachbesserung R4-1, Gegenpruefung H1/H2) - auch make_package.sh
#   (Selbsttest, --quellbaum, --paket) laeuft ueber diese Funktionen:
#   H1  Bis dahin galt allein die Rueckgabe des Gates - und die erzeugt der gepruefte Code selbst. Eine 0-Byte-
#       Gate-Datei, eine vor main() abgeschnittene oder 'main()' ohne sys.exit gaben fuer Selbsttest UND Pruefung
#       immer 0: ein falsches Tuerarchiv, eine v1-Liste und ein PC-Paket mit veraenderter Datei gingen mit
#       ANDROID-GATES-OK bzw. "== Fertig ==" durch, auch wenn dasselbe Log SELBSTTEST-FEHLER zeigte. Jetzt:
#       gate_festhalten  das Gate laeuft nur als private Kopie, deren sha256 der in release/apk_asset_gate.sha256
#                        festgehaltenen gleicht (wie apk_signer.sha256; aendert jemand das Gate, haelt er dort den
#                        neuen Wert fest) - gate_laufen prueft das vor JEDEM Lauf erneut.
#       gate_laufen      Ausgabe in eine Datei (danach gezeigt), Urteil aus Rueckgabe UND Ausgabe (gate_urteil):
#                        letzte Zeile = Schlusszeile des Modus, genau eine Urteilszeile, Rueckgabe passend (OK <-> 0,
#                        FEHLER/ABWEICHUNG <-> 1, sonst "keine Aussage" = 2), Zaehlzeilen passend zur Schlusszeile;
#                        beim Selbsttest JEDE Fallzeile 1..n genau einmal, [ok] und rc = soll, n und die inneren
#                        Proben nicht unter den Mindestzahlen unten. So faellt auch ein NEU GEPINNTES kaputtes Gate.
#   H2  APK_GATE_DATEI kam aus der Umgebung: ein leeres oder altes Gate ersetzte still die ganze Asset-Pruefung
#       (Gegenpruefung A4/A5: v0.8.19-APK mit Liste v1 -> ANDROID-GATES-OK). Die Variable wird beim Laden verworfen;
#       make_package.sh setzt NACH dem Laden APK_GATE_KOPIE (seine festgehaltene, selbstgetestete Kopie).
# =============================================================================

APK_PRUEF_TMP=""
APK_GEPRUEFT_KENNUNG=""
APK_GEPRUEFT_KOPIE=""
APK_SIGNER_SOLL_DATEI="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/apk_signer.sha256"
GATE_QUELLE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/apk_asset_gate.py"
GATE_PIN_DATEI="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/apk_asset_gate.sha256"
# Mindestzahlen des Selbsttests (Stand Nachbesserung R4-1: 258 Faelle, 132 innere Proben). Mehr ist gut; weniger
# heisst, jemand hat Faelle entfernt - dann bewusst hier senken (git-Diff), nicht still.
GATE_SELBSTTEST_MIN_FAELLE=258
GATE_SELBSTTEST_MIN_INNEN=132
if [[ -n "${APK_GATE_DATEI:-}" ]]; then
    echo "   (Umgebung APK_GATE_DATEI=$APK_GATE_DATEI wird IGNORIERT - geprueft wird immer mit release/apk_asset_gate.py," \
         "festgehalten in release/apk_asset_gate.sha256; Gegenpruefung R4-1 H2)" >&2
fi
unset APK_GATE_DATEI
APK_GATE_KOPIE=""                 # nur make_package.sh setzt es (nach dem source) - nie aus der Umgebung
GATE_KOPIE=""                     # gate_festhalten: die gepruefte private Kopie
GATE_SHA256=""                    # gate_pin_pruefen: sha256 der zuletzt gepruften Gate-Datei
GATE_APK_EINTRAEGE=""             # nur fuer gate_laufen apk: unzip-Zaehlung der assets/-Eintraege (Gegenprobe)

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

# --- Asset-Gate festhalten + Urteil (Nachbesserung R4-1, H1/H2 - Kopf oben) -------------------------------------
gate_pin_pruefen() {       # $1 = Gate-Datei; Abbruch, wenn ihre sha256 nicht die festgehaltene ist -> GATE_SHA256
    local datei="$1" soll ist
    GATE_SHA256=""
    [[ -f "$GATE_PIN_DATEI" ]] || die "Gate-Pin fehlt: $GATE_PIN_DATEI (festgehaltene sha256 von release/apk_asset_gate.py)"
    soll="$(sed -e 's/#.*//' -e 's/[[:space:]]//g' "$GATE_PIN_DATEI" | tr -d '\n')"
    soll="${soll,,}"
    [[ "$soll" =~ ^[0-9a-f]{64}$ ]] || die "Gate-Pin $GATE_PIN_DATEI ist kein SHA-256 (64 Hexziffern): '$soll'"
    [[ -f "$datei" ]] || die "Asset-Gate fehlt: $datei"
    ist="$(apk_kennung "$datei")" || die "Asset-Gate nicht lesbar: $datei"
    ist="${ist%% *}"
    [[ "$ist" == "$soll" ]] || die "Asset-Gate ist NICHT das festgehaltene: $datei
        sha256 $ist ($(wc -c < "$datei") B), festgehalten: $soll (release/apk_asset_gate.sha256)
        Ein leeres, abgeschnittenes, veraendertes oder altes Gate prueft nichts, und seine Rueckgabe 0 hiesse nichts
        (Gegenpruefung R4-1, H1). Ist die Aenderung am Gate Absicht: Selbsttest, dann den neuen Wert festhalten:
            \"\$PY\" release/apk_asset_gate.py --selbsttest && sha256sum release/apk_asset_gate.py
        (die 64 Hexziffern in release/apk_asset_gate.sha256 eintragen und mit der Gate-Aenderung committen)"
    GATE_SHA256="$ist"
}

gate_festhalten() {        # $1 = privater Ordner -> GATE_KOPIE = gepruefte Kopie von release/apk_asset_gate.py
    local ziel="$1/apk_asset_gate.py"
    [[ -d "$1" ]] || die "gate_festhalten: Ordner fehlt: $1"
    [[ ! -e "$ziel" ]] || die "gate_festhalten: $ziel existiert schon"
    cp "$GATE_QUELLE" "$ziel" || die "Asset-Gate nicht kopierbar: $GATE_QUELLE"
    gate_pin_pruefen "$ziel"
    GATE_KOPIE="$ziel"
    echo "   Gate: private Kopie von release/apk_asset_gate.py ($ziel)"
    echo "         sha256 $GATE_SHA256 = festgehalten (release/apk_asset_gate.sha256)"
}

# gate_urteil <modus> <ausgabe-datei> <rueckgabe> -> 0 = OK, 1 = das Gate meldet Befunde (Ausgabe stimmig),
# 2 = keine Aussage. Unabhaengiger Code (nicht das Gate): liest nur die Ausgabe.
gate_urteil() {
    "$PY" - "$1" "$2" "$3" "$GATE_SELBSTTEST_MIN_FAELLE" "$GATE_SELBSTTEST_MIN_INNEN" "${GATE_APK_EINTRAEGE:-}" <<'PY'
import re, sys
modus, log, rc = sys.argv[1], sys.argv[2], int(sys.argv[3])
min_faelle, min_innen, apk_eintraege = int(sys.argv[4]), int(sys.argv[5]), sys.argv[6]
MARKE = {"selbsttest": "SELBSTTEST", "apk": "APK-ASSET-GATE", "quellbaum": "APK-ASSET-GATE-QUELLBAUM",
         "paket": "APK-ASSET-GATE-PAKET"}[modus]
text = open(log, "rb").read().decode("utf-8", "replace").replace("\r", "")
zeilen = [z for z in text.split("\n") if z.strip()]


def ende(code, grund):
    print("   Gate-Urteil (%s, Rueckgabe %d): %s" % (modus, rc, grund))
    sys.exit(code)


def genau_eine(muster, name):
    t = [m for m in (re.fullmatch(muster, z) for z in zeilen) if m]
    if len(t) != 1:
        ende(2, "Zeile '%s' %d-mal statt genau einmal - keine Aussage" % (name, len(t)))
    return [int(g) for g in t[0].groups()]


def tuer_soll():
    t = [m for m in (re.fullmatch(r"   Tuer-Soll: .* (\d+)/(\d+) wie die Engine-Tabelle .*", z) for z in zeilen) if m]
    if not t or any(m.group(1) != m.group(2) or int(m.group(2)) == 0 for m in t):
        ende(2, "Tuer-Soll-Zeilen fehlen oder melden nicht g/g - keine Aussage")
    return "%d Tuer-Soll-Zeilen g/g" % len(t)


if not zeilen:
    ende(2, "KEINE Ausgabe - das Gate lief nicht (leere oder abgeschnittene Datei? falscher Interpreter?)")
letzte = zeilen[-1]
m_ok = re.fullmatch(r"== %s-OK: (.+) ==" % re.escape(MARKE), letzte)
m_neg = re.fullmatch(r"== %s-(FEHLER|ABWEICHUNG): (.+) ==" % re.escape(MARKE), letzte)
if m_neg:
    if rc == 1:
        ende(1, "das Gate meldet %s - Schlusszeile und Rueckgabe stimmen ueberein" % m_neg.group(1))
    ende(2, "Schlusszeile meldet %s, aber Rueckgabe %d statt 1 - Ausgangspfad des Gates defekt" % (m_neg.group(1), rc))
if not m_ok:
    ende(2, "letzte Zeile ist nicht '== %s-OK: ... ==', sondern %r - keine Aussage" % (MARKE, letzte[:140]))
if rc != 0:
    ende(2, "Schlusszeile OK, aber Rueckgabe %d - Ausgangspfad des Gates defekt" % rc)
urteile = [z for z in zeilen if re.match(r"== (SELBSTTEST|APK-ASSET-GATE(-PAKET|-QUELLBAUM)?)-(OK|FEHLER|ABWEICHUNG)\b", z)]
if len(urteile) != 1:
    ende(2, "%d Urteilszeilen statt genau einer - keine Aussage" % len(urteile))
for z in zeilen:
    if z.startswith("ABBRUCH") or z.startswith("Traceback") or "[FEHLER]" in z:
        ende(2, "OK-Schlusszeile, aber im selben Lauf %r - keine Aussage" % z[:140])
rest = m_ok.group(1)
if modus == "selbsttest":
    m = re.fullmatch(r"(\d+)/(\d+) Faelle \(.*\)", rest)
    if not m:
        ende(2, "Schlusszeile ohne 'n/n Faelle' - keine Aussage")
    ok_n, n = int(m.group(1)), int(m.group(2))
    if ok_n != n or n < min_faelle:
        ende(2, "SELBSTTEST-OK meldet %d/%d Faelle, verlangt n/n mit n >= %d (release/apk_pruefen.sh "
                "GATE_SELBSTTEST_MIN_FAELLE) - weniger Faelle = schwaecherer Selbsttest" % (ok_n, n, min_faelle))
    a, b = genau_eine(r"   Innere Proben: (\d+)/(\d+) \(.*\)", "Innere Proben: m/m")
    if a != b or b < min_innen:
        ende(2, "Innere Proben %d/%d, verlangt m/m mit m >= %d (GATE_SELBSTTEST_MIN_INNEN)" % (a, b, min_innen))
    faelle = [m for m in (re.fullmatch(r"   \[(ok|FEHLER)\] (\d+) (.*) rc=(-?\d+) \(soll (-?\d+)\)(.*)", z) for z in zeilen) if m]
    if sorted(int(f.group(2)) for f in faelle) != list(range(1, n + 1)):
        ende(2, "%d Fallzeilen, Nummern nicht genau 1..%d - keine Aussage" % (len(faelle), n))
    for f in faelle:
        if f.group(1) != "ok" or f.group(4) != f.group(5) or f.group(6).strip():
            ende(2, "Fallzeile %s nicht [ok] mit rc = soll: %r" % (f.group(2), f.group(0).strip()[:140]))
    ende(0, "SELBSTTEST-OK %d/%d, jede Fallzeile [ok] mit rc = soll, innere Proben %d/%d (Mindestzahlen %d/%d)"
         % (n, n, a, b, min_faelle, min_innen))
if modus == "apk":
    m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, "
                     r"ZIP-Struktur wie Android sie liest", rest)
    if not m:
        ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
    n = int(m.group(1))
    q, a, g, b = genau_eine(r"   SUMME\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)", "SUMME")
    mz, mb = genau_eine(r"   Manifest:\s+(\d+) Zeilen / (\d+) Bytes \(.*\)", "Manifest: n Zeilen")
    if n <= 0 or not (q == a == g == mz == n) or mb != b:
        ende(2, "Zaehlung passt nicht zur Schlusszeile (%d): Quelle %d, APK %d, gleich %d, Manifest %d Zeilen / %d B, "
                "Bytes gleich %d" % (n, q, a, g, mz, mb, b))
    for label in ("RE2/DOOR", "RE15DOOR"):
        t = genau_eine(r"   %s:\s+Quelle (\d+), APK (\d+), sha256 gleich (\d+)/(\d+)" % re.escape(label), label)
        if not (t[0] == t[1] == t[2] == t[3] > 0):
            ende(2, "%s: Quelle/APK/gleich %r nicht gleich bzw. 0" % (label, t))
    genau_eine(r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B, sha256 gleich", "TORSE.VBS ... sha256 gleich")
    zusatz = ""
    if apk_eintraege:
        if int(apk_eintraege) != a + 1:
            ende(2, "unzip zaehlt %s Eintraege unter assets/, das Gate %d Asset-Dateien + Manifest" % (apk_eintraege, a))
        zusatz = " = unzip-Zaehlung - 1"
    ende(0, "APK-ASSET-GATE-OK: %d Dateien, Quelle = APK%s = gleich = Manifestzeilen, RE2/DOOR + RE15DOOR + TORSE.VBS "
            "gleich, %s" % (n, zusatz, tuer_soll()))
if modus == "quellbaum":
    m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt", rest)
    if not m:
        ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
    n, k = int(m.group(1)), int(m.group(2))
    baum = [int(x.group(1)) for x in (re.fullmatch(r"   \S+\s+(\d+) Dateien", z) for z in zeilen) if x]
    if n <= 0 or len(baum) != k or sum(baum) != n or 0 in baum:
        ende(2, "Baumzeilen %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
    ende(0, "APK-ASSET-GATE-QUELLBAUM-OK: %d Dateien in %d Baeumen (Summe der Baumzeilen), %s" % (n, k, tuer_soll()))
if modus == "paket":
    m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen bytegleich, nichts zusaetzlich", rest)
    if not m:
        ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
    n, k = int(m.group(1)), int(m.group(2))
    baum = [(int(x.group(1)), int(x.group(2))) for x in (re.fullmatch(r"   \S+\s+(\d+)\s+(\d+)", z) for z in zeilen) if x]
    if n <= 0 or len(baum) != k or any(qq != gg or qq == 0 for qq, gg in baum) or sum(gg for _qq, gg in baum) != n:
        ende(2, "Baumzeilen (Quelle, gleich) %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
    ende(0, "APK-ASSET-GATE-PAKET-OK: %d Dateien in %d Baeumen, je Baum Quelle = gleich, %s" % (n, k, tuer_soll()))
ende(2, "unbekannter Modus")
PY
}

# gate_laufen <modus> <gate> [argumente ...] -> 0 = OK, 1 = Befunde, 2 = keine Aussage (siehe gate_urteil).
# Pin-Pruefung VOR jedem Lauf; Ausgabe in eine Datei (kein Pipe, das die Rueckgabe verschlucken koennte), danach gezeigt.
gate_laufen() {
    local modus="$1" gate="$2" log rc=0 urteil=0
    shift 2
    gate_pin_pruefen "$gate"
    log="$(mktemp "${TMPDIR:-/tmp}/re15_gate_ausgabe.XXXXXX")" || die "gate_laufen: kein Temp-Platz fuer die Ausgabe"
    log="$(apk_nativ "$log")"
    echo "   (Gate $modus laeuft, sha256 ${GATE_SHA256:0:16}...; Ausgabe folgt nach dem Ende)"
    "$PY" "$(apk_nativ "$gate")" "$@" > "$log" 2>&1 || rc=$?
    cat "$log"
    gate_urteil "$modus" "$log" "$rc" || urteil=$?
    rm -f "$log"
    return "$urteil"
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
    #    Nachbesserung R4-1 (H1/H2): immer eine gegen release/apk_asset_gate.sha256 gepruefte private Kopie -
    #    make_package.sh uebergibt seine (APK_GATE_KOPIE, schon selbstgetestet; Quellbaum, APK und Pakete pruefen
    #    so mit DEMSELBEN Code), sonst wird hier eine angelegt. Urteil aus Rueckgabe UND Ausgabe (gate_laufen).
    if [[ -n "${APK_GATE_KOPIE:-}" ]]; then
        gate="$APK_GATE_KOPIE"
        echo "   Gate: Kopie aus make_package.sh ($gate)"
    else
        gate_festhalten "$APK_PRUEF_TMP"
        gate="$GATE_KOPIE"
    fi
    echo "== Volle Asset-Pruefung 1/2: Selbsttest des Gates ($PY) =="
    rc=0; gate_laufen selbsttest "$gate" --selbsttest || rc=$?
    (( rc == 0 )) || die "Selbsttest des APK-Asset-Gates nicht bestanden (Urteil $rc, Meldung oben) - dem Gate ist nicht zu trauen"
    echo "== Volle Asset-Pruefung 2/2: APK gegen den Quellbaum =="
    rc=0; GATE_APK_EINTRAEGE="$n_assets"
    gate_laufen apk "$gate" --repo "$(apk_nativ "$repo")" "$(apk_nativ "$kopie")" || rc=$?
    GATE_APK_EINTRAEGE=""
    case "$rc" in
        0) ;;
        1) die "APK-Asset-Gate: die APK weicht vom Quellbaum ab (Befunde oben)" ;;
        *) die "APK-Asset-Gate: keine Aussage moeglich (Urteil $rc, Meldung oben)" ;;
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
