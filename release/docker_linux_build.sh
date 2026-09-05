#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Linux-/Steam-Deck-Release-Build (laeuft IN der Bauumgebung)
# =============================================================================
# BASIS: Debian 11 (glibc 2.31, GCC 10) — derselbe Stand wie das offizielle
# Steam-Runtime-3.0-SDK ("sniper"), gegen das Steam-Spiele auf dem Deck laufen.
#
# NICHT ubuntu:22.04 verwenden. Damit gebaut verlangte das v0.1.1-Binary
# GLIBC_2.34; SteamOS 3.4 liefert nur 2.33, das Spiel startet dort gar nicht.
# Auf Debian-11-Basis kommt das gleiche Binary mit GLIBC_2.29 aus.
# Deshalb existiert dazu ein Gate in release/make_package.sh (check_glibc).
#
# Aufruf (Host): release/build_linux_deck.sh — der waehlt Container/distrobox
# und ruft dieses Skript hier drinnen auf. Direkt geht auch:
#   podman run --rm -v "$PWD:/src" -w /src debian:11 bash release/docker_linux_build.sh
# =============================================================================
set -euo pipefail

if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
fi

# SDL2 2.28.5 wird statisch aus Quelle gebaut (FetchContent) und braucht die
# Entwicklungs-Header der Anzeige-/Audio-Stacks. In einer frischen Container-
# Basis fehlen sie; in einer bereits eingerichteten distrobox nicht — deshalb
# nur installieren, wenn wir root sind und der Compiler noch fehlt.
# (libdecor-0-dev gibt es in Debian 11 NICHT — SDL2 faellt dort auf den
# eingebauten Wayland-Dekorationspfad zurueck.)
if [[ "$(id -u)" == "0" ]] && ! command -v gcc >/dev/null 2>&1; then
    export DEBIAN_FRONTEND=noninteractive
    # ⛔ SNAPSHOT STATT LEBENDEM SPIEGEL (2026-09-04). Debian 11 ist in Rente; der
    # normale Spiegel liefert fuer bullseye noch einen INDEX, aber nicht mehr jedes
    # darin genannte Paket. Fuenfmal reproduziert, immer dieselben zwei Dateien:
    #     E: Failed to fetch .../libglib2.0-bin_2.66.8-1+deb11u8_amd64.deb  404
    #     E: Failed to fetch .../libglx-mesa0_20.3.5-1+deb11u1_amd64.deb    404
    # snapshot.debian.org haelt JEDEN historischen Stand dauerhaft vor - Index und
    # Pool passen dort per Konstruktion zusammen. Das macht den Release-Build zugleich
    # REPRODUZIERBAR: derselbe Zeitstempel liefert in einem Jahr dieselben Pakete.
    # Der Zeitstempel liegt in der Lebenszeit von bullseye/LTS und enthaelt die oben
    # genannten Versionen. Check-Valid-Until muss aus, weil die Release-Datei des
    # Snapshots aus Sicht von heute abgelaufen ist.
    # Der Zeitstempel muss MINDESTENS so neu sein wie das Basis-Image, sonst
    # verlangt es Herabstufungen: debian:11 (11.11) traegt libudev1 in deb11u8,
    # der Snapshot vom 2025-02-01 kennt nur deb11u6 ->
    #   libudev-dev : Depends: libudev1 (= 247.3-7+deb11u6) but ...u8 is to be installed
    # Durchprobiert mit der VOLLEN Paketliste: 2025-02-01 und 2026-01-01 fallen an
    #   libudev-dev : Depends: libudev1 (= ...deb11u6) but ...u8 is to be installed
    # 2026-06-01 faellt an
    #   libc6-dev : Depends: libc6 (= 2.31-13+deb11u13) but ...u14 is to be installed
    # 2026-07-01, -08-01 und -09-01 gehen vollstaendig durch. Genommen: der aelteste
    # davon, damit der Stand so nah wie moeglich am ausgelieferten Deck-Binary bleibt.
    SNAP=20260701T000000Z
    { echo "deb http://snapshot.debian.org/archive/debian/$SNAP/ bullseye main"
      echo "deb http://snapshot.debian.org/archive/debian-security/$SNAP/ bullseye-security main"
    } > /etc/apt/sources.list
    echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99snapshot
    echo 'Acquire::Retries "5";' >> /etc/apt/apt.conf.d/99snapshot
    PKGS="build-essential ninja-build git ca-certificates wget libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxss-dev libxfixes-dev libwayland-dev libxkbcommon-dev wayland-protocols libasound2-dev libpulse-dev libdbus-1-dev libudev-dev libgl1-mesa-dev libegl1-mesa-dev"
    apt-get update -qq
    if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
        # ⛔ DEBIAN 11 IST INS ARCHIV GEWANDERT (2026-09-04 gemessen). Der normale
        # Spiegel liefert fuer bullseye noch einen Index, aber nicht mehr jedes darin
        # genannte Paket:
        #   E: Failed to fetch .../libglib2.0-bin_2.66.8-1+deb11u8_amd64.deb  404
        # Zweimal hintereinander reproduziert, also kein Aussetzer. Das ist ein
        # Infrastruktur-Fehler, kein Code-Fehler - aber er legt den Release-Build lahm,
        # und der Linux-Build ist Pflicht (ein Paket nur fuer Windows hat schon einmal
        # zwoelf Releases lang das Deck-Paket mitgerissen, Memory
        # reai-v2-paket-plattform-kollateral).
        # archive.debian.org haelt alle bullseye-Pakete dauerhaft vor; Check-Valid-Until
        # muss dafuer aus, weil die Release-Datei abgelaufen ist. Eine eigene
        # Security-Suite fuehrt das Archiv NICHT mehr (gemessen: "does not have
        # a Release file") - die Updates sind ins Hauptarchiv gefaltet.
        # ⛔ ERST DEN INDEX WEGWERFEN. Der 404 nennt eine Paketversion, die im Pool
        # nicht mehr liegt (deb11u8) - das ist das Muster eines VERALTETEN Index im
        # Basis-Image, den ein blosses "apt-get update" nicht ersetzt. Gemessen
        # 2026-09-04: zweimal derselbe 404, danach mit geleerter Liste erneut versucht.
        echo "   apt-Index unvollstaendig - Listen leeren und neu laden"
        rm -rf /var/lib/apt/lists/*
        apt-get clean
        apt-get update -qq
        if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
            echo "   apt-Spiegel unvollstaendig - schalte auf archive.debian.org um"
            # ⛔ NUR DAS HAUPTARCHIV. Eine eigene Security-Suite fuehrt
            # archive.debian.org fuer bullseye unter KEINEM Namen mehr - beide
            # geprueft, beide 'does not have a Release file':
            #     debian-security bullseye-security
            #     debian-security bullseye/updates
            echo 'deb http://archive.debian.org/debian bullseye main' > /etc/apt/sources.list
            echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99archive
            apt-get update -qq
            # Das Basis-Image traegt Security-Versionen, die das Archiv nicht kennt;
            # ohne diese Freigaben endet es in 'held broken packages'. Der Container
            # ist ein Wegwerf-Container, das Spiel-Binary wird davon nicht beruehrt
            # (das glibc-Gate prueft es weiterhin einzeln).
            apt-get install -y -qq --no-install-recommends --allow-downgrades --allow-change-held-packages $PKGS >/dev/null
        fi
    fi
fi
# cmake: Debian 11 (bullseye) liefert nur 3.18, re15_port verlangt >=3.21, und
# bullseye-backports ist inzwischen archiviert (kein Release-File). Deshalb das
# offizielle Kitware-Binary-Tarball (statisch, aendert die glibc-Anforderung
# des SPIELS nicht — Gate check_glibc prueft weiterhin das Binary).
if ! cmake --version 2>/dev/null | grep -qE ' 3\.(2[1-9]|[3-9][0-9])| [4-9]\.'; then
    CMV=3.28.6
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMV}/cmake-${CMV}-linux-x86_64.tar.gz" -O /tmp/cmake.tgz
    tar -xzf /tmp/cmake.tgz -C /opt
    export PATH="/opt/cmake-${CMV}-linux-x86_64/bin:$PATH"
fi

BUILD="$REPO/release/lbuild"
cmake -S "$REPO/re15_port" -B "$BUILD" -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX"
cmake --build "$BUILD" -j"$(nproc)"

# ⛔ DIE SUMMENZEILE MUSS DA SEIN - "tail -3" hat sie am 2026-09-05 verschluckt.
# Ein Container-Lauf meldete nur "The following tests FAILED: 144 - ..." und ich konnte
# NICHT sehen, wie viele Haken ueberhaupt liefen. Schlimmer: laeuft ctest ins Leere
# ("No tests were found!!!"), beendet es mit EXIT 0 - falsches Gruen, genau die Falle,
# die local_build.sh fuer den lokalen Bau schon abfaengt (CLAUDE.md). Deshalb wird die
# Ausgabe vollstaendig gesichert, die Summenzeile ausgegeben UND ihr Vorhandensein
# erzwungen.
( cd "$BUILD" && ctest --timeout 600 --output-on-failure > ctest_out.txt 2>&1 ) || CT_RC=$?
CT_RC="${CT_RC:-0}"
grep -aE "tests passed|Total Test time" "$BUILD/ctest_out.txt" || true
grep -aA20 "The following tests FAILED" "$BUILD/ctest_out.txt" || true
if ! grep -aqE "[0-9]+% tests passed, [0-9]+ tests failed out of [0-9]+" "$BUILD/ctest_out.txt"; then
    echo "!!! ctest lieferte KEINE Summenzeile - kein Gruen ohne Zaehlung" >&2
    tail -20 "$BUILD/ctest_out.txt" >&2
    exit 1
fi
if [[ "$CT_RC" != "0" ]]; then
    echo "!!! ctest fehlgeschlagen (exit=$CT_RC)" >&2
    exit "$CT_RC"
fi

mkdir -p "$REPO/release/linux_out"
cp "$BUILD/platform/pc/re15_pc" "$REPO/release/linux_out/"
ldd    "$REPO/release/linux_out/re15_pc" > "$REPO/release/linux_out/ldd.txt" 2>&1 || true
objdump -T "$REPO/release/linux_out/re15_pc" 2>/dev/null \
    | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1 \
    > "$REPO/release/linux_out/glibc_max.txt" || true

echo "hoechste glibc-Anforderung: $(cat "$REPO/release/linux_out/glibc_max.txt" 2>/dev/null)"
echo LINUX-BUILD-OK
