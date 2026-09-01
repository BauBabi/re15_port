#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Linux-/Deck-Release-Binary bauen (Host-Wrapper)
# =============================================================================
# Waehlt die Bauumgebung und ruft darin release/docker_linux_build.sh auf.
# Ergebnis: release/linux_out/re15_pc  (Eingabe fuer release/make_package.sh)
#
# Reihenfolge:
#   1. --distrobox NAME (oder $RE15_BUILD_BOX): vorhandene distrobox nutzen —
#      das ist der Weg AUF DEM DECK selbst (SteamOS-Wurzel ist read-only, es
#      gibt dort weder gcc noch cmake).
#   2. docker/podman mit debian:11 (glibc 2.31 = Steam-Runtime-3.0-Stand).
#
# Warum nicht einfach auf dem Host bauen: SteamOS 3.7 hat glibc 2.41; das
# Binary liefe dann NUR auf gleich neuen Systemen. Siehe docker_linux_build.sh.
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
BOX="${RE15_BUILD_BOX:-}"
IMAGE="${RE15_BUILD_IMAGE:-debian:11}"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --distrobox) BOX="$2"; shift 2 ;;
        --image)     IMAGE="$2"; shift 2 ;;
        -h|--help)   sed -n '2,20p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done

if [[ -n "$BOX" ]]; then
    command -v distrobox >/dev/null 2>&1 || { echo "distrobox fehlt" >&2; exit 1; }
    echo "== Bauen in distrobox '$BOX' =="
    exec distrobox enter --no-tty "$BOX" -- bash -lc \
        "set -e; cd '$REPO' && bash release/docker_linux_build.sh"
fi

RUNNER=""
command -v docker >/dev/null 2>&1 && docker info >/dev/null 2>&1 && RUNNER=docker
[[ -z "$RUNNER" ]] && command -v podman >/dev/null 2>&1 && RUNNER=podman
[[ -n "$RUNNER" ]] || {
    echo "Weder docker noch podman nutzbar und keine --distrobox angegeben." >&2
    echo "Auf dem Deck:  $0 --distrobox re15-build" >&2
    exit 1
}

echo "== Bauen in $RUNNER ($IMAGE) =="
# ⛔ GIT-BASH SCHREIBT UNIX-PFADE IN ARGUMENTEN UM. Aus `-w /src` wird dort
# `C:/Program Files/Git/src`, und Docker bricht mit "working directory ... is invalid"
# ab (gemessen 2026-09-01). MSYS_NO_PATHCONV=1 schaltet das ab; zusaetzlich braucht die
# Volume-Quelle unter Windows einen Windows-Pfad. Auf Linux/Deck aendert beides nichts.
MOUNT="$REPO"
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*)
        command -v cygpath >/dev/null 2>&1 && MOUNT="$(cygpath -w "$REPO")"
        export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*'
        ;;
esac
exec "$RUNNER" run --rm -v "${MOUNT}:/src" -w /src "$IMAGE" \
    bash /src/release/docker_linux_build.sh
