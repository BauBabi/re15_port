#!/usr/bin/env bash
# Gegenpruefung R3: make_package.sh ECHT (unveraenderte Kopie, Sandbox build/r34a/pruefer_r3/sb_mp) - wird ein
# ALTER Android-Split-Satz derselben Version ungeprueft ausgeliefert, wenn die APK fehlt?
#   Lauf 1  APK (Referenz v0.8.19) da          -> soll EXIT 0, Android-Satz aus der geprueften APK
#   dann    Quellbaum aendert sich (Asset-Fix: 1 Byte in extracted_fx/effect0_blood.tim, NUR in der Sandbox -
#           der Hardlink wird vorher durch eine Kopie ersetzt, das Original bleibt unberuehrt)
#   Lauf 2  APK noch da                        -> soll EXIT 1 (APK passt nicht mehr zum Quellbaum)
#   dann    APK weg (abgelehnte APK entfernt / build_android.sh loescht sie vor Gradle, Gradle scheitert)
#   Lauf 3  ohne APK                           -> ???  (Befund: EXIT 0, alter Android-Satz in SHA256SUMS + git add)
#   Beweis  APK aus dem ausgelieferten Android-Satz gegen den AKTUELLEN Quellbaum -> Gate
# Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf. Nur /c/Python310/python direkt.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; S=$W/sb_mp; L=$W/logs/mp; mkdir -p "$L" "$S/release/linux_out"
PY=/c/Python310/python; FX=re15_port/shared_assets/extracted_fx/effect0_blood.tim
stand() {   # $1 = Etikett
    echo "--- Stand $1 ---"
    ( cd "$S/release" && ls -la --time-style=+%T re15_port_v0*.z* re15_port_v0*.apk 2>/dev/null | awk '{print "   " $6, $5, $7}' )
    echo "   SHA256SUMS.txt:"; sed 's/^/      /' "$S/release/SHA256SUMS.txt" 2>/dev/null || echo "      (fehlt)"
    echo "   git (Sandbox) vorgemerkt:"; git -C "$S" diff --cached --name-only | sed 's/^/      /'
}
mp() {      # $1 = Tag
    local rc=0 t0 t1
    t0=$(date +%s)
    bash "$S/release/make_package.sh" --version v0.8.19 --only linux > "$L/$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    echo "== $1: EXIT=$rc ($((t1 - t0)) s)"
    grep -a -E '^ABBRUCH|kein Android-Paket|Zippen: |APK im Split-Satz|APK-PRUEFUNG-OK|QUELLBAUM-OK|PAKET-OK|== Fertig|neue vorgemerkt' \
        "$L/$1.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-200
}
cp -p $W/bin/re15_port_v0.8.19/re15_pc "$S/release/linux_out/re15_pc"
cp -p build/r34a/ref_v0.8.19.apk "$S/release/re15_port_v0.8.19_android.apk"
echo "Sandbox: $(git -C "$S" log -1 --format='%h %ci'); Skripte = Arbeitsbaum:" \
     "$(for f in make_package.sh apk_pruefen.sh apk_asset_gate.py python_finden.sh; do cmp -s release/$f $S/release/$f && echo -n "$f=gleich " || echo -n "$f=ANDERS "; done)"

mp lauf1_mit_apk
stand "nach Lauf 1"
sha256sum "$S/release/re15_port_v0.8.19_android.z01" "$S/release/re15_port_v0.8.19_android.zip" > "$L/android_satz_lauf1.sha256"

# Quellbaum aendert sich (nur Sandbox): Link ersetzen, 1 Byte kippen
orig_sha="$(sha256sum "$FX" | cut -d' ' -f1)"
cp "$FX" "$S/$FX.neu"
"$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$S/$FX.neu"
mv -f "$S/$FX.neu" "$S/$FX"
echo "--- Quellbaum-Aenderung: $FX in der Sandbox jetzt $(sha256sum "$S/$FX" | cut -c1-16)..., Original im Arbeitsbaum" \
     "$( [[ "$(sha256sum "$FX" | cut -d' ' -f1)" == "$orig_sha" ]] && echo unveraendert || echo VERAENDERT) ($(echo "$orig_sha" | cut -c1-16)...)"

mp lauf2_apk_veraltet
stand "nach Lauf 2"
mkdir -p $W/apk; mv -f "$S/release/re15_port_v0.8.19_android.apk" "$W/apk/sandbox_abgelehnte.apk"
echo "--- APK aus release/ der Sandbox entfernt (abgelehnt; bzw. build_android.sh hat sie vor einem gescheiterten Gradle-Lauf geloescht)"

mp lauf3_ohne_apk
stand "nach Lauf 3"
echo "--- Android-Satz nach Lauf 3 gegen Lauf 1:"; sha256sum -c "$L/android_satz_lauf1.sha256" | sed 's/^/   /'

# Beweis: steckt im ausgelieferten Android-Satz eine APK, die zum AKTUELLEN Quellbaum passt?
rm -f "$W/apk/satz_voll.zip" "$W/apk/aus_satz.apk"
( cd "$S/release" && /c/msys64/usr/bin/zip -q -s 0 re15_port_v0.8.19_android.zip --out "$OLDPWD/$W/apk/satz_voll.zip" )
unzip -p "$W/apk/satz_voll.zip" re15_port_v0.8.19_android.apk > "$W/apk/aus_satz.apk"
echo "--- APK aus dem ausgelieferten Satz: sha256 $(sha256sum "$W/apk/aus_satz.apk" | cut -c1-16)..."
rc=0; "$PY" "$S/release/apk_asset_gate.py" --repo "$S" "$W/apk/aus_satz.apk" > "$L/gate_aus_satz.log" 2>&1 || rc=$?
echo "== Gate (APK aus dem Satz gegen den aktuellen Sandbox-Quellbaum): rc=$rc"
grep -a -E '^      |GATE-(OK|ABWEICHUNG)' "$L/gate_aus_satz.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-200
echo FERTIG
