#!/usr/bin/env bash
# Nachbesserung R4-1: release/apk_asset_gate.sha256 aus dem Arbeitsbaum-Gate neu schreiben (nur nach bestandenem
# Selbsttest aufrufen). Aufruf: pin_setzen.sh [<baum>]   (Standard: der r34a-Arbeitsbaum)
set -euo pipefail
B="${1:-C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android}"
cd "$B"
sha="$(/c/Python310/python -c "import hashlib,sys; print(hashlib.sha256(open(sys.argv[1],'rb').read()).hexdigest())" release/apk_asset_gate.py)"
[[ "$sha" =~ ^[0-9a-f]{64}$ ]] || { echo "sha unlesbar: $sha" >&2; exit 1; }
cat > release/apk_asset_gate.sha256 <<EOF
# Festgehaltene sha256 von release/apk_asset_gate.py (Zeilenenden LF, release/.gitattributes) - Nachbesserung R4-1,
# Gegenpruefung H1/H2 (analysis/befunde_runde34_android/android_r4_nachbesserung.md). release/apk_pruefen.sh
# (build_android.sh, make_package.sh) fuehrt das Asset-Gate NUR aus, wenn die sha256 seiner privaten Kopie diese ist:
# ein leeres, abgeschnittenes, veraendertes oder altes Gate gab bis dahin Rueckgabe 0 und liess jede Pruefung durch.
# Gate mit Absicht geaendert: erst  "\$PY" release/apk_asset_gate.py --selbsttest  (SELBSTTEST-OK), dann den Wert von
#   sha256sum release/apk_asset_gate.py
# hier eintragen und zusammen mit der Gate-Aenderung committen. Keine Umgebungsvariable ersetzt diese Datei.
$sha
EOF
echo "Pin: $sha"
