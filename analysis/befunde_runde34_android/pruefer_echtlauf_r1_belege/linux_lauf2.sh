#!/bin/bash
# Pruefer echtlauf r1 - build_android.sh --gate-only unter Linux (Image mit unzip), Baum/Archiv nur lesend
set -u
echo "== Umgebung =="; uname -srm; cat /etc/os-release | grep -E '^(PRETTY_NAME)='; python3 --version; type -P unzip
echo "== L7 build_android.sh --gate-only Referenz-APK =="
s=$(date +%s.%N); bash release/build_android.sh --gate-only /archiv/re15_port_v0.8.19_android.apk --version v0.8.19; rc=$?; e=$(date +%s.%N)
echo "L7_RC=$rc  Dauer $(awk "BEGIN{print $e-$s}") s"
echo "== L7n build_android.sh --gate-only Negativ-Kopie ohne RE15DOOR/P2DS.DO2 =="
python3 - <<'PY'
import zipfile
src = "/archiv/re15_port_v0.8.19_android.apk"; dst = "/tmp/neg_ohne_p2ds.apk"
with zipfile.ZipFile(src) as a, zipfile.ZipFile(dst, "w") as b:
    for i in a.infolist():
        if i.filename != "assets/shared_assets/RE15DOOR/P2DS.DO2":
            b.writestr(i, a.read(i), compress_type=i.compress_type)
PY
bash release/build_android.sh --gate-only /tmp/neg_ohne_p2ds.apk --version v0.8.19; echo "L7n_RC=$?"
