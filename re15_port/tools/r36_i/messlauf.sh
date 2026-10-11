#!/usr/bin/env bash
# Runde 36 Spur I — Messlauf mit der ECHTEN exe (Messkopie re15_pc_r36i.exe neben der exe,
# Muster tools/r35_e/messlauf.sh). Aufruf:
#   bash re15_port/tools/r36_i/messlauf.sh <name> <timeout_s> KEY=WERT ...
# Arbeitsordner: re15_port/build/r36i_mess/<name>/ (debug.log, Logs, Framedumps dort).
set -u
BAUM="$(cd "$(dirname "$0")/../../.." && pwd)"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
NAME="$1"; TO="$2"; shift 2
cp -f "$EXE_DIR/re15_pc.exe" "$EXE_DIR/re15_pc_r36i.exe"
WD="$BAUM/re15_port/build/r36i_mess/$NAME"
rm -rf "$WD"; mkdir -p "$WD"
cd "$WD"
env "$@" timeout "$TO" "$EXE_DIR/re15_pc_r36i.exe" > stdout.txt 2> stderr.txt
echo "exit=$? wd=$WD"
