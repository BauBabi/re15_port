#!/usr/bin/env bash
# Runde 4 (Vorbild pruefer_echtlauf_r3_belege/mp_isoliert_r3.sh): make_package.sh im Arbeitsbaum ECHT und
# UNVERAENDERT laufen lassen; nur die git-Schreibzugriffe am Ende (git add/rm --cached der Split-Volumes) gehen in
# einen WEGWERF-Index + WEGWERF-Objektspeicher (vorhandene Objekte ueber ALTERNATES lesbar) - sonst laegen ~550 MB
# vorgemerkte Paketdateien im Index dieses Arbeitsbaums, und der naechste Commit naehme sie mit.
# KEIN Python-Shim, PATH unveraendert. Aufruf: LOG=<log> mp_isoliert_r4.sh <make_package-Argumente>
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/r4/git_iso
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
exec bash "$BAUM/analysis/befunde_runde34_android/android_gate_r4_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
