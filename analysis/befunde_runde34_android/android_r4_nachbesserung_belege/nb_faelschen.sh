#!/usr/bin/env bash
# Nachbesserung R4-1: signierte Faelschungen der NEUEN APK (Basis $1) fuer Kette und Emulator - mit den Werkzeugen der
# Pruefer (r3_faelschen.py, u1_faelschen_fk.py, r3_signieren.sh; unveraendert):
#   FW_sig  Liste + Zeile nur aus drei Leerzeichen (V1)
#   FD_sig  RE15DOOR/P07G.DO2 1 Byte gekippt UND passende sha256 in der Liste (das Geraet entpackt es klaglos)
#   FK_sig  Kelvin-Paar K.bin / U+212A.bin mit passenden Listenzeilen (H5)
#   R_sig   ohne shared_assets/RE2/CDEMD0.EMS (Eintrag + Listenzeile, Kopf nachgerechnet) - Mess-APK fuer die Waisen
# Aufruf: nb_faelschen.sh <basis.apk> <zielordner>
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python
R3=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
U1=analysis/befunde_runde34_android/pruefer_umgehung_r4_1_belege
BASIS="$1"; Z="$2"; mkdir -p "$Z"
ok=0
sig() { bash $R3/r3_signieren.sh "$1" "$2" || ok=1; rm -f "$1"; }

# FW: Leerraumzeile
"$PY" $R3/r3_faelschen.py "$BASIS" "$Z/FW.apk" --manifest-anhang '   \n' || ok=1
sig "$Z/FW.apk" "$Z/FW_sig.apk"

# FD: P07G.DO2 mit einem gekippten Byte + passende sha256 in der Liste
"$PY" - "$BASIS" "$Z" <<'PYEOF' || ok=1
import hashlib, sys, zipfile
basis, z = sys.argv[1], sys.argv[2]
name = "assets/shared_assets/RE15DOOR/P07G.DO2"
d = bytearray(zipfile.ZipFile(basis).read(name))
alt = hashlib.sha256(d).hexdigest()
d[len(d) // 2] ^= 0x01
neu = hashlib.sha256(d).hexdigest()
open(z + "/P07G_gekippt.DO2", "wb").write(d)
rel = name[len("assets/"):]
open(z + "/FD_sub.txt", "w").write("%d\\t%s\\t%s=%d\\t%s\\t%s" % (len(d), alt, rel, len(d), neu, rel))
print("FD: %s %d B, sha %s.. -> %s.." % (rel, len(d), alt[:16], neu[:16]))
PYEOF
"$PY" $R3/r3_faelschen.py "$BASIS" "$Z/FD.apk" --daten "assets/shared_assets/RE15DOOR/P07G.DO2=$Z/P07G_gekippt.DO2" \
    --manifest-sub "$(cat "$Z/FD_sub.txt")" || ok=1
sig "$Z/FD.apk" "$Z/FD_sig.apk"

# FK: Kelvin-Paar
"$PY" $U1/u1_faelschen_fk.py "$BASIS" "$Z/FK.apk" || ok=1
sig "$Z/FK.apk" "$Z/FK_sig.apk"

# R: ohne CDEMD0.EMS (Mess-APK fuer Abbruch + Update mit gestrichenem Pfad)
"$PY" - "$BASIS" "$Z" <<'PYEOF' || ok=1
import sys, zipfile
basis, z = sys.argv[1], sys.argv[2]
m = zipfile.ZipFile(basis).read("assets/re15_assets.txt")
zeilen = m.split(b"\n")
kopf, rest = zeilen[0], [x for x in zeilen[1:] if x]
weg = [x for x in rest if x.endswith(b"\tshared_assets/RE2/CDEMD0.EMS")]
assert len(weg) == 1, weg
g = int(weg[0].split(b"\t")[0])
teile = kopf.split(b" ")
n, b = int(teile[4]), int(teile[5])
neu = b"# re15 assets v2 %d %d\n" % (n - 1, b - g) + b"".join(x + b"\n" for x in rest if x not in weg)
open(z + "/R_manifest.txt", "wb").write(neu)
print("R: Kopf %r -> %r (ohne CDEMD0.EMS, %d B)" % (kopf, neu.split(b"\n")[0], g))
PYEOF
"$PY" $R3/r3_faelschen.py "$BASIS" "$Z/R.apk" --ohne assets/shared_assets/RE2/CDEMD0.EMS \
    --daten "assets/re15_assets.txt=$Z/R_manifest.txt" || ok=1
sig "$Z/R.apk" "$Z/R_sig.apk"

ls -la "$Z"/*_sig.apk
exit $ok
