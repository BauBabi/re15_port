#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port - ist die Container-Umgebung fuer die Release-Bauten bereit? (Runde 38, Spur V)
# =============================================================================
# build_win_cross.sh und build_linux_deck.sh brauchen docker (oder podman) mit laufender Engine
# (build_win_cross.sh:35-38, build_linux_deck.sh:59-66 - sonst "Weder docker noch podman nutzbar").
# Gemessen 2026-10-16: Docker Desktop lief nicht, `docker info` -> rc=1
#   "open //./pipe/dockerDesktopLinuxEngine: The system cannot find the file specified".
# Bisher hat der Mensch Docker Desktop von Hand gestartet (Runde 37 UEBERGABE: "Docker Desktop musste
# gestartet werden"). Dieses Skript macht das selbst - zuerst ueber die Docker-Desktop-CLI
# (`docker desktop start`, auf dieser Maschine CLI v0.1.4, wartet selbst bis bereit), sonst ueber das Programm
# "C:/Program Files/Docker/Docker/Docker Desktop.exe".
#
# AUFRUF
#   bash release/docker_bereit.sh            pruefen; laeuft nichts, Docker Desktop starten und warten,
#                                            bis `docker info` antwortet
#   bash release/docker_bereit.sh --pruefen  nur pruefen, nichts starten (Trockenlauf von release_alles.sh)
#   --warten SEK                             Obergrenze fuers Warten (Standard 300)
# Rueckgabe 0 = bereit, 1 = nicht bereit (mit --pruefen: aber Docker Desktop ist da und wuerde gestartet),
# 3 = nicht bereit und nichts zu starten (nur --pruefen), 2 = Aufruffehler. Meldung auf stderr.
#
# Wartegrenze: PORT-WAHL (Bau-Umgebung), keine Messgroesse. Gemessen dauerte ein Start 20 s
# (analysis/befunde_runde32/release_v0818.md:24, "15:08:18 gestartet, 15:08:38 bereit"); 300 s ist nur die
# Grenze, ab der aufgegeben wird - gepollt wird alle 5 s, ein schnellerer Start endet frueher.
# =============================================================================
set -euo pipefail

NUR_PRUEFEN=0
WARTEN=300
while [[ $# -gt 0 ]]; do
    case "$1" in
        --pruefen) NUR_PRUEFEN=1; shift ;;
        --warten)  [[ "${2:-}" =~ ^[0-9]+$ ]] || { echo "--warten braucht Sekunden" >&2; exit 2; }
                   WARTEN="$2"; shift 2 ;;
        -h|--help) awk 'NR == 1 { next } /^set -euo pipefail/ { exit } { print }' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done

# Zeitgrenzen 60 s (docker/podman info) und 30 s (docker desktop version): PORT-WAHL (Bau-Umgebung), keine Messgroesse.
# Gemessen (Abnahme 0, H2): bei gestopptem Docker Desktop kehrt `docker info` erst an der Grenze zurueck (rc 124) -
# ohne Grenze stuende der Trockenlauf so lange wie docker info.
engine_antwortet() {   # 0 und Name auf stdout, wenn docker oder podman eine laufende Engine hat
    if command -v docker >/dev/null 2>&1 && timeout 60 docker info >/dev/null 2>&1; then
        echo "docker $(docker version --format '{{.Server.Version}}' 2>/dev/null || true)"; return 0
    fi
    if command -v podman >/dev/null 2>&1 && timeout 60 podman info >/dev/null 2>&1; then
        echo "podman"; return 0
    fi
    return 1
}

if wer="$(engine_antwortet)"; then
    echo "Container-Engine bereit: $wer"
    exit 0
fi

# Wie liesse sich Docker Desktop starten?
WEG=""
DESKTOP=""
if command -v docker >/dev/null 2>&1 && timeout 30 docker desktop version >/dev/null 2>&1; then
    WEG=cli
fi
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*)
        for kand in "/c/Program Files/Docker/Docker/Docker Desktop.exe" \
                    "${ProgramW6432:+$(cygpath -u "$ProgramW6432" 2>/dev/null)/Docker/Docker/Docker Desktop.exe}"; do
            if [[ -n "$kand" && -f "$kand" ]]; then DESKTOP="$kand"; break; fi
        done ;;
esac
if [[ -z "$WEG" && -n "$DESKTOP" ]]; then WEG=programm; fi

if [[ $NUR_PRUEFEN -eq 1 ]]; then
    case "$WEG" in
        cli)      echo "Container-Engine antwortet nicht - ein echter Lauf startet Docker Desktop ('docker desktop start') und wartet bis ${WARTEN} s" >&2; exit 1 ;;
        programm) echo "Container-Engine antwortet nicht - ein echter Lauf startet Docker Desktop ($DESKTOP) und wartet bis ${WARTEN} s" >&2; exit 1 ;;
    esac
    echo "Container-Engine antwortet nicht (weder docker noch podman) und Docker Desktop ist nicht zu finden" >&2
    exit 3
fi

t0=$(date +%s)
case "$WEG" in
    cli)
        echo "Docker Desktop laeuft nicht - starte: docker desktop start --timeout $WARTEN"
        if ! timeout $((WARTEN + 30)) docker desktop start --timeout "$WARTEN"; then
            echo "   ('docker desktop start' meldete einen Fehler - warte trotzdem auf docker info)"
        fi ;;
    programm)
        echo "Docker Desktop laeuft nicht - starte: $DESKTOP"
        # Start-Process loest das Programm vom aufrufenden Fenster (ein '&' aus Git-Bash haengt es an diese Shell).
        powershell.exe -NoProfile -NonInteractive -Command "Start-Process -FilePath '$(cygpath -w "$DESKTOP")'" >/dev/null 2>&1 \
            || { echo "Docker Desktop liess sich nicht starten: $DESKTOP" >&2; exit 1; } ;;
    *)
        echo "Container-Engine antwortet nicht (weder docker noch podman). Docker/Podman starten und neu aufrufen." >&2
        exit 1 ;;
esac
while :; do
    if wer="$(engine_antwortet)"; then
        echo "Container-Engine bereit nach $(( $(date +%s) - t0 )) s: $wer"
        exit 0
    fi
    if (( $(date +%s) - t0 >= WARTEN )); then
        echo "Docker Desktop antwortet nach ${WARTEN} s noch nicht (docker info). Von Hand pruefen." >&2
        exit 1
    fi
    sleep 5
done
