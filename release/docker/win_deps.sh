#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Bau-Abhaengigkeiten des Windows-Cross-Baus (Debian 11 + mingw-w64)
# =============================================================================
# EINE Quelle fuer Paketliste, Snapshot-Zeitstempel und cmake-Version, genutzt von
# release/docker/Dockerfile.wincross (vorgebautes Image, release/build_win_cross.sh)
# und per "source" von release/docker_win_build.sh (frisches debian:11: installiert
# wie bisher; im vorgebauten Image: tut nichts). Bis 2026-09-28 stand der Block
# woertlich in docker_win_build.sh und lief bei jedem Lauf neu (Dossier
# analysis/befunde_runde30/nachtrag-linux-bau.md).
# Bau-Umgebung, NICHT Spiel-Verhalten: keine Original-Adresse, keine Port-Konstante.
# =============================================================================
set -euo pipefail

if [[ "$(id -u)" == "0" ]] && ! command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    export DEBIAN_FRONTEND=noninteractive
    # ⛔ SNAPSHOT STATT LEBENDEM SPIEGEL - dieselbe Haertung wie in
    # release/docker/linux_deps.sh (frueher docker_linux_build.sh), hier am 2026-09-05 nachgezogen, nachdem der
    # Windows-Build an genau demselben Fehler starb:
    #     E: Failed to fetch .../libperl5.32_5.32.1-4+deb11u5_amd64.deb  404
    # Debian 11 ist in Rente: der normale Spiegel liefert fuer bullseye noch einen
    # INDEX, aber nicht mehr jedes darin genannte Paket. snapshot.debian.org haelt
    # jeden historischen Stand vor, Index und Pool passen dort per Konstruktion
    # zusammen - und der Release-Build wird dadurch reproduzierbar.
    # Zeitstempel und Begruendung: siehe release/docker/linux_deps.sh (2026-07-01 ist der
    # aelteste, der mit debian:11 (11.11) ohne Herabstufungen durchgeht).
    SNAP=20260701T000000Z
    { echo "deb http://snapshot.debian.org/archive/debian/$SNAP/ bullseye main"
      echo "deb http://snapshot.debian.org/archive/debian-security/$SNAP/ bullseye-security main"
    } > /etc/apt/sources.list
    echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99snapshot
    echo 'Acquire::Retries "5";' >> /etc/apt/apt.conf.d/99snapshot
    PKGS="gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 binutils-mingw-w64-x86-64 ninja-build git ca-certificates wget make"
    apt-get update -qq
    if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
        echo "   Snapshot-Spiegel unvollstaendig - raeume die Listen und versuche erneut"
        rm -rf /var/lib/apt/lists/*
        apt-get clean
        apt-get update -qq
        if ! apt-get install -y -qq --no-install-recommends $PKGS >/dev/null; then
            echo "   apt-Spiegel unvollstaendig - schalte auf archive.debian.org um"
            echo 'deb http://archive.debian.org/debian bullseye main' > /etc/apt/sources.list
            echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99archive
            apt-get update -qq
            apt-get install -y -qq --no-install-recommends \
                --allow-downgrades --allow-change-held-packages $PKGS >/dev/null
        fi
    fi
fi

# cmake wie im Linux-Skript: Kitware-Tarball (bullseye liefert nur 3.18).
if ! cmake --version 2>/dev/null | grep -qE ' 3\.(2[1-9]|[3-9][0-9])| [4-9]\.'; then
    CMV=3.28.6
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMV}/cmake-${CMV}-linux-x86_64.tar.gz" -O /tmp/cmake.tgz
    tar -xzf /tmp/cmake.tgz -C /opt
    export PATH="/opt/cmake-${CMV}-linux-x86_64/bin:$PATH"
fi
# Im vorgebauten Image muss cmake ohne das export oben auffindbar sein (ein RUN-Schritt
# vererbt seine Umgebung nicht): als root nach /usr/local/bin verlinken.
if [[ "$(id -u)" == "0" && -n "${CMV:-}" && -d "/opt/cmake-${CMV}-linux-x86_64/bin" ]]; then
    ln -sf "/opt/cmake-${CMV}-linux-x86_64/bin/"* /usr/local/bin/
fi
