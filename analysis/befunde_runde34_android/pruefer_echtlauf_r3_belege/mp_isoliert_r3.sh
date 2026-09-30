#!/usr/bin/env bash
# Pruefer echtlauf r3 (Vorbild mp_isoliert_nb2.sh / mp_isoliert.sh der r2): make_package.sh im Arbeitsbaum ECHT und
# UNVERAENDERT laufen lassen; nur die git-Schreibzugriffe am Ende (git add/rm --cached der Split-Volumes) gehen in einen
# WEGWERF-Index + WEGWERF-Objektspeicher (vorhandene Objekte ueber ALTERNATES lesbar). Grund: den echten Index dieses
# Baums benutzt auch der parallele Gegenpruefer Umgehung R3 zum Committen - ~550 MB vorgemerkte Paketdateien wuerden
# sonst in dessen naechsten Commit rutschen. KEIN Python-Shim, PATH unveraendert.
# Aufruf: LOG=<log> mp_isoliert_r3.sh <make_package-Argumente>; Rueckgabe = die von make_package.sh.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/pruefer_echtlauf_r3/git_iso
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
exec bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r3_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
