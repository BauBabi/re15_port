#!/usr/bin/env bash
# Pruefer echtlauf r2 - im Image re15-deck:latest (Steam Runtime 3 "sniper", hat unzip): die GANZE Kette
# (build_android.sh --gate-only -> apk_pruefen.sh) unter Linux OHNE Android-SDK muss geschlossen scheitern
# (bis R1 lief sie hier mit "aapt nicht gefunden - uebersprungen" gruen durch), plus das Gate selbst.
set -u
REF=/archiv/re15_port_v0.8.19_android.apk
schritt() {
    local titel="$1"; shift
    local t0 t1 rc=0
    echo; echo "### $titel"; echo "### \$ $*"
    t0=$(date +%s.%N)
    "$@" > /tmp/schritt.out 2>&1 || rc=$?
    t1=$(date +%s.%N)
    cat /tmp/schritt.out
    awk -v r="$rc" -v a="$t0" -v b="$t1" 'BEGIN { printf "### -> rc=%d  dauer=%.1f s\n", r, b - a }'
}
echo "=== Umgebung ==="
head -3 /etc/os-release; uname -srm; bash --version | head -1; python3 --version
command -v unzip java aapt cygpath 2>&1 | sed 's/^/which: /'; echo "HOME=$HOME"; ls -d "$HOME/Android/Sdk" 2>&1
schritt "D1 build_android.sh --gate-only ohne Android-SDK (muss rc 1 liefern)" bash /src/release/build_android.sh --gate-only "$REF" --version v0.8.19
schritt "D2 build_android.sh --gate-only ohne --version (Bedienfehler, rc 2)" bash /src/release/build_android.sh --gate-only "$REF"
schritt "D3 Gate direkt (Python des Images)" python3 /src/release/apk_asset_gate.py --repo /src "$REF"
echo; echo "=== Ende $(date '+%F %T') ==="
