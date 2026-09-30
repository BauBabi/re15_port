#!/usr/bin/env bash
# Pruefer echtlauf R4-1: APK M = aktueller Stand + EINE gleich grosse Inhaltsaenderung (ROOM4010.RDT, 1 Byte XOR 0x5A
# in der Mitte), ECHT gebaut mit build_android.sh --version v0.8.19-pe1m (versionCode 81900 wie N -> Update per -r).
# Die Quelldatei wird nur fuer die Bauminuten geaendert (per rename: fremde Hardlinks behalten den alten Inhalt) und
# danach per git restore zurueckgesetzt, mit Nachweis (hash-object = Index-Blob, cmp gegen die Sicherung, git status).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
W=build/r34a/pruefer_e1
F=re15_port/shared_assets/PSX/STAGE4/ROOM4010.RDT
mkdir -p $W/orig $W/apk
blob_index=$(git ls-files -s -- "$F" | awk '{print $2}')
blob_datei=$(git hash-object -- "$F")
echo "Index-Blob $blob_index, Datei-Blob $blob_datei, Linkzahl $(stat -c %h "$F")"
[[ "$blob_index" == "$blob_datei" ]] || { echo "Datei weicht schon vor dem Lauf vom Index ab"; exit 2; }
cp -p "$F" $W/orig/ROOM4010.RDT
/c/Python310/python.exe - "$F" "$W/m_ROOM4010.RDT" <<'PY'
import sys, hashlib
src, dst = sys.argv[1], sys.argv[2]
d = bytearray(open(src, 'rb').read())
o = len(d) // 2
print("Groesse %d, Offset %d: 0x%02x -> 0x%02x" % (len(d), o, d[o], d[o] ^ 0x5A))
alt = hashlib.sha256(d).hexdigest()
d[o] ^= 0x5A
open(dst, 'wb').write(d)
print("sha256 alt %s\nsha256 neu %s" % (alt, hashlib.sha256(d).hexdigest()))
PY
zurueck() {
    git restore --source=HEAD --worktree -- "$F" release/SHA256SUMS_android.txt
    echo "ZURUECK AN: $(date '+%F %T')"
    echo "--- zurueckgesetzt: hash-object $(git hash-object -- "$F") Index $blob_index; cmp mit Sicherung rc $(cmp -s "$F" $W/orig/ROOM4010.RDT; echo $?)"
    echo "--- git status: [$(git status --short -- release/ re15_port/ synchro/ | tr '\n' ' ')]"
}
trap zurueck EXIT
echo "AENDERUNG AN: $(date '+%F %T')"
mv -f "$W/m_ROOM4010.RDT" "$F"
echo "Datei jetzt: $(sha256sum "$F" | cut -c1-64) $(stat -c %s "$F") B"
date '+BAU START %F %T'
bash release/build_android.sh --version v0.8.19-pe1m --no-toolchain; rc=$?
date '+BAU ENDE %F %T'
echo "EXIT=$rc"
if [[ $rc -eq 0 ]]; then mv -f release/re15_port_v0.8.19-pe1m_android.apk $W/apk/M_v0.8.19-pe1m.apk; fi
exit $rc
