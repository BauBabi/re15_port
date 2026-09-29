#!/usr/bin/env bash
# check_tree-Sonde: die ECHTE Funktion aus einem make_package.sh gegen ein Schattenpaket
# $1 = make_package.sh (Fassung), $2 = Repo, $3 = Schattenpaket
set -euo pipefail
SKRIPT="$1"; REPO="$2"; OUT="$3"
eval "$(awk '/^die\(\) \{/{print} /^check_tree\(\) \{/{f=1} f{print} f&&/^}$/{f=0}' "$SKRIPT")"
eval "$(grep -E '^(RE2|RE15DOOR|SYNCHRO)=' "$SKRIPT")"
eval "$(grep -E '^FX_REQUIRED=' "$SKRIPT")"
check_tree "$OUT"
echo "CHECK_TREE_OK"
