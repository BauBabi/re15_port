#!/usr/bin/env bash
# Gegenpruefung R4-2: signierte Faelschungen von NB1.apk (Nachbesserung R4-1, v0.8.20-nb1, Liste v2) mit den Werkzeugen der
# Pruefer (r3_faelschen.py, r3_signieren.sh; unveraendert; zipalign + derselbe Debug-Schluessel):
#   FS_sig  Liste: sha256 von RE15DOOR/P07G.DO2 durch "f" x 64 ersetzt (Groesse/Kopf gleich, Daten unveraendert) -
#           echtes Gate: "Manifest-Pruefsumme falsch"; Geraet: entpacken() verwirft die Datei bei JEDEM Lauf (fail closed)
#   FD_sig  RE15DOOR/P07G.DO2 1 Byte gekippt UND passende sha256 in der Liste (wie nb_faelschen.sh: das Geraet entpackt es
#           klaglos - der schaedliche Fall; nur der Quellbaum-Vergleich des Gates faengt ihn)
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
PY=/c/Python310/python
R3=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
BASIS=build/r34a/nb/apk/NB1.apk; Z=build/r34a/pruefer_u2/apk; mkdir -p "$Z"
ok=0
sig() { bash $R3/r3_signieren.sh "$1" "$2" || ok=1; rm -f "$1"; }
"$PY" - "$BASIS" "$Z" <<'PYEOF' || ok=1
import hashlib, sys, zipfile
basis, z = sys.argv[1], sys.argv[2]
zf = zipfile.ZipFile(basis)
name = "assets/shared_assets/RE15DOOR/P07G.DO2"
rel = name[len("assets/"):]
d = bytearray(zf.read(name))
alt = hashlib.sha256(d).hexdigest()
m = zf.read("assets/re15_assets.txt").decode()
zeile = "%d\t%s\t%s" % (len(d), alt, rel)
assert m.count("\n" + zeile + "\n") == 1, "Zeile nicht genau einmal"
open(z + "/FS_sub.txt", "w").write("%d\t%s\t%s=%d\t%s\t%s" % (len(d), alt, rel, len(d), "f" * 64, rel))
d[len(d) // 2] ^= 0x01
neu = hashlib.sha256(d).hexdigest()
open(z + "/P07G_gekippt.DO2", "wb").write(d)
open(z + "/FD_sub.txt", "w").write("%d\t%s\t%s=%d\t%s\t%s" % (len(d), alt, rel, len(d), neu, rel))
print("P07G.DO2 %d B, sha %s..; FS: Liste -> ffff..; FD: Daten 1 Byte gekippt, sha -> %s.." % (len(d), alt[:16], neu[:16]))
PYEOF
"$PY" $R3/r3_faelschen.py "$BASIS" "$Z/FS.apk" --manifest-sub "$(cat "$Z/FS_sub.txt")" || ok=1
sig "$Z/FS.apk" "$Z/FS_sig.apk"
"$PY" $R3/r3_faelschen.py "$BASIS" "$Z/FD.apk" --daten "assets/shared_assets/RE15DOOR/P07G.DO2=$Z/P07G_gekippt.DO2" \
    --manifest-sub "$(cat "$Z/FD_sub.txt")" || ok=1
sig "$Z/FD.apk" "$Z/FD_sig.apk"
ls -la "$Z"/*_sig.apk; sha256sum "$Z"/*_sig.apk
exit $ok
