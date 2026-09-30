#!/usr/bin/env bash
# Runde 4 (Kette): raeumen make_package.sh / apk_pruefen.sh ihre Temp-Ordner ab (cb6617fd)? Vorher blieb die
# Gate-Kopie nach JEDEM erfolgreichen Zip-Lauf liegen (rm aus /c/msys64/usr/bin kennt /tmp als C:/msys64/tmp).
# Gezaehlt werden vor/nach jedem Lauf: /tmp/re15_make_package.*, /tmp/re15_apk_pruefen.*, /tmp/re15_apk_satz.*,
# /tmp/apk_gate_selbsttest_*, C:/msys64/tmp/re15_* und C:/msys64/tmp/apk_gate_selbsttest_*.
#   T1  Sandbox, --only linux, APK = Referenz (Zippen, APK-Kette, Satzpruefung)      -> EXIT 0, keine Reste
#   T2  build_android.sh --gate-only <Referenz> (Arbeitsbaum)                       -> EXIT 0, keine Reste
#   T3  Sandbox, Abbruch NACH der PATH-Ergaenzung (Ordner release/<satz>.z07 laesst "rm -f" scheitern) -> keine Reste
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; S=$W/sb_tmp; L=$W/logs/tmp; mkdir -p "$L"
N=re15_port_v0.8.19
reste() { ls -d /tmp/re15_make_package.* /tmp/re15_apk_pruefen.* /tmp/re15_apk_satz.* /tmp/apk_gate_selbsttest_* \
             C:/msys64/tmp/re15_* C:/msys64/tmp/apk_gate_selbsttest_* 2>/dev/null | wc -l; }
rm -rf "$S"; bash $B/r4_sandbox_anlegen.sh "$S" > "$L/sandbox.txt" 2>&1 || { echo "Sandbox fehlgeschlagen"; exit 1; }
mkdir -p "$S/release/linux_out"; cp -p $W/bin/linux/re15_pc "$S/release/linux_out/re15_pc"
cp -p build/r34a/ref_v0.8.19.apk "$S/release/${N}_android.apk"
lauf() {    # $1 = Tag, Rest = Kommando
    local tag="$1" rc=0 v n; shift
    v=$(reste)
    "$@" > "$L/$tag.log" 2>&1 || rc=$?
    n=$(reste)
    echo "== $tag: EXIT=$rc, Temp-Reste vorher $v, nachher $n $( [[ $n == "$v" ]] && echo '(abgeraeumt)' || echo 'LECK')"
    grep -a -E '^ABBRUCH|APK im Split-Satz|SHA256SUMS.txt geschrieben|ANDROID-GATES-OK|== Fertig|rm: ' "$L/$tag.log" | tr -d '\r' | cut -c1-160 | sed 's/^/   /'
}
lauf T1_zip_mit_apk bash "$S/release/make_package.sh" --version v0.8.19 --only linux
lauf T2_gate_only   bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19
rm -f "$S/release/${N}_android.apk" "$S/release/${N}_android".z*
mkdir -p "$S/release/${N}_linux_steamdeck_x64.z07"
lauf T3_abbruch_nach_path bash "$S/release/make_package.sh" --version v0.8.19 --only linux --zip-only
tail -3 "$L/T3_abbruch_nach_path.log" | tr -d '\r' | cut -c1-160 | sed 's/^/   (T3 Ende) /'
rm -rf "$S"
echo FERTIG
