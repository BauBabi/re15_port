#!/usr/bin/env bash
# make_package.sh, APK-Block (Zeilen 439-468, per awk UNVERAENDERT ausgeschnitten) + Zipzeit-Vergleich
# (Zeilen 662-663) in einer Sandbox: HERE = Sandbox (Kopien von apk_pruefen.sh + apk_asset_gate.py),
# REPO = echter Baum. Die APK unter dem Auslieferungsnamen wird WAEHREND des Gate-Selbsttests gegen
# K0 (unsigniert, Assets gleich) getauscht. Frage: welche Datei haelt APK_KENNUNG fest?
set -euo pipefail
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=build/r34a/pruefer_umgehung_r2
MP=release/make_package.sh
die() { echo "ABBRUCH: $*" >&2; exit 1; }
HERE="$(pwd)/$W/mp_sandbox/release"; REPO="$(pwd)"; VERSION=v0.8.19; NAME="re15_port_${VERSION}"; DO_ZIP=1
mkdir -p "$HERE"; cp -f release/apk_pruefen.sh release/apk_asset_gate.py "$HERE/"
cp -f $W/ref.apk "$HERE/${NAME}_android.apk"
source release/python_finden.sh
# Frische ist nicht Gegenstand dieser Sonde: in der Sandbox (HERE/.. = build/...) findet git log fuer
# re15_port/engine... keinen Commit; dann kehrt die echte Funktion mit '[[ -n "$src_t" ]] || return' = 1
# zurueck und set -e beendet die Sonde STUMM (so im ersten Lauf, 23:44-23:51, geschehen). Deshalb ein Stub:
check_binary_fresh() { echo "   (Sonde: Frische-Pruefung uebersprungen - nicht Gegenstand)"; }
BLOCK="$(awk 'NR>=439 && NR<=468' $MP)"
[[ "$(head -1 <<<"$BLOCK")" == 'source "$HERE/apk_pruefen.sh"' && "$(tail -1 <<<"$BLOCK")" == "fi" ]] || die "Block-Grenzen stimmen nicht"
LOG=$W/logs/mp_toctou_block.log
( for i in $(seq 1 900); do
      if grep -q "Volle Asset-Pruefung 1/2" $LOG 2>/dev/null; then
          cp -f $W/apk/K0.apk "$HERE/${NAME}_android.apk.neu" && mv -f "$HERE/${NAME}_android.apk.neu" "$HERE/${NAME}_android.apk"
          echo "$(date +%T) getauscht (waehrend des Selbsttests), Zeile $(wc -l < $LOG) des Block-Logs" > $W/logs/mp_toctou_tausch.txt; break
      fi; sleep 0.2
  done ) &
eval "$BLOCK" > $LOG 2>&1
wait
cat $W/logs/mp_toctou_tausch.txt; cat $LOG | grep -E "Frische|Verified using v2|SELBSTTEST|GATE-OK|APK-PRUEFUNG-OK|gepruefte APK|getauscht|ABBRUCH"
# Zipzeit (make_package.sh:662-663, unveraendert als Vergleich):
APK="$HERE/${NAME}_android.apk"
apk_jetzt="$(apk_kennung "$APK")"
[[ "$apk_jetzt" == "$APK_KENNUNG" ]] && echo "Zipzeit-Vergleich: = gepruefte Datei -> wuerde gezippt"
echo "K0 (unsigniert):  $(sha256sum $W/apk/K0.apk | cut -c1-64)"
echo "ref (signiert):   $(sha256sum $W/ref.apk | cut -c1-64)"
