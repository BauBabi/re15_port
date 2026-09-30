#!/usr/bin/env bash
# Nachbesserung R2 - laeuft IM Container (Baum /src NUR LESEND, /out beschreibbar), Image re15-linux-build:deb11.
# Aufruf (Host): MSYS_NO_PATHCONV=1 docker run --rm -v <baum>:/src:ro -v <ausgabe>:/out re15-linux-build:deb11 \
#                bash /src/analysis/befunde_runde34_android/nachbesserung_r2_belege/linux_lauf_nb2.sh
set -u
REF=/src/build/r34a/ref_v0.8.19.apk
GATE=/src/release/apk_asset_gate.py
schritt() {   # $1 = Titel, Rest = Kommando; Rueckgabe getrennt abgefangen, Dauer gemessen
    local titel="$1"; shift
    local t0 t1 rc=0
    echo; echo "### $titel"; echo "### \$ $*"
    t0=$(date +%s.%N)
    "$@" > /tmp/schritt.out 2>&1 || rc=$?
    t1=$(date +%s.%N)
    grep -vE '^\s+\[ok\]' /tmp/schritt.out | tail -25
    awk -v r="$rc" -v a="$t0" -v b="$t1" 'BEGIN { printf "### -> rc=%d  dauer=%.1f s\n", r, b - a }'
}
echo "=== Umgebung ==="; head -2 /etc/os-release; python3 --version; bash --version | head -1
printf 'CR im Gate: %s, erste Zeile: %s\n' "$(tr -cd '\r' < "$GATE" | wc -c)" "$(head -1 "$GATE")"
schritt "L1 python_finden.sh" bash /src/release/python_finden.sh
schritt "L2 --selbsttest (python3)" python3 "$GATE" --selbsttest
schritt "L3 --quellbaum (Tuer-Soll unter Linux, 9p)" python3 "$GATE" --repo /src --quellbaum
schritt "L4 Referenz-APK gegen /src (9p)" python3 "$GATE" --repo /src "$REF"
schritt "L5 Direktaufruf /src/release/apk_asset_gate.py --quellbaum (Kopf als Bash)" /src/release/apk_asset_gate.py --repo /src --quellbaum
# L6 PC-Paket aus dem Quellbaum wie copy_common (Container-Dateisystem), --paket gut und mit 1 gekipptem Byte
mkdir -p /tmp/paket/shared_assets /tmp/paket/synchro
cp -a /src/re15_port/shared_assets/PSX /src/re15_port/shared_assets/extracted_fx /src/re15_port/shared_assets/RE2 \
      /src/re15_port/shared_assets/RE15DOOR /tmp/paket/shared_assets/
for s in /src/synchro/STAGE*; do cp -a "$s" /tmp/paket/synchro/; done
schritt "L6a --paket (Kopie wie copy_common)" python3 "$GATE" --repo /src --paket /tmp/paket
python3 -c "p='/tmp/paket/shared_assets/RE15DOOR/P2DS.DO2'; b=bytearray(open(p,'rb').read()); b[500]^=1; open(p,'wb').write(b)"
schritt "L6b --paket, 1 Byte in RE15DOOR/P2DS.DO2 gekippt" python3 "$GATE" --repo /src --paket /tmp/paket
schritt "L7 build_android.sh --gate-only ohne Android-SDK (muss geschlossen abbrechen)" bash /src/release/build_android.sh --gate-only "$REF" --version v0.8.19
