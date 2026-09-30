#!/usr/bin/env bash
# Nachbesserung R4-1: die ganze Kette mit den NEUEN Skripten in der Sandbox build/r34a/nb/sb (angelegt mit
# android_gate_r4_belege/r4_sandbox_anlegen.sh; Quellbaum per Hardlink, eigenes git-Repo). release/ und der Index des
# Arbeitsbaums bleiben unberuehrt. Je Lauf Rueckgabe selbst abgefangen, kein Pipe um den Aufruf.
#   A = build_android.sh --gate-only, B = make_package.sh --only linux
# Gates: ECHT (Arbeitsbaum, Pin echt), Mutant mit echtem Pin (erste Schicht: Pin), Mutant UMGEPINNT (zweite Schicht:
# Urteil aus der Ausgabe). Aufruf: nb_kette.sh [A] [B]
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python; W=build/r34a/nb; S=$W/sb; L=$W/logs/kette; M=$W/mut3; F=$W/faelsch
REF=build/r34a/ref_v0.8.19.apk
NB1=$W/apk/NB1.apk
TEILE="${*:-A B}"
mkdir -p "$L" "$M"
[[ -d "$S" ]] || { echo "Sandbox fehlt: $S"; exit 9; }
for f in make_package.sh apk_pruefen.sh python_finden.sh apk_asset_gate.py apk_asset_gate.sha256 zip_exec_bit.py \
         apk_signer.sha256 build_android.sh; do
    cp -p "release/$f" "$S/release/$f"; cmp -s "release/$f" "$S/release/$f" || { echo "Kopie weicht ab: $f"; exit 9; }
done
cp -p re15_port/platform/android/app/build.gradle "$S/re15_port/platform/android/app/build.gradle"
echo "Sandbox-Skripte = Arbeitsbaum (Gate-Pin $(tail -1 release/apk_asset_gate.sha256 | tr -d '\r' | cut -c1-16)...)"

# Mutanten des NEUEN Gates
G=release/apk_asset_gate.py
: > "$M/G0.py"
"$PY" - "$G" "$M" <<'PYEOF'
import sys
g, m = sys.argv[1], sys.argv[2]
t = open(g, "rb").read().decode("utf-8")
def ers(name, alt, neu):
    assert t.count(alt) == 1, (name, t.count(alt))
    open(m + "/" + name + ".py", "wb").write(t.replace(alt, neu).encode("utf-8"))
ers("G1", 'if __name__ == "__main__":\n    sys.exit(main())', 'if __name__ == "__main__":\n    main()')
ers("G2", "RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2", "RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 0, 2")
ers("G3", "        return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER", "        return RC_GLEICH")
ers("V1", '        z = z.rstrip(b"\\r")\n        if not z:\n', '        z = z.rstrip(b"\\r")\n        if not z.strip():\n')
i = t.index("\n# =============================================================================================\ndef main(")
open(m + "/G4.py", "wb").write(t[:i + 1].encode("utf-8"))
print("Mutanten G1 G2 G3 G4 V1 erzeugt")
PYEOF
[[ -f "$M/ALT.py" ]] || git show be8b60f3:release/apk_asset_gate.py > "$M/ALT.py"

pin_echt() { cp -p release/apk_asset_gate.sha256 "$S/release/apk_asset_gate.sha256"; }
gate_setzen() {   # $1 = ECHT oder Datei, $2 = echt|umpinnen
    if [[ "$1" == ECHT ]]; then cp -p "$G" "$S/release/apk_asset_gate.py"; else cp "$1" "$S/release/apk_asset_gate.py"; fi
    if [[ "$2" == umpinnen ]]; then
        "$PY" -c "import hashlib,sys; print(hashlib.sha256(open(sys.argv[1],'rb').read()).hexdigest())" "$S/release/apk_asset_gate.py" \
            > "$S/release/apk_asset_gate.sha256"
    else
        pin_echt
    fi
    echo "--- Sandbox-Gate := $(basename "$1") ($(wc -c < "$S/release/apk_asset_gate.py") B), Pin: $2"
}
zeige() {
    grep -a -E 'ANDROID-GATES-OK|APK-PRUEFUNG-OK|^ABBRUCH|Gate-Urteil|IGNORIERT|NICHT das festgehaltene|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE(-PAKET|-QUELLBAUM)?-(OK|ABWEICHUNG)|Manifest im alten|Nicht-ASCII|Inhalt weicht ab|SHA256SUMS.txt geschrieben|neue vorgemerkt|== Fertig|fremde Datei|andere Plattform|Satz re15' \
        "$1" | tr -d '\r' | sed 's/^/     /' | cut -c1-230
}

if [[ " $TEILE " == *" A "* ]]; then
    ga() {   # $1 = Tag, $2 = APK, $3 = Version, Rest = env-Zuweisungen
        local tag="$1" apk="$2" ver="$3" rc=0 t0; shift 3; t0=$(date +%s)
        env "$@" bash "$S/release/build_android.sh" --gate-only "$apk" --version "$ver" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== A $tag ($(basename "$apk") --version $ver${*:+, env $*}): EXIT=$rc ($(( $(date +%s) - t0 )) s)"
        zeige "$L/$tag.log"
    }
    gate_setzen ECHT echt
    ga A0_echt_NB1   "$NB1" v0.8.20-nb1
    ga A1_echt_REF   "$REF" v0.8.19
    ga A8_echt_FD    "$F/FD_sig.apk" v0.8.20-nb1
    ga A11_echt_FK   "$F/FK_sig.apk" v0.8.20-nb1
    ga A6_echt_FW    "$F/FW_sig.apk" v0.8.20-nb1
    gate_setzen "$M/G0.py" echt;      ga A2_G0_pin_REF  "$REF" v0.8.19
    gate_setzen "$M/G0.py" umpinnen;  ga A2b_G0_um_REF  "$REF" v0.8.19
    gate_setzen "$M/G4.py" umpinnen;  ga A2c_G4_um_REF  "$REF" v0.8.19
    gate_setzen "$M/G1.py" umpinnen;  ga A3_G1_um_REF   "$REF" v0.8.19
    gate_setzen "$M/G1.py" umpinnen;  ga A3b_G1_um_FD   "$F/FD_sig.apk" v0.8.20-nb1
    gate_setzen "$M/G2.py" umpinnen;  ga A10_G2_um_FD   "$F/FD_sig.apk" v0.8.20-nb1
    gate_setzen "$M/G3.py" umpinnen;  ga A13_G3_um_FD   "$F/FD_sig.apk" v0.8.20-nb1
    gate_setzen "$M/V1.py" umpinnen;  ga A7_V1_um_FW    "$F/FW_sig.apk" v0.8.20-nb1
    gate_setzen ECHT echt
    ga A4_env_G0_REF  "$REF" v0.8.19 APK_GATE_DATEI="$(cygpath -m "$PWD/$M/G0.py")"
    ga A5_env_ALT_REF "$REF" v0.8.19 APK_GATE_DATEI="$(cygpath -m "$PWD/$M/ALT.py")"
    # Kelvin-Paar auch im Quellbaum der Sandbox (NTFS haelt beide Namen) - danach wieder weg
    K1="$S/re15_port/shared_assets/PSX/K.bin"
    K2="$("$PY" -c "import sys; print(sys.argv[1] + '/re15_port/shared_assets/PSX/' + chr(0x212A) + '.bin')" "$S")"
    printf 'AAAAA' > "$K1"; "$PY" -c "import sys; open(sys.argv[1],'wb').write(b'BBBBB')" "$K2"
    ga A12_echt_FK_quelle "$F/FK_sig.apk" v0.8.20-nb1
    rm -f "$K1"; "$PY" -c "import os,sys; os.remove(sys.argv[1])" "$K2"
    ls "$S/re15_port/shared_assets/PSX/" | grep -c -i '\.bin$' | sed 's/^/     .bin in Sandbox-PSX danach: /'
    cmp -s "$G" "$S/release/apk_asset_gate.py" && cmp -s release/apk_asset_gate.sha256 "$S/release/apk_asset_gate.sha256" \
        && echo "   Sandbox-Gate + Pin = Arbeitsbaum"
    echo "Pruefkopien uebrig: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_pruefen.* 2>/dev/null | wc -l)"
fi

if [[ " $TEILE " == *" B "* ]]; then
    NAME=re15_port_v0.8.19
    mp() {
        local tag="$1" rc=0 t0; shift; t0=$(date +%s)
        rm -f "$S/release/SHA256SUMS.txt"
        bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== B $tag ($*): EXIT=$rc ($(( $(date +%s) - t0 )) s)"
        zeige "$L/$tag.log"
        if [[ -f "$S/release/SHA256SUMS.txt" ]]; then
            echo "     SHA256SUMS.txt: $(tr -d '\r' < "$S/release/SHA256SUMS.txt" | awk '{print $2}' | tr '\n' ' ')"
        else
            echo "     SHA256SUMS.txt: (keine)"
        fi
        echo "     git-Index (Sandbox): $(git -C "$S" diff --cached --name-status | tr '\t\n' ': ')"
        git -C "$S" reset -q
    }
    gate_setzen ECHT echt
    mp B0_no_zip --no-zip
    T="$S/release/pkg-linux/$NAME/shared_assets/PSX/DATA/TEX.TIM"
    [[ -f "$T" && "$(stat -c %h "$T")" == 1 ]] || { echo "Paket-TEX.TIM fehlt oder ist ein Link: $T"; exit 1; }
    cp -p "$T" "$L/TEX.TIM.orig"
    "$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$T"
    echo "Paket-TEX.TIM veraendert: $(sha256sum "$T" | cut -c1-16)..., Quelle $(sha256sum re15_port/shared_assets/PSX/DATA/TEX.TIM | cut -c1-16)..."
    mp B1_echt_paket_kaputt --zip-only
    gate_setzen "$M/G0.py" echt;     mp B2_G0_pin_paket_kaputt --zip-only
    gate_setzen "$M/G0.py" umpinnen; mp B2b_G0_um_paket_kaputt --zip-only
    gate_setzen "$M/G1.py" umpinnen; mp B3_G1_um_paket_kaputt --zip-only
    gate_setzen ECHT echt
    cp -p "$L/TEX.TIM.orig" "$T" && echo "Paket-TEX.TIM zurueck: $(sha256sum "$T" | cut -c1-16)..."
    "$PY" - "$S/release" "$NAME" <<'PYEOF'
import sys, zipfile, os
d, name = sys.argv[1], sys.argv[2]
for fn in (name + "_ANDROID.zip", name + "_android.apk.zip"):
    with zipfile.ZipFile(os.path.join(d, fn), "w") as z:
        z.writestr(name + "_android.apk", b"ALTER UNGEPRUEFTER INHALT - kein APK\n")
    print("   angelegt: %s (%d B)" % (fn, os.path.getsize(os.path.join(d, fn))))
PYEOF
    mp B4_fremdname --zip-only
    mp B5_fremdname_ohne_android --zip-only --ohne-android
    rm -f "$S/release/${NAME}_ANDROID.zip" "$S/release/${NAME}_android.apk.zip"
    mp B6_kontrolle --zip-only
    # andere PC-Plattform derselben Version aus einem "frueheren Lauf" (ein Volume, gueltiger Katalog)
    "$PY" - "$S/release" "$NAME" <<'PYEOF'
import sys, zipfile, os
d, name = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(os.path.join(d, name + "_win64.zip"), "w") as z:
    z.writestr(name + "/re15_pc.exe", b"MZ frueherer Lauf")
print("   angelegt: %s_win64.zip" % name)
PYEOF
    mp B7_andere_plattform --zip-only
    rm -f "$S/release/${NAME}_win64.zip"
    cp -p "$S/release/${NAME}_linux_steamdeck_x64.z01" "$S/release/${NAME}_win64.z01"
    mp B8_andere_plattform_ohne_zip --zip-only
    rm -f "$S/release/${NAME}_win64.z01"
    cmp -s "$G" "$S/release/apk_asset_gate.py" && cmp -s release/apk_asset_gate.sha256 "$S/release/apk_asset_gate.sha256" \
        && echo "   Sandbox-Gate + Pin = Arbeitsbaum"
    echo "Temp-Reste: $(ls -d "${TMPDIR:-/tmp}"/re15_make_package.* "${TMPDIR:-/tmp}"/re15_gate_ausgabe.* 2>/dev/null | wc -l)"
fi
echo FERTIG
