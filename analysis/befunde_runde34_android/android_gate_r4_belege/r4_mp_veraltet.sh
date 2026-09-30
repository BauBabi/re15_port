#!/usr/bin/env bash
# Runde 4 (Kette B1): Negativ-Kontrolle wie pruefer_umgehung_r3_belege/r3_mp_veraltet.sh - make_package.sh ECHT
# in einer Sandbox (r4_sandbox_anlegen.sh: Skripte = Arbeitsbaum, Quellbaum per Hardlink, eigenes git-Repo),
# jeweils --version v0.8.19 --only linux:
#   L1  APK = Referenz v0.8.19 -> EXIT 0, Android-Satz aus der geprueften APK; danach Release-Commit in der
#       Sandbox: der Satz ist VERSIONIERT (wie v0.8.19 im echten Repo)
#   --  Quellbaum aendert sich (1 Byte in extracted_fx/effect0_blood.tim, nur Sandbox, Link vorher geloest)
#   L2  APK noch da -> EXIT 1 (APK weicht vom Quellbaum ab)
#   --  APK weg (abgelehnt bzw. build_android.sh loescht sie vor einem gescheiterten Gradle-Lauf)
#   L3n ohne APK, NEUES Skript  -> soll: ABBRUCH vor den Kopierminuten, SHA256SUMS.txt + Index unberuehrt
#   L3a ohne APK, ALTES Skript (Stand 9d2337e4, von R3 geprueft) -> Kontrolle: EXIT 0 und der alte Satz in SUMS
#       (danach git reset --hard auf den Release-Commit der Sandbox)
#   L4  ohne APK, --ohne-android -> soll: EXIT 0, kein Android-Satz in SHA256SUMS.txt, Satzdateien weg, im
#       Index als geloescht vorgemerkt, nur Linux-Volumes neu vorgemerkt
#   L5  Satz UNVERSIONIERT, aber vorgemerkt (wie nach L1 ohne Commit) + --ohne-android --zip-only -> Satz weg,
#       nicht mehr im Index
#   L6  Satz unversioniert, nicht vorgemerkt + --no-zip -> EXIT 0, Satz bleibt liegen, wird NICHT vorgemerkt
# Rueckgabe je Lauf selbst abgefangen (kein Pipe um den Aufruf). Nur /c/Python310/python direkt.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; S=$W/sb_mp; L=$W/logs/mp; mkdir -p "$L"
PY=/c/Python310/python; FX=re15_port/shared_assets/extracted_fx/effect0_blood.tim; N=re15_port_v0.8.19
rm -rf "$S"; bash $B/r4_sandbox_anlegen.sh "$S" || { echo "Sandbox fehlgeschlagen"; exit 1; }
mkdir -p "$S/release/linux_out"
cp -p $W/bin/linux/re15_pc "$S/release/linux_out/re15_pc"
stand() {   # $1 = Etikett
    echo "--- Stand $1 ---"
    ( cd "$S/release" && ls -la --time-style=+%T re15_port_v0*.z* re15_port_v0*.apk 2>/dev/null | awk '{print "   " $6, $5, $7}' )
    echo "   SHA256SUMS.txt:"; sed 's/^/      /' "$S/release/SHA256SUMS.txt" 2>/dev/null || echo "      (fehlt)"
    echo "   Index gegen HEAD (Sandbox):"; git -C "$S" diff --cached --name-status | sed 's/^/      /'
    echo "   versioniert: $(git -C "$S" ls-files 'release/re15_port_v0*' | tr '\n' ' ')"
}
mp() {      # $1 = Tag, Rest = Zusatzargumente
    local tag="$1" rc=0 t0 t1; shift
    t0=$(date +%s)
    bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
    t1=$(date +%s)
    echo "== $tag ($*): EXIT=$rc ($((t1 - t0)) s)"
    grep -a -E '^ABBRUCH|Android-Satz|kein Android-Paket|--ohne-android|Zippen: |APK im Split-Satz|APK-PRUEFUNG-OK|QUELLBAUM-OK|SELBSTTEST-(OK|FEHLER)|PAKET-OK|SHA256SUMS.txt geschrieben|== Fertig|neue vorgemerkt|Assets kopieren' \
        "$L/$tag.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-200
}
echo "Sandbox: $(git -C "$S" log -1 --format='%h %ci')"

cp -p build/r34a/ref_v0.8.19.apk "$S/release/$N"_android.apk
mp L1_mit_apk
stand "nach L1"
git -C "$S" add release/SHA256SUMS.txt
GIT_AUTHOR_DATE="2026-09-29T12:30:00+0200" GIT_COMMITTER_DATE="2026-09-29T12:30:00+0200" \
    git -C "$S" commit -q -m "Release v0.8.19 (Sandbox): Linux + Android" && echo "--- Release-Commit in der Sandbox: $(git -C "$S" log -1 --format='%h')"
sha256sum "$S/release/$N"_android.z01 "$S/release/$N"_android.zip "$S/release/SHA256SUMS.txt" > "$L/nach_L1.sha256"
mkdir -p "$W/satz_L1"; cp -p "$S/release/$N"_android.z* "$W/satz_L1/"

orig="$(sha256sum "$FX" | cut -d' ' -f1)"
cp "$FX" "$S/$FX.neu"
"$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$S/$FX.neu"
mv -f "$S/$FX.neu" "$S/$FX"
echo "--- Quellbaum-Aenderung: $FX in der Sandbox $(sha256sum "$S/$FX" | cut -c1-16)..., Original im Arbeitsbaum" \
     "$( [[ "$(sha256sum "$FX" | cut -d' ' -f1)" == "$orig" ]] && echo unveraendert || echo VERAENDERT)"

mp L2_apk_veraltet
rm -f "$S/release/$N"_android.apk
echo "--- APK aus release/ der Sandbox entfernt"

mp L3n_ohne_apk_neu
stand "nach L3n"
echo "--- SUMS + Satz gegen den Release-Commit:"; sha256sum -c "$L/nach_L1.sha256" | sed 's/^/   /'

cp -p $W/alt/make_package.sh "$S/release/make_package.sh"
cp -p $W/alt/apk_pruefen.sh "$S/release/apk_pruefen.sh"
mp L3a_ohne_apk_ALTES_skript
stand "nach L3a (altes Skript)"
git -C "$S" reset -q --hard && echo "--- Sandbox zurueck auf den Release-Commit (reset --hard; Quellbaum/APK sind gitignoriert)"
for f in make_package.sh apk_pruefen.sh; do cmp -s "release/$f" "$S/release/$f" && echo "   $f = Arbeitsbaum" || echo "   $f ANDERS"; done

mp L4_ohne_android --ohne-android
stand "nach L4"

git -C "$S" reset -q --hard
git -C "$S" rm -q --cached "release/$N"_android.z01 "release/$N"_android.zip
GIT_AUTHOR_DATE="2026-09-29T12:40:00+0200" GIT_COMMITTER_DATE="2026-09-29T12:40:00+0200" \
    git -C "$S" commit -q -m "Sandbox: Android-Satz unversioniert" && echo "--- Satz unversioniert (Datei bleibt), dann wieder vorgemerkt:"
git -C "$S" add -f "release/$N"_android.z01 "release/$N"_android.zip
stand "vor L5"
mp L5_vorgemerkt_ohne_android_zip_only --ohne-android --zip-only
stand "nach L5"

cp -p "$W/satz_L1/"* "$S/release/"
git -C "$S" reset -q
stand "vor L6 (Satz unversioniert, nicht vorgemerkt)"
mp L6_no_zip --no-zip
stand "nach L6"
echo FERTIG
