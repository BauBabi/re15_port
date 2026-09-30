#!/usr/bin/env bash
# Nachbesserung R4-1: Befunde H1/H2/H3/H5 VOR dem Fix selbst nachmessen (Stand HEAD, Werkzeuge der Pruefer
# u1_mutanten.py + r4_sandbox_anlegen.sh). Eigene Sandbox build/r34a/nb/sb; release/ und der Index des
# Arbeitsbaums bleiben unberuehrt. Rueckgabe je Lauf selbst abgefangen, kein Pipe um einen Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python; W=build/r34a/nb; L=$W/logs/repro; S=$W/sb; M=$W/mut
U=analysis/befunde_runde34_android/pruefer_umgehung_r4_1_belege
R4=analysis/befunde_runde34_android/android_gate_r4_belege
REF=build/r34a/ref_v0.8.19.apk
mkdir -p "$L"
TEILE="${*:-M S A B H5}"
zeige() { grep -a -E 'ANDROID-GATES-OK|APK-PRUEFUNG-OK|^ABBRUCH|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)|Manifest im alten|QUELLBAUM-OK|PAKET-(OK|ABWEICHUNG)|SHA256SUMS.txt geschrieben|neue vorgemerkt|== Fertig|kein Android|ohne-android|Gate:' "$1" | tr -d '\r' | sed 's/^/     /' | cut -c1-200; }
if [[ " $TEILE " == *" M "* ]]; then
    "$PY" $U/u1_mutanten.py release/apk_asset_gate.py "$M" G0_leer G1_kein_sys_exit V1_leerzeile_strip V2_steuer_1f
    for n in G0_leer G1_kein_sys_exit; do
        rc=0; t0=$(date +%s); "$PY" "$M/$n/apk_asset_gate.py" --selbsttest > "$L/selbst_$n.log" 2>&1 || rc=$?
        echo "== Selbsttest $n: EXIT=$rc ($(( $(date +%s) - t0 )) s), Ausgabe $(wc -c < "$L/selbst_$n.log") B"; zeige "$L/selbst_$n.log"
    done
fi
if [[ " $TEILE " == *" S "* && ! -d "$S" ]]; then
    bash $R4/r4_sandbox_anlegen.sh "$S" > "$L/sandbox.txt" 2>&1 || { echo "Sandbox fehlgeschlagen"; cat "$L/sandbox.txt"; exit 1; }
    tail -3 "$L/sandbox.txt"
fi
gate_setzen() { if [[ "$1" == ECHT ]]; then cp -p release/apk_asset_gate.py "$S/release/apk_asset_gate.py"; else cp "$1" "$S/release/apk_asset_gate.py"; fi
    echo "--- Sandbox-Gate := $1 ($(wc -c < "$S/release/apk_asset_gate.py") B)"; }
if [[ " $TEILE " == *" A "* ]]; then
    ga() { local tag="$1" apk="$2" ver="$3" rc=0 t0; shift 3; t0=$(date +%s)
        env "$@" bash "$S/release/build_android.sh" --gate-only "$apk" --version "$ver" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== A $tag ($(basename "$apk") --version $ver${*:+, env $*}): EXIT=$rc ($(( $(date +%s) - t0 )) s)"; zeige "$L/$tag.log"; }
    gate_setzen "$M/G0_leer/apk_asset_gate.py"
    ga A2_G0_ref0819 "$REF" v0.8.19
    gate_setzen ECHT
    ga A4_env_G0_ref0819 "$REF" v0.8.19 APK_GATE_DATEI="$(cygpath -m "$PWD/$M/G0_leer/apk_asset_gate.py")"
fi
if [[ " $TEILE " == *" B "* ]]; then
    if [[ ! -f "$W/bin/linux/re15_pc" ]]; then
        ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19; ZIP=/c/msys64/usr/bin/zip; UNZIP=/c/msys64/usr/bin/unzip
        rm -rf "$W/bin"; mkdir -p "$W/bin/satz" "$W/bin/linux"
        for f in re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do cp -p "$ARCH/$f" "$W/bin/satz/$f"; done
        ( cd "$W/bin/satz" && grep -E 'linux_steamdeck' "$ARCH/SHA256SUMS.txt" | sha256sum -c - ) || exit 1
        ( cd "$W/bin/satz" && $ZIP -q -s 0 re15_port_v0.8.19_linux_steamdeck_x64.zip --out linux_ganz.zip && $UNZIP -q -o linux_ganz.zip 're15_port_v0.8.19/re15_pc' -d ex ) || exit 1
        cp -p "$W/bin/satz/ex/re15_port_v0.8.19/re15_pc" "$W/bin/linux/re15_pc"; rm -rf "$W/bin/satz"
        ls -la --time-style=full-iso "$W/bin/linux/re15_pc"
    fi
    mkdir -p "$S/release/linux_out"; cp -p "$W/bin/linux/re15_pc" "$S/release/linux_out/re15_pc"
    NAME=re15_port_v0.8.19
    mp() { local tag="$1" rc=0 t0; shift; t0=$(date +%s)
        bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
        echo "== B $tag ($*): EXIT=$rc ($(( $(date +%s) - t0 )) s)"; zeige "$L/$tag.log"
        [[ -f "$S/release/SHA256SUMS.txt" ]] && echo "     SHA256SUMS.txt: $(tr -d '\r' < "$S/release/SHA256SUMS.txt" | awk '{print $2}' | tr '\n' ' ')"
        echo "     git-Index (Sandbox): $(git -C "$S" diff --cached --name-status | tr '\t\n' ': ')"; }
    gate_setzen ECHT
    [[ -d "$S/release/pkg-linux/$NAME" ]] || mp B0_no_zip --no-zip
    "$PY" - "$S/release" "$NAME" <<'PYEOF'
import sys, zipfile, os
d, name = sys.argv[1], sys.argv[2]
for fn in (name + "_ANDROID.zip", name + "_android.apk.zip"):
    with zipfile.ZipFile(os.path.join(d, fn), "w") as z:
        z.writestr(name + "_android.apk", b"ALTER UNGEPRUEFTER INHALT - kein APK\n")
    print("   angelegt: %s (%d B)" % (fn, os.path.getsize(os.path.join(d, fn))))
PYEOF
    mp B4_fremdname_ohne_apk --zip-only
    git -C "$S" reset -q
    mp B5_fremdname_ohne_android --zip-only --ohne-android
    git -C "$S" reset -q
    rm -f "$S/release/${NAME}_ANDROID.zip" "$S/release/${NAME}_android.apk.zip" "$S/release/SHA256SUMS.txt" "$S/release/${NAME}"_linux_steamdeck_x64.z*
fi
if [[ " $TEILE " == *" H5 "* ]]; then
    "$PY" - <<'PYEOF'
import importlib.util, hashlib
spec = importlib.util.spec_from_file_location("gate", "release/apk_asset_gate.py"); g = importlib.util.module_from_spec(spec); spec.loader.exec_module(g)
a, b = b"AAAAA", b"BBBBB"
for titel, p1, p2 in (("Kelvin/K", "shared_assets/PSX/K.bin", "shared_assets/PSX/\u212a.bin"),
                      ("NFC/NFD", "shared_assets/PSX/\u00e9.bin", "shared_assets/PSX/e\u0301.bin"),
                      ("sz/ss", "shared_assets/PSX/stra\u00dfe.bin", "shared_assets/PSX/strasse.bin"),
                      ("Ae/ae", "shared_assets/PSX/\u00c4.bin", "shared_assets/PSX/\u00e4.bin")):
    zeilen = sorted([(p1.encode(), a), (p2.encode(), b)])
    roh = b"# re15 assets v2 2 10\n" + b"".join(b"5\t%s\t%s\n" % (hashlib.sha256(d).hexdigest().encode(), p) for p, d in zeilen)
    e, _z, v1, fehler = g.manifest_lesen(roh)
    print("   H5 %-9s Gate manifest_lesen: %d Eintraege, Fehler %r -> %s" % (titel, len(e), fehler, "ANGENOMMEN" if not fehler else "abgelehnt"))
PYEOF
fi
echo FERTIG
