#!/usr/bin/env bash
# Nachbesserung R1: Gate + Selbsttest + python_finden unter Linux (Docker re15-linux-build:deb11, Baum NUR LESEND
# unter /src). Aufruf im Container: bash /src/analysis/befunde_runde34_android/nachbesserung_r1_belege/linux_lauf_nb.sh
set -u
cd /src || exit 99
echo "== $(. /etc/os-release; echo "$PRETTY_NAME"), $(python3 --version), bash $BASH_VERSION"
echo "== L1 python_finden"; bash release/python_finden.sh; echo "rc=$?"
echo "== L2 Selbsttest"; t0=$(date +%s); python3 release/apk_asset_gate.py --selbsttest > /tmp/st.log 2>&1; rc=$?
grep -E "FEHLER|SELBSTTEST" /tmp/st.log; echo "rc=$rc ($(( $(date +%s) - t0 )) s)"
for a in build/r34a/ref_v0.8.19.apk build/r34a/nb/apk/F2_signiert.apk build/r34a/nb/apk/F3_nul.apk \
         build/r34a/nb/apk/F4_lfh_crc.apk build/r34a/nb/apk/F5_lfh_usize.apk build/r34a/nb/apk/N5_weniger.apk; do
    t0=$(date +%s); python3 release/apk_asset_gate.py --repo /src "$a" > /tmp/g.log 2>&1; rc=$?
    echo "== $(basename "$a"): rc=$rc ($(( $(date +%s) - t0 )) s) | $(grep -E '^== APK-ASSET|^      |ABBRUCH' /tmp/g.log | head -2 | cut -c1-150 | tr '\n' '#')"
done
