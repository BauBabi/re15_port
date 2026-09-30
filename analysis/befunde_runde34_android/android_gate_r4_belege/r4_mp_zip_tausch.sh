#!/usr/bin/env bash
# Runde 4 (Kette B3): der ECHTE Fluss von make_package.sh (Sandbox, --version v0.8.19 --only linux, APK = Referenz
# v0.8.19) - was, wenn beim Zippen nicht die gepruefte Datei im Android-Satz landet? Eine zip-ATTRAPPE vorn im PATH
# reicht jeden Aufruf an /c/msys64/usr/bin/zip durch, legt aber beim Zippen der APK (Argument *_android.apk) die
# CRC32-/Groessen-GLEICHE Faelschung aus r4_zip_kennung_sonde.sh (gleicher Name) in den Satz. Die APK unter
# release/ selbst bleibt die gepruefte (die Kennungspruefung VOR dem Zippen sieht nichts).
#   LN  NEUES make_package.sh -> soll EXIT 1 nach dem Zippen ("NICHT die gepruefte Datei"), keine SUMS-Zeile
#   LA  ALTES make_package.sh (9d2337e4) -> Kontrolle: EXIT 0, "APK im Split-Satz = gepruefte APK (CRC32 ...)"
# Rueckgabe je Lauf selbst abgefangen. Nur /c/Python310/python direkt.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; S=$W/sb_b3; L=$W/logs/b3; mkdir -p "$L"
N=re15_port_v0.8.19_android.apk; F=$W/crc/F.apk
[[ -f "$F" ]] || { echo "Faelschung fehlt (r4_zip_kennung_sonde.sh zuerst)"; exit 1; }
rm -rf "$S" "$W/b3_attrappe"; bash $B/r4_sandbox_anlegen.sh "$S" || { echo "Sandbox fehlgeschlagen"; exit 1; }
mkdir -p "$S/release/linux_out" "$W/b3_attrappe/bin" "$W/b3_attrappe/faelschung"
cp -p $W/bin/linux/re15_pc "$S/release/linux_out/re15_pc"
cp -p build/r34a/ref_v0.8.19.apk "$S/release/$N"
cp -p "$F" "$W/b3_attrappe/faelschung/$N"
# ABSOLUT (Lauf 1 dieses Werkzeugs gab einen relativen Pfad weiter; make_package zippt mit cwd = release/ -> zip
# "Nothing to do", EXIT 12 fuer NEU und ALT - ein Fehler der Sonde, nicht des Skripts)
FAELSCHUNG="$(cygpath -m "$(cd "$W/b3_attrappe/faelschung" && pwd)/$N")"
cat > "$W/b3_attrappe/bin/zip" <<ZIP
#!/usr/bin/env bash
# zip-Attrappe (Runde 4, B3): alles durchreichen, nur die APK beim Zippen gegen die Faelschung tauschen
args=()
for a in "\$@"; do
    case "\$a" in
        *_android.apk) args+=("$FAELSCHUNG"); echo "[zip-attrappe] \$a -> Faelschung (gleiche Groesse/CRC32)" >&2 ;;
        *) args+=("\$a") ;;
    esac
done
exec /c/msys64/usr/bin/zip "\${args[@]}"
ZIP
chmod +x "$W/b3_attrappe/bin/zip"
ATTRAPPE="$(cd "$W/b3_attrappe/bin" && pwd)"
echo "Faelschung fuer die Attrappe: $FAELSCHUNG"
echo "Referenz: $(sha256sum build/r34a/ref_v0.8.19.apk | cut -c1-16)..., Faelschung: $(sha256sum "$F" | cut -c1-16)...," \
     "Groesse $(stat -c %s build/r34a/ref_v0.8.19.apk)/$(stat -c %s "$F")"
mp() {      # $1 = Tag
    local tag="$1" rc=0 t0 t1
    t0=$(date +%s)
    PATH="$ATTRAPPE:$PATH" bash "$S/release/make_package.sh" --version v0.8.19 --only linux > "$L/$tag.log" 2>&1 || rc=$?
    t1=$(date +%s)
    echo "== $tag: EXIT=$rc ($((t1 - t0)) s)"
    grep -a -E '^ABBRUCH|zip-attrappe|APK-PRUEFUNG-OK|gepruefte APK|Zippen: |APK-Satz|APK im Split-Satz|SHA256SUMS.txt geschrieben|== Fertig' \
        "$L/$tag.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-200
    echo "   SHA256SUMS.txt (Sandbox): $(grep -c '' "$S/release/SHA256SUMS.txt" 2>/dev/null || echo fehlt) Zeilen," \
         "Android: $(grep -c '_android' "$S/release/SHA256SUMS.txt" 2>/dev/null || echo 0)"
}
mp LN_neues_skript
cp -p $W/alt/make_package.sh "$S/release/make_package.sh"; cp -p $W/alt/apk_pruefen.sh "$S/release/apk_pruefen.sh"
echo "--- Sandbox-Skripte := ALT (9d2337e4)"
mp LA_altes_skript
for f in make_package.sh apk_pruefen.sh; do cp -p "release/$f" "$S/release/$f"; done
echo "--- was im Satz von LA steckt (zusammenfuehren + entpacken):"
( cd "$S/release" && /c/msys64/usr/bin/zip -q -s 0 re15_port_v0.8.19_android.zip --out "$(cygpath -m "$OLDPWD/$W/b3_attrappe")/ganz.zip" )
echo "   sha256 der APK im Satz: $(unzip -p "$W/b3_attrappe/ganz.zip" | sha256sum | cut -c1-16)... (Faelschung: $(sha256sum "$F" | cut -c1-16)...)"
rm -f "$W/b3_attrappe/ganz.zip"
echo FERTIG
