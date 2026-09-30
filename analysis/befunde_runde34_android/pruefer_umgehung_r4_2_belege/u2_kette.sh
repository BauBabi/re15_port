#!/usr/bin/env bash
# Gegenpruefung R4-2 (Umgehung): die Kette mit den Skripten des Arbeitsbaums in der EIGENEN Sandbox build/r34a/pruefer_u2/sb
# (r4_sandbox_anlegen.sh, Quellbaum per Hardlink, eigenes git-Repo; release/ und Index des Arbeitsbaums unberuehrt).
# Teile: P = Pin-Datei-Varianten (gate_pin_pruefen direkt), A = build_android.sh --gate-only, B = make_package.sh.
# Je Lauf Rueckgabe selbst abgefangen, Ausgabe in eine Datei (kein Pipe um den Aufruf). Aufruf: u2_kette.sh [P] [A] [B]
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python; W=build/r34a/pruefer_u2; S=$W/sb; L=$W/logs/kette; M=$W/mut/k1; F=$W/apk
NB1=build/r34a/nb/apk/NB1.apk
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
TEILE="${*:-P A B}"
mkdir -p "$L"
[[ -d "$S" ]] || { echo "Sandbox fehlt: $S"; exit 9; }
G=release/apk_asset_gate.py
sandbox_echt() {
    for f in make_package.sh apk_pruefen.sh python_finden.sh apk_asset_gate.py apk_asset_gate.sha256 zip_exec_bit.py \
             apk_signer.sha256 build_android.sh; do
        cp -p "release/$f" "$S/release/$f"; cmp -s "release/$f" "$S/release/$f" || { echo "Kopie weicht ab: $f"; exit 9; }
    done
}
sandbox_echt
echo "Sandbox-Skripte = Arbeitsbaum (Pin $(grep -v '^#' release/apk_asset_gate.sha256 | tr -d '\r' | cut -c1-16)...)"
gate_setzen() {   # $1 = ECHT oder Datei, $2 = echt|umpinnen
    if [[ "$1" == ECHT ]]; then cp -p "$G" "$S/release/apk_asset_gate.py"; else cp "$1" "$S/release/apk_asset_gate.py"; fi
    if [[ "$2" == umpinnen ]]; then
        "$PY" -c "import hashlib,sys; print(hashlib.sha256(open(sys.argv[1],'rb').read()).hexdigest())" "$S/release/apk_asset_gate.py" \
            > "$S/release/apk_asset_gate.sha256"
    else
        cp -p release/apk_asset_gate.sha256 "$S/release/apk_asset_gate.sha256"
    fi
    echo "--- Sandbox-Gate := $(basename "$1") ($(wc -c < "$S/release/apk_asset_gate.py") B), Pin: $2"
}
zeige() {
    grep -a -E 'ANDROID-GATES-OK|APK-PRUEFUNG-OK|^ABBRUCH|Gate-Urteil|NICHT das festgehaltene|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE(-PAKET|-QUELLBAUM)?-(OK|ABWEICHUNG)|Manifest-Pruefsumme|Inhalt weicht ab|SHA256SUMS.txt geschrieben|neue vorgemerkt|== Fertig|fremde Datei|andere Plattform|Satz re15|--ohne-android|Android-Satz' \
        "$1" | tr -d '\r' | sed 's/^/     /' | cut -c1-230
}

if [[ " $TEILE " == *" P "* ]]; then
    echo "=== P: Pin-Datei-Varianten (gate_pin_pruefen aus der Sandbox-Kopie von apk_pruefen.sh, echtes Gate) ==="
    ECHT_SHA="$(grep -v '^#' release/apk_asset_gate.sha256 | tr -d '\r\n ')"
    pin_fall() {   # $1 = Titel, $2 = Inhalt (printf-Format)
        local t="$L/pin_test.sha256" rc=0 aus
        printf "$2" > "$t"
        aus="$(bash -c '
            die() { echo "DIE: $*" | head -2; exit 1; }
            PY=/c/Python310/python
            source "$1/release/apk_pruefen.sh" 2>/dev/null
            GATE_PIN_DATEI="$2"
            gate_pin_pruefen "$3" && echo "ANGENOMMEN sha=${GATE_SHA256:0:16}..."' _ "$S" "$t" "$G" 2>&1)" || rc=$?
        printf '   %-58s rc=%d  %s\n' "$1" "$rc" "$(echo "$aus" | head -1 | cut -c1-120)"
    }
    U="${ECHT_SHA^^}"
    pin_fall "P1 echt, Grossbuchstaben + CRLF + Kommentar"           "# Kommentar\r\n$U  # hinten\r\n"
    pin_fall "P2 alter + neuer Wert je Zeile (Nachtragen statt Ersetzen)" "0000000000000000000000000000000000000000000000000000000000000000\n$ECHT_SHA\n"
    pin_fall "P3 Zeile aus sha256sum (Wert + *Pfad)"                   "$ECHT_SHA *release/apk_asset_gate.py\n"
    pin_fall "P4 Wert zweimal in einer Zeile"                          "$ECHT_SHA $ECHT_SHA\n"
    pin_fall "P5 leer"                                                 ""
    pin_fall "P6 nur Kommentar mit dem Wert"                           "# $ECHT_SHA\n"
    pin_fall "P7 Wert mit Praefix sha256:"                             "sha256:$ECHT_SHA\n"
    pin_fall "P8 Wert eines anderen Gates (be8b60f3)"                  "$(git show be8b60f3:release/apk_asset_gate.py | sha256sum | cut -c1-64)\n"
    rm -f "$L/pin_test.sha256"
fi

if [[ " $TEILE " == *" A "* ]]; then
    echo "=== A: build_android.sh --gate-only (Sandbox) ==="
    ga() {   # $1 = Tag, $2 = APK, $3 = Version
        local tag="$1" apk="$2" ver="$3" rc=0 t0; t0=$(date +%s)
        bash "$S/release/build_android.sh" --gate-only "$apk" --version "$ver" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== A $tag ($(basename "$apk") --version $ver): EXIT=$rc ($(( $(date +%s) - t0 )) s)"
        zeige "$L/$tag.log"
    }
    gate_setzen ECHT echt
    ga A0_echt_NB1 "$NB1" v0.8.20-nb1
    ga A1_echt_FS  "$F/FS_sig.apk" v0.8.20-nb1
    ga A2_echt_FD  "$F/FD_sig.apk" v0.8.20-nb1
    # Ueberlebender des Selbsttests (Abschnitt 1): manifest_pruefen 'a_sha != m_sha' -> 'a_sha > m_sha', NEU GEPINNT
    gate_setzen "$M/E_manifest_pruefen_Z1247_gt.py" umpinnen
    ga A3_Zgt_um_FS "$F/FS_sig.apk" v0.8.20-nb1
    ga A3b_Zgt_um_FD "$F/FD_sig.apk" v0.8.20-nb1
    gate_setzen ECHT echt
    # Urteil = einzige Instanz: eine Ein-Zeilen-Aenderung in gate_urteil (Sandbox-Kopie von apk_pruefen.sh), echtes Gate + Pin
    "$PY" - "$S/release/apk_pruefen.sh" <<'PYEOF'
import sys
p = sys.argv[1]
t = open(p, "rb").read().decode("utf-8")
alt = '        ende(1, "das Gate meldet %s - Schlusszeile und Rueckgabe stimmen ueberein" % m_neg.group(1))'
assert t.count(alt) == 1, t.count(alt)
open(p, "wb").write(t.replace(alt, alt.replace("ende(1,", "ende(0,")).encode("utf-8"))
print("--- Sandbox apk_pruefen.sh: gate_urteil 'ende(1, \"das Gate meldet' -> 'ende(0,' (1 Zeichen)")
PYEOF
    ga A4_urteil0_echt_FD "$F/FD_sig.apk" v0.8.20-nb1
    sandbox_echt
    cmp -s "$G" "$S/release/apk_asset_gate.py" && cmp -s release/apk_asset_gate.sha256 "$S/release/apk_asset_gate.sha256" \
        && cmp -s release/apk_pruefen.sh "$S/release/apk_pruefen.sh" && echo "   Sandbox-Gate + Pin + apk_pruefen.sh = Arbeitsbaum"
    echo "Pruefkopien uebrig: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_pruefen.* 2>/dev/null | wc -l)"
fi

if [[ " $TEILE " == *" B "* ]]; then
    echo "=== B: make_package.sh --version v0.8.19 --only linux (Sandbox) ==="
    NAME=re15_port_v0.8.19
    R=$S/release
    mp() {
        local tag="$1" rc=0 t0; shift; t0=$(date +%s)
        rm -f "$R/SHA256SUMS.txt"
        bash "$R/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== B $tag ($*): EXIT=$rc ($(( $(date +%s) - t0 )) s)"
        zeige "$L/$tag.log"
        if [[ -f "$R/SHA256SUMS.txt" ]]; then
            echo "     SHA256SUMS.txt: $(tr -d '\r' < "$R/SHA256SUMS.txt" | awk '{print $2}' | tr '\n' ' ')"
        else
            echo "     SHA256SUMS.txt: (keine)"
        fi
        echo "     git-Index (Sandbox): $(git -C "$S" diff --cached --name-status | tr '\t\n' ': ')"
        echo "     release/ danach: $(cd "$R" && ls ${NAME}_* 2>/dev/null | tr '\n' ' ')"
        git -C "$S" reset -q
    }
    # B1: veralteter Satz der ANDEREN PC-Plattform (win64 aus dem Archiv v0.8.19), Quellbaum seitdem geaendert
    for f in ${NAME}_win64.z01 ${NAME}_win64.zip; do cp -p "$ARCH/$f" "$R/$f"; done
    T="$S/re15_port/shared_assets/PSX/DATA/TEX.TIM"
    cp -p "$T" "$T.orig_link_kopie"; rm -f "$T"; cp -p "$T.orig_link_kopie" "$T"; rm -f "$T.orig_link_kopie"
    "$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[1000]^=1; open(p,'wb').write(d)" "$T"
    echo "   Sandbox-Quellbaum: PSX/DATA/TEX.TIM 1 Byte gekippt (eigene Kopie, Hardlink aufgeloest): $(sha256sum "$T" | cut -c1-16)..."
    mp B1_win_alt_linux_neu
    for p in linux_steamdeck_x64 win64; do
        if [[ -f "$R/${NAME}_$p.zip" ]]; then
            rm -rf "$W/satz_$p"; mkdir -p "$W/satz_$p"
            /c/msys64/usr/bin/zip -q -s 0 "$(cygpath -m "$R/${NAME}_$p.zip")" --out "$(cygpath -m "$W/satz_$p/ganz.zip")" \
                && echo "     TEX.TIM im Satz $p: $(/c/msys64/usr/bin/unzip -p "$(cygpath -m "$W/satz_$p/ganz.zip")" "$NAME/shared_assets/PSX/DATA/TEX.TIM" | sha256sum | cut -c1-16)..."
            rm -rf "$W/satz_$p"
        fi
    done
    echo "     TEX.TIM Quellbaum jetzt: $(sha256sum "$T" | cut -c1-16)..., Arbeitsbaum: $(sha256sum re15_port/shared_assets/PSX/DATA/TEX.TIM | cut -c1-16)..."
    # B2/B3: alter Android-Satz derselben Version, keine APK, --zip-only (+ --ohne-android)
    for f in ${NAME}_android.z01 ${NAME}_android.zip; do cp -p "$ARCH/$f" "$R/$f"; done
    mp B2_android_alt_zip_only --zip-only
    mp B3_android_alt_ohne_android --zip-only --ohne-android
    # B4: fremde Namen
    printf 'x' > "$R/${NAME}_Android.zip"
    mp B4a_fremd_Android_gross --zip-only
    rm -f "$R/${NAME}_Android.zip"
    printf 'x' > "$R/${NAME}_android.zip.bak"
    mp B4b_fremd_zip_bak --zip-only --ohne-android
    rm -f "$R/${NAME}_android.zip.bak"
    # aufraeumen: TEX.TIM wieder als Hardlink, Saetze weg
    rm -f "$T"; ln "re15_port/shared_assets/PSX/DATA/TEX.TIM" "$T"
    cmp -s "$T" re15_port/shared_assets/PSX/DATA/TEX.TIM && echo "   TEX.TIM wieder Hardlink = Arbeitsbaum"
    rm -f "$R"/${NAME}_*.z* "$R/SHA256SUMS.txt"
    sandbox_echt
fi
echo "Temp-Reste: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_pruefen.* "${TMPDIR:-/tmp}"/re15_make_package.* "${TMPDIR:-/tmp}"/re15_apk_satz.* "${TMPDIR:-/tmp}"/re15_gate_ausgabe.* 2>/dev/null | wc -l)"
