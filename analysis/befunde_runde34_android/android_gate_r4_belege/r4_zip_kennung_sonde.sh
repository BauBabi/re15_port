#!/usr/bin/env bash
# Runde 4 (Kette B3): Funktions-Sonde wie pruefer_umgehung_r3_belege/r3_zip_kennung_sonde.sh - verify_split und
# verify_apk_im_zip werden per awk UNVERAENDERT aus release/make_package.sh (NEU) bzw. aus dem Stand 9d2337e4
# (ALT, von R3 geprueft) geschnitten und gegen zwei Split-Saetze gefahren (gezippt wie make_package.sh: zip -q -s
# 90m -j), Kennung der geprueften APK = Referenz v0.8.19:
#   K  Satz aus der Referenz-APK                                  -> NEU rc 0, ALT rc 0
#   F  Satz aus einer CRC32-/Groessen-GLEICHEN Faelschung (r3_crc_gleich.py, 1 Byte + 4 Ausgleichsbytes in
#      RE15DOOR/P07G.DO2)                                          -> NEU soll rc 1, ALT rc 0 (der Befund)
#   E  Satz K + fremde Datei daneben (verify_split, Runde 4 nachgeschaerft)
# PATH beim Funktionsaufruf wie im Zip-Abschnitt von make_package.sh (zip aus /c/msys64/usr/bin). Nur /c/Python310/python.
# ⛔ Die awk-Schnitte laufen VOR dem PATH-Wechsel: ein awk aus /c/msys64/usr/bin (andere MSYS-Laufzeit) erbt von
# Git-Bash das O_APPEND von ">>" nicht und schreibt ab Byte 0 - Lauf 1 dieser Sonde verlor so verify_split.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; D=$W/crc; B3R=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
PY=/c/Python310/python; R=build/r34a/ref_v0.8.19.apk; N=re15_port_v0.8.19_android.apk
rm -rf "$D"; mkdir -p "$D/K" "$D/F"
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
"$PY" $B3R/r3_crc_gleich.py "$R" "$D/F/$N" "$q"
cp -p "$R" "$D/K/$N"
kennung_ref="$("$PY" -c "
import hashlib, zlib, sys
h, c, n = hashlib.sha256(), 0, 0
for b in iter(lambda f=open(sys.argv[1], 'rb'): f.read(1 << 20), b''):
    h.update(b); c = zlib.crc32(b, c); n += len(b)
print('%s %08x %d' % (h.hexdigest(), c & 0xFFFFFFFF, n))" "$R")"
echo "Kennung der geprueften APK (Referenz): $kennung_ref"
for x in K F; do
    ( cd "$D/$x" && /c/msys64/usr/bin/zip -q -s 90m -j re15_port_v0.8.19_android.zip "$N" ) || { echo "zip $x fehlgeschlagen"; exit 1; }
    mv "$D/$x/$N" "$D/$x.apk"                   # im Satz-Ordner nur die Volumes
done
awk '/^verify_split\(\) \{/,/^\}$/' release/make_package.sh > "$D/funktionen_neu.sh"
awk '/^verify_apk_im_zip\(\) \{/,/^\}$/' release/make_package.sh >> "$D/funktionen_neu.sh"
awk '/^verify_split\(\) \{/,/^\}$/' $W/alt/make_package.sh > "$D/funktionen_alt.sh"
awk '/^verify_apk_im_zip\(\) \{/,/^\}$/' $W/alt/make_package.sh >> "$D/funktionen_alt.sh"
echo "ausgeschnitten: NEU $(grep -c '' "$D/funktionen_neu.sh") Zeilen, ALT $(grep -c '' "$D/funktionen_alt.sh") Zeilen"
for stand in neu alt; do
    for x in K F; do
        rc=0
        ( set -uo pipefail; export PATH="/c/msys64/usr/bin:$PATH"; source "$D/funktionen_$stand.sh" || exit 90
          cd "$D/$x" || exit 91
          verify_split re15_port_v0.8.19_android.zip 1 || exit $?
          verify_apk_im_zip re15_port_v0.8.19_android.zip "$N" "$kennung_ref" || exit $? ) > "$D/lauf_${stand}_$x.log" 2>&1 || rc=$?
        echo "== ${stand^^} Satz $x: rc=$rc"
        sed 's/^/   /' "$D/lauf_${stand}_$x.log" | tr -d '\r' | cut -c1-200
    done
done
# E  Satz K + eine FREMDE Datei re15_port_v0.8.19_android.z05 daneben (nicht aus diesem zip-Lauf) -> verify_split
#    NEU soll rc 1 (Runde 4, 80559d2f), ALT rc 0
cp -rp "$D/K" "$D/E"; printf "fremd" > "$D/E/re15_port_v0.8.19_android.z05"
for stand in neu alt; do
    rc=0
    ( set -uo pipefail; export PATH="/c/msys64/usr/bin:$PATH"; source "$D/funktionen_$stand.sh" || exit 90
      cd "$D/E" || exit 91; verify_split re15_port_v0.8.19_android.zip 1 || exit $? ) > "$D/lauf_${stand}_E.log" 2>&1 || rc=$?
    echo "== ${stand^^} Satz E (fremde .z05 daneben): verify_split rc=$rc"
    sed 's/^/   /' "$D/lauf_${stand}_E.log" | tr -d '' | cut -c1-200
done
echo "--- Temp-Reste der Satzpruefung: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_satz.* 2>/dev/null | wc -l)"
echo FERTIG
