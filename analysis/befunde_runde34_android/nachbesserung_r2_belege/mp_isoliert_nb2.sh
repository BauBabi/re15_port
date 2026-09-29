#!/usr/bin/env bash
# Nachbesserung R2 (wie mp_isoliert_nb.sh der Runde 1): make_package.sh im Arbeitsbaum ECHT laufen lassen,
# nur die git-Schreibzugriffe (git add/rm --cached der Split-Volumes) in einen WEGWERF-Index + WEGWERF-
# Objektspeicher lenken; vorhandene Objekte ueber ALTERNATES lesbar. Rueckgabe = die von make_package.sh.
# Aufruf: LOG=<log> mp_isoliert_nb2.sh <make_package-Argumente>
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/nb2/git_iso
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
exec bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
