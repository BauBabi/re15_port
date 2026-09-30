#!/usr/bin/env bash
# pe2_bild.sh <datei.png> - Bildschirm des Emulators (screencap) als PNG
set -u
B=$(cd "$(dirname "$0")" && pwd)
bash "$B/pe2_adb.sh" exec-out screencap -p > "$1"
ls -la "$1"
