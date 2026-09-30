#!/usr/bin/env bash
# Gegenpruefung R3: Nimmt make_package.sh verify_apk_im_zip (Pruefung NACH dem Zippen: "steckt im Split-Satz
# die GEPRUEFTE APK?") eine ANDERE Datei gleicher Groesse und gleicher CRC32 an?
# verify_split + verify_apk_im_zip werden per awk UNVERAENDERT aus release/make_package.sh geschnitten;
# gezippt wird wie make_package.sh:700 (zip -q -s 90m -j). Kennung der geprueften APK = Referenz v0.8.19.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; B=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege; D=$W/crc
PY=/c/Python310/python; R=build/r34a/ref_v0.8.19.apk; N=re15_port_v0.8.19_android.apk
rm -rf "$D"; mkdir -p "$D/satz"
# Datenoffset von RE15DOOR/P07G.DO2 in der Referenz (Local Header lesen), gekippt wird Byte 1000 darin
q="$("$PY" - "$R" <<'PY'
import struct, sys
d = open(sys.argv[1], "rb")
d.seek(306927600); k = d.read(30)
assert k[:4] == b"PK\x03\x04"
nl, xl = struct.unpack("<HH", k[26:30]); name = d.read(nl)
assert name == b"assets/shared_assets/RE15DOOR/P07G.DO2", name
print(306927600 + 30 + nl + xl + 1000)
PY
)"
echo "q = $q (Byte 1000 der Daten von assets/shared_assets/RE15DOOR/P07G.DO2)"
"$PY" $B/r3_crc_gleich.py "$R" "$D/satz/$N" "$q"
kennung_ref="$("$PY" -c "
import hashlib, zlib, sys
h, c, n = hashlib.sha256(), 0, 0
for b in iter(lambda f=open(sys.argv[1], 'rb'): f.read(1 << 20), b''):
    h.update(b); c = zlib.crc32(b, c); n += len(b)
print('%s %08x %d' % (h.hexdigest(), c & 0xFFFFFFFF, n))" "$R")"
echo "Kennung der geprueften APK (Referenz): $kennung_ref"
( cd "$D/satz" && /c/msys64/usr/bin/zip -q -s 90m -j re15_port_v0.8.19_android.zip "$N" ) || { echo "zip fehlgeschlagen"; exit 1; }
mv "$D/satz/$N" "$D/faelschung.apk"          # im Satz-Ordner nur die Volumes lassen
ls -la "$D/satz"
# Funktionen unveraendert aus make_package.sh
awk '/^verify_split\(\) \{/,/^\}$/' release/make_package.sh > "$D/funktionen.sh"
awk '/^verify_apk_im_zip\(\) \{/,/^\}$/' release/make_package.sh >> "$D/funktionen.sh"
echo "ausgeschnitten: $(grep -c '' "$D/funktionen.sh") Zeilen ($(grep -E '^verify_' "$D/funktionen.sh" | tr '\n' ' '))"
# shellcheck disable=SC1090
source "$D/funktionen.sh"
cd "$D/satz"
rc=0; verify_split re15_port_v0.8.19_android.zip 1 || rc=$?; echo "verify_split rc=$rc"
rc=0; verify_apk_im_zip re15_port_v0.8.19_android.zip "$N" "$kennung_ref" || rc=$?; echo "verify_apk_im_zip rc=$rc"
cd - > /dev/null
echo "--- was im Satz steckt:"
rc=0; "$PY" release/apk_asset_gate.py --repo . "$D/faelschung.apk" > "$W/logs/gate_crc_faelschung.log" 2>&1 || rc=$?
echo "Gate rc=$rc: $(grep -a -m2 -E '^      ' "$W/logs/gate_crc_faelschung.log" | tr -d '\r' | sed 's/^ *//' | tr '\n' ' ' | cut -c1-200)"
J="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java"; SDK="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
rc=0; v="$("$J" -jar "$(cygpath -m "$SDK/lib/apksigner.jar")" verify "$(cygpath -m "$D/faelschung.apk")" 2>&1)" || rc=$?
echo "apksigner verify rc=$rc: $(echo "$v" | grep -m2 -E 'ERROR|DOES NOT' | tr '\n' ' ' | cut -c1-200)"
echo FERTIG
