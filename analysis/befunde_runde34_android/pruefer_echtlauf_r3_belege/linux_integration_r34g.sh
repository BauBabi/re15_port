#!/usr/bin/env bash
# Pruefer echtlauf r3 - Integration mit dem NAECHSTEN Paket: die Granaten-Zweige (r34g/a..d, c0) fuegen
# re15_port/shared_assets/RE2/CORE00.ESP (8572 B) und RE2/TEX.TIM (132320 B) hinzu (gleiche Blobs in allen fuenf
# Zweigen, e030f790... / e2f3e1b3...). Was macht die Kette mit diesem Quellbaum? Laeuft IM Container, /src NUR LESEND,
# Quellbaum-Kopie + Pakete unter /tmp (Container). Die zwei Dateien liegen (per git cat-file aus r34g/d-re2fx) unter
# /src/build/r34a/pruefer_echtlauf_r3/r34g_assets/.
# Aufruf (Host): MSYS_NO_PATHCONV=1 docker run --rm -v <baum>:/src:ro re15-linux-build:deb11 bash <dieses Skript>
set -u
W=/src/build/r34a/pruefer_echtlauf_r3
REF=$W/ref_v0.8.19.apk
NEU=$W/neu_v0.8.19.apk
GATE=/src/release/apk_asset_gate.py
schritt() {
    local titel="$1"; shift
    local t0 t1 rc=0
    echo; echo "### $titel"; echo "### \$ $*"
    t0=$EPOCHREALTIME
    "$@" > /tmp/schritt.out 2>&1 || rc=$?
    t1=$EPOCHREALTIME
    grep -vE '^\s+\[ok\]' /tmp/schritt.out | tail -40
    awk -v r="$rc" -v a="$t0" -v b="$t1" 'BEGIN { printf "### -> rc=%d  dauer=%.1f s\n", r, b - a }'
}
echo "=== Umgebung ==="; head -2 /etc/os-release; python3 --version
sha256sum $W/r34g_assets/CORE00.ESP $W/r34g_assets/TEX.TIM
mkdir -p /tmp/repo/re15_port/shared_assets /tmp/repo/re15_port/platform/android/app /tmp/repo/re15_port/include \
         /tmp/repo/re15_port/engine/src/gen /tmp/repo/synchro
cp -a /src/re15_port/shared_assets/PSX /src/re15_port/shared_assets/extracted_fx /src/re15_port/shared_assets/RE2 \
      /src/re15_port/shared_assets/RE15DOOR /tmp/repo/re15_port/shared_assets/
cp -a /src/re15_port/platform/android/app/build.gradle /tmp/repo/re15_port/platform/android/app/
cp -a /src/re15_port/include/re15_door_seq.h /tmp/repo/re15_port/include/
cp -a /src/re15_port/engine/src/gen/re15_tuer_eigen.inc /src/re15_port/engine/src/gen/re2_tuer_tabelle.inc \
      /src/re15_port/engine/src/gen/tuer_zuordnung.inc /tmp/repo/re15_port/engine/src/gen/
for s in /src/synchro/STAGE*; do cp -a "$s" /tmp/repo/synchro/; done
# altes Paket (Stand v0.8.19, OHNE die neuen Dateien) als harte Links, BEVOR die neuen Dateien in den Quellbaum kommen
mkdir -p /tmp/paket_alt/shared_assets /tmp/paket_alt/synchro
cp -al /tmp/repo/re15_port/shared_assets/PSX /tmp/repo/re15_port/shared_assets/extracted_fx /tmp/repo/re15_port/shared_assets/RE2 \
       /tmp/repo/re15_port/shared_assets/RE15DOOR /tmp/paket_alt/shared_assets/
for s in /tmp/repo/synchro/STAGE*; do cp -al "$s" /tmp/paket_alt/synchro/; done
# Quellbaum nach dem Zusammenfuehren der Granaten-Zweige: + RE2/CORE00.ESP, + RE2/TEX.TIM
cp $W/r34g_assets/CORE00.ESP $W/r34g_assets/TEX.TIM /tmp/repo/re15_port/shared_assets/RE2/
# neues Paket (wie copy_common auf dem neuen Quellbaum)
mkdir -p /tmp/paket_neu/shared_assets /tmp/paket_neu/synchro
cp -al /tmp/repo/re15_port/shared_assets/PSX /tmp/repo/re15_port/shared_assets/extracted_fx /tmp/repo/re15_port/shared_assets/RE2 \
       /tmp/repo/re15_port/shared_assets/RE15DOOR /tmp/paket_neu/shared_assets/
for s in /tmp/repo/synchro/STAGE*; do cp -al "$s" /tmp/paket_neu/synchro/; done

schritt "I1 --quellbaum mit den zwei neuen RE2-Dateien" python3 "$GATE" --repo /tmp/repo --quellbaum
schritt "I2 Referenz-APK v0.8.19 (ohne die neuen Dateien) gegen den neuen Quellbaum = veraltete APK" python3 "$GATE" --repo /tmp/repo "$REF"
schritt "I3 NEUE APK (Bau Abschnitt 1, ohne die neuen Dateien) gegen den neuen Quellbaum" python3 "$GATE" --repo /tmp/repo "$NEU"
schritt "I4 --paket: neues Paket (mit den neuen Dateien)" python3 "$GATE" --repo /tmp/repo --paket /tmp/paket_neu
schritt "I5 --paket: altes Paket (ohne die neuen Dateien) gegen den neuen Quellbaum" python3 "$GATE" --repo /tmp/repo --paket /tmp/paket_alt
echo; echo "=== Ende $(date '+%F %T') ==="
