#!/bin/bash
# Pruefer echtlauf r1 - Linux-Lauf im Bau-Image (Baum /src und Archiv /archiv NUR LESEND eingehaengt)
set -u
echo "== Umgebung =="; uname -srm; cat /etc/debian_version; command -v python3; python3 --version; bash --version | head -1
for t in unzip timeout readlink cygpath; do printf '   %-9s %s\n' "$t" "$(type -P $t || echo FEHLT)"; done
echo "OSTYPE=$OSTYPE"
echo "== Referenz-APK (Archiv, ro) =="; sha256sum /archiv/re15_port_v0.8.19_android.apk; grep apk /archiv/SHA256SUMS_android.txt
echo "== L1 python_finden.sh (Diagnose) =="; bash release/python_finden.sh; echo "L1_RC=$?"
echo "== L2 --selbsttest =="; s=$(date +%s.%N); python3 release/apk_asset_gate.py --selbsttest; rc=$?; e=$(date +%s.%N); echo "L2_RC=$rc  Dauer $(echo "$e - $s" | bc 2>/dev/null || awk "BEGIN{print $e-$s}") s"
echo "== L3 Referenz-APK gegen /src (9p-Mount, ro) =="; s=$(date +%s.%N); python3 release/apk_asset_gate.py --repo /src /archiv/re15_port_v0.8.19_android.apk; rc=$?; e=$(date +%s.%N); echo "L3_RC=$rc  Dauer $(awk "BEGIN{print $e-$s}") s"
echo "== L4 Negativ: Kopie ohne RE15DOOR/P2DS.DO2 (im Container gebaut, /tmp) =="
python3 - <<'PY'
import zipfile
src = "/archiv/re15_port_v0.8.19_android.apk"; dst = "/tmp/neg_ohne_p2ds.apk"
with zipfile.ZipFile(src) as a, zipfile.ZipFile(dst, "w") as b:
    n = 0
    for i in a.infolist():
        if i.filename == "assets/shared_assets/RE15DOOR/P2DS.DO2":
            n += 1; continue
        b.writestr(i, a.read(i), compress_type=i.compress_type)
print("entfernt:", n)
PY
python3 release/apk_asset_gate.py --repo /src /tmp/neg_ohne_p2ds.apk; echo "L4_RC=$?"
echo "== L5 Negativ: falsche Repo-Wurzel =="; python3 release/apk_asset_gate.py --repo /tmp /archiv/re15_port_v0.8.19_android.apk; echo "L5_RC=$?"
echo "== L6 python_finden per source unter set -euo pipefail, dann Gate ueber \$PY =="
bash -c 'set -euo pipefail; source release/python_finden.sh; echo "PY=$PY PY_VERSION=$PY_VERSION"; rc=0; "$PY" release/apk_asset_gate.py --repo /src /archiv/re15_port_v0.8.19_android.apk >/tmp/l6.txt || rc=$?; tail -1 /tmp/l6.txt; echo "L6_RC=$rc"'
echo "== L7 build_android.sh --gate-only (Linux-Host-Pfad) =="
if type -P unzip >/dev/null; then bash release/build_android.sh --gate-only /archiv/re15_port_v0.8.19_android.apk --version v0.8.19; echo "L7_RC=$?"; else echo "   unzip fehlt im Image -> L7 nur mit nachinstalliertem unzip (siehe L7b)"; fi
