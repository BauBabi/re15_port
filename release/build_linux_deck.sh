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
        -h|--help)   sed -n '2,30p' "${BASH_SOURCE[0]}"; exit 0 ;;
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

# ⛔ GIT-BASH SCHREIBT UNIX-PFADE IN ARGUMENTEN UM. Aus `-w /src` wird dort
# `C:/Program Files/Git/src`, und Docker bricht mit "working directory ... is invalid"
# ab (gemessen 2026-09-01). MSYS_NO_PATHCONV=1 schaltet das ab; zusaetzlich braucht jede
# Host-Pfad-Angabe (Volume-Quelle, Dockerfile, Kontext) unter Windows einen Windows-Pfad.
# Auf Linux/Deck aendert beides nichts.
hp() { printf '%s' "$1"; }
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*)
        command -v cygpath >/dev/null 2>&1 && hp() { cygpath -w "$1"; }
        export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*'
        ;;
esac

T0=$(date +%s)
if [[ -z "$IMAGE" ]]; then
    # Das vorgebaute Image ersetzt die apt-/cmake-Installation, die frueher bei JEDEM
    # Lauf neu lief (gemessen 81 s). Docker-Schichtcache: ohne Aenderung an
    # release/docker/linux_deps.sh dauert dieser Schritt Sekunden.
    IMAGE=re15-linux-build:deb11
    echo "== Bau-Image $IMAGE (release/docker/Dockerfile.linux) =="
    "$RUNNER" build -q -t "$IMAGE" -f "$(hp "$HERE/docker/Dockerfile.linux")" "$(hp "$HERE/docker")" >/dev/null
    echo "== Phase Image: $(( $(date +%s) - T0 )) s"
fi

if [[ "$MODE" == mount ]]; then
    echo "== Bauen in $RUNNER ($IMAGE), Repo direkt als /src eingehaengt (--mount) =="
    exec "$RUNNER" run --rm -v "$(hp "$REPO"):/src" -w /src "$IMAGE" \
        bash /src/release/docker_linux_build.sh
fi

# --- Kopie-Modus ---------------------------------------------------------------
# KOPIERT wird, was Bau und Tests in Masse lesen (per strace -y aus einem vollen
# Lauf ermittelt, Dossier nachtrag-linux-bau.md). Alles andere erreicht der Container
# ueber die Rueckfall-Links — langsam, aber vollstaendig. Ein Eintrag hier aendert
# also nur die Geschwindigkeit, nie das Ergebnis.
KOPIE=(
    re15_port
    synchro
    info/re2leon/PSX.EXE
    info/re2leon/PL0/RDT
    info/re2leon/COMMON/DOOR
    info/re2leon/COMMON/DATA
    info/re2leon/COMMON/BIN
    info/Re1.5/PSX.EXE
    release/docker_linux_build.sh
    release/docker
)
# NIE verlinken: das Bauverzeichnis gehoert ins Container-Dateisystem, linux_out wird
# eingehaengt.
NIE=( release/lbuild release/linux_out )

command -v git >/dev/null 2>&1 && ( cd "$REPO" && git rev-parse --git-dir >/dev/null 2>&1 ) || {
    echo "Kopie-Modus braucht ein git-Repo (Dateiliste). Ohne: $0 --mount" >&2; exit 1; }

STAGE="$(mktemp -d "${TMPDIR:-/tmp}/re15_stage.XXXXXX")"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/.re15_stage"

# 1) Dateiliste: versioniert + unversioniert-nicht-ignoriert. Versionierte, aber im
#    Arbeitsbaum geloeschte Dateien fallen heraus (die gibt es auf dem Host auch nicht).
: > "$STAGE/files.lst"
while IFS= read -r -d '' f; do
    [[ -e "$REPO/$f" ]] && printf '%s\0' "$f" >> "$STAGE/files.lst"
done < <(cd "$REPO" && git ls-files -z --cached --others --exclude-standard -- "${KOPIE[@]}")

# 2) Rueckfall-Links (NUL-getrennt).
auf_kopiepfad() {   # 0, wenn $1 ein KOPIE-Pfad oder ein Vorfahr eines KOPIE-Pfads ist
    local k
    for k in "${KOPIE[@]}"; do
        [[ "$k" == "$1" || "$k" == "$1/"* ]] && return 0
    done
    return 1
}
nie() {
    local n
    for n in "${NIE[@]}"; do
        [[ "$1" == "$n" || "$1" == "$n/"* ]] && return 0
    done
    return 1
}
LINKS="$STAGE/.re15_stage/host_links.lst"
: > "$LINKS"
link() { nie "$1" || printf '%s\0' "$1" >> "$LINKS"; }
declare -A ELTERN=([.]=1)
for k in "${KOPIE[@]}"; do
    d="$k"
    while [[ "$d" == */* ]]; do d="${d%/*}"; ELTERN[$d]=1; done
done
shopt -s dotglob nullglob
for d in "${!ELTERN[@]}"; do          # a) oberste Ebene + b) Geschwister teilweise kopierter Verzeichnisse
    if [[ "$d" == . ]]; then base="$REPO"; pre=""; else base="$REPO/$d"; pre="$d/"; fi
    for e in "$base"/*; do
        rel="$pre${e##*/}"
        auf_kopiepfad "$rel" || link "$rel"
    done
done
shopt -u dotglob nullglob
while IFS= read -r -d '' f; do       # c) git-ignorierte Eintraege in kopierten Verzeichnissen
    link "${f%/}"
done < <(cd "$REPO" && git ls-files -z --others --ignored --exclude-standard --directory -- "${KOPIE[@]}")

N_DATEIEN=$(tr -cd '\0' < "$STAGE/files.lst" | wc -c)
N_LINKS=$(tr -cd '\0' < "$LINKS" | wc -c)
tar -cf "$STAGE/src.tar" -C "$STAGE" .re15_stage -C "$REPO" --null -T "$STAGE/files.lst"
echo "== Phase Quellbaum packen: $(( $(date +%s) - T0 )) s  ($N_DATEIEN Dateien, $(( $(wc -c < "$STAGE/src.tar") / 1048576 )) MB, $N_LINKS Rueckfall-Links)"

mkdir -p "$REPO/release/linux_out"
echo "== Bauen in $RUNNER ($IMAGE), Quellbaum als Kopie im Container =="
RC=0
"$RUNNER" run --rm -i \
    -v "$(hp "$REPO"):/host" \
    -v "$(hp "$REPO/release/linux_out"):/src/release/linux_out" \
    -w /src ${RE15_CTEST_JOBS:+-e RE15_CTEST_JOBS="$RE15_CTEST_JOBS"} "$IMAGE" \
    bash -c 't=$(date +%s); tar -C /src -xf - && echo "== Phase Quellbaum auspacken: $(( $(date +%s) - t )) s" && exec bash /src/release/docker_linux_build.sh' \
    < "$STAGE/src.tar" || RC=$?
echo "== Wrapper gesamt: $(( $(date +%s) - T0 )) s (rc=$RC)"
exit "$RC"
