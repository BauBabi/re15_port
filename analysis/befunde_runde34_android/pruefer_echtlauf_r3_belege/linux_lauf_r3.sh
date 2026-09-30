#!/usr/bin/env bash
# Pruefer echtlauf r3 - laeuft IM Container (Baum /src NUR LESEND; alle Kopien/Faelschungen unter /tmp im Container).
# Aufruf (Host): MSYS_NO_PATHCONV=1 docker run --rm -v <baum>:/src:ro re15-linux-build:deb11 \
#                bash /src/analysis/befunde_runde34_android/pruefer_echtlauf_r3_belege/linux_lauf_r3.sh
# Jeder Schritt: Rueckgabe getrennt abgefangen (keine Pipe auf das Kommando), Dauer gemessen.
set -u
W=/src/build/r34a/pruefer_echtlauf_r3
REF=$W/ref_v0.8.19.apk            # Kopie des Archivs (sha256 514bebd5... = Archiv-SUMS)
NEU=$W/neu_v0.8.19.apk            # der Bau aus Abschnitt 1 dieses Dossiers
GATE=/src/release/apk_asset_gate.py
schritt() {   # $1 = Titel, Rest = Kommando
    local titel="$1"; shift
    local t0 t1 rc=0
    echo; echo "### $titel"; echo "### \$ $*"
    t0=$EPOCHREALTIME
    "$@" > /tmp/schritt.out 2>&1 || rc=$?
    t1=$EPOCHREALTIME
    grep -vE '^\s+\[ok\]' /tmp/schritt.out | tail -40
    echo "### (Zeilen [ok] ausgeblendet: $(grep -cE '^\s+\[ok\]' /tmp/schritt.out))"
    awk -v r="$rc" -v a="$t0" -v b="$t1" 'BEGIN { printf "### -> rc=%d  dauer=%.1f s\n", r, b - a }'
}
echo "=== Umgebung ==="
head -2 /etc/os-release; uname -srm; bash --version | head -1; echo "OSTYPE=$OSTYPE"
command -v python3 python unzip cygpath java aapt readlink timeout 2>&1 | sed 's/^/which: /'
python3 --version
sha256sum "$REF" "$NEU"
printf 'CR im Gate: %s, erste Zeile: %s, Modus: %s\n' "$(tr -cd '\r' < "$GATE" | wc -c)" "$(head -1 "$GATE")" "$(stat -c %a "$GATE")"

schritt "L1 python_finden.sh (Diagnoseaufruf)" bash /src/release/python_finden.sh
schritt "L2 --selbsttest (python3)" python3 "$GATE" --selbsttest
schritt "L3 --quellbaum gegen den Mount /src (9p)" python3 "$GATE" --repo /src --quellbaum
schritt "L4 Referenz-APK gegen den Mount /src (9p)" python3 "$GATE" --repo /src "$REF"

# Kopie der vom Gate gelesenen Teile des Quellbaums auf das Container-Dateisystem (kein 9p)
t0=$EPOCHREALTIME
mkdir -p /tmp/repo/re15_port/shared_assets /tmp/repo/re15_port/platform/android/app /tmp/repo/re15_port/include \
         /tmp/repo/re15_port/engine/src/gen /tmp/repo/synchro
cp -a /src/re15_port/shared_assets/PSX /src/re15_port/shared_assets/extracted_fx /src/re15_port/shared_assets/RE2 \
      /src/re15_port/shared_assets/RE15DOOR /tmp/repo/re15_port/shared_assets/
cp -a /src/re15_port/platform/android/app/build.gradle /tmp/repo/re15_port/platform/android/app/
cp -a /src/re15_port/include/re15_door_seq.h /tmp/repo/re15_port/include/
cp -a /src/re15_port/engine/src/gen/re15_tuer_eigen.inc /src/re15_port/engine/src/gen/re2_tuer_tabelle.inc \
      /src/re15_port/engine/src/gen/tuer_zuordnung.inc /tmp/repo/re15_port/engine/src/gen/
for s in /src/synchro/STAGE*; do cp -a "$s" /tmp/repo/synchro/; done
t1=$EPOCHREALTIME
awk -v a="$t0" -v b="$t1" -v g="$(du -sh /tmp/repo | cut -f1)" 'BEGIN { printf "\nKopie nach /tmp/repo: %.1f s, %s\n", b - a, g }'

schritt "L5 Referenz-APK gegen die Kopie /tmp/repo" python3 "$GATE" --repo /tmp/repo "$REF"
schritt "L6 NEUE APK (Bau Abschnitt 1) gegen die Kopie /tmp/repo" python3 "$GATE" --repo /tmp/repo "$NEU"
schritt "L7 Direktaufruf /src/release/apk_asset_gate.py (Kopf als Bash) --repo /tmp/repo --quellbaum" \
        /src/release/apk_asset_gate.py --repo /tmp/repo --quellbaum

# L8 Negativ: Kopie der Referenz, EIN Byte in den Daten von RE2/DOOR/DOOR36.DO2 gekippt (eigener Rohzugriff)
python3 - "$REF" /tmp/neg_door36.apk <<'PY'
import shutil, struct, sys
src, dst = sys.argv[1], sys.argv[2]
shutil.copyfile(src, dst)
d = open(dst, "rb").read()
i = d.rfind(b"PK\x05\x06"); n, cs, co = struct.unpack("<HII", d[i + 10:i + 20])
p = co; ziel = b"assets/shared_assets/RE2/DOOR/DOOR36.DO2"
for _ in range(n):
    nl, xl, kl = struct.unpack("<HHH", d[p + 28:p + 34]); off = struct.unpack("<I", d[p + 42:p + 46])[0]
    if d[p + 46:p + 46 + nl] == ziel:
        ln, lx = struct.unpack("<HH", d[off + 26:off + 30]); pos = off + 30 + ln + lx + 777
        with open(dst, "r+b") as f:
            f.seek(pos); b = f.read(1); f.seek(pos); f.write(bytes([b[0] ^ 0x10]))
        print("gekippt: Offset", pos, "in", ziel.decode()); break
    p += 46 + nl + xl + kl
PY
schritt "L8 Negativ: 1 Byte in RE2/DOOR/DOOR36.DO2 gekippt" python3 "$GATE" --repo /tmp/repo /tmp/neg_door36.apk

# L9 Negativ: Quellbaum-Kopie mit Zusatzdatei RE15DOOR/P99X.DO2 (APK unveraendert = Referenz)
cp /tmp/repo/re15_port/shared_assets/RE15DOOR/P07G.DO2 /tmp/repo/re15_port/shared_assets/RE15DOOR/P99X.DO2
schritt "L9 Negativ: Quellbaum hat RE15DOOR/P99X.DO2 zusaetzlich" python3 "$GATE" --repo /tmp/repo "$REF"
rm -f /tmp/repo/re15_port/shared_assets/RE15DOOR/P99X.DO2

# L10 Negativ: abgeschnittene APK (erste 100 MB)
head -c 100000000 "$REF" > /tmp/neg_kurz.apk
schritt "L10 Negativ: APK abgeschnitten (100 MB)" python3 "$GATE" --repo /tmp/repo /tmp/neg_kurz.apk

# L11 --paket: Paketordner wie copy_common (harte Links aus /tmp/repo), gut und mit 1 gekipptem Byte (Link vorher geloest)
mkdir -p /tmp/paket/shared_assets /tmp/paket/synchro
cp -al /tmp/repo/re15_port/shared_assets/PSX /tmp/repo/re15_port/shared_assets/extracted_fx /tmp/repo/re15_port/shared_assets/RE2 \
       /tmp/repo/re15_port/shared_assets/RE15DOOR /tmp/paket/shared_assets/
for s in /tmp/repo/synchro/STAGE*; do cp -al "$s" /tmp/paket/synchro/; done
schritt "L11a --paket (wie copy_common)" python3 "$GATE" --repo /tmp/repo --paket /tmp/paket
f=/tmp/paket/shared_assets/RE2/TORSE.VBS; cp "$f" "$f.x" && mv -f "$f.x" "$f"
python3 -c "p='$f'; b=bytearray(open(p,'rb').read()); b[4321]^=1; open(p,'wb').write(b)"
schritt "L11b --paket, 1 Byte in RE2/TORSE.VBS gekippt" python3 "$GATE" --repo /tmp/repo --paket /tmp/paket

schritt "L12 build_android.sh --gate-only ohne Android-SDK (muss geschlossen abbrechen)" \
        bash /src/release/build_android.sh --gate-only "$REF" --version v0.8.19
echo; echo "=== Ende $(date '+%F %T') ==="
