#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Windows-Release-exe als Docker-Cross-Bau (Host-Wrapper)
# =============================================================================
# Baut das Image re15-wincross-build:deb11 (release/docker/Dockerfile.wincross =
# debian:11 + win_deps.sh) und startet darin release/docker_win_build.sh mit dem
# Quellbaum als KOPIE im Container-Dateisystem (release/docker/kopie_lauf.sh).
# Ergebnis: release/win_out/re15_pc.exe  (Eingabe fuer release/make_package.sh)
#
# Frueher: docker run -v <repo>:/src debian:11 bash /src/release/docker_win_build.sh
# — kalt 45+ min in den SDL2-Pruefungen (~10-19 s je Pruefung ueber den
# 9p-Bindmount, Dossier analysis/befunde_runde30/nachtrag-linux-bau.md), weshalb
# Release-Arbeitsbaeume den warmen release/wxbuild-Cache aus dem Haupt-Checkout
# hineinkopieren mussten. Mit der Kopie ist der Kaltbau der Normalfall.
#
#   --image NAME   NAME unveraendert nehmen (z.B. debian:11: installiert wie frueher)
#   --mount        alter Weg: Repo direkt als /src einhaengen (Vergleich, Fehlersuche)
#   --version V    Version fuers Titelbild (sonst Umgebung RE15_VERSION, sonst letzter Tag + " dev")
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
IMAGE="${RE15_BUILD_IMAGE_WIN:-}"
MODE=kopie

while [[ $# -gt 0 ]]; do
    case "$1" in
        --image) IMAGE="$2"; shift 2 ;;
        --mount) MODE=mount; shift ;;
        --version) [[ -n "${2:-}" ]] || { echo "--version braucht einen Wert" >&2; exit 2; }
                   RE15_VERSION="$2"; shift 2 ;;
        -h|--help) sed -n '2,18p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done
# Runde 39 Spur T: Version ins Titelbild. Der Container hat kein .git - ohne Angabe bestimmt sie der Host
# (wie re15_port/cmake/re15_version_r39.cmake: letzter Tag vX.Y.Z + " dev").
if [[ -z "${RE15_VERSION:-}" ]]; then
    RE15_VERSION="$(cd "$REPO" && git describe --tags --abbrev=0 --match 'v[0-9]*' 2>/dev/null || true)"
    if [[ -n "$RE15_VERSION" ]]; then RE15_VERSION="$RE15_VERSION dev"; fi
fi
export RE15_VERSION
echo "== RE15_VERSION (Titelbild): '${RE15_VERSION:-}' =="

RUNNER=""
command -v docker >/dev/null 2>&1 && docker info >/dev/null 2>&1 && RUNNER=docker
[[ -z "$RUNNER" ]] && command -v podman >/dev/null 2>&1 && RUNNER=podman
[[ -n "$RUNNER" ]] || { echo "Weder docker noch podman nutzbar." >&2; exit 1; }

# shellcheck source=docker/kopie_lauf.sh
source "$HERE/docker/kopie_lauf.sh"

if [[ -z "$IMAGE" ]]; then
    IMAGE=re15-wincross-build:deb11
    bild_bauen "$HERE/docker/Dockerfile.wincross" "$IMAGE"
fi

# Ohne Tests (RE15_BUILD_TESTS=OFF) liest der Bau nur re15_port (Quellen + die
# Existenzpruefung von shared_assets/PSX im Configure). Alles andere: Rueckfall-Links.
# Port-Wahl, keine Original-Adresse (Bau-Umgebung; gemessen 4613 Dateien, 333 MB).
KOPIE=(
    re15_port
    release/docker_win_build.sh
    release/docker
)
NIE=( release/wxbuild release/win_out )
SKRIPT=release/docker_win_build.sh
AUSGABE=release/win_out
ENV_WEITER=( RE15_VERSION )   # Runde 39 Spur T: -> docker_win_build.sh -> cmake (Umgebung RE15_VERSION)
container_lauf
