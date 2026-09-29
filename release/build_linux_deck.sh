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
#   2. docker/podman mit dem vorgebauten Image re15-linux-build:deb11
#      (release/docker/Dockerfile.linux = debian:11 + linux_deps.sh; glibc 2.31 =
#      Steam-Runtime-3.0-Stand). --image NAME nimmt stattdessen NAME unveraendert
#      (z.B. --image debian:11: dann installiert docker_linux_build.sh wie frueher).
#
# QUELLBAUM ALS KOPIE (2026-09-28, analysis/befunde_runde30/nachtrag-linux-bau.md):
# Der Linux-Bau dauerte ueber eine Stunde, weil unter Docker Desktop JEDER
# Dateizugriff auf den Windows-Bindmount ueber 9p geht (~10 ms je Operation,
# gemessen). Standard ist deshalb: Quellbaum auf dem Host per tar packen, in den
# Container streamen, dort unter /src auspacken (gleicher Pfad wie frueher, damit
# die eingebauten Pfade im Binary gleich bleiben). Das Repo haengt nur noch als
# Rueckfall unter /host; alles Nicht-Kopierte wird in docker_linux_build.sh als
# Link /src/<pfad> -> /host/<pfad> angelegt — es fehlt keine Datei, die es im Repo
# gibt. release/linux_out wird direkt eingehaengt (dort landet das Binary).
#   --mount   alter Weg: Repo direkt als /src einhaengen (Vergleichslaeufe, Fehlersuche)
#   RE15_CTEST_JOBS=N  ctest parallel (Opt-in, Standard seriell; -j8: ctest 101 s statt 392 s)
# Diagnose je Lauf (auch bei Rot): release/linux_out/diag/ (ctest-Ausgabe, LastTest.log,
# Fingerabdruck je Test, .ninja_log; bei rotem ctest das Binary als re15_pc.UNGEPRUEFT-...).
#
# Warum nicht einfach auf dem Host bauen: SteamOS 3.7 hat glibc 2.41; das
# Binary liefe dann NUR auf gleich neuen Systemen. Siehe docker_linux_build.sh.
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"
BOX="${RE15_BUILD_BOX:-}"
IMAGE="${RE15_BUILD_IMAGE:-}"
MODE=kopie

while [[ $# -gt 0 ]]; do
    case "$1" in
        --distrobox) BOX="$2"; shift 2 ;;
        --image)     IMAGE="$2"; shift 2 ;;
        --mount)     MODE=mount; shift ;;
        -h|--help)   sed -n '2,32p' "${BASH_SOURCE[0]}"; exit 0 ;;
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

# Image, Kopie, Rueckfall-Links, Pfadumschreibung unter Git-Bash: release/docker/kopie_lauf.sh
# shellcheck source=docker/kopie_lauf.sh
source "$HERE/docker/kopie_lauf.sh"

if [[ -z "$IMAGE" ]]; then
    # Das vorgebaute Image ersetzt die apt-/cmake-Installation, die frueher bei JEDEM
    # Lauf neu lief (gemessen 81 s bis zur ersten cmake-Zeile).
    IMAGE=re15-linux-build:deb11
    bild_bauen "$HERE/docker/Dockerfile.linux" "$IMAGE"
fi

# KOPIERT wird, was Bau und Tests lesen. Grundlage: die Pfade in den Tests
# (CMAKE_SOURCE_DIR/.. und relative Literale) plus ein Spurlauf mit strace -y, der
# jedes open ueber einen Rueckfall-Link protokolliert hat (2026-09-28: 149 opens,
# 128 Dateien, 2,3 MB — pri/STAGE1 und vier Dateien unter analysis/; Dossier
# nachtrag-linux-bau.md). Alles andere erreicht der Container ueber die
# Rueckfall-Links — langsam, aber vollstaendig. Ein Eintrag hier aendert also nur
# die Geschwindigkeit, nie das Ergebnis. Port-Wahl, keine Original-Adresse (Bau-Umgebung;
# gemessen: 18577 Dateien, 550 MB, packen 17 s + auspacken 14-15 s je Lauf).
KOPIE=(
    re15_port
    synchro
    pri
    info/re2leon/PSX.EXE
    info/re2leon/PL0/RDT
    info/re2leon/COMMON/DOOR
    info/re2leon/COMMON/DATA
    info/re2leon/COMMON/BIN
    info/Re1.5/PSX.EXE
    analysis/kartensymbole/symbolkatalog.csv
    analysis/befunde_2026-09-21/10f0-quader-silhouette/messung/pfad_10f0_aus_befundlog.txt
    analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr
    analysis/befunde_runde30/sicherung_werkzeug/soll
    release/docker_linux_build.sh
    release/docker
)
# NIE verlinken: das Bauverzeichnis gehoert ins Container-Dateisystem, linux_out wird
# eingehaengt.
NIE=( release/lbuild release/linux_out )
SKRIPT=release/docker_linux_build.sh
AUSGABE=release/linux_out
ENV_WEITER=( RE15_CTEST_JOBS )
container_lauf
