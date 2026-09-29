#!/usr/bin/env bash
# Nachbesserung R1: die VOLLE Kette (release/build_android.sh --gate-only -> release/apk_pruefen.sh) gegen
# Referenz + Faelschungen; Rueckgabe je Lauf selbst abgefangen, volle Logs unter build/r34a/nb/kette/.
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 99
OUT="$1"; D=build/r34a/nb/apk; L=build/r34a/nb/kette; mkdir -p "$L"
echo "# build_android.sh --gate-only (Kette apk_pruefen.sh), Stand $(git rev-parse --short HEAD)+Arbeitsbaum, $(date '+%F %T')" > "$OUT"
lauf() {  # $1 = Name, Rest = Argumente/Umgebung per env
    local name="$1"; shift
    local rc=0 t0 t1
    t0=$(date +%s)
    "$@" > "$L/$name.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-26s EXIT=%d (%3d s) | %s\n' "$name" "$rc" "$((t1 - t0))" \
        "$(grep -E '^ABBRUCH|ANDROID-GATES-OK|^--gate-only braucht|DOES NOT VERIFY|unzulaessiger|Inhalt weicht|verschluesselter|fehlt in der APK|versionName ist nicht' "$L/$name.log" | head -3 | cut -c1-150 | tr '\n' '#')" >> "$OUT"
}
lauf ref_ok            bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v0.8.19
lauf ref_falsche_version bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v9.9.9
lauf ref_ohne_version  bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk
lauf B4_sdk_fehlt      env ANDROID_SDK_ROOT=/c/gibt_es_nicht bash release/build_android.sh --gate-only build/r34a/ref_v0.8.19.apk --version v9.9.9
for a in K0_neu F4_lfh_crc F2_signiert F1_crc_koll_sig F6_case_sig N2_utf8_sig N3_verschl_sig; do
    lauf "$a" bash release/build_android.sh --gate-only "$D/$a.apk" --version v0.8.19
done
echo "ENDE $(date '+%F %T')" >> "$OUT"
