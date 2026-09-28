#!/usr/bin/env bash
# Rauchstart eines Linux-Binarys im Container (Repo unter /src, damit der eingebaute
# Asset-Pfad /src/re15_port/shared_assets/PSX aufloest). Zwei Starts:
#   1. RE15_ASSET_SELFTEST=1 -> "[selftest] RESULT ..." in debug.log (wie make_package.sh)
#   2. Spielstart wie integration_boot_bg_pin: RE15_NO_INTRO, RE15_TITLE_SHOT (Titel
#      laeuft selbst weiter), RE15_INV_SHOT (sauberes exit(0) bei Spielbild 34)
#   (--headless und RE15_EXIT_AT allein kehren nicht zurueck - der Titel wartet auf
#    Eingabe: rc=124 nach 120 s bei ALTEM und NEUEM Binary gleich, gemessen 2026-09-29.)
# Aufruf: rauchstart.sh <binary> <arbeitsverzeichnis>
set -u
BIN="$1"; WD="$2"
mkdir -p "$WD/selftest" "$WD/start"
( cd "$WD/selftest" && env -u RE15_ASSET_ROOT -u RE15_CD_ROOT RE15_ASSET_SELFTEST=1 timeout 60 "$BIN" >stdout.txt 2>stderr.txt ); echo "selftest rc=$?"
grep -m1 '^\[selftest\] RESULT' "$WD/selftest/debug.log" 2>/dev/null || echo "(keine RESULT-Zeile)"
( cd "$WD/start" && RE15_NO_INTRO=1 RE15_TITLE_SHOT=title.bmp RE15_INV_SHOT=exit34.bmp timeout 120 "$BIN" >stdout.txt 2>stderr.txt ); echo "start rc=$?"
( cd "$WD/start" && sha256sum *.bmp 2>/dev/null | cut -c1-16,65- ) || echo "(keine Bilder)"
grep -c . "$WD/start/debug.log" 2>/dev/null | sed 's/^/debug.log Zeilen: /'
