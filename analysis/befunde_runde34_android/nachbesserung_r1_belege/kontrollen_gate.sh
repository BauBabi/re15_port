#!/usr/bin/env bash
# Nachbesserung R1: Gate (release/apk_asset_gate.py) direkt gegen Referenz + alle Faelschungen, je Python.
# Rueckgabe je Lauf selbst abgefangen (kein Pipe um das Gate). Aufruf aus dem Baum: bash kontrollen_gate.sh <ausgabe>
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 99
OUT="$1"; D=build/r34a/nb/apk
echo "# Gate $(git rev-parse --short HEAD)+Arbeitsbaum, Quellbaum = dieser Baum, $(date '+%F %T')" > "$OUT"
for P in /c/Python310/python.exe /c/msys64/mingw64/bin/python3.exe; do
  V=$($P -c 'import sys;print(sys.version.split()[0])' </dev/null)
  for a in ../../ref_v0.8.19 K0_neu F1_crc_koll F1_crc_koll_sig F2_backslash F2_signiert F3_nul F4_lfh_crc F5_lfh_usize \
           F6_case F6_case_sig F7_abgeschnitten F8_cd_sig N1_lfh_name N2_utf8 N2_utf8_sig N3_verschl N3_verschl_sig N4_meth99 N5_weniger; do
    rc=0; o=$($P release/apk_asset_gate.py --repo . "$D/$a.apk" 2>&1) || rc=$?
    erst="$(grep -E '^      ' <<<"$o" | head -2 | cut -c7-190 | tr '\n' '#')"
    [[ -n "$erst" ]] || erst="$(grep -E 'ABBRUCH' <<<"$o" | head -1 | cut -c1-190)"
    printf '%-8s %-18s EXIT=%d | %s | %s\n' "$V" "$(basename "$a")" "$rc" "$(grep -E '^== APK' <<<"$o" | tail -1)" "$erst" >> "$OUT"
  done
done
echo "ENDE $(date '+%F %T')" >> "$OUT"
