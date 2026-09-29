#!/usr/bin/env bash
# Pruefer echtlauf r2 - laeuft IM Container (Baum /src NUR LESEND, Archiv /archiv NUR LESEND, /out beschreibbar).
# Aufruf (Host): MSYS_NO_PATHCONV=1 docker run --rm -v <baum>:/src:ro -v <archiv v0.8.19>:/archiv:ro
#                -v <ausgabe>:/out <image> bash /src/analysis/befunde_runde34_android/pruefer_echtlauf_r2_belege/linux_lauf_r2.sh
# Jeder Schritt: Rueckgabe getrennt abgefangen (keine Pipe auf das Kommando), Dauer gemessen.
set -u
REF=/archiv/re15_port_v0.8.19_android.apk
NEU=/src/build/r34a/pruefer_echtlauf_r2/neu_v0.8.19.apk
GATE=/src/release/apk_asset_gate.py
schritt() {   # $1 = Titel, Rest = Kommando
    local titel="$1"; shift
    local t0 t1 rc=0
    echo
    echo "### $titel"
    echo "### \$ $*"
    t0=$(date +%s.%N)
    "$@" > /tmp/schritt.out 2>&1 || rc=$?
    t1=$(date +%s.%N)
    cat /tmp/schritt.out
    printf '### -> rc=%d  dauer=%.1f s\n' "$rc" "$(echo "$t1 - $t0" | bc)"
}
echo "=== Umgebung ==="
head -2 /etc/os-release; uname -srm; bash --version | head -1
echo "OSTYPE=$OSTYPE"; command -v python3 python unzip cygpath java aapt 2>&1 | sed 's/^/which: /'
python3 --version
sha256sum "$REF" "$NEU"
ls -la "$GATE" /src/release/python_finden.sh /src/release/apk_pruefen.sh
printf 'CR im Gate: %s\n' "$(tr -cd '\r' < "$GATE" | wc -c)"

schritt "L1 python_finden.sh (Diagnoseaufruf)" bash /src/release/python_finden.sh
schritt "L2 --selbsttest" python3 "$GATE" --selbsttest
schritt "L3 Referenz-APK gegen den Quellbaum (9p-Mount)" python3 "$GATE" --repo /src "$REF"
schritt "L4 frisch gebaute APK (r2-Bau) gegen den Quellbaum (9p-Mount)" python3 "$GATE" --repo /src "$NEU"

# L5 Negativ: Kopie der Referenz im Container, EIN Byte in den Daten von RE15DOOR/P07G.DO2 gekippt
python3 - "$REF" /tmp/neg_p07g.apk <<'PY'
import shutil, struct, sys, zipfile
src, dst = sys.argv[1], sys.argv[2]
shutil.copyfile(src, dst)
with zipfile.ZipFile(dst) as z:
    i = z.getinfo("assets/shared_assets/RE15DOOR/P07G.DO2")
with open(dst, "r+b") as f:
    f.seek(i.header_offset + 26); ln, lx = struct.unpack("<HH", f.read(4))
    pos = i.header_offset + 30 + ln + lx + 1000
    f.seek(pos); b = f.read(1); f.seek(pos); f.write(bytes([b[0] ^ 0x01]))
print("gekippt: Offset", pos, "in", i.filename)
PY
schritt "L5 Negativ: 1 Byte in RE15DOOR/P07G.DO2 gekippt" python3 "$GATE" --repo /src /tmp/neg_p07g.apk
# L6 Negativ: Eintrag RE2/TORSE.VBS fehlt (per zipfile ohne diesen Eintrag neu geschrieben, Manifest unveraendert)
python3 - "$REF" /tmp/neg_torse.apk <<'PY'
import sys, zipfile
src, dst = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(src) as a, zipfile.ZipFile(dst, "w") as b:
    for i in a.infolist():
        if i.filename == "assets/shared_assets/RE2/TORSE.VBS":
            continue
        b.writestr(i, a.read(i), compress_type=i.compress_type)
print("ohne TORSE.VBS geschrieben")
PY
schritt "L6 Negativ: TORSE.VBS fehlt in der APK" python3 "$GATE" --repo /src /tmp/neg_torse.apk
schritt "L7 Bedienfehler: --repo ohne build.gradle" python3 "$GATE" --repo /tmp "$REF"

# L8 wie die Aufrufer: source python_finden.sh unter set -euo pipefail, Gate ueber "$PY"
cat > /tmp/l8.sh <<'SH'
set -euo pipefail
source /src/release/python_finden.sh
echo "PY=$PY PY_VERSION=$PY_VERSION"
rc=0; "$PY" /src/release/apk_asset_gate.py --repo /src /archiv/re15_port_v0.8.19_android.apk > /tmp/l8.gate 2>&1 || rc=$?
tail -2 /tmp/l8.gate; echo "gate-rc=$rc"; exit $rc
SH
schritt "L8 source python_finden.sh + \"\$PY\" (set -euo pipefail)" bash /tmp/l8.sh

# L9 Direktaufruf ueber den Shebang (#!/usr/bin/env python3) - unter Linux zulaessig?
schritt "L9 Direktaufruf ./apk_asset_gate.py --selbsttest (Shebang)" /src/release/apk_asset_gate.py --selbsttest

# L10 Kopie des Quellbaums auf das Container-Dateisystem (kein 9p): Gate + Laufzeit
t0=$(date +%s.%N)
mkdir -p /tmp/repo/re15_port/shared_assets /tmp/repo/re15_port/platform/android/app /tmp/repo/synchro
cp -a /src/re15_port/shared_assets/PSX /src/re15_port/shared_assets/extracted_fx /src/re15_port/shared_assets/RE2 \
      /src/re15_port/shared_assets/RE15DOOR /tmp/repo/re15_port/shared_assets/
cp -a /src/re15_port/platform/android/app/build.gradle /tmp/repo/re15_port/platform/android/app/
cp -a /src/synchro/STAGE* /tmp/repo/synchro/
t1=$(date +%s.%N)
printf 'Kopie nach /tmp/repo: %.1f s, %s\n' "$(echo "$t1 - $t0" | bc)" "$(du -sh /tmp/repo | cut -f1)"
schritt "L10 Referenz-APK gegen die Kopie auf dem Container-Dateisystem" python3 "$GATE" --repo /tmp/repo "$REF"
echo
echo "=== Ende $(date '+%F %T') ==="
