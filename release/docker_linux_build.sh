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
# und ruft dieses Skript hier drinnen auf. Direkt geht auch (unter Windows LANGSAM,
# siehe DATEIZUGRIFF):
#   podman run --rm -v "$PWD:/src" -w /src debian:11 bash release/docker_linux_build.sh
#
# ⛔ DATEIZUGRIFF (2026-09-28 gemessen, analysis/befunde_runde30/nachtrag-linux-bau.md).
# Unter Docker Desktop (WSL2) ist ein Windows-Bindmount ein 9p-Mount (drvfs,
# msize=65536): 5000x stat 54,4 s statt 3,3 s, 2000 kleine Dateien schreiben 93,6 s
# statt 1,1 s, 315 MB shared_assets lesen 142 s statt 0,17 s. Der Bau schreibt und
# liest aber genau das: SDL2-Konfigurations-Pruefungen, Objektdateien, Header, die
# Test-Assets. build_linux_deck.sh legt deshalb den Quellbaum als KOPIE in das
# Container-Dateisystem unter /src (gleicher Pfad wie frueher -> gleiche eingebaute
# Pfade im Binary) und haengt das Repo nur als Rueckfall unter /host ein. Alles, was
# nicht kopiert wurde, wird hier als Link /src/<pfad> -> /host/<pfad> angelegt: eine
# Datei, die es im Repo gibt, fehlt im Container NIE (kein stilles SKIP).
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -d /src/re15_port ]]; then
    REPO=/src
else
    REPO="$(cd "$HERE/.." && pwd)"
fi

# Dauer je Phase (die Frage "warum dauert der Linux-Bau ueber eine Stunde" war ohne
# diese Zahlen nicht zu beantworten).
T_START=$(date +%s); T_PHASE=$T_START; PHASEN=""
phase() {
    local now; now=$(date +%s)
    echo "== Phase $1: $(( now - T_PHASE )) s  (gesamt $(( now - T_START )) s)"
    PHASEN="${PHASEN}$1=$(( now - T_PHASE ))s "
    T_PHASE=$now
}
die() { echo "!!! $*" >&2; exit 1; }

# --- Kopierter Quellbaum: Rueckfall-Links auf das eingehaengte Repo -----------
# shellcheck source=docker/rueckfall_links.sh
source "$HERE/docker/rueckfall_links.sh"

# Ein fehlgeschlagener Lauf darf kein altes Binary liegen lassen, das make_package.sh
# klaglos einpackt (Memory reai-v2-releasebau-pipe-schluckt-fehler: v0.8.15 lag nach
# einem roten ctest noch das Binary des VORIGEN Release in linux_out).
OUT="$REPO/release/linux_out"
mkdir -p "$OUT"
rm -rf "$OUT/re15_pc" "$OUT/ldd.txt" "$OUT/glibc_max.txt" "$OUT/diag"

# Bau-Abhaengigkeiten (apt aus snapshot.debian.org + cmake 3.28.6). Im vorgebauten
# Image (release/docker/Dockerfile.linux) und in einer distrobox ist alles schon da.
# shellcheck source=docker/linux_deps.sh
source "$HERE/docker/linux_deps.sh"
phase Pakete

BUILD="$REPO/release/lbuild"
[[ -L "$BUILD" ]] && die "$BUILD ist ein Link - der Bau muss im Container-Dateisystem laufen"
mkdir -p "$BUILD"
echo "   Dateisystem: Quelle $(stat -f -c %T "$REPO/re15_port"), Bau $(stat -f -c %T "$BUILD"), nproc $(nproc)"
cmake -S "$REPO/re15_port" -B "$BUILD" -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX"
phase Configure
cmake --build "$BUILD" -j"$(nproc)"
phase Compile+Link

# ⛔ DIE SUMMENZEILE MUSS DA SEIN - "tail -3" hat sie am 2026-09-05 verschluckt.
# Ein Container-Lauf meldete nur "The following tests FAILED: 144 - ..." und ich konnte
# NICHT sehen, wie viele Haken ueberhaupt liefen. Schlimmer: laeuft ctest ins Leere
# ("No tests were found!!!"), beendet es mit EXIT 0 - falsches Gruen, genau die Falle,
# die local_build.sh fuer den lokalen Bau schon abfaengt (CLAUDE.md). Deshalb wird die
# Ausgabe vollstaendig gesichert, die Summenzeile ausgegeben UND ihr Vorhandensein
# erzwungen.
CTEST_JOBS="${RE15_CTEST_JOBS:-1}"
# Kein Grafikgeraet im Container (Mesa-Software-GL): Bildpruefungen, die eine echte GPU
# brauchen, melden sich damit SICHTBAR als uebersprungen (integration_r30_titel_puls);
# ihre Engine-Pruefungen bleiben Pflicht. Unter Windows (echte GPU) laufen sie voll.
export RE15_OHNE_GPU=1
( cd "$BUILD" && ctest --timeout 600 -j"$CTEST_JOBS" --output-on-failure > ctest_out.txt 2>&1 ) || CT_RC=$?
CT_RC="${CT_RC:-0}"
phase ctest
# Diagnose IMMER nach draussen (auch bei Rot): Ausgabe, volles Protokoll, Fingerabdruck.
DIAG="$OUT/diag"
mkdir -p "$DIAG"
cp "$BUILD/ctest_out.txt" "$DIAG/" 2>/dev/null || true
cp "$BUILD/Testing/Temporary/LastTest.log" "$DIAG/" 2>/dev/null || true
cp "$BUILD/.ninja_log" "$DIAG/ninja_log.txt" 2>/dev/null || true
bash "$HERE/docker/ctest_fingerprint.sh" "$BUILD/Testing/Temporary/LastTest.log" "$DIAG/ctest_fingerprint" || true
grep -aE "tests passed|Total Test time" "$BUILD/ctest_out.txt" || true
grep -aA20 "The following tests FAILED" "$BUILD/ctest_out.txt" || true
grep -aA20 "The following tests did not run" "$BUILD/ctest_out.txt" || true
if ! grep -aqE "[0-9]+% tests passed, [0-9]+ tests failed out of [0-9]+" "$BUILD/ctest_out.txt"; then
    echo "!!! ctest lieferte KEINE Summenzeile - kein Gruen ohne Zaehlung" >&2
    tail -20 "$BUILD/ctest_out.txt" >&2
    exit 1
fi
if [[ "$CT_RC" != "0" ]]; then
    # Das Binary eines roten Laufs kommt NICHT nach linux_out/re15_pc (make_package.sh
    # liest nur das). Zur Diagnose/zum Vergleich liegt es unter eindeutigem Namen in
    # diag/ — der Bau-Container ist nach dem Lauf weg, mit ihm das Bauverzeichnis.
    cp "$BUILD/platform/pc/re15_pc" "$DIAG/re15_pc.UNGEPRUEFT-ctest-rot" 2>/dev/null || true
    echo "!!! ctest fehlgeschlagen (exit=$CT_RC)" >&2
    exit "$CT_RC"
fi
# Gelaufen = registriert: eine Suite, die weniger Tests AUSFUEHRT als konfiguriert
# sind, ist kein Gruen. Untergrenze = die von local_build.sh (EINE Zahl im Repo).
RAN="$(grep -aoE 'tests failed out of [0-9]+' "$BUILD/ctest_out.txt" | tail -1 | grep -oE '[0-9]+$' || true)"
REG="$(cd "$BUILD" && ctest -N | grep -aoE 'Total Tests: [0-9]+' | grep -oE '[0-9]+$' || true)"
[[ -n "$RAN" && -n "$REG" ]] || die "Testzahl nicht lesbar (gelaufen '$RAN', registriert '$REG')"
[[ "$RAN" == "$REG" ]] || die "ctest lief $RAN Tests, registriert sind $REG"
MIN="$(grep -oE 'RE15_MIN_TESTS:-[0-9]+' "$REPO/re15_port/tools/local_build.sh" 2>/dev/null | head -1 | grep -oE '[0-9]+$' || true)"
if [[ -n "$MIN" ]]; then
    [[ "$RAN" -ge "$MIN" ]] || die "nur $RAN Tests, local_build.sh verlangt >= $MIN - Suite kollabiert?"
fi
echo "   Tests: $RAN gelaufen = $REG registriert (Untergrenze ${MIN:-keine})"

cp "$BUILD/platform/pc/re15_pc" "$OUT/"
ldd    "$OUT/re15_pc" > "$OUT/ldd.txt" 2>&1 || true
objdump -T "$OUT/re15_pc" 2>/dev/null \
    | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1 \
    > "$OUT/glibc_max.txt" || true

echo "hoechste glibc-Anforderung: $(cat "$OUT/glibc_max.txt" 2>/dev/null)"
phase Ausgabe
echo "== Phasen: ${PHASEN}gesamt=$(( $(date +%s) - T_START ))s"
echo LINUX-BUILD-OK
