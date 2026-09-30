#!/usr/bin/env bash
# Gegenpruefung R3: Faelschung ausrichten (zipalign -P 16 4, wie AGP) und mit DEMSELBEN Schluessel signieren,
# den der Release-Bau benutzt (~/.android/debug.keystore = release/apk_signer.sha256 432bc749...).
# Aufruf: r3_signieren.sh <ein.apk> <aus.apk>   -> aus.apk (zipalign -c ok, apksigner verify ok)
set -u
SDK="$(cygpath -u "$LOCALAPPDATA")/Android/Sdk/build-tools/35.0.0"
JAVA="/c/Program Files/Eclipse Adoptium/jdk-17.0.15.6-hotspot/bin/java"
ein="$1"; aus="$2"; tmp="$aus.ausgerichtet"
rm -f "$tmp" "$aus"
"$SDK/zipalign.exe" -f -P 16 4 "$(cygpath -m "$ein")" "$(cygpath -m "$tmp")" || { echo "zipalign rc=$?"; exit 1; }
"$JAVA" -jar "$(cygpath -m "$SDK/lib/apksigner.jar")" sign --ks "$(cygpath -m ~/.android/debug.keystore)" \
    --ks-pass pass:android --ks-key-alias androiddebugkey --key-pass pass:android --alignment-preserved true \
    --in "$(cygpath -m "$tmp")" --out "$(cygpath -m "$aus")" || { echo "apksigner sign rc=$?"; rm -f "$tmp"; exit 1; }
rm -f "$tmp" "$aus.idsig"
rc=0; v="$("$JAVA" -jar "$(cygpath -m "$SDK/lib/apksigner.jar")" verify -v --print-certs "$(cygpath -m "$aus")" 2>&1)" || rc=$?
za=0; "$SDK/zipalign.exe" -c -P 16 4 "$(cygpath -m "$aus")" > /dev/null 2>&1 || za=$?
echo "$(basename "$aus"): verify rc=$rc zipalign-c rc=$za | $(grep -E '^Verified using v2|^Signer #1 certificate SHA-256' <<<"$v" | tr '\n' ' ')"
