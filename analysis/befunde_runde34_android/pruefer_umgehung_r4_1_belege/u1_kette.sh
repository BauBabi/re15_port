#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1: GANZE Kette mit kaputten Gates (H1), fremdem Gate per Umgebung (H2), Mutant V1 + signierter
# Faelschung (H4) und fremd benannten Android-Dateien (H3). Alles in EIGENER Sandbox (android_gate_r4_belege/
# r4_sandbox_anlegen.sh: Skripte = Kopien des Arbeitsbaums, Quellbaum per Hardlink, eigenes git-Repo) - release/ und
# der git-Index des Arbeitsbaums werden nicht beruehrt. Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf.
# Aufruf: u1_kette.sh [A] [B]     (A = build_android.sh --gate-only, B = make_package.sh)
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_u1; B=analysis/befunde_runde34_android/pruefer_umgehung_r4_1_belege
R4=analysis/befunde_runde34_android/android_gate_r4_belege; B3R=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
PY=/c/Python310/python; S=$W/sb; L=$W/logs/kette; A=$W/apk; M=$W/mut
REF=build/r34a/ref_v0.8.19.apk
TEILE="${*:-A B}"
mkdir -p "$L" "$A"
if [[ ! -d "$S" ]]; then bash $R4/r4_sandbox_anlegen.sh "$S" > "$L/sandbox_anlegen.txt" 2>&1 || { echo "Sandbox fehlgeschlagen"; cat "$L/sandbox_anlegen.txt"; exit 1; }; fi
tail -3 "$L/sandbox_anlegen.txt"
gate_setzen() {   # $1 = Datei oder "ECHT"
    if [[ "$1" == ECHT ]]; then cp -p release/apk_asset_gate.py "$S/release/apk_asset_gate.py"; else cp "$1" "$S/release/apk_asset_gate.py"; fi
    echo "--- Sandbox-Gate := $1 ($(sha256sum "$S/release/apk_asset_gate.py" | cut -c1-16)..., $(wc -c < "$S/release/apk_asset_gate.py") B)"
}
zeige() {   # $1 = Log
    grep -a -E 'ANDROID-GATES-OK|APK-PRUEFUNG-OK|^ABBRUCH|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)|Manifest im alten|Manifest-Zeile|QUELLBAUM-OK|PAKET-(OK|ABWEICHUNG)|Inhalt weicht ab|SHA256SUMS.txt geschrieben|neue vorgemerkt|== Fertig|Volle Asset-Pruefung|Asset-Gate: Selbsttest|kein Android|ohne-android' \
        "$1" | tr -d '\r' | sed 's/^/     /' | cut -c1-200
}

# ------------------------------------------------------------------------------------------ A: --gate-only
if [[ " $TEILE " == *" A "* ]]; then
    [[ -f "$A/N2.apk" ]] || cp -p release/re15_port_v0.8.20-n1f_android.apk "$A/N2.apk"
    echo "N2-Kopie: $(sha256sum "$A/N2.apk" | cut -c1-16)... (release/re15_port_v0.8.20-n1f_android.apk, N1-Endstand)"
    if [[ ! -f "$A/FW_sig.apk" ]]; then
        "$PY" $B3R/r3_faelschen.py "$A/N2.apk" "$A/FW.apk" --manifest-anhang '   \n' > "$L/faelschen_FW.txt" 2>&1 \
            || { echo "FAELSCHEN FEHLGESCHLAGEN"; cat "$L/faelschen_FW.txt"; exit 1; }
        bash $B3R/r3_signieren.sh "$A/FW.apk" "$A/FW_sig.apk" | tee "$L/signieren_FW.txt"; rm -f "$A/FW.apk"
    fi
    "$PY" - "$A/FW_sig.apk" <<'PYEOF'
import sys, zipfile
m = zipfile.ZipFile(sys.argv[1]).read("assets/re15_assets.txt")
print("   FW_sig Manifest: %d B, Kopf %r, letzte 2 Zeilen %r" % (len(m), m.split(b"\n")[0], m.split(b"\n")[-3:]))
PYEOF
    [[ -f "$M/ALT_be8b60f3/apk_asset_gate.py" ]] || { mkdir -p "$M/ALT_be8b60f3"; git show be8b60f3:release/apk_asset_gate.py > "$M/ALT_be8b60f3/apk_asset_gate.py"; }
    ga() {   # $1 = Tag, $2 = APK, $3 = Version, Rest = env-Zuweisungen
        local tag="$1" apk="$2" ver="$3" rc=0 t0 t1; shift 3
        t0=$(date +%s)
        env "$@" bash "$S/release/build_android.sh" --gate-only "$apk" --version "$ver" > "$L/$tag.log" 2>&1 || rc=$?
        t1=$(date +%s)
        echo "== A $tag ($(basename "$apk") --version $ver${*:+, env $*}): EXIT=$rc ($((t1 - t0)) s)"
        zeige "$L/$tag.log"
    }
    gate_setzen ECHT
    ga A0_echt_N2       "$A/N2.apk"     v0.8.20-n1f
    ga A1_echt_ref0819  "$REF"          v0.8.19
    ga A6_echt_FW       "$A/FW_sig.apk" v0.8.20-n1f
    gate_setzen "$M/G0_leer/apk_asset_gate.py"
    ga A2_G0_ref0819    "$REF"          v0.8.19
    gate_setzen "$M/G1_kein_sys_exit/apk_asset_gate.py"
    ga A3_G1_ref0819    "$REF"          v0.8.19
    gate_setzen "$M/V1_leerzeile_strip/apk_asset_gate.py"
    ga A7_V1_FW         "$A/FW_sig.apk" v0.8.20-n1f
    gate_setzen ECHT
    ga A4_env_G0_ref0819  "$REF" v0.8.19 APK_GATE_DATEI="$(cygpath -m "$PWD/$M/G0_leer/apk_asset_gate.py")"
    ga A5_env_ALT_ref0819 "$REF" v0.8.19 APK_GATE_DATEI="$(cygpath -m "$PWD/$M/ALT_be8b60f3/apk_asset_gate.py")"
    cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" && echo "   Sandbox-Gate = Arbeitsbaum"
    echo "Pruefkopien uebrig: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_pruefen.* 2>/dev/null | wc -l)"
fi

# ------------------------------------------------------------------------------------------ B: make_package.sh
if [[ " $TEILE " == *" B "* ]]; then
    mkdir -p "$S/release/linux_out"; cp -p $W/bin/linux/re15_pc "$S/release/linux_out/re15_pc"
    NAME=re15_port_v0.8.19
    mp() {      # $1 = Tag, Rest = Zusatzargumente
        local tag="$1" rc=0 t0 t1; shift
        t0=$(date +%s)
        bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
        t1=$(date +%s)
        echo "== B $tag ($*): EXIT=$rc ($((t1 - t0)) s)"
        zeige "$L/$tag.log"
        if [[ -f "$S/release/SHA256SUMS.txt" ]]; then
            echo "     SHA256SUMS.txt: $(tr -d '\r' < "$S/release/SHA256SUMS.txt" | awk '{print $2}' | tr '\n' ' ')"
        fi
        echo "     git-Index (Sandbox) gegen HEAD: $(git -C "$S" diff --cached --name-status | tr '\t\n' ': ')"
    }
    gate_setzen ECHT
    rm -f "$S/release/SHA256SUMS.txt"
    mp B0_no_zip --no-zip
    T="$S/release/pkg-linux/$NAME/shared_assets/PSX/DATA/TEX.TIM"
    [[ -f "$T" ]] || { echo "Paketordner fehlt: $T"; exit 1; }
    echo "Linkzahl Paket-TEX.TIM: $(stat -c %h "$T") (1 = eigene Kopie)"; [[ "$(stat -c %h "$T")" == 1 ]] || exit 1
    cp -p "$T" "$L/TEX.TIM.orig"
    "$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$T"
    echo "Paket-TEX.TIM jetzt $(sha256sum "$T" | cut -c1-16)..., Quelle $(sha256sum re15_port/shared_assets/PSX/DATA/TEX.TIM | cut -c1-16)..."
    mp B1_echt_paket_kaputt --zip-only
    gate_setzen "$M/G0_leer/apk_asset_gate.py"
    mp B2_G0_paket_kaputt --zip-only
    echo "     Satz enthaelt die veraenderte TEX.TIM? $(/c/msys64/usr/bin/unzip -p "$(cygpath -m "$PWD/$S/release/${NAME}_linux_steamdeck_x64.zip")" 2>/dev/null >/dev/null; echo '(Split-Satz: Pruefung ueber zip -s 0 unten)')"
    rm -rf "$W/satzprobe"; mkdir -p "$W/satzprobe"
    /c/msys64/usr/bin/zip -q -s 0 "$(cygpath -m "$PWD/$S/release/${NAME}_linux_steamdeck_x64.zip")" --out "$(cygpath -m "$PWD/$W/satzprobe/ganz.zip")" \
        && echo "     TEX.TIM im ausgelieferten Satz: $(/c/msys64/usr/bin/unzip -p "$(cygpath -m "$PWD/$W/satzprobe/ganz.zip")" "$NAME/shared_assets/PSX/DATA/TEX.TIM" | sha256sum | cut -c1-16)..."
    rm -rf "$W/satzprobe"
    gate_setzen "$M/G1_kein_sys_exit/apk_asset_gate.py"
    mp B3_G1_paket_kaputt --zip-only
    gate_setzen ECHT
    cp -p "$L/TEX.TIM.orig" "$T" && echo "Paket-TEX.TIM zurueck: $(sha256sum "$T" | cut -c1-16)..."
    # H3: fremd benannte Android-Dateien derselben Version (kein kanonischer ${NAME}_android.z*)
    "$PY" - "$S/release" "$NAME" <<'PYEOF'
import sys, zipfile, os
d, name = sys.argv[1], sys.argv[2]
for fn in (name + "_ANDROID.zip", name + "_android.apk.zip"):
    with zipfile.ZipFile(os.path.join(d, fn), "w") as z:
        z.writestr(name + "_android.apk", b"ALTER UNGEPRUEFTER INHALT - kein APK\n")
    print("   angelegt: %s (%d B)" % (fn, os.path.getsize(os.path.join(d, fn))))
PYEOF
    ls "$S/release/" | grep -i android | sed 's/^/     release\//'
    mp B4_fremdname_ohne_apk --zip-only
    git -C "$S" reset -q
    mp B5_fremdname_ohne_android --zip-only --ohne-android
    git -C "$S" reset -q
    rm -f "$S/release/${NAME}_ANDROID.zip" "$S/release/${NAME}_android.apk.zip"
    cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" && echo "   Sandbox-Gate = Arbeitsbaum"
fi
echo FERTIG
