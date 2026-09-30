set -euo pipefail
REPO="$1"; HERE="$2"; NAME="$3"; DO_ZIP=1
die() { echo "ABBRUCH: $*" >&2; exit 1; }
source "$REPO/release/python_finden.sh" || die "kein Python"
eval "$(awk '/^APK_PRUEF=/{f=1} f{print} f&&/^fi$/{exit}' "$REPO/release/make_package.sh")"
echo "APK-BLOCK-OK"
