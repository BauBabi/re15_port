# =============================================================================
# RE1.5-Port — Bauumgebung fuer das Steam Deck
# =============================================================================
# Basis ist das offizielle Steam-Runtime-3.0-SDK ("sniper"): Debian 11 mit
# GCC 10.3 und glibc 2.31 — exakt die Bibliotheksbasis, gegen die Steam-Spiele
# auf dem Deck laufen. Wer gegen ein neueres System baut, bekommt ein Binary,
# das auf SteamOS mit "GLIBC_2.3x not found" abbricht.
#
# Bauen:
#   docker build -t re15-deck re15_port/targets/steamdeck
#   docker run --rm -v "$PWD:/src" re15-deck            # baut nach targets/steamdeck/build
#
# Ergebnis: build/platform/pc/re15_pc — dynamisch nur gegen libc/libm/libdl/
# libpthread gelinkt (SDL2 steckt statisch drin), benoetigt maximal GLIBC_2.29.
# =============================================================================
FROM registry.gitlab.steamos.cloud/steamrt/sniper/sdk:latest

# SDL2 wird aus Quelle gebaut (FetchContent) und braucht die Entwicklungs-Header
# der Anzeige-/Audio-Stacks. Im SDK-Image sind sie nicht vorinstalliert.
RUN apt-get update -qq \
 && apt-get install -y -qq --no-install-recommends \
      libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxss-dev \
      libwayland-dev libxkbcommon-dev wayland-protocols \
      libasound2-dev libpulse-dev \
      libgl1-mesa-dev libegl1-mesa-dev \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY build.sh /usr/local/bin/re15-build
RUN chmod +x /usr/local/bin/re15-build

ENTRYPOINT ["/usr/local/bin/re15-build"]
