#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1, B1-Varianten mit echter APK in der Sandbox (nach u1_kette.sh B: pkg-linux liegt, Gate echt):
#   P1  APK (Kopie des frischen v0.8.19-Baus mit Liste v2) da, --zip-only       -> soll EXIT 0, Satz + SUMS; danach
#       Release-Commit in der Sandbox (Satz versioniert, wie nach einer Auslieferung)
#   P2  wie P1, aber die APK wird NACH "APK-PRUEFUNG-OK" geloescht (Waechter) -> soll EXIT 1 "verschwand", SUMS/Index unveraendert
#   P3  ohne APK, alter Satz liegt (versioniert), --zip-only                    -> soll EXIT 1 (B1-Abbruch)
# Rueckgabe je Lauf selbst abgefangen.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_u1; S=$W/sb; L=$W/logs/kette; A=$W/apk; NAME=re15_port_v0.8.19
APKZ="$S/release/${NAME}_android.apk"
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" || { echo "Sandbox-Gate nicht echt"; exit 1; }
[[ -d "$S/release/pkg-linux/$NAME" ]] || { echo "pkg-linux fehlt (u1_kette.sh B zuerst)"; exit 1; }
zeige() { grep -a -E '^ABBRUCH|APK-PRUEFUNG-OK|SELBSTTEST-(OK|FEHLER)|APK im Split-Satz|verschwand|SHA256SUMS.txt geschrieben|neue vorgemerkt|== Fertig|kein Android' "$1" \
          | tr -d '\r' | sed 's/^/     /' | cut -c1-190; }
stand() { echo "     release/*android*: $(ls "$S/release" | grep -i android | tr '\n' ' ')"
          [[ -f "$S/release/SHA256SUMS.txt" ]] && echo "     SHA256SUMS.txt: $(tr -d '\r' < "$S/release/SHA256SUMS.txt" | awk '{print $2}' | tr '\n' ' ') (sha $(sha256sum "$S/release/SHA256SUMS.txt" | cut -c1-12))"
          echo "     git-Index gegen HEAD: $(git -C "$S" diff --cached --name-status | tr '\t\n' ': ')"; }
mp() { local tag="$1" rc=0 t0 t1; shift; t0=$(date +%s)
       bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
       t1=$(date +%s); echo "== $tag ($*): EXIT=$rc ($((t1 - t0)) s)"; zeige "$L/$tag.log"; stand; }
cp -p "$A/V0819neu.apk" "$APKZ"; echo "APK in der Sandbox: $(sha256sum "$APKZ" | cut -c1-16)... ($(stat -c %y "$APKZ"))"
mp P1_mit_apk --zip-only
git -C "$S" add -A release/ >/dev/null 2>&1; git -C "$S" -c core.autocrlf=false commit -q -m "Release-Commit (Sandbox) nach P1" && echo "   Sandbox: Release-Commit $(git -C "$S" log -1 --format=%h)"
git -C "$S" ls-files release | grep -E "android|SHA256" | sed 's/^/     versioniert: /'
# P2: Waechter loescht die APK nach der Pruefung
( until grep -a -q "APK-PRUEFUNG-OK" "$L/P2_apk_geloescht.log" 2>/dev/null; do sleep 0.5; done
  rm -f "$APKZ"; echo "   [Waechter $(date +%T)] APK nach APK-PRUEFUNG-OK geloescht" ) &
WPID=$!
: > "$L/P2_apk_geloescht.log"
mp P2_apk_geloescht --zip-only
wait $WPID 2>/dev/null
mp P3_ohne_apk_satz_versioniert --zip-only
echo FERTIG
